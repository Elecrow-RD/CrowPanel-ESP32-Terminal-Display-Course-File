// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: chart_test

#include "ui.h"
lv_chart_series_t * ui_Chart1_series_1 = NULL;
lv_chart_series_t * ui_Chart1_series_2 = NULL;

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Chart1 = lv_chart_create(ui_Screen1);
    lv_obj_set_width(ui_Chart1, 388);
    lv_obj_set_height(ui_Chart1, 191);
    lv_obj_set_x(ui_Chart1, 4);
    lv_obj_set_y(ui_Chart1, 3);
    lv_obj_set_align(ui_Chart1, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_Chart1, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_Chart1, 24);
    lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
    lv_chart_set_div_line_count(ui_Chart1, 50, 24);
    lv_chart_set_axis_tick(ui_Chart1, LV_CHART_AXIS_PRIMARY_X, 10, 5, 24, 1, true, 50);
    lv_chart_set_axis_tick(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 25, 1, true, 50);
    lv_chart_set_axis_tick(ui_Chart1, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 25, 1, true, 25);
    ui_Chart1_series_1 = lv_chart_add_series(ui_Chart1, lv_color_hex(0xD63A3A),
                                                                 LV_CHART_AXIS_PRIMARY_Y);
    static lv_coord_t ui_Chart1_series_1_array[24] = { 0 };
    lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_1, ui_Chart1_series_1_array);
    ui_Chart1_series_2 = lv_chart_add_series(ui_Chart1, lv_color_hex(0x2D5FCD),
                                                                 LV_CHART_AXIS_SECONDARY_Y);
    static lv_coord_t ui_Chart1_series_2_array[24] = { 0 };
    lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_2, ui_Chart1_series_2_array);



    ui_Label1 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, -143);
    lv_obj_set_y(ui_Label1, -110);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "Temperature:");
    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0xD63C3C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label1, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label2 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label2, 134);
    lv_obj_set_y(ui_Label2, -110);
    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label2, "Humidity:");
    lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0x305ED6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

}
