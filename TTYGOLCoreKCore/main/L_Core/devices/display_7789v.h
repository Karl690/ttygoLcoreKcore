#pragma  once

#define LCD_PIXEL_CLOCK_HZ     (6528000) // 170 (h) * 320 (w) * 2 (sizeof(lv_color_t)) * 60 (max fps)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

#define LCD_PIN_NUM_DATA0          39
#define LCD_PIN_NUM_DATA1          40
#define LCD_PIN_NUM_DATA2          41
#define LCD_PIN_NUM_DATA3          42
#define LCD_PIN_NUM_DATA4          45
#define LCD_PIN_NUM_DATA5          46
#define LCD_PIN_NUM_DATA6          47
#define LCD_PIN_NUM_DATA7          48

#define LCD_PIN_RD          	   GPIO_NUM_9
#define LCD_PIN_PWR          	   15
#define LCD_PIN_NUM_PCLK           GPIO_NUM_8		//LCD_WR
#define LCD_PIN_NUM_CS             6
#define LCD_PIN_NUM_DC             7
#define LCD_PIN_NUM_RST            5
#define LCD_PIN_NUM_BK_LIGHT       38

#define KEY_PIN					14
// The pixel number in horizontal and vertical
#define LCD_H_RES              170 //320
#define LCD_V_RES              320
#define LVGL_LCD_BUF_SIZE            (LCD_H_RES * LCD_V_RES)
// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

#define LCD_TICK_PERIOD_MS    2
void InitLCDAndLVGL();