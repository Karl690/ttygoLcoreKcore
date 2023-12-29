#include <stdio.h>
#include "ui.h"
#include "K_Core/display/DisplayList.h"

lv_obj_t* ui_variables_screen;
lv_obj_t* title;

lv_obj_t* ui_variables_vars_panels[DISPLAY_VAR_LIST_SIZE];
lv_obj_t* ui_variables_selected_panel = NULL;
DISPLAY_VAR_TYPE ui_variables_selected_index;

uint8_t ui_variable_temp_string[256] = { 0 };
void ui_variables_refresh_list()
{
	if (!ui_variables_selected_panel) return;
	DisplayVariableInfo* ui_info = (DisplayVariableInfo*)ui_variables_selected_panel->user_data;
	if (!ui_info) return;
	uint8_t i = 0;
	lv_obj_t* obj = NULL;
	while (ui_info[i].VariablePointer != 0)
	{	
		if (!ui_info[i].lv_object)
		{
			i++;
			continue;
		}
		obj = ui_info[i].lv_object;
		switch (ui_info[i].FuncType)
		{	
		case FUNC_TITLE:
			lv_label_set_text(obj, ui_info[i].Label);
			break;
		case FUNC_INT:
			lv_label_set_text_fmt(obj, "%d", *(int*)ui_info[i].VariablePointer); //
			break;
		case FUNC_INT16:
			lv_label_set_text_fmt(obj, "%d", (int)*(uint16_t*)ui_info[i].VariablePointer); //
			break;
		case FUNC_INT32:				
			lv_label_set_text_fmt(obj, "%d", (int)*(uint32_t*)ui_info[i].VariablePointer); //
			break;
		case FUNC_FLOAT:
			lv_label_set_text_fmt(obj, "%.3f", *(float*)ui_info[i].VariablePointer); //
			break;
		case FUNC_HEX8:
		case FUNC_HEX16:
		case FUNC_HEX32:
			lv_label_set_text_fmt(obj, "0x%X", *(int*)ui_info[i].VariablePointer); //
			break;
		case FUNC_ASCII:
			lv_label_set_text_fmt(obj, "%s", (char*)ui_info[i].VariablePointer); //
			break;
		case FUNC_BOOLEAN:
			lv_label_set_text_fmt(obj, "%s", *(bool*)ui_info[i].VariablePointer ? "TRUE" : "FALSE"); //
			break;
		case FUNC_BAR_STATUS: {
			char** values = (char**)ui_info[i].Offset;
			lv_label_set_text_fmt(obj, "%s", values[*(uint8_t*)ui_info[i].VariablePointer]); //*(uint8_t*)ui_info[i].VariablePointer]
			break;
			}
		case FUNC_MEMDISPASCIIHEX: {
			uint8_t* MemoryDumpPointer = (uint8_t*)ui_info[i].VariablePointer;
			char* temp = (char*)ui_variable_temp_string;
			char workChar;
			char workstring[9] = "        "; //dummy empty string
			memset(ui_variable_temp_string, 0, 40);
			for (int count = 0; count < 5; count++)
			{
				//if (*MemoryDumpPointer > 0x20)
				workChar = *(MemoryDumpPointer + count);
				if ((workChar <= '~')&&(workChar >= ' '))
				{
					workstring[count] = workChar;	
				}
				else
				{
					workstring[count] = '-';	
				}
				sprintf(temp, "%02X ", workChar);
				temp += 3;
		
			}
			strcat((char*)ui_variable_temp_string, "  ");
			strcat((char*)ui_variable_temp_string, workstring);
			lv_label_set_text_fmt(obj, "%s", ui_variable_temp_string);
			}
			break;
		default:
			break;
		}
		i++;	
	}
}

lv_obj_t* ui_variables_update_list(void* obj)
{
	if (!obj) return NULL;
	DisplayVariableInfo* ui_info = (DisplayVariableInfo*) obj;
	uint8_t i = 0;
	lv_obj_t* item;
	lv_obj_t* label;
	lv_obj_t* value = NULL;
	lv_obj_t* panel = lv_obj_create(ui_variables_screen);
	lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE); /// Flags	
	lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_size(panel, SCREEN_WIDTH, SCREEN_HEIGHT);
	lv_obj_set_style_bg_color(panel, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN);
	lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_style_pad_all(panel, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
	
	panel->user_data = obj;
	uint32_t x = 0, y = 0, line_height = 30;
	
	while (ui_info[i].VariablePointer != 0)
	{	
		if (ui_info[i].FuncType == FUNC_TITLE)
		{
			line_height = 30;
			item = ui_create_titlebar(panel, TITLEBAR_BACKGROUND_COLOR);
			label = lv_label_create(item);	
			lv_obj_set_size(item, LV_PCT(100), 30);			
			lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);	
			lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);	
			lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
			lv_label_set_text(label, ui_info[i].Label);
		}
		else if (ui_info[i].FuncType == FUNC_LABEL)
		{
			line_height = 30;
			item = ui_create_titlebar(panel, ui_info[i].Color_1);
			lv_obj_set_style_pad_all(item, 2, LV_PART_MAIN);
			label = lv_label_create(item);	
			lv_obj_set_size(item, LV_PCT(50), 20);			
			lv_obj_set_style_text_color(label, lv_color_hex(ui_info[i].Color_2), LV_PART_MAIN);	
			lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);	
			lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
			lv_label_set_text(label, ui_info[i].Label);
		} else if (ui_info[i].FuncType == FUNC_MEMDISPASCIIHEX)
		{
			line_height = 20;
			item = lv_obj_create(panel);
			lv_obj_set_size(item, SCREEN_WIDTH, line_height);
			lv_obj_set_style_pad_all(item, 2, LV_PART_MAIN);
			value = lv_label_create(item);
			lv_obj_set_align(value, LV_ALIGN_LEFT_MID);			
			ui_info[i].lv_object = value;
			lv_label_set_recolor(value, true);	
		} else {
			line_height = 25;
			item = lv_obj_create(panel);
			lv_obj_set_size(item, LV_PCT(100), line_height);
			lv_obj_set_style_bg_color(item, lv_color_hex(0x0), LV_PART_MAIN);	
			lv_obj_set_style_border_width(item, 0, LV_PART_MAIN);		
			lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE); /// Flags
		
			label = lv_label_create(item);
			
			lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
			lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);		
			lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
			lv_label_set_text(label, ui_info[i].Label);
			
			value = lv_label_create(item);
			lv_obj_set_align(value, LV_ALIGN_RIGHT_MID);			
			ui_info[i].lv_object = value;
			lv_label_set_recolor(value, true);	
			switch (ui_info[i].FuncType)
			{
			case FUNC_INT:
				lv_label_set_text_fmt(value, "%d", *(int*)ui_info[i].VariablePointer); //
				break;
			case FUNC_INT16:
				lv_label_set_text_fmt(value, "%d", (int)*(uint16_t*)ui_info[i].VariablePointer); //
				break;
			case FUNC_INT32:				
				lv_label_set_text_fmt(value, "%d", (int)*(uint32_t*)ui_info[i].VariablePointer); //
				break;
			case FUNC_FLOAT:
				lv_label_set_text_fmt(value, "%.3f", *(float*)ui_info[i].VariablePointer); //
				break;
			case FUNC_HEX8:
			case FUNC_HEX16:
			case FUNC_HEX32:
				lv_label_set_text_fmt(value, "0x%X", *(int*)ui_info[i].VariablePointer); //
				break;
			case FUNC_ASCII:
				lv_label_set_text_fmt(value, "%s", (char*)ui_info[i].VariablePointer); //
				break;
			case FUNC_BOOLEAN:
				lv_label_set_text_fmt(value, "%s", *(bool*)ui_info[i].VariablePointer ? "TRUE" : "FALSE"); //
				break;
			default:
				break;
			}
			
			lv_obj_set_style_text_color(value, lv_color_hex(ui_info[i].Color_2), LV_PART_MAIN);
			lv_obj_set_style_text_font(value, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);		
		}
		lv_obj_set_pos(item, x, y);
		y += line_height;
		i++;	
	}
	return panel;
}
void ui_variables_refresh_timer_handler(lv_timer_t* timer)
{
	ui_variables_refresh_list();
}
void ui_variables_screen_init(void)
{
	ui_variables_screen = ui_create_screen();
	
	ui_variables_vars_panels[DISPLAY_VAR_SYSTEM] = ui_variables_update_list(GeneralVarsTable);
	ui_variables_vars_panels[DISPLAY_VAR_BLUETOOTH] = ui_variables_update_list(BluetoothVarsTable);
	ui_variables_vars_panels[DISPLAY_VAR_ADC] = ui_variables_update_list(AdcVarsTable);

	ui_variables_selected_panel = ui_variables_vars_panels[DISPLAY_VAR_SYSTEM];
	ui_variables_selected_index = DISPLAY_VAR_SYSTEM;
	lv_obj_clear_flag(ui_variables_selected_panel, LV_OBJ_FLAG_HIDDEN);
	lv_timer_create(ui_variables_refresh_timer_handler, 100, NULL);
}


void ui_variables_change_panel()
{
	if (ui_variables_selected_panel)
	{
		lv_obj_add_flag(ui_variables_selected_panel, LV_OBJ_FLAG_HIDDEN);
	}
	if (ui_variables_selected_index + 1 >= DISPLAY_VAR_LIST_SIZE) ui_variables_selected_index = DISPLAY_VAR_SYSTEM;
	else ui_variables_selected_index++;
	ui_variables_selected_panel = ui_variables_vars_panels[ui_variables_selected_index];
	lv_obj_clear_flag(ui_variables_selected_panel, LV_OBJ_FLAG_HIDDEN);
}

