#include "communication.h"
#include "RevisionHistory.h"
#include "K_Core/serial/serial.h"
#include "parser.h"
#include "L_Core/bluetooth/ble.h"


uint8_t comm_buffer[256];
uint8_t comm_rx_buffer[RX_BUF_SIZE];
uint8_t comm_rx_urgent_buffer[RX_BUF_SIZE];
uint8_t comm_tx_buffer[TX_BUF_SIZE];

ComBuffer comm_raw_rx_combuffer = { 0 };
ComBuffer comm_raw_rx_urgent_combuffer = { 0 };

void comm_init_buffers()
{
	//comm_init_buffer(&ComUart1, serial_uart1_rx_buffer, RX_BUF_SIZE, serial_uart1_tx_buffer, TX_BUF_SIZE, NULL, 0); //assign buffers and zero 
	//comm_init_buffer(&ComUart2, serial_uart2_rx_buffer, RX_BUF_SIZE, serial_uart2_tx_buffer, TX_BUF_SIZE, NULL, 0); //assign buffers and zero 
	
	//
	// normal rx processing buffer, all characters are funneled into these buffers 
	comm_raw_rx_combuffer.buffer     			= comm_rx_buffer;
	comm_raw_rx_combuffer.Buffer_Size			= RX_BUF_SIZE; //this number is used to keep in bounds
	comm_raw_rx_combuffer.Head					= 0; //start of que
	comm_raw_rx_combuffer.Tail					= 0; //end of the que
	comm_raw_rx_combuffer.RxLineCount			= 0; // if there is a valid command waiting
	memset(comm_raw_rx_combuffer.buffer, 0, comm_raw_rx_combuffer.Buffer_Size);
	//	//urgent rx buffer
	comm_raw_rx_urgent_combuffer.buffer     			= comm_rx_urgent_buffer;
	comm_raw_rx_urgent_combuffer.Buffer_Size			= RX_URGENT_BUF_SIZE; //this number is used to keep in bounds
	comm_raw_rx_urgent_combuffer.Head					= 0; //start of que
	comm_raw_rx_urgent_combuffer.Tail					= 0; //end of the que
	comm_raw_rx_urgent_combuffer.RxLineCount			= 0; // if there is a valid command waiting
	memset(comm_raw_rx_urgent_combuffer.buffer, 0, comm_raw_rx_urgent_combuffer.Buffer_Size);

	//karllvana look at the resetasciparsepointers() it is commented out now
	parser_reset_gcode_buffer(&comm_raw_rx_combuffer);
	parser_reset_gcode_buffer(&comm_raw_rx_urgent_combuffer);
	
}
void comm_init_buffer(COMPORT* CMPORT, uint8_t *rx_buffer, size_t rx_size, uint8_t *tx_buffer, size_t tx_size, uint8_t *rxUrgent_buffer, size_t rxUrgent_size)
{	
	//Initialize comport buffers and assign default values
	CMPORT->RxBuffer.buffer     			= rx_buffer;
	CMPORT->RxBuffer.Buffer_Size			= rx_size; //this number is used to keep in bounds
	CMPORT->RxBuffer.Head				= 0; //start of que
	CMPORT->RxBuffer.Tail				= 0; //end of the que
	CMPORT->RxBuffer.RxLineCount			= 0; // if there is a valid command waiting
	memset(CMPORT->RxBuffer.buffer, 0, rx_size);
	
	CMPORT->TxBuffer.buffer     			= rx_buffer;
	CMPORT->TxBuffer.Buffer_Size	= tx_size; //this number is used to keep in bounds
	CMPORT->TxBuffer.Head				= 0; // index of where to put the next char
	CMPORT->TxBuffer.Tail				= 0; // index of where to pull the next char
	memset(CMPORT->TxBuffer.buffer, 0, tx_size);
	
	if (rxUrgent_size > 0)
	{	
		CMPORT->RxUrgentBuffer.buffer     			= rxUrgent_buffer;
		CMPORT->RxUrgentBuffer.Buffer_Size	=	rxUrgent_size; //this number is used to keep in bounds
		CMPORT->RxUrgentBuffer.Head				= 0; // index of where to put the next char
		CMPORT->RxUrgentBuffer.Tail				= 0; // index of where to pull the next char
		memset(CMPORT->RxUrgentBuffer.buffer, 0, tx_size);	
	}
		
	CMPORT->UrgentFlag					= 0;
	CMPORT->AcksWaiting					= 0;
	
}

void comm_add_buffer_to_buffer(ComBuffer *targetBuffer, uint8_t* buf, uint16_t size)
{
	uint16_t index = 0;
	for (index = 0; index < size; index++)
	{
		targetBuffer->buffer[targetBuffer->Head] = buf[index];
		targetBuffer->Head++;
		targetBuffer->Head &= (targetBuffer->Buffer_Size - 1);
	}
}

void comm_add_string_to_buffer(ComBuffer *targetBuffer, char* SourceString)
{
	uint16_t size = strlen(SourceString);
	comm_add_buffer_to_buffer(targetBuffer, (uint8_t*)SourceString, size);
}

void comm_add_char_to_buffer(ComBuffer *targetBuffer, uint8_t RawChar)
{
	targetBuffer->buffer[targetBuffer->Head] = RawChar;
	targetBuffer->Head++;
	targetBuffer->Head &= (targetBuffer->Buffer_Size - 1);
}



void comm_process_rx_characters(COMPORT* comport, uint8_t* buf, uint16_t len)
{
	ComBuffer* WorkBuf = comport->UrgentFlag ? &comm_raw_rx_urgent_combuffer : &comm_raw_rx_combuffer; //point to the correct buffer
	
	int index = 0;
	uint8_t WorkRxChar;
	while (index < len)	
	{	
		WorkRxChar = buf[index];
		if (WorkRxChar  > 0x19)
		{
			//normal ascii character processing, below 20 hex are special control characters
			comm_add_char_to_buffer(WorkBuf, WorkRxChar);
		}
		//if you get here, you must process special characters
		else
		{			
			switch (WorkRxChar)
			{
			case   0:                                       break; //null end of the string
			case TERMINATE_WAIT_CHAR:   		//if (rawChar==1)
				// Process_TERMINATE_WAIT_CHAR(); 
				break;

			case PAUSE_AT_END_OF_MOVE_CHAR:     //if (rawChar==2)
				//requestToPauseAtEndOfMove = 1; 
				break;//set flag to stop at end of this move

			case PAUSE_AT_END_OF_LAYER_CHAR:     //if (rawChar==3)
				//requestToPauseAtEndOfLayer = 1; 
				break; //(when M790 is executed set flag to stop at end of this layer

			case AVAILABLE_4:	break; //if (rawChar==4)

			case SEND_STATUS_CHAR:    break; //if (rawChar==5)
				//M_Code_M775();break;// send live status on health of motion controller

			case ASCII_ACK: //if (rawChar==6)
				//ShowNextDisplay(); break;

			case PING_CHAR:     //if (rawChar==7)
				//comm_add_string_to_buffer(&MasterCommPort->TxBuffer, (char*)CONNECTIONSTRING);
				break;

			case ABORT_CHAR:  
				//				MotorX.PULSES_TO_GO = 0;
				//				MotorY.PULSES_TO_GO = 0;
				//				MotorZ.PULSES_TO_GO = 0;
								//requestToAbortAtEndOfMove = 1; break;  //if (rawChar==8)
									//this is a job abort, flush buffer NOW!!!!

			case URGENT_911_CMD_CHAR:     //if (rawChar==9)
				comport->UrgentFlag = 1; //tell them this is a hot inject command line
				break;
			case CR_CHAR:
			case CMD_END_CHAR:  //if (rawChar==10) 0xA or 0xD  can trigger the end of line			
				//gcodeCmdsReceived++;
				comm_add_char_to_buffer(WorkBuf, WorkRxChar);
				comport->UrgentFlag = 0; return;
			
				
			case JOG_Z_TABLE_UP:    
				//JogMotorZFlag = 1; break;   //if (rawChar==11)

			case JOG_Z_TABLE_DOWN:  
				//JogMotorZFlag = -1; break;   //if (rawChar==12)

			case REPETREL_COMM_WATCHDOG_CHAR:   // (rawChar==14)
				//_repetrelCommWatchCount = REPETREL_COMM_WATCHDOG_START_VALUE;
				break;

			case JOG_DISPLAYplus: 
				//ShowNextDisplay(); 
				break;											// (rawChar==15)
			case JOG_DISPLAYminus:	
				//ShowPreviousDisplay(); 
				break;
			case VARIABLE_RESET:   
				// ClearSliceTimes(); ResetParseCounters(); 
				break;				//if (rawChar==17)
			}
		}
		index++; //increment to next INCOMING character position
	}	
}

void comm_process_tx(COMPORT* comport)
{
	uint8_t workcharacter;
	uint16_t numberOfXmitCharactersToSend = 0;
	if ((comport->TxBuffer.Head != comport->TxBuffer.Tail) || comport->AcksWaiting)
	{	
		while (comport->AcksWaiting)
		{
			comm_add_char_to_buffer(&comport->TxBuffer, ASCII_ACK);
			comport->AcksWaiting--;
		}
		while (comport->TxBuffer.Head != comport->TxBuffer.Tail)
		{ 
			workcharacter = comport->TxBuffer.buffer[comport->TxBuffer.Tail];
			if (comport->id == COMM_TYPE_BLESERVER)
				comm_buffer[numberOfXmitCharactersToSend] = workcharacter;
			// else
			//	serial_uart_write_byte(comport->id, workcharacter);
			comport->TxBuffer.Tail++;
			comport->TxBuffer.Tail &= (comport->TxBuffer.Buffer_Size - 1);
			numberOfXmitCharactersToSend++;
			if (workcharacter == CMD_END_CHAR || workcharacter == NULL_CHAR)break;//one line at a time please
			if (numberOfXmitCharactersToSend > CMD_MAX_SIZE) break;//limit the transmission packet size
		}
		if (numberOfXmitCharactersToSend > 0 && comport->id == COMM_TYPE_BLESERVER) {
			ble_server_send_data(comm_buffer, numberOfXmitCharactersToSend);
		}
		return;//only process 1 message per tick
	}
}

void comm_check_tx()
{
	comm_process_tx(&bleDevice);	
	//comm_process_tx(&ComUart1);
	//comm_process_tx(&ComUart2);
}
