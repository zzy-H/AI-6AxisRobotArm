/*
 * Copyright 2026 NXP
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "guider_fonts.h"

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *screen_btn_1;
	lv_obj_t *screen_btn_1_label;
	lv_obj_t *screen_btn_2;
	lv_obj_t *screen_btn_2_label;
	lv_obj_t *screen_btn_3;
	lv_obj_t *screen_btn_3_label;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_label_2;
	lv_obj_t *screen_label_3;
	lv_obj_t *screen_1;
	lv_obj_t *screen_1_label_1;
	lv_obj_t *screen_1_label_2;
	lv_obj_t *screen_1_slider_1;
	lv_obj_t *screen_1_slider_2;
	lv_obj_t *screen_1_slider_3;
	lv_obj_t *screen_1_slider_4;
	lv_obj_t *screen_1_slider_5;
	lv_obj_t *screen_1_slider_6;
	lv_obj_t *screen_1_label_3;
	lv_obj_t *screen_1_label_4;
	lv_obj_t *screen_1_label_5;
	lv_obj_t *screen_1_label_6;
	lv_obj_t *screen_1_label_7;
	lv_obj_t *screen_1_btn_1;
	lv_obj_t *screen_1_btn_1_label;
	lv_obj_t *screen_1_btn_2;
	lv_obj_t *screen_1_btn_2_label;
	lv_obj_t *screen_1_btn_3;
	lv_obj_t *screen_1_btn_3_label;
	lv_obj_t *screen_1_ddlist_1;
	lv_obj_t *screen_1_btn_4;
	lv_obj_t *screen_1_btn_4_label;
	lv_obj_t *screen_1_label_8;
	lv_obj_t *screen_1_label_9;
	lv_obj_t *screen_1_label_10;
	lv_obj_t *screen_1_label_11;
	lv_obj_t *screen_1_label_12;
	lv_obj_t *screen_1_btn_5;
	lv_obj_t *screen_1_btn_5_label;
	lv_obj_t *screen_1_btn_6;
	lv_obj_t *screen_1_btn_6_label;
	lv_obj_t *screen_1_btn_7;
	lv_obj_t *screen_1_btn_7_label;
	lv_obj_t *screen_1_btn_8;
	lv_obj_t *screen_1_btn_8_label;
	lv_obj_t *screen_1_btn_9;
	lv_obj_t *screen_1_btn_9_label;
	lv_obj_t *screen_1_btn_10;
	lv_obj_t *screen_1_btn_10_label;
	lv_obj_t *screen_1_btn_11;
	lv_obj_t *screen_1_btn_11_label;
	lv_obj_t *screen_1_btn_12;
	lv_obj_t *screen_1_btn_12_label;
	lv_obj_t *screen_1_btn_13;
	lv_obj_t *screen_1_btn_13_label;
	lv_obj_t *screen_1_btn_14;
	lv_obj_t *screen_1_btn_14_label;
	lv_obj_t *screen_1_btn_15;
	lv_obj_t *screen_1_btn_15_label;
	lv_obj_t *screen_1_btn_16;
	lv_obj_t *screen_1_btn_16_label;
	lv_obj_t *screen_1_label_13;
	lv_obj_t *screen_2;
	lv_obj_t *screen_2_label_1;
	lv_obj_t *screen_2_btn_1;
	lv_obj_t *screen_2_btn_1_label;
}lv_ui;

void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_screen(lv_ui *ui);
void setup_scr_screen_1(lv_ui *ui);
void setup_scr_screen_2(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif

