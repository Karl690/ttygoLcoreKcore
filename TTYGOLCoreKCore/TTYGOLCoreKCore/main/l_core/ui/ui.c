#include <lvgl.h>
#include "../lcd/lcd.h"
#include "ui_log.h"
#include "ui.h"
void ui_init(void)
{
	
	lv_obj_t *log_label = lv_label_create(lv_scr_act());
	lv_obj_align(log_label, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_width(log_label, LV_PCT(100));
	lv_label_set_long_mode(log_label, LV_LABEL_LONG_SCROLL);
	lv_label_set_recolor(log_label, true);
	lv_label_set_text(log_label, "Scan WiFi");

//	lv_label_set_text(log_label, "Scan WiFi");
//	lv_obj_t* obj = lv_obj_create(lv_scr_act());
//	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE); /// Flags
//	lv_obj_set_style_bg_color(obj, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
//	lv_obj_set_style_bg_opa(obj,
//		255,
//		LV_PART_MAIN | 
//	LV_STATE_DEFAULT);
//	//lv_scr_load_anim(ui_log_screen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);	
}