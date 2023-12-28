
#include "sequence.h"
#include "K_Core/communication/parser.h"
#include "mcode.h"
#include "can.h"


void seq_sequence()
{
	if (parser_cmd_que_head == parser_cmd_que_tail) return;
	SimpleCommand* workedCommand = &parser_cmd_que[parser_cmd_que_tail];
	switch (workedCommand->type)
	{
		
	case CMD_CAN:
		can_process_message(workedCommand);
		break;
	case CMD_GCODE:
		//mcode_process_message(workedCommand);
		break;
	case CMD_SPS30:
		//sps30_process_message(workedCommand);
		break;
	}
	
	parser_cmd_que_tail++;
	parser_cmd_que_tail &= (SIZE_OF_COMMAND_QUEUE - 1);
}

/*

void seq_sequence()
{
	if (!cmd_commands_in_que) return;
	cmd_commands_in_que--; //count down the number of commands
		//return;
	seq_current_command_pointer = seq_next_command_pointer;//set the working pointer to the right spot in the execution que
	seq_next_command_pointer++; //point forward to the next command for the next time.abortInProgress
	if (seq_next_command_pointer > (SIZE_OF_COMMAND_QUEUE - 1)) seq_next_command_pointer = 1; //circular que so make sure you do not overrun
	
	cmd_execution_ptr = &cmd_que[seq_current_command_pointer]; //now we are pointing to the right command[] in the instruction que
	
	switch ((uint16_t)cmd_execution_ptr->M)
	{
	case 700:	MCODE_700(); break;
	case 701:	MCODE_701(); break;
//	case 10:	BCODE_10(&cmd_bleCommand); break;
//	case 11:	BCODE_11(&cmd_bleCommand); break;
//	case 100:	BCODE_100(&cmd_bleCommand); break;
	default:
		break;
	}
//	seq_current_command_pointer++;
//	if (seq_current_command_pointer >= SIZE_OF_COMMAND_QUEUE) seq_current_command_pointer = 1;
}
*/