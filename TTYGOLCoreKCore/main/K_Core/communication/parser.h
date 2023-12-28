#pragma once
#include "main.h"
#include "communication.h"
#define UNPROCESSED -1 // used to init cmdLink on cmdQue CleanUpPointers ARG_N_PRESENT

#define SIZE_OF_COMMAND_QUEUE 10
#define MAX_COMMAND_LEN 256


typedef enum
{
	CMD_CAN,
	CMD_GCODE,
	CMD_SPS30,
}cmd_type_t;

typedef struct tagCMD
{
	cmd_type_t type;
	uint8_t cmd[MAX_COMMAND_LEN];
	uint16_t length;
} SimpleCommand;


extern uint16_t parser_cmd_que_head;
extern uint16_t parser_cmd_que_tail;
extern uint16_t parser_cmd_rx_length;

extern SimpleCommand parser_cmd_que[SIZE_OF_COMMAND_QUEUE];

void parser_reset_gcode_buffer(ComBuffer* BufferToReset);
void parser_incomming_into_gcodes();