#pragma once
#include "main.h"
#include "driver/uart.h"
#include "K_Core/communication/communication.h"


#define SERIAL_485_TXD_PIN	(GPIO_NUM_42)
#define SERIAL_485_RXD_PIN	(GPIO_NUM_1)
#define SERIAL_485_RTS_PIN	(GPIO_NUM_2)

#define SERIAL_UART1_TXD_PIN	(GPIO_NUM_14)  //GPIO_NUM_43= U0TXD //GPIO_NUM_14
#define SERIAL_UART1_RXD_PIN	(GPIO_NUM_21)   //GPIO_NUM_44= U0RXD //GPIO_NUM_21
#define SERIAL_UART1_RTS_PIN	(UART_PIN_NO_CHANGE)
#define SERIAL_UART1_CTS_PIN	(UART_PIN_NO_CHANGE)

#define SERIAL_UART2_TXD_PIN	(GPIO_NUM_12) 
#define SERIAL_UART2_RXD_PIN	(GPIO_NUM_13) 
#define SERIAL_UART2_RTS_PIN	(UART_PIN_NO_CHANGE)
#define SERIAL_UART2_CTS_PIN	(UART_PIN_NO_CHANGE)

#define SERIAL_UART UART_NUM_1
#define SERIAL_RS485 UART_NUM_2

#define SERIAL_BAUD_RATE 19200


extern COMPORT ComUart1;
extern COMPORT ComUart2;
extern uint8_t serial_uart1_last_read_buffer[256];
extern uint8_t serial_uart2_last_read_buffer[256];
void serial_init();

bool serial_uart_write_byte(int uart_port, char byte);
void serial_add_char_to_buffer(ComBuffer *targetBuffer, uint8_t RawChar);
void serial_add_string_to_buffer(ComBuffer *targetBuffer, char* SourceString);
void serial_add_buffer_to_buffer(ComBuffer *targetBuffer, uint8_t* buf, uint16_t size);