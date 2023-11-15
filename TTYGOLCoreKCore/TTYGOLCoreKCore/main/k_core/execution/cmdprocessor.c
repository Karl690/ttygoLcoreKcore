
#include "cmdprocessor.h"
#include "bcode.h"
uint32_t cmd_bleNextCommandInsertionPointer = 1;
uint32_t cmd_bleCurrentPointer = 1;
uint32_t cmd_bleCommandsInQue = 0;
char cmd_bleCommandsInQueBuffer[SIZE_OF_COMMAND_QUEUE][MAX_COMMAND_LEN];

GMBCOMMAND cmd_bleCommand;

void cmd_ble_sequener()
{
    if(!cmd_bleCommandsInQue) return;						//no commands to proces, so return
    char* cmd = &cmd_bleCommandsInQueBuffer[cmd_bleCurrentPointer][0];

	if (!convertCommandData(cmd))
	{
		return;
	}

	switch ((uint8_t)cmd_bleCommand.B)
	{
	case 10: BCODE_10(&cmd_bleCommand); break;
	case 100: BCODE_100(&cmd_bleCommand); break;
	default:
		break;
	}
    cmd_bleCurrentPointer ++;
    if(cmd_bleCurrentPointer >= SIZE_OF_COMMAND_QUEUE) cmd_bleCurrentPointer = 1;
	cmd_bleCommandsInQue--;
}

uint8_t convertCommandData(char* cmd)
{
	if (!cmd) return 0;
	char *ch = strtok(cmd, " "); // split command with space
	uint8_t idx = 0;
	memset(&cmd_bleCommand, 0xFF, sizeof(GMBCOMMAND));	//Reset command buffer
	
	while (ch != NULL) {
		if (idx == 0)
		{
			switch (ch[0])
			{	
			case 'B': cmd_bleCommand.type = BLE; break;
			case 'G': cmd_bleCommand.type = GCODE; break;
			case 'M': cmd_bleCommand.type = MCODE; break;
			}
		}
		
		if (ch[1] != 0) 
		{
			char cmd = ch[0];
			ch++;
			switch (cmd)
			{
			case 'A': cmd_bleCommand.A = atof(ch); break;
			case 'B': cmd_bleCommand.B = atof(ch); break;
			case 'C': cmd_bleCommand.C = atof(ch); break;
			case 'D': cmd_bleCommand.D = atof(ch); break;
			case 'E': cmd_bleCommand.E = atof(ch); break;
			case 'F': cmd_bleCommand.F = atof(ch); break;
			case 'G': cmd_bleCommand.G = atof(ch); break;
			case 'H': cmd_bleCommand.H = atof(ch); break;
			case 'I': cmd_bleCommand.I = atof(ch); break;
			case 'J': cmd_bleCommand.J = atof(ch); break;
			case 'K': cmd_bleCommand.K = atof(ch); break;
			case 'L': cmd_bleCommand.L = atof(ch); break;
			case 'M': cmd_bleCommand.M = atof(ch); break;
			case 'N': cmd_bleCommand.N = atof(ch); break;
			case 'O': cmd_bleCommand.O = atof(ch); break;
			case 'P': cmd_bleCommand.P = atof(ch); break;
			case 'Q': cmd_bleCommand.Q = atof(ch); break;
			case 'R': cmd_bleCommand.R = atof(ch); break;
			case 'S': cmd_bleCommand.S = atof(ch); break;
			case 'T': cmd_bleCommand.T = atof(ch); break;
			case 'V': cmd_bleCommand.V = atof(ch); break;
			case 'W': cmd_bleCommand.W = atof(ch); break;
			case 'X': cmd_bleCommand.X = atof(ch); break;
			case 'Y': cmd_bleCommand.T = atof(ch); break;
			case 'Z': cmd_bleCommand.Z = atof(ch); break;
			}	
		}
		ch = strtok(NULL, " "); // split command with space
		idx++;
	}
	if (idx == 0) return 0; // invalid command
	if (cmd_bleCommand.type == UNKNOWN) return 0; //invalid command
	return 1;
}