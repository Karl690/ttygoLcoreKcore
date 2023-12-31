//#define USE_OPC
#pragma once
#include <stdint.h>

#define USE_UI
//#define ESP32_SC01
#define ESP32_TTYGO

#define SDCARD_MOUNT_POINT "/sd-card"
#define SYSTEM_CONFIG_FILE SDCARD_MOUNT_POINT"/config.ini"
#define AMPLIIFER_CSV_FILE SDCARD_MOUNT_POINT"/amplifier.csv"

#define CMD_MAX_SIZE 50
#define RX_BUF_SIZE  0x800
#define TX_BUF_SIZE  0x400
#define RX_URGENT_BUF_SIZE 0x400	//1k
#define SOAP_STRING_SIZE 0x400

#ifdef ESP32_SC01
#define SCREEN_WIDTH	480
#define SCREEN_HEIGHT	320
#endif

#ifdef ESP32_TTYGO
#define SCREEN_WIDTH				170
#define SCREEN_HEIGHT				320
#endif
typedef struct
{
	uint8_t ssid[32];
	uint8_t password[32];
	uint8_t autoconnect;
	uint8_t status; //1: connected, 0: disconnected
	uint8_t ip[30];
	uint8_t subnet[30];
} WIFI_CONFIG;

typedef struct
{
	uint8_t autostart; 
	uint8_t status; //1: connected, 0: disconnected	
	uint8_t server_enabled;
	uint8_t client_enabled;
} BLUETOOTH_CONFIG;
typedef struct
{
	uint8_t automount; 
	uint8_t status; //1: connected, 0: disconnected	
} SDCARD_CONFIG;

typedef struct
{
	uint8_t username[30]; 
	uint8_t userpassword[30]; 
	uint8_t autostart; 
	uint8_t status; //1: connected, 0: disconnected	
} OPC_CONFIG;
typedef struct 
{
	uint8_t initialized;
	WIFI_CONFIG wifi;
	BLUETOOTH_CONFIG bluetooth;
	OPC_CONFIG opc;
	SDCARD_CONFIG sdcard;
}SYSTEMCONFIG;
