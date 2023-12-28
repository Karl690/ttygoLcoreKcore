
#include "mcode.h"
#include "L_Core/storage/nvs.h"
#include "K_Core/tools/tools.h"
#include "K_Core/communication/communication.h"
#include "L_Core/bluetooth/ble.h"

char mcode_string[100] = { 0 };


//ask all ble devices connected "who are you?".
void MCODE_700()
{
	
	//sprintf(mcode_string, "M701 T%d\n", (int)ARG_T);
	//comm_add_string_to_buffer(&bleDevice.TxBuffer, mcode_string);
}

// this is responsive with T (head address)for  700
void MCODE_701()
{
	
}