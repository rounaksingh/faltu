#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "main.h"
#include "scsi.h"

#define INQUIRY_ERROR_EVPD -1

int inquiry(uint8_t evpd, uint8_t page_code, uint16_t allocation_length, uint8_t control)
{
	INQUIRY_CMD_REQUEST inq_cmd_req;
	INQUIRY_CMD_RESPONSE inq_cmd_res;

	int ret_val;
	int actual_no_of_bytes_received;

	inq_cmd_req.op_code=0x12;
	if(evpd==0)
	{
		inq_cmd_req.evpd=evpd;
		inq_cmd_req.page_code=0;
	}
	else if(evpd==1)
	{
		inq_cmd_req.evpd=evpd;
		inq_cmd_req.page_code=page_code;
	}
	else
		return INQUIRY_ERROR_EVPD;

	inq_cmd_req.allocation_length=allocation_length;
	inq_cmd_req.control=control;

	// send the data using bulk transfer
	ret_val = flash_drive_send_data((uint8_t *)&inq_cmd_req, sizeof(INQUIRY_CMD_REQUEST), 0);
	if(ret_val<0)
	{
		return ret_val;
	}
	// sleep for linux for 5 seconds
	//usleep(5000000);

	// receive the data using bulk transfer
	ret_val = flash_drive_receive_data((uint8_t *)&inq_cmd_res, sizeof(INQUIRY_CMD_RESPONSE), &actual_no_of_bytes_received, 5000);
	if(ret_val<0)
	{
		return ret_val;
	}

	printf("\n%d %d\n",sizeof(INQUIRY_CMD_RESPONSE), actual_no_of_bytes_received );
	printf("\n%d\n", (int)inq_cmd_res.peripheral);

	return 0;
}