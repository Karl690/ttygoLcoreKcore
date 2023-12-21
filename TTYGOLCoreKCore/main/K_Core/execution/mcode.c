
#include "mcode.h"
#include "L_Core/storage/nvs.h"
#include "K_Core/tools/tools.h"
#include "K_Core/communication/communication.h"
#include "L_Core/bluetooth/ble.h"

#include "cmdprocessor.h"
char mcode_string[100] = { 0 };
/* BCODE 0: disconnect ble-client
	format: B0 
	response: NO
**/
void BCODE_0()
{
	ble_server_disconnect();
}
/* BCODE 10: Reqeust the head position
	format: B10 
	response: >RG:[Head position]
**/
void BCODE_10()
{
	tools_report_register();
}

/* BCODE 11: Set the head position
	format: B11 T11  
	response: >RG:[Head position]
**/
void BCODE_11()
{
	if (!ARG_PRESENT(cmd_execution_ptr->T)) return;
	toolInfo.Address = cmd_execution_ptr->T;
	ble_server_status = BLE_SERVER_HEADSET;
	storage_nvs_set_blob(NVS_KEY_TOOLINFO, &toolInfo, sizeof(ToolInfo));	
	tools_report_register();
	//ui_ble_set_headindex(toolInfo.Address);
}
/* BCODE 100: Set the tool params
	format: B100 T# P# Q# R# S#	
		   T: Head address
		   P: Desired Temperature
		   Q: Desired Heater Duty cycle
		   R: Desired Fan Duty cycle
		   S: Aux analog
**/
void BCODE_100()
{
	if (!ARG_T_PRESENT) return;
	if (ARG_P_PRESENT) toolInfo.DesiredTemperature = ARG_P;
	if (ARG_Q_PRESENT) toolInfo.DesiredHeaterDutyCycle = ARG_Q;
	if (ARG_R_PRESENT) toolInfo.DesiredFanDutyCycle = ARG_R;
	if (ARG_S_PRESENT) toolInfo.ActualAux = ARG_S;
	
	/* this is for testing.*/
	toolInfo.ActualTemperature = toolInfo.DesiredTemperature;
	toolInfo.ActualHeaterDutyCycle = toolInfo.DesiredHeaterDutyCycle;
	toolInfo.ActualFanDutyCycle = toolInfo.DesiredFanDutyCycle;
	
}

//ask all ble devices connected "who are you?".
void MCODE_700()
{
	//comm_add_string_to_buffer(&bleDevice.TxBuffer, "M700\n");
	sprintf(mcode_string, "M701 T%d\n", (int)ARG_T);
	
	comm_add_string_to_buffer(&bleDevice.TxBuffer, mcode_string);
}

// this is responsive with T (head address)for  700
void MCODE_701()
{
	
}