
#include <lvgl.h>
#define LCD_H_RES            320
#define LCD_V_RES            170
#define LVGL_LCD_BUF_SIZE            (LCD_H_RES * LCD_V_RES) / 10
#define LCD_PIXEL_CLOCK_HZ   (16 * 1000 * 1000)

//#define TOUCH_MODULES_CST_MUTUAL
//#define TOUCH_MODULES_CST_SELF
//#define TOUCH_MODULES_ZTW622
//#define TOUCH_MODULES_L58
extern lv_disp_drv_t disp_drv;
void lcd_init();
/* Setting up tick task for lvgl */
void lv_tick_task(void *arg);

void gui_task(void *args);