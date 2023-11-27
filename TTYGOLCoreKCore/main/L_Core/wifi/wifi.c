#include <string.h>
#include "main.h"
#include "wifi.h"

#define WIFI_CONNECTED_BIT			BIT0
#define WIFI_FAIL_BIT				BIT1
#define WIFI_DISCONNECTED_BIT		BIT2
#define EXAMPLE_ESP_MAXIMUM_RETRY 10
static int s_retry_num = 0;
static esp_netif_t *s_sta_netif = NULL;

static EventGroupHandle_t s_wifi_event_group;

static void wifi_event_handler(void* arg,
	esp_event_base_t event_base,
	int32_t event_id,
	void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		}
		else {
			xEventGroupSetBits(s_wifi_event_group, WIFI_DISCONNECTED_BIT);
			//xSemaphoreGive(s_semph_get_ip_addrs);
		}
		ESP_LOGI(TAG, "connect to the AP fail");
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;		
		
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		//if (ipAddressLabel) lv_label_set_text_fmt(ipAddressLabel, "#ff00ff %d.%d.%d.%d #", IP2STR(&wifi_info.ip));
		//s_retry_num = 0;
		sprintf((char*)systemconfig.wifi.ip,
			IPSTR,
			IP2STR(&event->ip_info.ip));
		sprintf((char*)systemconfig.wifi.subnet,
			IPSTR,
			IP2STR(&event->ip_info.netmask));
		systemconfig.wifi.status = 1;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
		//xSemaphoreGive(s_semph_get_ip_addrs);
	}
	else if (event_base == WIFI_EVENT) {
		// FMOD
		ESP_LOGE(TAG, "Unhandled WIFI_EVENT event_id = %d", (int)event_id);
		//if (ipAddressLabel) lv_label_set_text_fmt(ipAddressLabel, "#ff00ff Not Connected #");
	}
	else if (event_base == IP_EVENT) {
		// FMOD
		ESP_LOGE(TAG, "Unhandled IP_EVENT event_id = %d", (int)event_id);
		//if (ipAddressLabel) lv_label_set_text_fmt(ipAddressLabel, "#ff00ff Not Connected #");
	}
}


void wifi_init(void)
{	
	s_wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_netif_init());
	
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));	
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE)); // FMOD
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_disconnect(void)
{
	esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, NULL);
	esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, NULL);
	s_retry_num = EXAMPLE_ESP_MAXIMUM_RETRY;
	esp_wifi_disconnect();
	systemconfig.wifi.ip[0] = 0;
	systemconfig.wifi.subnet[0] = 0;
	systemconfig.wifi.status = 0;
}


bool wifi_connect(void)
{
	ESP_LOGI(TAG, "Start connect.");
	xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
	wifi_config_t cfg_sta;
	bzero(&cfg_sta, sizeof(cfg_sta));
	strncpy((char*) cfg_sta.sta.ssid, (const char*)systemconfig.wifi.ssid, sizeof(cfg_sta.sta.ssid));
	strncpy((char*) cfg_sta.sta.password, (const char*)systemconfig.wifi.password, sizeof(cfg_sta.sta.password));
	esp_wifi_set_config(WIFI_IF_STA, (wifi_config_t *) &cfg_sta);
	esp_wifi_set_ps(WIFI_PS_NONE);
	
	s_retry_num = 0;
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL);
	if (esp_wifi_connect() != ESP_OK) return false;
	
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
		WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY);
	if (bits & WIFI_CONNECTED_BIT) {
		return true;
	}
	systemconfig.wifi.ip[0] = 0;
	systemconfig.wifi.subnet[0] = 0;
	systemconfig.wifi.status = 0;
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL, NULL);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL);
	return false;
}