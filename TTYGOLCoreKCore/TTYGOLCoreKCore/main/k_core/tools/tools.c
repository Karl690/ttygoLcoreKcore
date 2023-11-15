#include "tools.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/communication/communication.h"
ToolInfo toolInfo = { 0 };
char tempstring[256] = { 0 };
void tools_init()
{
	toolInfo.Address = 11;
}

void tools_report_information()
{
	sprintf(tempstring, ">RT:T%d %d %d %d %d\n", toolInfo.Address, toolInfo.ActualTemperature, toolInfo.ActualHeaterDutyCycle, toolInfo.ActualFanDutyCycle, toolInfo.ActualAux);
	commnuication_add_string_to_ble_buffer(&bleServerDevice.TxBuffer, tempstring);
}

void tools_report_register()
{
	sprintf(tempstring, ">RG:%d\n", toolInfo.Address);
	commnuication_add_string_to_ble_buffer(&bleServerDevice.TxBuffer, tempstring);
}

void tools_report_unregister()
{
	sprintf(tempstring, ">RM:%d\n", toolInfo.Address);
	commnuication_add_string_to_ble_buffer(&bleServerDevice.TxBuffer, tempstring);
}

