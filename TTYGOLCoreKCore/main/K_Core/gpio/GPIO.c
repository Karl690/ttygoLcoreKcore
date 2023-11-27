#include "main.h"
#include "GPIO.h"
#include "pinout.h"
void gpio_init() 
{
	gpio_set_direction(LCD_BL, GPIO_MODE_OUTPUT);
}