#pragma once
#include "main.h"

/*ESP32S3*/
#define PIN_LCD_BL                   (gpio_num_t)38

#define PIN_LCD_D0                   (gpio_num_t)39
#define PIN_LCD_D1                   (gpio_num_t)40
#define PIN_LCD_D2                   (gpio_num_t)41
#define PIN_LCD_D3                   (gpio_num_t)42
#define PIN_LCD_D4                   (gpio_num_t)45
#define PIN_LCD_D5                   (gpio_num_t)46
#define PIN_LCD_D6                   (gpio_num_t)47
#define PIN_LCD_D7                   (gpio_num_t)48

#define PIN_POWER_ON                 (gpio_num_t)15

#define PIN_LCD_RES                  (gpio_num_t)5
#define PIN_LCD_CS                   (gpio_num_t)6
#define PIN_LCD_DC                   (gpio_num_t)7
#define PIN_LCD_WR                   (gpio_num_t)8
#define PIN_LCD_RD                   (gpio_num_t)9

#define PIN_BUTTON_1                 (gpio_num_t)0
#define PIN_BUTTON_2                 (gpio_num_t)14
#define PIN_BAT_VOLT                 (gpio_num_t)4

#define PIN_IIC_SCL                  (gpio_num_t)17
#define PIN_IIC_SDA                  (gpio_num_t)18

#define PIN_TOUCH_INT                (gpio_num_t)16
#define PIN_TOUCH_RES                (gpio_num_t)21

/* External expansion */
#define PIN_SD_CMD                   (gpio_num_t)13
#define PIN_SD_CLK                   (gpio_num_t)11
#define PIN_SD_D0                    (gpio_num_t)12