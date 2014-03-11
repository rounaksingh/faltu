/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include "pff.h"
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
	FILINFO fno;			/* File information object */
	WORD bw, br, i;
	BYTE buff[SIZE_OF_BUFFER];
	FRESULT rc;

	printf("\nMount a volume.\n");
	rc = pf_mount(&fatfs);
	if (rc) die(rc);

	/*printf("\nOpen a test file (message.txt).\n");
	//rc = pf_open("MESSAGE.TXT");			// 10 bytes
	// rc = pf_open("Fuses.txt");			//222 bytes
	// rc = pf_open("cutecom.log");			//1.5MiB
	rc = pf_open("usbmon.txt");			//108kiB
	if (rc) die(rc);

	printf("\nType the file content.\n");
	for (;;) {
		rc = pf_read(buff, sizeof(buff), &br);	 //Read a chunk of file 
		if (rc || !br) break;			// Error or end of file
		for (i = 0; i < br; i++)		// Type the data 
			putchar(buff[i]);
	}
	if (rc) die(rc);

	printf("File Read Complete.\n");
	*/
#if _USE_WRITE
	printf("\nOpen a file to write (write.txt).\n");
	rc = pf_open("usbmon.txt");
	if (rc) die(rc);

	printf("\nWrite a text data. (Hello world!)\n");
	for (;;) {
		//rc = pf_write("Hello world!\r\n", 14, &bw);
		rc = pf_write("sdkfglaksdjflkasjdf!\r\n", 22, &bw);
		if (rc || !bw) break;
	}
	if (rc) die(rc);

	rc = pf_write(0, 0, &bw);
	if (rc) die(rc);
	printf("\nTerminate the file write process.\n");
#endif

#if _USE_DIR
	printf("\nOpen root directory.\n");
	rc = pf_opendir(&dir, "/USB");
	if (rc) die(rc);

	printf("\nDirectory listing...\n");
	for (;;) {
		rc = pf_readdir(&dir, &fno);	/* Read a directory item */
		if (rc || !fno.fname[0]) break;	/* Error or end of dir */
		if (fno.fattrib & AM_DIR)
			printf("   <dir>  %s\n", fno.fname);
		else
			printf("%8lu  %s\n", fno.fsize, fno.fname);
	}
	if (rc) die(rc);
#endif

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
