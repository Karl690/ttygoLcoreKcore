#include "main.h"
#include "K_Core/K_Core.h"
#include "L_Core/wifi/wifi.h"
#include "L_Core/bluetooth/ble.h"
#include "L_Core/storage/partition.h"
#include "L_Core/storage/nvs.h"
#ifdef USE_SDCARD
#include "L_Core/sd-card/sd-card.h"
#endif
#include "L_Core/server/server.h"
#ifdef USE_OPC
#include "L_Core/open62541/opc.h"
#endif

#include "K_Core/display/DisplayList.h"
const char *TAG = "L_Core";
bool IsInitialized = false;
SYSTEMCONFIG systemconfig;

void app_main(void)
{
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	IsInitialized = false;
	
	// storage_partition_init();
	storage_nvs_init();
	load_configuration();
#ifdef USE_SDCARD
	if (sdcard_init())
	{	
		if (systemconfig.sdcard.automount)
		{	
			systemconfig.sdcard.status = sdcard_mount();		
		}
	}
#endif
	//wifi_init();
	ble_init();
#ifdef USE_OPC
	InitOPC();
#endif
#ifdef USE_UI
	InitLCDAndLVGL();
	InitUI();
#endif	
	K_Core_Main();
	
	IsInitialized = true;
	return;
}


bool load_configuration()
{
	if (!storage_nvs_get_blob(NVS_KEY_CONFIG, &systemconfig)) return false;
	return true;
}

bool save_configuration()
{
	systemconfig.initialized = 1;
	return storage_nvs_set_blob(NVS_KEY_CONFIG, &systemconfig, sizeof(SYSTEMCONFIG));
	//return storage_partition_write(STORAGE_ADDRESS_SETTINGS, &systemconfig, sizeof(SYSTEMCONFIG));
}