#include "lcd.h"
#include "../lv_conf.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "k_core/gpio/pinout.h"
#define LV_TICK_PERIOD_MS 1


//#include "TouchLib.h"
esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
lv_disp_drv_t disp_drv; // contains callback functions
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;
bool inited_touch = false;
static TaskHandle_t g_lvgl_task_handle;
typedef struct {
	uint8_t cmd;
	uint8_t data[14];
	uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
	{ 0x11, { 0 }, 0 | 0x80 },
	{ 0x3A, { 0X05 }, 1 },
	{ 0xB2, { 0X0B, 0X0B, 0X00, 0X33, 0X33 }, 5 },
	{ 0xB7, { 0X75 }, 1 },
	{ 0xBB, { 0X28 }, 1 },
	{ 0xC0, { 0X2C }, 1 },
	{ 0xC2, { 0X01 }, 1 },
	{ 0xC3, { 0X1F }, 1 },
	{ 0xC6, { 0X13 }, 1 },
	{ 0xD0, { 0XA7 }, 1 },
	{ 0xD0, { 0XA4, 0XA1 }, 2 },
	{ 0xD6, { 0XA1 }, 1 },
	{ 0xE0, { 0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32 }, 14 },
	{ 0xE1, { 0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37 }, 14 },

};


#if defined(TOUCH_MODULES_CST_MUTUAL)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS, PIN_TOUCH_RES);
#elif defined(TOUCH_MODULES_CST_SELF)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);
#endif

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	if (is_initialized_lvgl) {
		lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
		lv_disp_flush_ready(disp_driver);
	}
	return false;
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
	esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
	int offsetx1 = area->x1;
	int offsetx2 = area->x2;
	int offsety1 = area->y1;
	int offsety2 = area->y2;
	// copy a buffer's content to a specific area of the display
	esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}


void lcd_init()
{	
	gpio_set_direction(PIN_LCD_RD, GPIO_MODE_OUTPUT);	
	gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);	
	gpio_set_level(PIN_LCD_RD, 1);
	esp_lcd_i80_bus_handle_t i80_bus = NULL;
	esp_lcd_i80_bus_config_t bus_config = {
		.dc_gpio_num = PIN_LCD_DC,
		.wr_gpio_num = PIN_LCD_WR,
		.clk_src = LCD_CLK_SRC_PLL160M,
		.data_gpio_nums = {
			PIN_LCD_D0,
			PIN_LCD_D1,
			PIN_LCD_D2,
			PIN_LCD_D3,
			PIN_LCD_D4,
			PIN_LCD_D5,
			PIN_LCD_D6,
			PIN_LCD_D7,
		},
		.bus_width = 8,
		.max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
	};
	esp_lcd_new_i80_bus(&bus_config, &i80_bus);

	esp_lcd_panel_io_i80_config_t io_config = {
		.cs_gpio_num = PIN_LCD_CS,
		.pclk_hz = LCD_PIXEL_CLOCK_HZ,
		.trans_queue_depth = 20,
		.on_color_trans_done = notify_lvgl_flush_ready,
		.user_ctx = &disp_drv,
		.lcd_cmd_bits = 8,
		.lcd_param_bits = 8,
		.dc_levels = {
		.dc_idle_level = 0,
		.dc_cmd_level = 0,
		.dc_dummy_level = 0,
		.dc_data_level = 1,
	},
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_panel_dev_config_t panel_config = {
		.reset_gpio_num = PIN_LCD_RES,
		.color_space = (lcd_color_rgb_endian_t)ESP_LCD_COLOR_SPACE_RGB,
		.bits_per_pixel = 16,
	};
	esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
	esp_lcd_panel_reset(panel_handle);
	esp_lcd_panel_init(panel_handle);

	esp_lcd_panel_invert_color(panel_handle, true);

	esp_lcd_panel_swap_xy(panel_handle, true);
	esp_lcd_panel_mirror(panel_handle, false, true);
	// the gap is LCD panel specific, even panels with the same driver IC, can
	// have different gap value
	esp_lcd_panel_set_gap(panel_handle, 0, 35);
	gpio_set_level(PIN_LCD_BL, 1);
	
	lv_init();
	lv_disp_buf = (lv_color_t *)heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

	lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
	/*Initialize the display*/
	lv_disp_drv_init(&disp_drv);
	/*Change the following line to your display resolution*/
	disp_drv.hor_res = LCD_H_RES;
	disp_drv.ver_res = LCD_V_RES;
	disp_drv.flush_cb = lvgl_flush_cb;
	disp_drv.draw_buf = &disp_buf;
	disp_drv.user_data = panel_handle;
	lv_disp_drv_register(&disp_drv);
	
	
#ifdef TOUCH_MODULES_CST_MUTUAL
	inited_touch = touch.init();
	if (inited_touch) {
		touch.setRotation(1);
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = lv_touchpad_read;
		lv_indev_drv_register(&indev_drv);
	}
#endif
	
	//*** LVGL : Setup & Initialize the input device driver ***
//	static lv_indev_drv_t indev_drv;
//	lv_indev_drv_init(&indev_drv);
//	indev_drv.type = LV_INDEV_TYPE_POINTER;
//	indev_drv.read_cb = touchpad_read;
//	lv_indev_drv_register(&indev_drv);

//	const esp_timer_create_args_t lvgl_tick_timer_args = {
//		.callback = &lv_tick_task,
//		.name = "lvgl_tick"
//	};
//	esp_timer_handle_t lvgl_tick_timer = NULL;
//	ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
//	ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2 * 1000));

	//    // Setup theme
	//    theme_current = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
	//                                          lv_palette_main(LV_PALETTE_RED),
	//                                          LV_USE_THEME_DEFAULT, /*Light or dark mode*/
	//                                          &lv_font_montserrat_14);
	//
	//    //lv_disp_set_theme(disp, theme_current); /*Assign the theme to the display*/
	//    //bg_theme_color = theme_current->flags & LV_USE_THEME_DEFAULT ? DARK_COLOR_CARD : LIGHT_COLOR_CARD;
	//    bg_theme_color = theme_current->flags & LV_USE_THEME_DEFAULT ? lv_palette_darken(LV_PALETTE_GREY, 5) : lv_color_hex(0xBFBFBD);

	
	is_initialized_lvgl = true;
}

void lv_tick_task(void *arg)
{
	(void)arg;
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

void gui_task(void *args)
{
	//ESP_LOGI(TAG, "Start to run LVGL");
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(10));

		/* Try to take the semaphore, call lvgl related function on success */
	   // lvglLock lock;
		//if (OpcHeartBeatLabel) lv_label_set_text_fmt(OpcHeartBeatLabel, "#ff00ff %d #", (int)OpcHeartBeat);
		lv_task_handler();
		//lv_timer_handler_run_in_period(5); /* run lv_timer_handler() every 5ms */
	}
}


