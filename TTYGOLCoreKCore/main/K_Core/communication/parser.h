#pragma once
#include "main.h"
#include "communication.h"


void parser_incomming_ble_process();
void parser_incomming_serial_process();
void parser_serial_data(COMPORT* comport, uint8_t urgent);
void parser_ble_data(BleDevice* device, uint8_t urgent);
void parser_parse_ble_data(char* data);
