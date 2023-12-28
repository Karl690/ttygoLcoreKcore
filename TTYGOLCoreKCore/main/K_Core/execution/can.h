#pragma once
#include "main.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/communication/parser.h"

#include <stdint.h>

#define CAN_DEV_HOST				0x40
#define CAN_HEAD_12					12
#define CAN_BROADCAST_ADDRESS		0xFF


typedef struct
{
	uint32_t ID;
	uint8_t Immediate;
	uint8_t TargetAddress;
	uint8_t SourceAddress;
	uint8_t MsgType;
	uint8_t MsgId;
	uint8_t Page;
	uint8_t Data[8];
	uint8_t DataSize;
}CANMsg;

typedef enum {
	SWITCH_CONTROL_BY_TEMP,
	SWITCH_CONTROL_BY_FEEDRATE,
	SWITCH_CONTROL_BY_FAN
} switch_control_t;



////////CAN MESSAGE TYPE////////////////////
#define CAN_READ                    0b00        // 0x00
#define CAN_WRITE                   0b01        // 0x01

#define CAN_MSG_BUFFER_SIZE			0x10

////////CAN MESSAGE ID////////////////////
#define CAN_MSG_BOOT				0x00
#define CAN_MSG_HEARTBEAT			0x01
#define CAN_MSG_PING				0x02

#define CAN_MSG_ERASE_SOAPSTRING	0x10
#define CAN_MSG_READ_SOAPSTRING		0x11
#define CAN_MSG_WRITE_SOAPSTRING	0x12

#define CAN_MSG_HSS_CONTROL			0x20

#define CAN_MSG_MOTOR_VELOCIYFACTOR		0x30
#define CAN_MSG_UNPRIMNE				0x40
#define CAN_MSG_EXTRUSION_UNPRIME				0x41
#define CAN_MSG_EXTRUDING_STOP			0x42
#define CAN_MSG_EXTRUSION_PRIME			0x43
#define CAN_MSG_PRIME_RUN				0x44
#define CAN_MSG_PRIME					0x45
#define CAN_MSG_HEAD_EXTRUDE_CONTROL	0x46
#define CAN_MSG_EXTRUSION_UINT			0x47
void can_process_message(SimpleCommand* cmd);
void can_send_live_message();