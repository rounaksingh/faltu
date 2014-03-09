#ifndef _MAIN_H_
#define _MAIN_H_

#include "libusb.h"

#define USB_DEVICE_VID 						0x0781
#define USB_DEVICE_PID						0x5151
#define STARTING_LBA						17			// address of starting sector.
#define NO_OF_BYTES_TO_READ 				65500		// No of Bytes to read please 

#define USB_DETACH_KERNEL_DRIVER_ERROR		1
#define USB_INIT_ERROR						1
#define USB_CLAIM_INTERFACE_ERROR			1
#define USB_RELEASE_INTERFACE_ERROR			1
#define USB_ATTACH_KERNEL_DRIVER_ERROR		1
#define USB_OPEN_ERROR						1

//general values for Descriptor of USB flash drive (For BULK ONLY USB TRANSFER)
#define B_CONFIGURATION_VALUE	 				1
#define BULK_ENDPOINT_IN 						0x81 		//default Endpoint for receiving for mass storage
#define BULK_ENDPOINT_OUT 						0x02		//default Endpoint for sending for mass storage
#define OUT_TIMEOUT								1000
#define IN_TIMEOUT								1000
#define CONTROL_TIMEOUT							1000

// Macros for getting MaxLUN using a control transfer
// as per USB Mass Storage Bulk Only Specification 1.0
#define GETMAXLUN_CONTROL_REQUEST_TYPE		0xA1		//Class, Interface and Device to Host
#define GETMAXLUN_CONTROL_REQUEST 			0xFE
#define GETMAXLUN_CONTROL_VALUE				0x00
#define GETMAXLUN_CONTROL_LENGTH 			0x01
#define GETMAXLUN_RESPONSE_DATA_LENGTH		0x01

extern struct libusb_device_handle *devh;

int flash_drive_init(void);
int flash_drive_deinit(void);
int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes);
int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_transferred_bytes);
int flash_drive_reset();
int flash_drive_GetMaxLUN(uint8_t *MaxLUN);

#endif