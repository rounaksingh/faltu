#include "scsi_code.h"
#include <inttypes.h>

// GRPCODE(3bits)+zeros(5bits)
#define CDB_16BYTE_GRP_CODE		0x80
#define CDB_12BYTE_GRP_CODE		0xA0

#define byte uint8_t

typedef struct{

	uint8_t op_code;						//operation op_code
	uint8_t misc_cdb_info_and_section_action;					//3 bits misc CDB information and 5 bits of section action
	uint32_t lba_lsb;						//logical block address
	union
	{
		uint32_t transfer_length;
		uint32_t param_list_length;
		uint32_t allocation_length;
	};
	uint8_t misc_cdb_info;					//misc CDB information
	uint8_t control;
} CDB_12BYTE;

typedef struct{

	uint8_t op_code;						//operation op_code
	uint8_t misc_cdb_info_1;					//misc CDB information
	uint64_t lba_lsb;						//logical block address
	union
	{
		uint32_t transfer_length;
		uint32_t param_list_length;
		uint32_t allocation_length;
	};
	uint8_t misc_cdb_info_2;					//misc CDB information
	uint8_t control;
} CDB_LONG_LBA_16BYTE;

typedef struct{
	uint8_t op_code;
	uint8_t evpd;
	uint8_t page_code;
	uint16_t allocation_length;
	uint8_t control;

} INQUIRY_CMD_REQUEST;

typedef struct{
	byte peripheral;
	byte removble;
	byte version;
	byte response_data_format;
	byte additionallength;
	byte sccstp;
	byte bqueetc;
	byte cmdque;
	char vendor_id[8];
	char product_id[16];
	char product_rev[4];

} INQUIRY_CMD_RESPONSE;



extern int inquiry(uint8_t evpd, uint8_t page_code, uint16_t allocation_length, uint8_t control);