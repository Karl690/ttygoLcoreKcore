#include "configure.h"
#include "main.h"
#include "taskmanager.h"
#include "l_core/ui/ui.h"
#include "k_core/adc/adc.h"

#include "k_core/communication/parser.h"
#include "k_core/communication/communication.h"
#include "k_core/tools/tools.h"
#include "k_core/execution/cmdprocessor.h"
esp_timer_handle_t systickTimer;


uint16_t SliceCnt = 0; // current slice being processed
uint16_t SliceOffset = 0;
uint32_t HeartBeat = 0;
const PFUNC F1000HZ[NUM_1000HZ] =
{
	Spare,
	// keep as last call in this array
	communication_check_tx,
	Spare,
	parser_incomming_ble_process,
	Spare, //parser_incomming_serial_process,
	cmd_ble_sequener,
	Spare,
	Spare,
};

const PFUNC F100HZ[NUM_100HZ] =
{
	Spare,
	adc_get_process_rawdata,
	Spare, //serial_uart_check_rxtx, //Spare,
	Spare, //serial_rs485_check_rxtx, //Spare,
	Spare,
	Spare,
	Spare,
	Spare,
};

const PFUNC F10HZ[NUM_10HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
};

const PFUNC F1HZ[NUM_1HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	ReportToolInfo,
	BlinkHeartBeat,
};
/*
 * to make the slice scedular more effecient, we are going to use a binary counter, not base 10
 *
 */
//void TaskManager()
void func_SystickCallback(void* arg)
{

	SliceCnt++;
	SliceOffset = SliceCnt & 0x0007; //precalculate the slice index into the jump table
	if (SliceOffset)
	{
		F1000HZ[SliceOffset](); //run the task
		
		return;//toggle pin so we can see on Oscillosclope and exit
	}
	if (SliceCnt & 0x0038)
	{
		SliceOffset = (SliceCnt >> 3) & 0x0007;
		F100HZ[SliceOffset]();
		SliceOffset += 8;
		return;//toggle pin so we can see on Oscillosclope and exit
	}

	if (SliceCnt & 0x01C0)
	{
		SliceOffset = (SliceCnt >> 6)  & 0x0007;
		F10HZ[SliceOffset]();
		SliceOffset += 16;
		return;//toggle pin so we can see on Oscillosclope and exit
	}

	SliceOffset = (SliceCnt >> 9)  & 0x0007;
	F1HZ[SliceOffset]();
	SliceOffset += 24;
	
	return;//toggle pin so we can see on Oscillosclope and exit

}

void Spare(void)
{
	// placeholder call for empty slice
}
void BlinkHeartBeat(void)
{	
	HeartBeat++;
	//AddSerialStringToBuffer(&ComUart.TxBuffer, "a");
	//AddSerialStringToBuffer(&Com485.TxBuffer, "b");
}

void ReportToolInfo()
{
	tools_report_information();
}

void taskamanger_task(void* arg)
{
	
}
void Init_TaskManager()
{
	//xTaskCreatePinnedToCore(taskamanger_task, "taskamanger_task", 1024 * 2, NULL, 10, NULL, 0);
	esp_timer_create_args_t systickTimerArgs = {
		.callback = func_SystickCallback,
		.arg = NULL,
		.dispatch_method = ESP_TIMER_TASK,
		.name = "systick timer"
	};
	esp_timer_create(&systickTimerArgs, &systickTimer); // Create the timer
	esp_timer_start_periodic(systickTimer, 250); // Start the timer in automatic reload mode with a period of 125 microseconds
}