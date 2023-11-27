#include "communication.h"
#include "RevisionHistory.h"
#include "K_Core/serial/serial.h"
#include "L_Core/bluetooth/ble.h"

void communication_buffers_serial_init(uint8_t UartIndex, COMPORT* ComPort, uint8_t* RxBuffer, uint8_t* RxUgrentBuffer, uint8_t* TxBuffer)
{	
	//Initialize Secs serial's buffers
	ComPort->uart_id = UartIndex;
	ComPort->ComType                = COMTYPE_AUX; //primary control port for PC and REPETREL comm
	ComPort->RxBuffer.buffer     	= RxBuffer;
	ComPort->RxBuffer.Buffer_Size  = RX_BUF_SIZE; //this number is used to keep in bounds
	ComPort->RxBuffer.Head          = 0; //start of que
	ComPort->RxBuffer.Tail          = 0; //end of the que
	ComPort->RxBuffer.RxLineCount = 0; // if there is a valid command waiting
	memset(ComPort->RxBuffer.buffer, 0, RX_BUF_SIZE);
	ComPort->AcksWaiting            = 0;
	
	ComPort->TxBuffer.buffer     	= TxBuffer;
	ComPort->TxBuffer.Buffer_Size  = TX_BUF_SIZE; //this number is used to keep in bounds
	ComPort->TxBuffer.Head		    = 0; // index of where to put the next char
	ComPort->TxBuffer.Tail	        = 0; // index of where to pull the next char
	memset(ComPort->TxBuffer.buffer, 0, TX_BUF_SIZE);	
	
	ComPort->RxUrgentBuffer.buffer      = RxUgrentBuffer;
	ComPort->RxUrgentBuffer.Buffer_Size  = RX_URGENT_BUF_SIZE; //this number is used to keep in bounds
	ComPort->RxUrgentBuffer.Head        = 0; //start of que
	ComPort->RxUrgentBuffer.Tail        = 0; //end of the que
	ComPort->RxUrgentBuffer.RxLineCount = 0; // if there is a valid command waiting
	ComPort->UrgentFlag					= 0;
	ComPort->AcksWaiting				= 0;
	memset(ComPort->RxUrgentBuffer.buffer, 0, RX_BUF_SIZE);
}

void communication_buffers_ble_init(uint8_t id, BleDevice* device)
{
	//Initialize Secs serial's buffers
	device->ble_id = id;
	device->RxBuffer.Head          = 0; //start of que
	device->RxBuffer.Tail          = 0; //end of the que
	device->RxBuffer.commandPtr		= device->RxBuffer.command;
	memset(device->RxBuffer.buffer, 0, RX_BUF_SIZE);
	device->AcksWaiting            = 0;
	
	device->TxBuffer.Head		    = 0; // index of where to put the next char
	device->TxBuffer.Tail	        = 0; // index of where to pull the next char
	device->TxBuffer.commandPtr		= device->TxBuffer.command;
	memset(device->TxBuffer.buffer, 0, TX_BUF_SIZE);	
	
	device->RxUrgentBuffer.Head        = 0; //start of que
	device->RxUrgentBuffer.Tail        = 0; //end of the que	
	device->RxUrgentBuffer.commandPtr		= device->RxUrgentBuffer.command;
	memset(device->RxUrgentBuffer.buffer, 0, RX_BUF_SIZE);
	device->UrgentFlag					= 0;
	device->AcksWaiting				= 0;
}
void commnuication_add_buffer_to_serial_buffer(ComBuffer *targetBuffer, uint8_t* buf, uint16_t size)
{
	uint16_t index = 0;
	for (index = 0; index < size; index++)
	{
		targetBuffer->buffer[targetBuffer->Head] = buf[index];
		targetBuffer->Head++;
		targetBuffer->Head &= targetBuffer->Buffer_Size;
	}
}

void commnuication_add_string_to_serial_buffer(ComBuffer *targetBuffer, char* SourceString)
{
	uint16_t size = strlen(SourceString);
	commnuication_add_buffer_to_serial_buffer(targetBuffer, (uint8_t*)SourceString, size);
}

void commnuication_add_char_to_serial_buffer(ComBuffer *targetBuffer, uint8_t RawChar)
{
	targetBuffer->buffer[targetBuffer->Head] = RawChar;
	targetBuffer->Head++;
	targetBuffer->Head &= targetBuffer->Buffer_Size;
}


void commnuication_add_char_to_ble_buffer(BleBuffer *targetBuffer, uint8_t RawChar)
{
	targetBuffer->buffer[targetBuffer->Head] = RawChar;
	targetBuffer->Head++;
	targetBuffer->Head &= TX_BUF_SIZE;
}

void commnuication_add_buffer_to_ble_buffer(BleBuffer *targetBuffer, uint8_t* buf, uint16_t size)
{
	uint16_t index = 0;
	for (index = 0; index < size; index++)
	{
		targetBuffer->buffer[targetBuffer->Head] = buf[index];
		targetBuffer->Head++;
		targetBuffer->Head &= TX_BUF_SIZE;
	}
}
void commnuication_add_string_to_ble_buffer(BleBuffer *targetBuffer, char* SourceString)
{
	uint16_t size = strlen(SourceString);
	commnuication_add_buffer_to_ble_buffer(targetBuffer, (uint8_t*)SourceString, size);
}


void commnuication_process_rx_serial_characters(COMPORT* comport, uint8_t* buf, uint16_t len)
{
	ComBuffer* WorkBuf = comport->UrgentFlag ? &comport->RxUrgentBuffer : &comport->RxBuffer; //point to the correct buffer
	
	int index = 0;
	uint8_t WorkRxChar;
	while (index < len)	
	{	
		WorkRxChar = buf[index];
		if (WorkRxChar  > 0x19)
		{
			//normal ascii character processing, below 20 hex are special control characters
			commnuication_add_char_to_serial_buffer(WorkBuf, WorkRxChar);
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
				commnuication_add_string_to_serial_buffer(&comport->TxBuffer, (char*)CONNECTIONSTRING);
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

			case CMD_END_CHAR:  //if (rawChar==10) 0xA or 0xD  can trigger the end of line			
				//gcodeCmdsReceived++;
				commnuication_add_char_to_serial_buffer(WorkBuf, CMD_END_CHAR);
				comport->UrgentFlag = 0; return;
			case  13: 		return;//return char, just ignore
				
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

void commnuication_process_rx_ble_characters(BleDevice* device, uint8_t* buf, uint16_t len)
{
	BleBuffer* WorkBuf = device->UrgentFlag ? &device->RxUrgentBuffer : &device->RxBuffer; //point to the correct buffer
	int index = 0;
	uint8_t WorkRxChar;
	while(index < len)	
	{	
		WorkRxChar = buf[index];
		if (WorkRxChar  > 0x19)
		{
			//normal ascii character processing, below 20 hex are special control characters
			commnuication_add_char_to_ble_buffer(WorkBuf, WorkRxChar);
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
				commnuication_add_string_to_ble_buffer(&device->TxBuffer, (char*)CONNECTIONSTRING);
				break;

			case ABORT_CHAR:  
				//				MotorX.PULSES_TO_GO = 0;
				//				MotorY.PULSES_TO_GO = 0;
				//				MotorZ.PULSES_TO_GO = 0;
								//requestToAbortAtEndOfMove = 1; break;  //if (rawChar==8)
									//this is a job abort, flush buffer NOW!!!!

			case URGENT_911_CMD_CHAR:     //if (rawChar==9)
				device->UrgentFlag = 1; //tell them this is a hot inject command line
				break;

			case CMD_END_CHAR:  //if (rawChar==10) 0xA or 0xD  can trigger the end of line			
				//gcodeCmdsReceived++;
				commnuication_add_char_to_ble_buffer(WorkBuf, CMD_END_CHAR);
				device->UrgentFlag = 0; return;
			case  13: 		return;//return char, just ignore
				
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



void communication_process_tx_ble(BleDevice* device)
{
	uint8_t workcharacter;
	uint16_t numberOfXmitCharactersToSend = 0;
	if ((device->TxBuffer.Head != device->TxBuffer.Tail) || device->AcksWaiting)
	{
//		while (device->AcksWaiting)
//		{
//			//device->TxBuffer.command[numberOfXmitCharactersToSend] = ASCII_ACK; //stuff the acks in the front of the serial stream
//			device->AcksWaiting--;
//			numberOfXmitCharactersToSend++;
//		}
		while (device->TxBuffer.Head != device->TxBuffer.Tail)
		{ 
			workcharacter = device->TxBuffer.buffer[device->TxBuffer.Tail];
			device->TxBuffer.command[numberOfXmitCharactersToSend] = workcharacter;
			device->TxBuffer.Tail++;
			device->TxBuffer.Tail &= TX_BUF_SIZE;
			numberOfXmitCharactersToSend++;
			if (workcharacter == CMD_END_CHAR || workcharacter == NULL_CHAR )break;//one line at a time please
			if (numberOfXmitCharactersToSend > CMD_MAX_SIZE) break;//limit the transmission packet size
		}
		if(numberOfXmitCharactersToSend > 0)
			ble_server_send_data((uint8_t*)device->TxBuffer.command, numberOfXmitCharactersToSend);
		return;//only process 1 message per tick
	}
}


void communication_process_tx_serial(COMPORT* serial)
{
	//	serial_uart_write_byte(1, 0x77);
	//	return;
	if (serial->AcksWaiting)
	{
		serial_uart_write_byte(serial->uart_id, ASCII_ACK);
		serial->AcksWaiting--;
	}
	else if (serial->TxBuffer.Head != serial->TxBuffer.Tail)
	{
		serial_uart_write_byte(serial->uart_id, serial->TxBuffer.buffer[serial->TxBuffer.Tail]);
		serial->TxBuffer.Tail++;
		serial->TxBuffer.Head &= serial->TxBuffer.Buffer_Size;
	}	
}

void communication_check_tx()
{
	communication_process_tx_ble(&bleServerDevice);
	
	communication_process_tx_serial(&ComUart1);
	communication_process_tx_serial(&ComUart2);
}
