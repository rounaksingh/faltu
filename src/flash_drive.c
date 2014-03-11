/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <inttypes.h>
#include "flash_drive.h"
#include "libusb.h"
#include "MassStoreCommands.h"
#include "print_struct.h"
#include "config.h"

// The pointer to libusb device handle. This variable is internal to flash_drive.c
static struct libusb_device_handle *devh = NULL;
// variable to save the info about the attached kernel driver to interface of the device.
uint8_t active_kernel_driver=0;

/**
 * Opens a USB flash device using its Vendor ID and Product ID. 
 * The function is called only from within the file.
 * 
 * @param  vendor_id  USB Device Vendor ID
 * @param  product_id USB Device Product ID
 * @return            Pointer to Libusb Device Handle
 */
static libusb_device_handle* libusb_open_device_with_id(uint16_t vendor_id, uint16_t product_id)
{
	struct libusb_device **devs;
	struct libusb_device *found = NULL;
	struct libusb_device *dev;
	struct libusb_device_handle *handle = NULL;
	size_t i = 0;
	int r;

	if (libusb_get_device_list(NULL, &devs) < 0)
		return NULL;

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
		{
			handle=NULL;
			goto out;
			 
		}
		if (desc.idVendor == vendor_id && desc.idProduct == product_id) {
			found = dev;
			break;
		}
	}

	if (found) {
		r = libusb_open(found, &handle);
		if (r < 0)
		{
			printf("%d\n",r);
			handle = NULL;
		}
	}

out:
	libusb_free_device_list(devs, 1);
	return handle;
}/* End libusb_open_device_with_id() */
/*****************************************************************************************************************/

/**
 * Initializes the USB Flash drive. Function finds the USB flash device using its VID and PID, then
 * initiate a sequence of functions to enumerate the device and start the communication with it.
 * @return  Errors/Success (For more info on Errors, Please refer the flash_drive.h file)
 */
int flash_drive_init(void)
{
	int ret_val;
	SCSI_Inquiry_Response_t req_sense_res;
	SCSI_Capacity_t capacity;

	// libusb initialization
	ret_val = libusb_init(NULL);
	if (ret_val < 0)
		return FLASH_DRIVE_INIT_ERR_LIBUSB_INIT;
	
	// Find the USB device and open it for communication.
	devh = libusb_open_device_with_id(USB_DEVICE_VID, USB_DEVICE_PID);
	if(devh == 0)
	{	
		printf("\nCouldnot Open the device with VID= %04X and PID= %04X.\n",USB_DEVICE_VID, USB_DEVICE_PID);
		return FLASH_DRIVE_INIT_ERR_LIBUSB_OPEN_DEVICE;
	}
	
	printf("\nDevice opened with VID= %04X and PID= %04X.",USB_DEVICE_VID, USB_DEVICE_PID);


	// check for the kernel driver, if attached at the interface 0
	// If attached then dettach and set the active_kernel_driver variable 
	// to notify the reattach process at the end of the program.
	ret_val=libusb_kernel_driver_active(devh, BULK_ONLY_DEFAULT_INTERFACE_NUMBER);
	
	// Return Value: (As per the libusb 1.0 manual)
	// 0 : No Kernel Driver active.
	// 1 : Kernel Driver Active.
	// else : libusb_kernel_driver_active Error 
	if(ret_val == 0)
	{
		printf("\nKernel Driver not active on the Interface.\n");
		active_kernel_driver=0;
	}
	else if(ret_val == 1)
	{
		printf("\nKernel Driver active on the Interface.\nExecuting procedure to dettach Kernel driver...\n");

		// Important to remove the USB flash drive from the kernel access
		ret_val=libusb_detach_kernel_driver(devh, BULK_ONLY_DEFAULT_INTERFACE_NUMBER);
		if(ret_val<0)
		{
			printf("\nCouldnot detach the device.\n");
			return FLASH_DRIVE_INIT_ERR_LIBUSB_DETTACH_KERNEL_DRIVER;
		}
		else
		{
			active_kernel_driver=1;
			printf("\nDevice detached.\n");
		}
		
	}
	else
	{
		printf("\nCould not check the kernel_driver_active Error: %d.\n", ret_val);
		return FLASH_DRIVE_INIT_ERR_LIBUSB_KERNEL_ACTIVE;
	}


/*  // As per suggestion from Alan Stern from libusb mailing list
	// To see the suugestion 
	// http://libusb.6.n5.nabble.com/libusb-bulk-transfer-error-9-Pipe-Halt-at-EndPoint-0x81-while-receiving-data-from-usb-flash-drive-tt5712940.html#a5712943
	// configuration set to B_CONFIGURATION_VALUE
	ret_val=libusb_set_configuration(devh,B_CONFIGURATION_VALUE);
	if(ret_val<0)
	{
		printf("\nConfiguration failed.\n");
		return ret_val;
	}
	else
	{
		printf("\nConfigured.\n");
	}*/

	// Reset the USB device (using USB Control transfer) to receive the SCSI commands.
	// Though as per USB Mass Storage specification (Bulk Only), 
	// a reset (control transfer) request should be sent to USB flash drive as first command,
	// But the libusb provides us a reset functions so we are going to use it.
	ret_val=flash_drive_reset();
	if(ret_val<0)
	{
		printf("\nCouldnot reset device. Error: %d\n",ret_val);
		return FLASH_DRIVE_INIT_ERR_LIBUSB_RESET_DRIVER;
	}

	printf("\nReset device completed.\n");

	// claim the interface 0 for data transfer (Interface Zero is normally the )
	ret_val=libusb_claim_interface(devh, BULK_ONLY_DEFAULT_INTERFACE_NUMBER);
	if(ret_val<0)
	{
		printf("\nCouldnot claim the interface.\n");
		return FLASH_DRIVE_INIT_ERR_LIBUSB_CLAIM_INTERFACE;
	}
	
	printf("\nInterface claimed.\n");

	// send Inquiry SCSI command
	ret_val = MassStore_Inquiry( BULK_ONLY_DEFAULT_LUN_NUMBER, &req_sense_res );
	if(ret_val > 0)
	{
		printf("MassStore_Inquiry Fail\n");
		return FLASH_DRIVE_INIT_ERR_SCSI_INQUIRY;
	}
	else
	{
		printf("MassStore_Inquiry successful.\n");
		
		// Print the struct Inquiry using a special print function in print_struct.c
		print_struct_INQ(&req_sense_res);
		
		// Test UNIT Ready
		ret_val=MassStore_TestUnitReady(BULK_ONLY_DEFAULT_LUN_NUMBER);
		if(ret_val > 0)
		{
			printf("Mass Storage Not Ready. Do a Request Sense.\n");
			return FLASH_DRIVE_INIT_ERR_TEST_UNIT_READY;
		}
		
		printf("Mass Storage Ready.\n");

		printf("\nReading Capacity of Mass Storage device (LUN=0):\n");
		
		// Read the Block Size and no of Blocks in USB flash drive
		// Since, the Petit FATfs is compatible with Block Size 512 bytes
		// therefore, it is important to check the Block Size.
		ret_val=MassStore_ReadCapacity( BULK_ONLY_DEFAULT_LUN_NUMBER, &capacity);
		if(ret_val > 0)
		{
			printf("Mass Storage Read Capacity failed.\n");
			return FLASH_DRIVE_INIT_ERR_READ_CAPACITY;
		}
		else
		{
			printf("\nMass Storage Read Capacity success.\n");
			printf("No of Blocks: 0x%04X, %d\n",capacity.Blocks, capacity.Blocks);
			printf("BlockSize: 0x%04X, %d bytes\n",capacity.BlockSize, capacity.BlockSize);
		}
		// Check for Block Size of flash in USB flash drive.
		// If Block Size is not 512 bytes then return Error.
		if(capacity.BlockSize!=512)	
		{
			printf("Error: libPFF is not compatiable with Block Size not equal to 512 bytes");
			return FLASH_DRIVE_INIT_ERR_BLOCKSIZE_NOT_512;
		}
	}

	return FLASH_DRIVE_INIT_SUCCESS;
}/* End flash_drive_init() */
/*****************************************************************************************************************/

/**
 * Deinitializes the USB flash_drive after use
 * @return Errors/Success (For more info on Errors, Please refer the flash_drive.h file)
 */
int flash_drive_deinit()
{
	int ret_val=0;

	// release the interface zero if claimed.
	ret_val=libusb_release_interface(devh, BULK_ONLY_DEFAULT_INTERFACE_NUMBER);
	if(ret_val<0)
	{
		printf("\nRelease of Interface Failed.	%d\n",ret_val);
		//BEWARE donot add return here because below functions are important for execution.
	}
	else
		printf("\nInterfaced released.\n");

	// Reattach the kernel driver
	if(active_kernel_driver==1)
	{
		ret_val=libusb_attach_kernel_driver(devh, BULK_ONLY_DEFAULT_INTERFACE_NUMBER);
		if(ret_val<0)
		{
			printf("\nReattach Failed.	%d\n",ret_val);
			// BEWARE: We are not returning here because at the bottom ret_val is not going to change 
			//  therefore it will automatically return.
			// return USB_ATTACH_KERNEL_DRIVER_ERROR;
			ret_val = FLASH_DRIVE_DEINIT_ERR_REATTACH_KERNEL_DRIVER;
		}
		else
			printf("\nReattach Completed.\n");
	}

	// please donot put the libusb_close() and libusb_exit() above libusb_attach_kernel_driver()
	// as libusb_attach_kernel_driver() requires the devh handler to attach the kernel driver 
	// to provided interface.
	if(devh)
		libusb_close(devh);

	libusb_exit(NULL);
	
	return FLASH_DRIVE_DEINIT_SUCCESS;
}/* End flash_drive_deinit() */
/*****************************************************************************************************************/

/**
 * [flash_drive_send_data description]
 * @param  data_ptr    [description]
 * @param  no_of_bytes [description]
 * @return             [description]
 */
int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes)
{
	int ret_val;
	int actual_length=2;

	ret_val=libusb_bulk_transfer(devh, BULK_ONLY_DEFAULT_ENDPOINT_OUT, data_ptr, no_of_bytes, &actual_length, OUT_TIMEOUT);
	if(ret_val<0)
	{
		#ifdef DEBUG_FLASH_DRIVE
		printf("\nSend Failure.	%d\n",ret_val);
		#endif
		return ret_val;
	}

	else
	{
		#ifdef DEBUG_FLASH_DRIVE
		printf("\nSend Complete. %d  %d\n",no_of_bytes, actual_length);	
		#endif
	}
	
	return 0;
}

int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_received_bytes)
{
	int ret_val;
	int retry=5;

	while(retry)
		{
			#ifdef DEBUG_FLASH_DRIVE
			printf("Retry %d",retry);
			#endif
			ret_val=libusb_bulk_transfer(devh, BULK_ONLY_DEFAULT_ENDPOINT_IN, data_ptr, no_of_bytes, no_of_actually_received_bytes, IN_TIMEOUT);
			if(ret_val==-9)
			{
				retry--;
				#ifdef DEBUG_FLASH_DRIVE
				printf("PIPE ERROR\n");
				#endif
				continue;
			}
			else if(ret_val<0)	
			{	
				retry--;
				#ifdef DEBUG_FLASH_DRIVE
				printf("\nReceive Failure.	Error:%d\tReceivedBytes:%d\tActuallyReceivedBytes:%d\n",ret_val,no_of_bytes,*no_of_actually_received_bytes);
				#endif
				continue;
			}
			else
			{
				#ifdef DEBUG_FLASH_DRIVE
				printf("\nReceive Complete. \tReceivedBytes:%d\tActuallyReceivedBytes:%d\n",no_of_bytes,*no_of_actually_received_bytes);
				#endif
				break;
			}
			
		}

	return ret_val;
}

// THought the MassStoreCommands.c has a specific function for reseting the USB flash device 
// by using SCSI commands 
int flash_drive_reset()
{
	int ret_val;
	//reset the device
	ret_val=libusb_reset_device(devh);

	return ret_val;
}

// MaxLUN should be read by using USB control transfer, there is no SCSI command.
// MaxLUN if zero then LUN not supported.
// if MaxLUN is one then the LUN no is zero
// if MaxLUN is n then the LUN no is from zero to (n-1)
// LUN numbers should be from 0 to 15 as per USB Mass Storage specification
int flash_drive_GetMaxLUN(uint8_t *MaxLUN)
{
	int ErrorCode;
	// For getting maximum LUN, we need to send a control transfer as per USB Mass Storage Bulk Only Specification 1.0 section 3.2
	// Taking Interface (USB) = Index (libusb_control_transfer) = 0
	ErrorCode=libusb_control_transfer(devh,	GETMAXLUN_CONTROL_REQUEST_TYPE, GETMAXLUN_CONTROL_REQUEST, GETMAXLUN_CONTROL_VALUE, 0,
	MaxLUN, GETMAXLUN_CONTROL_LENGTH, CONTROL_TIMEOUT);
	
	// if GET MAX LUN returns 0 means successful and if MaxLUNIndex is zero means media controller has STALL
	// therefore clear STALL or Endpoint halt 
	if (ErrorCode == 0 && *MaxLUN==0)
	{
		// Clear the pipe stall
		// here endpoint is zero (0) that is Control Endpoint
		ErrorCode = libusb_clear_halt(devh, 0);
	}

	return ErrorCode;
	
}
