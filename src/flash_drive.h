#ifndef __FLASH_DRIVE_H__
#define __FLASH_DRIVE_H__

// flash_init() Error Return Values
#define FLASH_DRIVE_INIT_SUCCESS							0
#define FLASH_DRIVE_INIT_ERR_LIBUSB_INIT					1
#define FLASH_DRIVE_INIT_ERR_LIBUSB_OPEN_DEVICE				2
#define FLASH_DRIVE_INIT_ERR_LIBUSB_DETTACH_KERNEL_DRIVER	3
#define FLASH_DRIVE_INIT_ERR_LIBUSB_RESET_DRIVER			4
#define	FLASH_DRIVE_INIT_ERR_LIBUSB_CLAIM_INTERFACE			5
#define FLASH_DRIVE_INIT_ERR_SCSI_INQUIRY					6
#define FLASH_DRIVE_INIT_ERR_TEST_UNIT_READY				7
#define FLASH_DRIVE_INIT_ERR_READ_CAPACITY					8
#define FLASH_DRIVE_INIT_ERR_BLOCKSIZE_NOT_512				9
#define FLASH_DRIVE_INIT_ERR_LIBUSB_KERNEL_ACTIVE			10

// flash_deinit() Error Return Values
#define FLASH_DRIVE_DEINIT_SUCCESS							0
#define FLASH_DRIVE_DEINIT_ERR_REATTACH_KERNEL_DRIVER		1

// DEFAULT values for Descriptor of USB flash drive (For BULK ONLY USB TRANSFER)
#define BULK_ONLY_DEFAULT_CONFIGURATION_VALUE	 			1
#define BULK_ONLY_DEFAULT_ENDPOINT_IN 						0x81
#define BULK_ONLY_DEFAULT_ENDPOINT_OUT 						0x02
#define BULK_ONLY_DEFAULT_LUN_NUMBER						0
#define BULK_ONLY_DEFAULT_INTERFACE_NUMBER					0

// libusb Timeout for Control and Bulk Transfers. Timeouts are in milliseconds.
#define OUT_TIMEOUT											1000
#define IN_TIMEOUT											1000
#define CONTROL_TIMEOUT										1000

// Macros for Reset using a control transfer
// as per USB Mass Storage Bulk Only Specification 1.0
#define RESET_CONTROL_REQUEST_TYPE		0x21		//Class, Interface and Device to Host
#define RESET_CONTROL_REQUEST 			0xFF
#define RESET_CONTROL_VALUE				0x00
#define RESET_CONTROL_LENGTH 			0x01
#define RESET_RESPONSE_DATA_LENGTH		0x00

// Macros for getting MaxLUN using a control transfer
// as per USB Mass Storage Bulk Only Specification 1.0
#define GETMAXLUN_CONTROL_REQUEST_TYPE		0xA1		//Class, Interface and Device to Host
#define GETMAXLUN_CONTROL_REQUEST 			0xFE
#define GETMAXLUN_CONTROL_VALUE				0x00
#define GETMAXLUN_CONTROL_LENGTH 			0x01
#define GETMAXLUN_RESPONSE_DATA_LENGTH		0x01

int flash_drive_init(void);
int flash_drive_deinit(void);
int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes);
int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_transferred_bytes);
int flash_drive_reset();
int flash_drive_GetMaxLUN(uint8_t *MaxLUN);

#endif