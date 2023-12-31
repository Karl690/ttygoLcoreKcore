#include "configure.h"
#include "main.h"

#include "serial.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include "K_Core/communication/communication.h"
int serial_uart_fd = -1;


uint8_t serial_uart1_rx_buffer[RX_BUF_SIZE];
uint8_t serial_uart1_rx_urgent_buffer[RX_BUF_SIZE];
uint8_t serial_uart1_tx_buffer[TX_BUF_SIZE];

uint8_t serial_uart2_rx_buffer[RX_BUF_SIZE];
uint8_t serial_uart2_rx_urgent_buffer[RX_BUF_SIZE];
uint8_t serial_uart2_tx_buffer[TX_BUF_SIZE];


COMPORT ComUart1, ComUart2;

uint8_t serial_uart1_last_read_buffer[256];
uint8_t serial_uart2_last_read_buffer[256];
void serial_uart_init(uint8_t port, int tx_pin, int rx_pin, int rts_pin, int cts_pin, bool is485)
{
	const uart_config_t uart_config = {
		.baud_rate = SERIAL_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};

	// We won't use a buffer for sending data.
	uart_driver_install(port, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(port, &uart_config);
	uart_set_pin(port, tx_pin, rx_pin, rts_pin, cts_pin);
	if (is485)
	{
		// Set RS485 half duplex mode	
		ESP_ERROR_CHECK(uart_set_mode(port, UART_MODE_RS485_HALF_DUPLEX));
	}
}

bool serial_uart_write_byte(int uart_port, char byte)
{
	if (uart_write_bytes(uart_port, &byte, 1) != 1) {
		ESP_LOGE(TAG, "Send data critical failure.");
		// add your code to handle sending failure here
		return false;
	}
	return true;
}
char serial_uart_read_byte(int uart_port)
{
	char byte;
	if(uart_read_bytes(uart_port, &byte, 1, (100 / portTICK_PERIOD_MS)) == 0) return 0;
	return byte;
}

void* serial_uart1_read_task(void* param)
{
	uint8_t buffer[256];
	while (1)
	{
		int len = uart_read_bytes(UART_NUM_1, buffer, 256, (100 / portTICK_PERIOD_MS));	
		if (len == 0) {
			buffer[0] = 0;
			continue;
		}
		memcpy(serial_uart1_last_read_buffer, buffer, len);
		serial_uart1_last_read_buffer[len] = 0;
		commnuication_add_buffer_to_serial_buffer(&ComUart1.RxBuffer, buffer, len);
	}
	return NULL;
}
void* serial_uart2_read_task(void* param)
{
	uint8_t buffer[256];
	while (1)
	{
		int len = uart_read_bytes(UART_NUM_2, buffer, 256, (100 / portTICK_PERIOD_MS));	
		if (len == 0) {
			buffer[0] = 0;
			continue;
		}
		memcpy(serial_uart2_last_read_buffer, buffer, len);
		serial_uart2_last_read_buffer[len] = 0;
		commnuication_add_buffer_to_serial_buffer(&ComUart2.RxBuffer, buffer, len);
	}
	return NULL;
}

void serial_init()
{
	pthread_t uart1_thread, uart2_thread;
	// initialize uart devices
	serial_uart_init(UART_NUM_1, SERIAL_UART1_TXD_PIN, SERIAL_UART1_RXD_PIN, SERIAL_UART1_RTS_PIN, SERIAL_UART1_CTS_PIN, false);
	serial_uart_init(UART_NUM_2, SERIAL_UART2_TXD_PIN, SERIAL_UART2_RXD_PIN, SERIAL_UART2_RTS_PIN, SERIAL_UART2_CTS_PIN, false);
	// initialize buffers
	communication_buffers_serial_init(UART_NUM_1, &ComUart1, serial_uart1_rx_buffer, serial_uart1_rx_urgent_buffer, serial_uart1_tx_buffer);
	communication_buffers_serial_init(UART_NUM_2, &ComUart2, serial_uart2_rx_buffer, serial_uart2_rx_urgent_buffer, serial_uart2_tx_buffer);
	pthread_create(&uart1_thread, NULL, serial_uart1_read_task, NULL);
	pthread_create(&uart2_thread, NULL, serial_uart2_read_task, NULL);
}
