#include "parser.h"
#include "RevisionHistory.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/execution/cmdprocessor.h"
uint32_t ProcessingError = 0;


void parser_serial_data(COMPORT* comport, uint8_t urgent)
{
	ComBuffer* WorkBuff = urgent == 0 ? &comport->RxBuffer : &comport->RxUrgentBuffer;
	uint8_t RawRxChar;
	uint8_t commentFlag = 0;
	while (WorkBuff->Head != WorkBuff->Tail)
	{
		RawRxChar = WorkBuff->buffer[WorkBuff->Tail]; //update the work character
		WorkBuff->Tail++;
		WorkBuff->Tail &= WorkBuff->Buffer_Size;

		if(commentFlag)		//in comment mode, we should plug into a comment buffer for large transfers
		{
			if(RawRxChar==CMD_END_CHAR)
			{//purge till we see a cr, eol
				WorkBuff->ReadyForAtof =1;
				*WorkBuff->GCodeArgPtr++ = 0;
				commentFlag=0;
				comport->AcksWaiting++;
				return;
			}else 
			{
				*WorkBuff->GCodeArgPtr++=RawRxChar;
				continue;
			}
			//continue;//dont process comments for now
		}

		switch(RawRxChar)
		{
		case CMD_END_CHAR:
			WorkBuff->ReadyForAtof =1;
			commentFlag=0;
			comport->UrgentFlag = 0; 
			comport->AcksWaiting++;
			return;
			//this section is valid variable charcaters, that can be converted by Atof(string) function
		case '1': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '2': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '3': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '4': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '5': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '6': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '7': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '8': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '9': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '0': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '.': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '-': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		case '+': *WorkBuff->GCodeArgPtr=RawRxChar;WorkBuff->GCodeArgPtr++;*WorkBuff->GCodeArgPtr=0; break;//load char, and set next as null
		//end of valid variable characters
		case ' ':   break;//*GCodeArgPtr=' ';GCodeArgPtr++;*GCodeArgPtr=0; break;//POINT TO THE NEXT POSITION PLEASE

		//this is the beginning of the KEY characters in gcode
		case 'A': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgA; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'B': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgB; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'C': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgC; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'D': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgD; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'E': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgE; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'F': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgF; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'G': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgG; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'H': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgH; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'I': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgI; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'J': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgJ; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'K': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgK; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'L': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgL; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'M': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgM; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'N': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgN; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'O': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgO; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'P': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgP; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'Q': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgQ; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'R': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgR; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'S': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgS; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'T': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgT; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'U': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgU; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'V': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgV; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'W': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgW; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'X': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgX; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'Y': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgY; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case 'Z': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgZ; *WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
			//

		case '*': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgSplat;*WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		case '/':
		case ';': WorkBuff->GCodeArgPtr = (char*)&WorkBuff->AsciiArgs.GCodeArgComment; commentFlag=1; WorkBuff->ArgumentLength=0; return;
			//					commentFlag=1;*WorkBuff->GCodeArgPtr++=RawRxChar;WorkBuff->ArgumentLength=0; return;
		}
		WorkBuff->ArgumentLength++;
		if (WorkBuff->ArgumentLength >= (!commentFlag?MAX_CHARS_FOR_PARAMETER: COMMENT_STRING_LENGTH))  // -1 because need room for the NULL
		{
			ProcessingError=1;
			return;//just dont do anything yet
		}
	}
}


void parser_ble_data(BleDevice* device, uint8_t urgent)
{
	BleBuffer* WorkBuff = urgent == 0 ? &device->RxBuffer : &device->RxUrgentBuffer;
	uint8_t RawRxChar;
	uint16_t commandLen = 0;
	while (WorkBuff->Head != WorkBuff->Tail)
	{
		RawRxChar = WorkBuff->buffer[WorkBuff->Tail]; //update the work character
		WorkBuff->Tail++;
		WorkBuff->Tail &= RX_BUF_SIZE;
		if (RawRxChar == CMD_END_CHAR)
		{
			//purge till we see a cr, eol
			WorkBuff->ReadyForAtof = 1;
			*WorkBuff->commandPtr++ = 0;
			//device->AcksWaiting++;
			
			strcpy(cmd_bleCommandsInQueBuffer[cmd_bleNextCommandInsertionPointer], WorkBuff->command); // copy the command string to Que buffer
			WorkBuff->commandPtr = WorkBuff->command; //reset the command buffer
			cmd_bleNextCommandInsertionPointer++;
			if (cmd_bleNextCommandInsertionPointer >= SIZE_OF_COMMAND_QUEUE) cmd_bleNextCommandInsertionPointer = 1;
			cmd_bleCommandsInQue ++;
			return;
		}
		else 
		{
			if (commandLen >= MAX_COMMAND_LEN) {
				ProcessingError ++;
				break;
			}
			*WorkBuff->commandPtr ++= RawRxChar;
		}
		commandLen++;
	}
}

void parser_incomming_ble_process()
{
	if (bleServerDevice.RxBuffer.Head != bleServerDevice.RxBuffer.Tail)
	{
		parser_ble_data(&bleServerDevice, 0);
		return;
	}
	if (bleServerDevice.RxUrgentBuffer.Head != bleServerDevice.RxUrgentBuffer.Tail)
	{
		parser_ble_data(&bleServerDevice, 1);
		return;
	}
	
}

void parser_incomming_serial_process()
{
	parser_serial_data(&ComUart1, 0);
	parser_serial_data(&ComUart1, 1);
	parser_serial_data(&ComUart2, 0);
	parser_serial_data(&ComUart2, 1);
}
