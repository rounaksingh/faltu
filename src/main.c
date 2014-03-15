/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include "ff.h"
#include "diskio.h"

/***********************************************************
	Function Name: signal_callback_handler
	Parameter: signum
	Description: This function handles SIGTERM(15) (kill), SIGTSTP(20) (Ctrl+Z)
				& SIGINT(2) (Ctrl+C)
	Return Value: void.
	
************************************************************/
void signal_callback_handler(int signum)
{
	int ret_val;

	printf("\nCaught signal %d\n",signum);
   
	// Cleanup and close up stuff
	//return value is signal number
	ret_val = disk_deinitialize();
	if(ret_val<0)
	{
		exit(ret_val);
	}
	else
	{
		exit(0);
	}
}

void die (		/* Stop with dying message */
	FRESULT rc	/* FatFs return value */
)
{
	printf("Failed with rc=%u.\n", rc);
	//for (;;) ;
	signal_callback_handler(1000);
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/
#define SIZE_OF_BUFFER			512			//64

int main (void)
{
	// Register signal and signal handler
   	signal(SIGINT, signal_callback_handler);
	signal(SIGTSTP, signal_callback_handler);
	signal(SIGTERM, signal_callback_handler);

	FATFS fatfs;			/* File system object */
	DIR dir;				/* Directory object */
	// FILINFO fno;			/* File information object */
	WORD i;
	 BYTE buff[SIZE_OF_BUFFER];
	FRESULT rc;

	FATFS FatFs;		/* FatFs work area needed for each volume */
	FIL Fil;			/* File object needed for each open file */

	UINT bw, br;

	printf("\nMount a volume.\n");
	rc = f_mount(&fatfs, "", 0);	/* Give a work area to the default drive */
	if (rc) die(rc);

	printf("\nOpen a test file (message.txt).\n");

	// open an existing file with read access
	if (f_open(&Fil, "Fuses.txt", FA_READ | FA_OPEN_EXISTING) == FR_OK) 	/* Create a file */
	{
		printf("\nType the file content.\n");
		for (;;) 
		{
			rc = f_read(&Fil, &buff, SIZE_OF_BUFFER, &br);
			if (rc || !br) break;			// Error or end of file
			for (i = 0; i < br; i++)		// Type the data 
				putchar(buff[i]);
		}
		if (rc) die(rc);

		f_close(&Fil);								/* Close the file */
	}
	if (rc) die(rc);
	printf("File Read Complete.\n");
	
	printf("\nTest completed.\n");

	disk_deinitialize ();

	// for (;;) ;
	return 0;
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
