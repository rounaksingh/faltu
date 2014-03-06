#ifndef _MAIN_H_
#define _MAIN_H_

// int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes, uint16_t timeout);
// int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_transferred_bytes, uint16_t timeout);
#include "libusb.h"

extern struct libusb_device_handle *devh;

int flash_drive_send_data(unsigned char *data_ptr, int no_of_bytes);
int flash_drive_receive_data(unsigned char *data_ptr, int no_of_bytes, int *no_of_actually_transferred_bytes);

#endif