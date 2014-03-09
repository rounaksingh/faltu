/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include "diskio.h"
#include "flash_drive.h"
#include "MassStoreCommands.h"
#include "print_struct.h"
/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	DSTATUS stat;

	stat=(DSTATUS)flash_drive_init();

	
	return stat;
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

	r=MassStore_ReadDeviceBlock(0, sector, 1, 512, &temp_buffer[0]);
	// print_hex_ascii(temp_buffer,512);
	if(r<0)
	{
		printf("Mass Storage Reading failed.\n");
		res=RES_ERROR;
	}
	else
	{
		printf("\nMass Storage Read success.\n");
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


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}

DSTATUS disk_deinitialize (void)
{
	int r;
	// release the interface zero if claimed.
	r=libusb_release_interface(devh,0);
	if(r<0)
	{
		printf("\nRelease of Interface Failed.	%d\n",r);
	}
	//}
	else
	printf("\nInterfaced released.\n");

	flash_drive_deinit();

	return RES_OK;

}
