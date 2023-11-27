#pragma once
#include "esp_log.h"
#include "nvs_flash.h"

#include <inttypes.h>
#include <string.h>
#include <esp_chip_info.h>
#include <esp_event.h>
#include <esp_flash.h>
#include <esp_ota_ops.h>
#include <soc/rtc.h>
//#include <fmt/core.h>
//#include <fmt/format.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_timer.h"
#include <driver/gpio.h>

#include "configure.h"


#ifdef USE_UI
#include "L_Core/devices/conf_WT32SCO1-Plus.h"
#include "L_Core/devices/display.h"
#include "L_Core/ui/ui.h"
#endif

extern const char *TAG;
extern SYSTEMCONFIG systemconfig;
extern bool IsInitialized;

bool load_configuration();
bool save_configuration();