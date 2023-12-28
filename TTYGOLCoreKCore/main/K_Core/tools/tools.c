#ifdef USE_UI
#include "L_Core/ui/ui.h"
#include "L_Core/ui/ui-bluetooth.h"
#endif
#include "tools.h"
#include "L_Core/storage/nvs.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/communication/communication.h"

ToolInfo toolInfo = { 0 };
char tempstring[256] = { 0 };

void tools_init()
{
	//storage_partition_read(STORAGE_ADDRESS_TOOLINFO, (uint8_t*)&toolInfo, sizeof(ToolInfo));
	storage_nvs_get_blob(NVS_KEY_TOOLINFO, &toolInfo);
	if (toolInfo.Address < HH_POSITION_DEFUALT_BLE) toolInfo.Address = HH_POSITION_DEFUALT_BLE;
#ifdef USE_UI
	// ui_ble_set_headindex(toolInfo.Address);
#endif
}

void tools_report_information()
{
	if (ble_server_status < BLE_SERVER_CONNECTED) return;
	memset(tempstring, 0, 256);
	sprintf(tempstring, ">RT:T%d %d %d %d %d\n", toolInfo.Address, toolInfo.ActualTemperature, toolInfo.ActualHeaterDutyCycle, toolInfo.ActualFanDutyCycle, toolInfo.ActualAux);
	comm_add_string_to_buffer(&bleDevice.TxBuffer, tempstring);
}

void tools_report_register()
{
	if (ble_server_status != BLE_SERVER_HEADSET)	ble_server_status = BLE_SERVER_CONNECTED;
	memset(tempstring, 0, 256);
	sprintf(tempstring, ">RG:%d\n", toolInfo.Address);
	comm_add_string_to_buffer(&bleDevice.TxBuffer, tempstring);
}

void tools_report_unregister()
{
	if (ble_server_status < BLE_SERVER_CONNECTED) return;
	memset(tempstring, 0, 256);
	sprintf(tempstring, ">RM:%d\n", toolInfo.Address);
	comm_add_string_to_buffer(&bleDevice.TxBuffer, tempstring);
}

int tool_rand_range(uint16_t min, uint16_t max)
{
	return rand() % (max + 1 - min) + min;
}
void tool_random_test()
{
	toolInfo.ActualAux = tool_rand_range(1, 10);
	toolInfo.ActualTemperature = tool_rand_range(20, 80);
	toolInfo.ActualFanDutyCycle = tool_rand_range(1, 100);
	toolInfo.ActualHeaterDutyCycle = tool_rand_range(40, 100);
}