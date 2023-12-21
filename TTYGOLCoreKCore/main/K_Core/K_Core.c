#include "configure.h"
#include "main.h"
#include "K_Core.h"
#include "gpio/GPIO.h"
#include "K_Core/communication/communication.h"
#include "taskmanager.h"
#include "display/DisplayList.h"
#include "RevisionHistory.h"
#include "adc/adc.h"
#include "tools/tools.h"
DisplayVariableInfo *ActiveVariableInfo = NULL;
void forground_task(void* arg);
void K_Core_Main()
{
	// Init_RCC();
	comm_init_buffers();
	gpio_init();
	adc_init();	
	tools_init();
	
	Init_TaskManager();
	
	while (1)
	{	
		vTaskDelay(50);
	
	}
}
