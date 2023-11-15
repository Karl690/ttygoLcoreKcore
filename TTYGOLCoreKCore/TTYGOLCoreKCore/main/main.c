/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "main.h"

#include "l_core/lcd/lcd.h"
#include "l_core/ui/ui.h"
#include "l_core/bluetooth/ble.h"
#include "k_core/gpio/pinout.h"
#include "k_core/K_Core.h"
extern "C" void app_main(void)
{
	esp_log_level_set("LLYGO", ESP_LOG_DEBUG); // enable DEBUG logs for this App
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	gpio_set_direction(PIN_POWER_ON, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_POWER_ON, 0);
	
	ble_init();
	lcd_init();
	ui_init();
	k_core_main();
}
