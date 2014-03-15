/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
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


/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/

DWORD get_fattime (void)
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	// Testing functions for time
	// printf ( "Current local time and date: %s", asctime (timeinfo) );
	// printf("Year: %d",timeinfo->tm_year-80);
		// printf("MOnth: %d",timeinfo->tm_mon);
		// printf("Day: %d",timeinfo->tm_mday);
		// printf("Hour: %d",timeinfo->tm_hour);

	// get the system time and accordingly return
	return	  ((DWORD)(timeinfo->tm_year-80) << 25)			// we need year like (current_year - 1980). 
															//so the system returns tm_year with reference to 1990. that is (current - 1900). 
															//Therefore, we are subtracting 80 from system time.
															// for more information, Please read the struct tm in time.h 
			| ((DWORD)(timeinfo->tm_mon+1) << 21)			// Months system returns [0-11]
			| ((DWORD)timeinfo->tm_mday << 16) 				// days system returns [1-31]
			| ((DWORD)timeinfo->tm_hour << 11)				// hour system returns [0-23]
			| ((DWORD)timeinfo->tm_min << 5)				// min 	system returns [0-59]
			| ((DWORD)timeinfo->tm_sec >> 1);				// seconds system returns [0-59]
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



