#include <lvgl.h>
#include "ui.h"
#include "ui_log.h"
lv_obj_t *ui_log_screen;

void ui_log_init()
{
	ui_log_screen = lv_obj_create(NULL) ;
	lv_obj_clear_flag(ui_log_screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_bg_color(ui_log_screen, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_log_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}
