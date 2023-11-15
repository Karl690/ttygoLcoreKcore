#include "configure.h"
#include "main.h"
#include "K_Core.h"
#include "taskmanager.h"
#include "RevisionHistory.h"
#include "adc/adc.h"
#include "tools/tools.h"
// DisplayVariableInfo *ActiveVariableInfo = NULL;
void forground_task(void* arg);
void k_core_main()
{	
	// gpio_init();
	adc_init();
	tools_init();
		
	
	Init_TaskManager();
	//xTaskCreatePinnedToCore(forground_task, "forground_task", 1024 * 2, NULL, 10, NULL, 1);
	while (1)
	{	

		vTaskDelay(50);
	
	}
}

void forground_task(void* arg) {
	while (1)
	{	
		vTaskDelay(50);
		//ui_opc_update_variableList();
	}
}

