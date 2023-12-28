#include "can.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/communication/communication.h"
#include "K_Core/tools/tools.h"
CANMsg canInMsg;
CANMsg canOutMsg;
uint32_t can_msg_process_count = 0;

uint8_t can_temp_buf[8] = { 0 };
char can_temp_string[256] = { 0 };
void can_convert_can2asscii(CANMsg* canMsg)
{
	memset(can_temp_string, 0, 256);
	sprintf(can_temp_string,
		"CAN%d %d %d %d %d %d %d "
		"%d %d "
		"%d\n",
		(int)canMsg->ID, //0
		canMsg->Immediate, //1
		canMsg->TargetAddress, //2
		canMsg->SourceAddress, //3
		canMsg->MsgType, // 4
		canMsg->MsgId,//5
		canMsg->Page, //6
		(int)(*(uint32_t*)canMsg->Data),//7
		(int)(*(uint32_t*)(canMsg->Data + 4)), //11
		canMsg->DataSize); //15
}


uint8_t can_convert_asscii2can(char* cmd)
{
	if (strncmp(cmd, "CAN", 3)) return 0;
	cmd += 3;
	
	char * token = strtok(cmd, " ");
	char item[10][20];
	// loop through the string to extract all other tokens
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		if (token == NULL) break;
		stpcpy(item[i], token);
		token = strtok(NULL, " ");
	}
	if (i != 10) return 0;//invalid format
	
	
	canInMsg.ID = atoi(item[0]);
	canInMsg.Immediate = atoi(item[1]);
	canInMsg.TargetAddress = atoi(item[2]);
	canInMsg.SourceAddress = atoi(item[3]);
	canInMsg.MsgType = atoi(item[4]);
	canInMsg.MsgId = atoi(item[5]);
	canInMsg.Page = atoi(item[6]);
	uint32_t a = atoi(item[7]);
	uint32_t b = atoi(item[8]);
	memcpy(canInMsg.Data, &a, 4);
	memcpy(canInMsg.Data + 4, &b, 4);
	canInMsg.DataSize = atoi(item[9]);
	if (canInMsg.TargetAddress != toolInfo.Address) return 0;
	return 1;
}

uint32_t can_generator_from_id(uint16_t target, uint8_t msgType, uint8_t msgId, uint8_t page, uint8_t immediate)
{
	return (uint32_t)((immediate << 28) + (target << 20) + ((uint32_t)CAN_DEV_HOST << 12) + (msgType << 11) + (msgId << 4) + page);
}
void CanAddTxBuffer(uint16_t target, uint8_t msgType, uint8_t msgId, uint8_t page, uint8_t immediate, uint8_t* data, uint8_t size)
{
	if (toolInfo.Address == HH_POSITION_UNPLUGGED) return;

	canOutMsg.ID = can_generator_from_id(target, msgType, msgId, page, immediate);

	if (data && size > 0) memcpy(canOutMsg.Data, data, size);
	canOutMsg.DataSize = size;
	canOutMsg.Immediate = immediate;
	canOutMsg.MsgType = msgType;
	canOutMsg.MsgId = msgId;
	canOutMsg.Page = page;
	canOutMsg.SourceAddress = toolInfo.Address;
	canOutMsg.TargetAddress = target;
	
	can_convert_can2asscii(&canOutMsg);
	comm_add_string_to_buffer(&bleDevice.TxBuffer, can_temp_string);

}

void can_process_message(SimpleCommand* cmd)
{
	if (!can_convert_asscii2can((char*)cmd->cmd))  return;
	
	switch (canInMsg.MsgType)
	{
	case CAN_WRITE:
		switch (canInMsg.MsgId)
		{
		case CAN_MSG_PING:
			can_send_live_message();
			break;
		case CAN_MSG_ERASE_SOAPSTRING:
			//EraseSoapString();
			break;
		case CAN_MSG_WRITE_SOAPSTRING:
			// AppendToSoapString(canInMsg.Data, canInMsg.DataSize);
			break;
		case CAN_MSG_HSS_CONTROL:
			switch (canInMsg.Page) {
			case SWITCH_CONTROL_BY_TEMP:
				toolInfo.DesiredTemperature = canInMsg.Data[0] + (canInMsg.Data[1] << 8);
				break;
			case SWITCH_CONTROL_BY_FAN:
				toolInfo.DesiredFanDutyCycle = canInMsg.Data[0] + (canInMsg.Data[1] << 8);
				break;
			case SWITCH_CONTROL_BY_FEEDRATE:
				toolInfo.ExtrusionControl.ExtrudeFeedRate = canInMsg.Data[0] + (canInMsg.Data[1] << 8);
				break;
			}
			break;
		case CAN_MSG_EXTRUSION_UINT:
			switch (canInMsg.Page)
			{
			case 0:
				toolInfo.ExtrusionControl.LayerHeight = (float)(canInMsg.Data[0] + (canInMsg.Data[1] << 8) + (canInMsg.Data[2] << 16) + (canInMsg.Data[3] << 24));
				toolInfo.ExtrusionControl.NozzleDiameter = (float)(canInMsg.Data[4] + (canInMsg.Data[5] << 8) + (canInMsg.Data[6] << 16) + (canInMsg.Data[7] << 24));
				break;
			case 1:
				toolInfo.ExtrusionControl.PulsesPerUnit = (float)(canInMsg.Data[0] + (canInMsg.Data[1] << 8) + (canInMsg.Data[2] << 16) + (canInMsg.Data[3] << 24));
				toolInfo.ExtrusionControl.UnitVolume = (float)(canInMsg.Data[4] + (canInMsg.Data[5] << 8) + (canInMsg.Data[6] << 16) + (canInMsg.Data[7] << 24));
				break;
			}
			break;
		case CAN_MSG_EXTRUSION_UNPRIME:
			toolInfo.ExtrusionControl.UnPrimeFeedRate = canInMsg.Data[0] + (canInMsg.Data[1] << 8);
			toolInfo.ExtrusionControl.UnPrimeSteps = canInMsg.Data[2] + (canInMsg.Data[3] << 8);
			toolInfo.ExtrusionControl.UnPrimeTimeMs = canInMsg.Data[4] + (canInMsg.Data[5] << 8);
			break;
		case CAN_MSG_UNPRIMNE:
			break;
		case CAN_MSG_EXTRUDING_STOP:
			break;
		case CAN_MSG_EXTRUSION_PRIME:
			toolInfo.ExtrusionControl.PrimeFeedRate = canInMsg.Data[0] + (canInMsg.Data[1] << 8);
			toolInfo.ExtrusionControl.PrimeSteps = canInMsg.Data[2] + (canInMsg.Data[3] << 8);
			toolInfo.ExtrusionControl.PrimeTimeMs = canInMsg.Data[4] + (canInMsg.Data[5] << 8);
			break;
		case CAN_MSG_PRIME_RUN:
			break;
		case CAN_MSG_PRIME: break;
		default: //that is for Custom packet from PH407 M799 code.
			
			break;
		}
	case CAN_MSG_MOTOR_VELOCIYFACTOR:
		toolInfo.VelocityFactor = canInMsg.Data[0] + (canInMsg.Data[1] << 8) + (canInMsg.Data[2] << 16) + (canInMsg.Data[3] << 24);
		break;
	case CAN_READ:
		switch (canInMsg.MsgId)
		{
		case CAN_MSG_READ_SOAPSTRING:
			can_msg_process_count = 0;
			//can_msg_process_type = TASK_CAN_READ_SOAPSTRING;
			break;
		case CAN_MSG_PING:
			CanAddTxBuffer(CAN_DEV_HOST, CAN_READ, CAN_MSG_PING, canInMsg.Page, 0, 0, 0);
			break;
		}
		break;
	}
}

void can_send_live_message()
{
	can_temp_buf[0] = (uint8_t)(toolInfo.ActualTemperature & 0xff);
	can_temp_buf[1] = (uint8_t)(toolInfo.ActualTemperature >> 8);
	can_temp_buf[2] = (uint8_t)(toolInfo.ActualHeaterDutyCycle & 0xff);
	can_temp_buf[3] = (uint8_t)(toolInfo.ActualHeaterDutyCycle >> 8);
	can_temp_buf[4] = (uint8_t)(toolInfo.ActualFanDutyCycle & 0xff);
	can_temp_buf[5] = (uint8_t)(toolInfo.ActualFanDutyCycle >> 8);
	can_temp_buf[6] = (uint8_t)(toolInfo.ActualAux & 0xff);
	can_temp_buf[7] = (uint8_t)(toolInfo.ActualAux >> 8);
	CanAddTxBuffer(CAN_DEV_HOST, CAN_READ, CAN_MSG_PING, 0, 0, can_temp_buf, 8);
}

