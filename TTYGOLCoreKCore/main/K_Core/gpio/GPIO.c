#include "main.h"
#include "GPIO.h"
#include "pinout.h"
#include "K_Core/display/DisplayList.h"
#include "L_Core/ui/ui-variables.h"
uint8_t gpio_key_status = 0;
void gpio_init() 
{
	gpio_set_direction(LCD_BL, GPIO_MODE_OUTPUT);
	gpio_set_direction(KEY_PIN, GPIO_MODE_INPUT);
}

void gpio_get_key_value()
{
	uint8_t value = gpio_get_level(KEY_PIN);
	if (!value && gpio_key_status != value)
	{	
		//it means thatKey press.
		// we change the Display list .
		ui_variables_change_panel();
	} 
	gpio_key_status = value;
}