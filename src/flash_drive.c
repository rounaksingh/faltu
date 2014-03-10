/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
// #include <unistd.h>
#include "flash_drive.h"
#include "libusb.h"
#include "MassStoreCommands.h"
#include "print_struct.h"

//#define DEBUG_FLASH_DRIVE		1

struct libusb_device_handle *devh = NULL;
int active_kernel_driver=0;
uint8_t *buffer;

/***********************************************************************************************/
/*LIBUSB related functions*/

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
}

static int find_dpfp_device(void)
{
	devh = libusb_open_device_with_id(USB_DEVICE_VID,USB_DEVICE_PID);
	return devh ? 0 : -1;
}

int flash_drive_init(void)
{
	
	int r, counter;
	SCSI_Inquiry_Response_t req_sense_res;
	SCSI_Capacity_t capacity;

	// libusb initialization
	r = libusb_init(NULL);
	if (r < 0)
		return USB_INIT_ERROR;
	
	r=find_dpfp_device();
	if(r<0)
	{	
		printf("\nCouldnot Open the device with VID= %04X and PID= %04X. Error : %d\n",USB_DEVICE_VID, USB_DEVICE_PID, r);
		return USB_OPEN_ERROR;
	}
	else
	printf("\ndevice opened.\n");
	
	// check for the kernel driver at the interface 0
	r=libusb_kernel_driver_active(devh,0);
	if(r==0)
	{
		printf("\nNot active.\n");
		active_kernel_driver=0;
	}
	else
	{
		printf("\nactive.\n");

		// Important to remove the USB flash drive from the kernel access
		r=libusb_detach_kernel_driver(devh, 0);
		if(r<0)
		{
			printf("\nCouldnot detach the device.\n");
			return USB_DETACH_KERNEL_DRIVER_ERROR;
		}
		else
		{
			active_kernel_driver=1;
			printf("\ndevice detached.\n");
		}
		
	}
/*  // As per suggestion from Alan Stern from libusb mailing list
	// To see the suugestion 
	// http://libusb.6.n5.nabble.com/libusb-bulk-transfer-error-9-Pipe-Halt-at-EndPoint-0x81-while-receiving-data-from-usb-flash-drive-tt5712940.html#a5712943
	// configuration set to B_CONFIGURATION_VALUE
	r=libusb_set_configuration(devh,B_CONFIGURATION_VALUE);
	if(r<0)
	{
		printf("\nConfiguration failed.\n");
		return r;
	}
	else
	{
		printf("\nConfigured.\n");
	}*/

	//reset the device
	r=flash_drive_reset();
	if(r<0)
	{
		printf("\nCouldnot reset. %d\n",r);
		return -23;
	}
	else
	{
		printf("\nRESET.\n");
	}

	// claim the interface 0 for data transfer
	r=libusb_claim_interface(devh, 0);
	if(r<0)
	{
		printf("\nCouldnot claim the interface.\n");
	}
	else{
	printf("\ninterface claimed.\n");
	}

	// send Inquiry SCSI command
	r=MassStore_Inquiry(0, &req_sense_res );
	if(r<0)
	{
		printf("MassStore_Inquiry Fail\n");
	}
	else
	{
		printf("MassStore_Inquiry successful.\n");
		print_struct_INQ(&req_sense_res);
		
		for(counter=0; counter<=1;counter++)
		{
			// Test UNIT Ready
			r=MassStore_TestUnitReady(0);
			if(r<0)
			{
				printf("Mass Storage Not Ready. Do a Request Sense.\n");
			}
			else
			{
				printf("Mass Storage Ready.\n");

			}

			// usleep(2000000);
		}
		
		printf("\nReading Capacity of Mass Storage device (LUN=0):\n");
		r=MassStore_ReadCapacity(0, &capacity);
		if(r<0)
		{
			printf("Mass Storage Read Capacity failed.\n");
		}
		else
		{
			printf("\nMass Storage Read Capacity success.\n");
			printf("No of Blocks: 0x%04X, %d\n",capacity.Blocks, capacity.Blocks);
			printf("BlockSize: 0x%04X, %d bytes\n",capacity.BlockSize, capacity.BlockSize);
		}

		if(capacity.BlockSize!=512)	
		{
			printf("Error: libPFF is not compatiable with Block Size not equal to 512 bytes");
			return 123;
		}
	}

	return 0;
}

int flash_drive_deinit()
{
	int r;

	// Reattch the kernel drivers
	if(active_kernel_driver==1)
	{
		r=libusb_attach_kernel_driver(devh,0);
		if(r<0)
		{
			printf("\nReattach Failed.	%d\n",r);
			return USB_ATTACH_KERNEL_DRIVER_ERROR;
		}
		else
			printf("\nReattach Completed.\n");
	}

	// free buffer
	// if(buffer)
	// 	free(buffer);

	if(devh)
		libusb_close(devh);

	libusb_exit(NULL);
	
	return 0;
}

int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes)
{
	int r;
	int actual_length=2;

	r=libusb_bulk_transfer(devh, BULK_ENDPOINT_OUT, data_ptr, no_of_bytes, &actual_length, OUT_TIMEOUT);
	if(r<0)
	{
		#ifdef DEBUG_FLASH_DRIVE
		printf("\nSend Failure.	%d\n",r);
		#endif
		return r;
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
	int r;
	int retry=5;

	while(retry)
		{
			// usleep(1000000);
			#ifdef DEBUG_FLASH_DRIVE
			printf("Retry %d",retry);
			#endif
			r=libusb_bulk_transfer(devh, BULK_ENDPOINT_IN, data_ptr, no_of_bytes, no_of_actually_received_bytes, IN_TIMEOUT);	
			if(r==-9)
			{
				retry--;
				#ifdef DEBUG_FLASH_DRIVE
				printf("PIPE ERROR\n");
				#endif
				continue;
			}
			else if(r<0)	
			{	
				retry--;
				#ifdef DEBUG_FLASH_DRIVE
				printf("\nReceive Failure.	Error:%d\tReceivedBytes:%d\tActuallyReceivedBytes:%d\n",r,no_of_bytes,*no_of_actually_received_bytes);
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

	return r;
}

// THought the MassStoreCommands.c has a specific function for reseting the USB flash device 
// by using SCSI commands 
int flash_drive_reset()
{
	int r;
	//reset the device
	r=libusb_reset_device(devh);

	return r;
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
// ***********************************************************
	// Function Name: signal_callback_handler
	// Parameter: signum
	// Description: This function handles SIGTERM(15) (kill), SIGTSTP(20) (Ctrl+Z)
				// & SIGINT(2) (Ctrl+C)
	// Return Value: void.
	// 
// ************************************************************
// void signal_callback_handler(int signum)
// {
// 	int ret_val;

// 	printf("\nCaught signal %d\n",signum);
   
// 	// Cleanup and close up stuff
// 	//return value is signal number
// 	ret_val = flash_drive_deinit();
// 	if(ret_val<0)
// 	{
// 		exit(ret_val);
// 	}
// 	else
// 	{
// 		exit(0);
// 	}
// }

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/

// int main (void)
// {
// 	// Register signal and signal handler
//    	signal(SIGINT, signal_callback_handler);
// 	signal(SIGTSTP, signal_callback_handler);
// 	signal(SIGTERM, signal_callback_handler);


// 	int r, counter;
// 	SCSI_Inquiry_Response_t req_sense_res;
// 	SCSI_Capacity_t capacity;

// 	r=flash_drive_init();
// 	if(r==0)
// 	{
// 		// reset the device
// 		r=libusb_reset_device(devh);
// 		if(r<0)
// 		{
// 			printf("\nCouldnot reset. %d\n",r);
// 			return -23;
// 		}
// 		else
// 		{
// 			printf("\nRESET.\n");
// 		}
		
// 		// claim the interface 0 for data transfer
// 		r=libusb_claim_interface(devh, 0);
// 		if(r<0)
// 		{
// 			printf("\nCouldnot claim the interface.\n");
// 		}
// 		else{
// 		printf("\ninterface claimed.\n");
// 		}

// 		// Data Transfer function
// //		flash_drive_transfer_data(devh);
// 		r=MassStore_Inquiry(0, &req_sense_res );
// 		if(r<0)
// 		{
// 			printf("MassStore_RequestSense Fail\n");
// 		}
// 		else
// 		{
// 			printf("MassStore_RequestSense successful.\n");
// 			print_struct_INQ(&req_sense_res);
			
// 			for(counter=0; counter<=1;counter++)
// 			{
// 				// Test UNIT Ready
// 				r=MassStore_TestUnitReady(0);
// 				if(r<0)
// 				{
// 					printf("Mass Storage Not Ready. Do a Request Sense.\n");
// 				}
// 				else
// 				{
// 					printf("Mass Storage Ready.\n");

// 				}

// 				usleep(2000000);
// 			}
			
// 			printf("\nReading Capacity of Mass Storage device (LUN=0):\n");
// 			r=MassStore_ReadCapacity(0, &capacity);
// 			if(r<0)
// 			{
// 				printf("Mass Storage Read Capacity failed.\n");
// 			}
// 			else
// 			{
// 				printf("\nMass Storage Read Capacity success.\n");
// 				printf("No of Blocks: 0x%04X, %d\n",capacity.Blocks, capacity.Blocks);
// 				printf("BlockSize: 0x%04X, %d bytes\n",capacity.BlockSize, capacity.BlockSize);
// 			}

// 			// Reading Sector zero to sector 2. (Number of sector: 3)
// 			printf("\nReading Sector from 0 to n:\n");

// 			// reading sectors 
// 			// No_of_bytes_to-read has max 65535
// 			// 
// 			uint16_t no_of_bytes_to_read = NO_OF_BYTES_TO_READ;
// 			uint32_t starting_LBA = STARTING_LBA;
// 			uint8_t current_sector;
// 			uint8_t no_of_sectors_to_read = no_of_bytes_to_read/capacity.BlockSize;

// 			buffer = malloc(sizeof(uint8_t) * no_of_bytes_to_read);
			
// 			r=MassStore_ReadDeviceBlock(0, starting_LBA, no_of_sectors_to_read, capacity.BlockSize, &buffer[0]);
// 			if(r<0)
// 			{
// 				printf("Mass Storage Reading failed.\n");
// 			}
// 			else
// 			{
// 				printf("\nMass Storage Read success.\n");
// 				for( r = 0 ; r < no_of_bytes_to_read; r = r + capacity.BlockSize )
// 				{
// 					current_sector = starting_LBA + (r/capacity.BlockSize);
// 					printf("\nBlockNo: %d\n", current_sector);
// 					print_hex_ascii(&buffer[r], capacity.BlockSize);
// 				}
				
// 			}

// 		}

// 		// release the interface zero if claimed.
// 		r=libusb_release_interface(devh,0);
// 		if(r<0)											//length :2 *actual_length=2
// 		{												//Timeout=0
// 			printf("\nRelease of Interface Failed.	%d\n",r);
// 		}
// 		//}
// 		else
// 		printf("\nInterfaced released.\n");

// 	}
// 	else
// 	{
// 		// error the return deinit.
// 	}

// 	flash_drive_deinit();
// 	return 0;
// }
