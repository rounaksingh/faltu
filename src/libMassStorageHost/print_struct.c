#include <stdio.h>
#include "MassStoreCommands.h"
#include "print_struct.h"

void print_hex_ascii(uint8_t *buf, uint16_t no_of_char)
{
	uint16_t i,j;

	//
	printf("\n     ");

	// put the numbering for identification at first
	for (i=0;i<0x10;i++)
	printf(" %02X ",i);
	
	// for loop for printing array of data in hex and ascii
	for(i=0;i<no_of_char;i++)
	{	
		// check for counter to be multiple of 0x10
		if(i%(0x10)==0)
		{	
			//print in standard ascii (not in extended ASCII)
			printf("\t");
			for(j=(i-0x10);j<i;j++)
			{
				// check condition for printing
				if(buf[j]>0x20 && buf[j]<0x80)
					printf("%c",buf[j]);
				else
					printf(".");
			}

			// print the data bit
			printf("\n%04X ",i);
		}
		//print the byte in hex form
		printf(" %02X ",buf[i]);

	}

	// calculating no of bytes in last line
	uint16_t byte_index_start_last_line=(no_of_char/0x10)*0x10;
	if(no_of_char==byte_index_start_last_line)
	{
		byte_index_start_last_line = no_of_char - 0x10;
	}
	
	uint16_t no_of_bytes_last_line = no_of_char-byte_index_start_last_line;

	// no of blanks bytes in last line
	uint16_t no_of_blanks_last_line=0x10-no_of_bytes_last_line;

	// fill blanks
	for(j=0;j<no_of_blanks_last_line;j++)
	printf("    ");

	printf("\t");

	for(j=byte_index_start_last_line;j<no_of_char;j++)
	{
		// check condition for printing
		if(buf[j]>0x20 && buf[j]<0x80)
			printf("%c",buf[j]);
		else
			printf(".");
	}

	//
	printf("\n");

}

void print_struct_CBW(CommandBlockWrapper_t *CommandBlockWrapper)
{
	printf("\nCommand Block Wrapper:\n");
	printf("Signature:\t\t%04X\n",CommandBlockWrapper->Signature);
	printf("Tag:\t\t\t%04X\n",CommandBlockWrapper->Tag);
	printf("DataTransferLength:\t%04X\n",CommandBlockWrapper->DataTransferLength);
	printf("Flags:\t\t\t%01X\n",CommandBlockWrapper->Flags);
	printf("LUN:\t\t\t%01X\n",CommandBlockWrapper->LUN);
	printf("SCSICommandLength:\t%01X\n",CommandBlockWrapper->SCSICommandLength);
	printf("SCSICommandData: ");
	print_hex_ascii(&(CommandBlockWrapper->SCSICommandData[0]), 0x10);
}

void print_struct_CSW(CommandStatusWrapper_t *CommandStatusWrapper)
{
	printf("\nCommand Status Wrapper:\n");
	printf("Signature:\t\t%04X\n",CommandStatusWrapper->Signature);
	printf("Tag:\t\t\t%04X\n",CommandStatusWrapper->Tag);
	printf("DataTransferResidue:\t%04X\n",CommandStatusWrapper->DataTransferResidue);
	printf("Status:\t\t\t%01X",CommandStatusWrapper->Status);
	
	if(CommandStatusWrapper->Status==0)
		printf("\t(Command Passed.)\n");
	else if(CommandStatusWrapper->Status==1)
		printf("\t(Command Error.)\n");
	else if(CommandStatusWrapper->Status==2)
		printf("\t(Phase Recovery Reset.)\n");
	else
		printf("\tError: Could not parse the STATUS");

}
