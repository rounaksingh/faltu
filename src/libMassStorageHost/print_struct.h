#ifndef _PRINT_STRUCT_H_
#define _PRINT_STRUCT_H_


void print_hex_ascii(uint8_t *buf, uint16_t no_of_char);
void print_struct_CBW(CommandBlockWrapper_t *CommandBlockWrapper);
void print_struct_CSW(CommandStatusWrapper_t *CommandStatusWrapper);

#endif