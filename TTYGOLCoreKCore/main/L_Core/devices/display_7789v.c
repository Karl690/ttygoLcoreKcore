#include "main.h"
#include "lv_conf.h"
#include  "configure.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include "lvgl.h"

static void example_increase_lvgl_tick(void *arg);
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
static void LCD_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

static void lvglTimerTask(void* param);
/* FUNCTION PROTOTYPES -------------------------------------------------------*/


static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
	lv_disp_flush_ready(disp_driver);
	return false;
}

static void LCD_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
	esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
	int offsetx1 = area->x1;
	int offsetx2 = area->x2;
	int offsety1 = area->y1;
	int offsety2 = area->y2;
	// copy a buffer's content to a specific area of the display
	esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}
#define BSP_NULL_CHECK(x, ret) assert(x)

static void example_increase_lvgl_tick(void *arg)
{
	/* Tell LVGL how many milliseconds has elapsed */
	lv_tick_inc(LCD_TICK_PERIOD_MS);
}
static void lvglTimerTask(void* param)
{
	ESP_LOGI(TAG, "Starting LVGL task");
	while (1)
	{
		if (!ui_is_ready) continue;
		uint32_t task_delay_ms = lv_timer_handler();
		if (task_delay_ms > 500)
		{
			task_delay_ms = 500;
		}
		else if (task_delay_ms < 5)
		{
			task_delay_ms = 5;
		}
		vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
	}
}

void InitLCDAndLVGL()
{
	static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
	static lv_disp_drv_t disp_drv; // contains callback functions
	//GPIO configuration
	ESP_LOGI(TAG, "Turn off LCD backlight");
	gpio_config_t pwr_gpio_config =
	{
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = 1ULL << LCD_PIN_PWR 
	};
	ESP_ERROR_CHECK(gpio_config(&pwr_gpio_config));
	gpio_set_level(LCD_PIN_PWR, LCD_BK_LIGHT_ON_LEVEL);

	gpio_config_t input_conf =
	{
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pin_bit_mask = 1ULL << LCD_PIN_RD 
	};
	ESP_ERROR_CHECK(gpio_config(&input_conf));

	gpio_config_t bk_gpio_config =
	{
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = 1ULL << LCD_PIN_NUM_BK_LIGHT 
	};
	ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
	gpio_set_level(LCD_PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

	ESP_LOGI(TAG, "Initialize Intel 8080 bus");
	esp_lcd_i80_bus_handle_t i80_bus = NULL;
	esp_lcd_i80_bus_config_t bus_config = {
		.clk_src = LCD_CLK_SRC_DEFAULT,
		.dc_gpio_num = LCD_PIN_NUM_DC,
		.wr_gpio_num = LCD_PIN_NUM_PCLK,
		.data_gpio_nums = {
		LCD_PIN_NUM_DATA0,
		LCD_PIN_NUM_DATA1,
		LCD_PIN_NUM_DATA2,
		LCD_PIN_NUM_DATA3,
		LCD_PIN_NUM_DATA4,
		LCD_PIN_NUM_DATA5,
		LCD_PIN_NUM_DATA6,
		LCD_PIN_NUM_DATA7,
	},
		.bus_width = 8,
		.max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t)
		//.psram_trans_align = EXAMPLE_PSRAM_DATA_ALIGNMENT,
		//.sram_trans_align = 4,
	};
	ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_i80_config_t io_config = {
		.cs_gpio_num = LCD_PIN_NUM_CS,
		.pclk_hz = LCD_PIXEL_CLOCK_HZ,
		.trans_queue_depth = 20,
		.dc_levels = {
		.dc_idle_level = 0,
		.dc_cmd_level = 0,
		.dc_dummy_level = 0,
		.dc_data_level = 1,
	},

		.on_color_trans_done = example_notify_lvgl_flush_ready,
		.user_ctx = &disp_drv,
		.lcd_cmd_bits = LCD_CMD_BITS,
		.lcd_param_bits = LCD_PARAM_BITS,
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

	esp_lcd_panel_handle_t panel_handle = NULL;

	ESP_LOGI(TAG, "Install LCD driver of st7789");
	esp_lcd_panel_dev_config_t panel_config = {
		.reset_gpio_num = LCD_PIN_NUM_RST,
		.rgb_endian = ESP_LCD_COLOR_SPACE_RGB,
		.bits_per_pixel = 16,
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

	esp_lcd_panel_reset(panel_handle);
	esp_lcd_panel_init(panel_handle);
	esp_lcd_panel_invert_color(panel_handle, true);

	esp_lcd_panel_swap_xy(panel_handle, false);

	// WITHOUT 180 DEGREES ROTATION
	// esp_lcd_panel_mirror(panel_handle, true, false); //Y AXIS SHIFTED BUT X AXIS OK
	//esp_lcd_panel_mirror(panel_handle, true, true); //Y AXIS SHIFTED AND X AXIS SHIFTED
	//esp_lcd_panel_mirror(panel_handle, false, false); //Everything is ok but its upside down
	//esp_lcd_panel_mirror(panel_handle, false, true); // Not upside down anymore but now the X axis is inverted

	// the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
	esp_lcd_panel_set_gap(panel_handle, 35, 0);

	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

	ESP_LOGI(TAG, "Initialize LVGL library");
	lv_init();
	// alloc draw buffers used by LVGL
	// it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
	lv_color_t *buf1 = heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	assert(buf1);
	//    lv_color_t *buf2 = heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA );
	//    assert(buf2);
	    // initialize LVGL draw buffers
	lv_disp_draw_buf_init(&disp_buf, buf1, NULL, LVGL_LCD_BUF_SIZE);

	ESP_LOGI(TAG, "Register display driver to LVGL");
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = LCD_H_RES;
	disp_drv.ver_res = LCD_V_RES;
	disp_drv.flush_cb = LCD_flush_cb;
	disp_drv.draw_buf = &disp_buf;
	disp_drv.user_data = panel_handle;
	// disp_drv.sw_rotate = 1;
	lv_disp_drv_register(&disp_drv);

	//Configuration is completed.


	ESP_LOGI(TAG, "Install LVGL tick timer");
	// Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
	const esp_timer_create_args_t lvgl_tick_timer_args = {
		.callback = &example_increase_lvgl_tick,
		.name = "lvgl_tick"
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LCD_TICK_PERIOD_MS * 1000));

	ESP_LOGI(TAG, "Display LVGL animation");
	
	xTaskCreatePinnedToCore(lvglTimerTask, "lvgl Timer", 10000, NULL, 4, NULL, 1);
}

