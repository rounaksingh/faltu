/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "diskio.h"
#include "flash_drive.h"
#include "MassStoreCommands.h"
#include "print_struct.h"

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
/**
 * Initializes the USB flash drive for opearations such as disk read, disk write.
 * Commands which initializes the USB flash drive, need to be written here.
 * @return  [description]
 */
DSTATUS disk_initialize (void)
{
	uint ret_val;

	ret_val = flash_drive_init();
	if(ret_val == FLASH_DRIVE_INIT_ERR_LIBUSB_OPEN_DEVICE)
	{
		return STA_NODISK;
	}
	else if(ret_val>0)
	{
		return STA_NOINIT;
	}

	return STA_INIT_SUCCESS;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* dest,			/* Pointer to the destination object */
	DWORD sector,		/* Sector number (LBA) */
	WORD sofs,			/* Offset in the sector */
	WORD count			/* Byte count (bit15:destination) */
)
{
	DRESULT res=0;
	BYTE temp_buffer[512];
	WORD i;
	// BYTE *temp_dest;
	// temp_dest=dest;
	int r;
/*
	if(MassStore_TestUnitReady(0))
	{
		return RES_NOTRDY;
	}
*/
	r=MassStore_ReadDeviceBlock(0, sector, 1, 512, &temp_buffer[0]);
	// print_hex_ascii(temp_buffer,512);
	if(r<0)
	{
		#ifdef DEBUG_DISKIO
		printf("Mass Storage Reading failed.\n");
		#endif
		res=RES_ERROR;
	}
	else
	{
		#ifdef DEBUG_DISKIO
		printf("\nMass Storage Read success.\n");
		#endif
		// Since the sofs and count will be under 512 
		// therefore the array temp_buffer does not go out of bound
		for(i=sofs;i<(sofs+count);i++)
		{
			*dest=temp_buffer[i];
			dest++;
		}

		res=RES_OK;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;
	static DWORD *write_lba_ptr;	
	static BYTE *temp_buffer_ptr;
	static WORD *counter_ptr;
	WORD i;

	if(MassStore_TestUnitReady(0))
	{
		return RES_NOTRDY;
	}

	if (!buff) {
		if (sc) {

			// Initiate write process
			write_lba_ptr=(DWORD *)malloc(sizeof(DWORD));
			if(!write_lba_ptr)
				return RES_ERROR;
			*write_lba_ptr = sc;
			temp_buffer_ptr=(BYTE *)malloc(sizeof(BYTE)*512);
			if(!temp_buffer_ptr)
				return RES_ERROR;
			counter_ptr=(WORD *)malloc(sizeof(WORD));
			if(!counter_ptr)
				return RES_ERROR;
			*counter_ptr=0;
			
			res = RES_OK;

		} else {

			// Finalize write process
			while(*counter_ptr<512)
			{
				// pad remaining bytes of sector with zeroes
				temp_buffer_ptr[*counter_ptr]=0;
				(*counter_ptr)++;
			}
			print_hex_ascii(temp_buffer_ptr,*counter_ptr);

			// write the sector
			if(MassStore_WriteDeviceBlock(0,*write_lba_ptr,1,512,temp_buffer_ptr))
			{	
				#ifdef DEBUG_DISKIO
				printf("MassStore_WriteDeviceBlock Failed.\n");
				#endif
				return RES_ERROR;
			}
			#ifdef DEBUG_DISKIO
			printf("MassStore_WriteDeviceBlock Success.\n");
			#endif

			if(write_lba_ptr)
				free(write_lba_ptr);
			if(temp_buffer_ptr)
				free(temp_buffer_ptr);
			if(counter_ptr)
				free(counter_ptr);

			res = RES_OK;
		}
	} else {

		// Send data to the disk
		for (i=0;i<sc;i++)
		{
			temp_buffer_ptr[*counter_ptr] = buff[i];
			printf("%c",buff[i]);

			(*counter_ptr)++;

				
		}

		res = RES_OK;
	}

	return res;
}

/**
 * [disk_deinitialize  description]
 * @return  [description]
 */
DSTATUS disk_deinitialize (void)
{
	int ret_val;

	// The flash_drive_deinit() releases the interface zero, reattach the kernel driver 
	// and closes the libusb library.
	ret_val=  flash_drive_deinit();
	if(ret_val<0)
	{
		return RES_ERROR;
	}
	return RES_OK;

}
