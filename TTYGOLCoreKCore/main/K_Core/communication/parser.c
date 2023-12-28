#include "parser.h"
#include "RevisionHistory.h"
#include "L_Core/bluetooth/ble.h"

uint16_t parser_cmd_que_head = 0;
uint16_t parser_cmd_que_tail = 0;
uint16_t parser_cmd_rx_length = 0;

SimpleCommand parser_cmd_que[SIZE_OF_COMMAND_QUEUE];

void parser_rx_characters(ComBuffer* WorkBuff)
{
	uint8_t RawRxChar;
	while (WorkBuff->Head != WorkBuff->Tail)
	{
		RawRxChar = WorkBuff->buffer[WorkBuff->Tail]; //update the work character
		parser_cmd_que[parser_cmd_que_head].cmd[parser_cmd_rx_length] = RawRxChar;
		WorkBuff->Tail++;
		WorkBuff->Tail &= (WorkBuff->Buffer_Size - 1);
		
		switch (RawRxChar)
		{
		case 0x7E: // for SPS30 
			if (parser_cmd_rx_length == 0)
			{
				parser_cmd_que[parser_cmd_que_head].type = CMD_SPS30;		
			}
			else if (parser_cmd_rx_length > 0)
			{
				parser_cmd_que_head++;
				parser_cmd_que[parser_cmd_que_head].length = parser_cmd_rx_length;				
				parser_cmd_que_head &= (SIZE_OF_COMMAND_QUEUE - 1);
			
				parser_cmd_rx_length = 0;
				return;
			}
			
			break;
		case CR_CHAR:
		case CMD_END_CHAR:
			if (parser_cmd_rx_length < 3)
			{
				//this is the invalid command
				parser_cmd_rx_length = 0;
				return;
			}
			if (parser_cmd_que[parser_cmd_que_head].cmd[0] == 'C' 
				&& parser_cmd_que[parser_cmd_que_head].cmd[1] == 'A' 
				&& parser_cmd_que[parser_cmd_que_head].cmd[2] == 'N') 
				parser_cmd_que[parser_cmd_que_head].type = CMD_CAN; //that is CAN message
			else
				parser_cmd_que[parser_cmd_que_head].type = CMD_GCODE; 
			parser_cmd_que_head++;
			parser_cmd_que[parser_cmd_que_head].length = parser_cmd_rx_length;
			parser_cmd_que_head &= (SIZE_OF_COMMAND_QUEUE - 1);
			
			parser_cmd_rx_length = 0;
			return;
		}
		
		
		parser_cmd_rx_length++;
		if (parser_cmd_rx_length >= MAX_COMMAND_LEN)
		{
			//it is over
			parser_cmd_que_head++;
			parser_cmd_que_head &= (SIZE_OF_COMMAND_QUEUE - 1);
			parser_cmd_rx_length = 0;
		}
	}
}


//this method will take the characters from the input buffer and parse them into
//the ascii args buffers, in preperation for converting to Float
//once a full line has been processed, a flag is set CommandReadyToProcessFlag=1;
//this will signal that the line is complete and that is ok to convert to float
//to prevent stepping on the
void parser_incomming_into_gcodes()
{	
	if (comm_raw_rx_combuffer.Head != comm_raw_rx_combuffer.Tail)
	{
		parser_rx_characters(&comm_raw_rx_combuffer);
		return;
	}
}


//initialize the pointers so we put the parsed characters inthe right place
void parser_reset_gcode_buffer(ComBuffer* BufferToReset)
{
	//this will reset the ascii input buffers by putting a null in the first character
	BufferToReset->AsciiArgs.GCodeArgComment[0] = 0; //set comment to null as well
	BufferToReset->AsciiArgs.GCodeArgComment[1] = 0; //many routines look past the SOAPSTRING_CHAR location for the comment string.
	BufferToReset->AsciiArgs.GCodeArgA[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgB[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgC[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgD[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgE[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgF[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgG[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgH[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgI[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgJ[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgK[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgL[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgM[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgN[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgO[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgP[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgQ[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgR[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgS[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgT[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgU[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgV[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgW[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgX[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgY[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgZ[0] = 0;
	BufferToReset->AsciiArgs.GCodeArgSplat[0] = 0;
	BufferToReset->GCodeArgPtr = (char*)&BufferToReset->AsciiArgs.GCodeArgComment; //default first character
}
