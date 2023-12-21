#include "DisplayList.h"
#include "L_Core/open62541/opc.h"
#include "L_Core/wifi/wifi.h"
#include "L_Core/bluetooth/ble.h"
#include "K_Core/adc/adc.h"
#include "K_Core/gpio/GPIO.h"
#include "K_Core/taskmanager.h"
#include "K_Core/K_Core.h"
#include "K_Core/tools/tools.h"
#include "RevisionHistory.h"
#include "L_Core/ui/ui-variables.h"
const char *BLE_STATUS[4] = { "Listening", "Paired", "Connected", "Headset" };
DisplayVariableInfo GeneralVarsTable[] = {
	{ (void*)1, "SYSTEM", FUNC_TITLE, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &MajorStep, "Version", FUNC_ASCII, COLOR_WHITE, COLOR_WHITE, 0, NULL },
	{ &RevisionDate, "Build", FUNC_ASCII, COLOR_WHITE, COLOR_WHITE, 0, NULL },
	{ &HeartBeat, "HB      ", FUNC_INT32, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &gpio_key_status, "KEY", FUNC_BOOLEAN, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ 0 },
};

DisplayVariableInfo BluetoothVarsTable[] = {
	{ (void*)1, "BLUETOOTH", FUNC_TITLE, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &toolInfo.Address, "Address", FUNC_INT, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &ble_server_status, "STATUS", FUNC_BAR_STATUS, COLOR_WHITE, COLOR_GREEN, (uint32_t)(void*)BLE_STATUS, NULL },
	{ &ble_server_total_sent, "XMT#", FUNC_INT, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &ble_server_status, "REV#", FUNC_INT, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ 0 },
};

DisplayVariableInfo AdcVarsTable[] = {
	{ (void*)1, "ADC", FUNC_TITLE, COLOR_WHITE, COLOR_GREEN, 0, NULL },
	{ &ADC_Channel[0].convAvg, "CH0", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ &ADC_Channel[0].adcAvg, "CH0-Raw", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ &ADC_Channel[1].convAvg, "CH1", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ &ADC_Channel[1].adcAvg, "CH1-Raw", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ &ADC_Channel[2].convAvg, "CH2", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ &ADC_Channel[2].adcAvg, "CH2-Raw", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, NULL },
	{ 0 },
};

