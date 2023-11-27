#include "esp_wifi.h"

extern esp_netif_ip_info_t wifi_info;
extern char ipAddress[];

void wifi_init(void);
esp_netif_ip_info_t wifi_get_ip_info();
void wifi_disconnect();
bool wifi_connect();