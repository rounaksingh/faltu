/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <malloc.h>
#include <unistd.h>

#include "libusb.h"
#include "pff.h"
#include "MassStoreCommands.h"

//#include "scsi.h"

#define USB_DETACH_KERNEL_DRIVER_ERROR	1
#define USB_INIT_ERROR					1
#define USB_CLAIM_INTERFACE_ERROR		1
#define USB_RELEASE_INTERFACE_ERROR		1
#define USB_ATTACH_KERNEL_DRIVER_ERROR	1
#define USB_OPEN_ERROR					1

#define USB_DEVICE_VID 						0x0781
#define USB_DEVICE_PID						0x5151

//general values for Descriptor of USB flash drive
#define B_CONFIGURATION_VALUE	 			1
#define BULK_ENDPOINT_IN 					0x81 		//default Endpoint for receiving for mass storage
#define BULK_ENDPOINT_OUT 					0x02		//default Endpoint for sending for mass storage
#define OUT_TIMEOUT							1000
#define IN_TIMEOUT							1000

static struct libusb_device_handle *devh = NULL;
int active_kernel_driver=0;

/*Functions for pff*/
void die (		/* Stop with dying message */
	FRESULT rc	/* FatFs return value */
)
{
	printf("Failed with rc=%u.\n", rc);
	for (;;) ;
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/

DWORD get_fattime (void)
{
	return	  ((DWORD)(2010 - 1980) << 25)	/* Fixed to Jan. 1, 2010 */
			| ((DWORD)1 << 21)
			| ((DWORD)1 << 16)
			| ((DWORD)0 << 11)
			| ((DWORD)0 << 5)
			| ((DWORD)0 >> 1);
}


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
	
	int r;

	// libusb initialization
	r = libusb_init(NULL);
	if (r < 0)
		return USB_INIT_ERROR;
	
	r=find_dpfp_device();
	if(r<0)
	{
		printf("\nCouldnot Open the device. %d\n",r);
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
/*
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
	
	libusb_close(devh);

	libusb_exit(NULL);
	return 0;
}


/*int flash_drive_send_data()
{
	int r;
	unsigned char *data_ptr="rc";
	int *actual_length=malloc(sizeof(int));
	*actual_length=2;

	r=libusb_bulk_transfer(devh,0x02,data_ptr,3,actual_length,0);	//Endpoint No:1 Data: data_ptr
	if(r<0)											//length :2 *actual_length=2
	{												//Timeout=0
		printf("\nTransfer Failed.	%d\n",r);
	}
	//}
	else
	printf("\nTransfer Completed.\n");

	free(actual_length);
	return 0;
}
*/

int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes)
{
	int r;
	int actual_length=2;

	r=libusb_bulk_transfer(devh, BULK_ENDPOINT_OUT, data_ptr, no_of_bytes, &actual_length, OUT_TIMEOUT);	//Endpoint No:1 Data: data_ptr
	if(r<0)											//length :2 *actual_length=2
	{												//Timeout=0
		printf("\nSend Failure.	%d\n",r);

		return r;
	}

	else
	{
		printf("\nSend Complete. %d  %d\n",no_of_bytes, actual_length);	
	}
	
	return 0;
}

int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_received_bytes)
{
	int r;
	int retry=5;

	while(retry)
		{
			usleep(1000000);
			printf("Retry %d",retry);
			r=libusb_bulk_transfer(devh, BULK_ENDPOINT_IN, data_ptr, no_of_bytes, no_of_actually_received_bytes, IN_TIMEOUT);	
			if(r==-9)
			{
				retry--;
				printf("PIPE ERROR\n");
				continue;
			}
			else if(r<0)	
			{	
				retry--;											
				printf("\nReceive Failure.	Error:%d\tReceivedBytes:%d\tActuallyReceivedBytes:%d\n",r,no_of_bytes,*no_of_actually_received_bytes);
				continue;
			}
			else
			{
				printf("\nReceive Complete. \tReceivedBytes:%d\tActuallyReceivedBytes:%d\n",no_of_bytes,*no_of_actually_received_bytes);
				break;	
			}
			
		}

	return r;
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/

int main (void)
{

	int r;
	SCSI_Inquiry_Response_t req_sense_res;

	r=flash_drive_init();
	if(r==0)
	{
		// reset the device
		r=libusb_reset_device(devh);
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

		// Data Transfer function
//		flash_drive_transfer_data(devh);
		r=MassStore_Inquiry(0, &req_sense_res );
		if(r<0)
		{
			printf("MassStore_RequestSense Fail\n");
		}
		else
		{
			printf("MassStore_RequestSense successful.\n");
		}
		int i;
		printf("\n Vendor::");
		for (i=0;i<8;i++)
		printf("%c", req_sense_res.VendorID[i]);

		printf("\n Product::");
		for (i=0;i<16;i++)
		printf("%c", req_sense_res.ProductID[i]);

		//To identify which SCSI commands the flash drive supports
		printf("\nPeripheral Qualifier: %X", req_sense_res.PeripheralQualifier);
		printf("\nVersion: %X", req_sense_res.Version);
		
		//Is device is removable
		printf("\nRemovable: %X", req_sense_res.Removable);

		// release the interface zero if claimed.
		r=libusb_release_interface(devh,0);
		if(r<0)											//length :2 *actual_length=2
		{												//Timeout=0
			printf("\nRelease of Interface Failed.	%d\n",r);
		}
		//}
		else
		printf("\nInterfaced released.\n");

	}
	else
	{
		// error the return deinit.
	}

	flash_drive_deinit();
	return 0;
/*	FATFS fatfs;			 // File system object 
	DIR dir;				 // Directory object 
	FILINFO fno;			 // File information object
	WORD bw, br, i;
	BYTE buff[64];
	FRESULT rc;

	printf("\nMount a volume.\n");
	rc = pf_mount(&fatfs);
	if (rc) die(rc);

	printf("\nOpen a test file (message.txt).\n");
	rc = pf_open("MESSAGE.TXT");
	if (rc) die(rc);

	printf("\nType the file content.\n");
	for (;;) {
		rc = pf_read(buff, sizeof(buff), &br);	// Read a chunk of file
		if (rc || !br) break;					// Error or end of file
		for (i = 0; i < br; i++)				// Type the data 
			putchar(buff[i]);
	}
	if (rc) die(rc);

#if _USE_WRITE
	printf("\nOpen a file to write (write.txt).\n");
	rc = pf_open("WRITE.TXT");
	if (rc) die(rc);

	printf("\nWrite a text data. (Hello world!)\n");
	for (;;) {
		rc = pf_write("Hello world!\r\n", 14, &bw);
		if (rc || !bw) break;
	}
	if (rc) die(rc);

	printf("\nTerminate the file write process.\n");
	rc = pf_write(0, 0, &bw);
	if (rc) die(rc);
#endif

#if _USE_DIR
	printf("\nOpen root directory.\n");
	rc = pf_opendir(&dir, "");
	if (rc) die(rc);

	printf("\nDirectory listing...\n");
	for (;;) {
		rc = pf_readdir(&dir, &fno);	 // Read a directory item 
		if (rc || !fno.fname[0]) break;	 // Error or end of dir 
		if (fno.fattrib & AM_DIR)
			printf("   <dir>  %s\n", fno.fname);
		else
			printf("%8lu  %s\n", fno.fsize, fno.fname);
	}
	if (rc) die(rc);
#endif

	printf("\nTest completed.\n");
	for (;;) ;*/
}
