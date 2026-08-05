/*
 * Copyright 2026 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
//#include "custom.h"


void setup_scr_screen_1(lv_ui *ui){

	//Write codes screen_1
	ui->screen_1 = lv_obj_create(NULL, NULL);

	//Write style LV_OBJ_PART_MAIN for screen_1
	static lv_style_t style_screen_1_main;
	lv_style_reset(&style_screen_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_main
	lv_style_set_bg_color(&style_screen_1_main, LV_STATE_DEFAULT, lv_color_make(0xb6, 0xb9, 0x0e));
	lv_style_set_bg_opa(&style_screen_1_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1, LV_OBJ_PART_MAIN, &style_screen_1_main);

	//Write codes screen_1_label_1
	ui->screen_1_label_1 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_1, "手动控制");
	lv_label_set_long_mode(ui->screen_1_label_1, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_1, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_1
	static lv_style_t style_screen_1_label_1_main;
	lv_style_reset(&style_screen_1_label_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_1_main
	lv_style_set_radius(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_1_main, LV_STATE_DEFAULT, &lv_font_simkai_40);
	lv_style_set_text_letter_space(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_1_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_1, LV_LABEL_PART_MAIN, &style_screen_1_label_1_main);
	lv_obj_set_pos(ui->screen_1_label_1, 287, 16);
	lv_obj_set_size(ui->screen_1_label_1, 300, 0);

	//Write codes screen_1_label_2
	ui->screen_1_label_2 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_2, "爪子");
	lv_label_set_long_mode(ui->screen_1_label_2, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_2, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_2
	static lv_style_t style_screen_1_label_2_main;
	lv_style_reset(&style_screen_1_label_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_2_main
	lv_style_set_radius(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_2_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_2_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_2, LV_LABEL_PART_MAIN, &style_screen_1_label_2_main);
	lv_obj_set_pos(ui->screen_1_label_2, 310, 80);
	lv_obj_set_size(ui->screen_1_label_2, 100, 0);

	//Write codes screen_1_slider_1
	ui->screen_1_slider_1 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_1
	static lv_style_t style_screen_1_slider_1_indic;
	lv_style_reset(&style_screen_1_slider_1_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_1_indic
	lv_style_set_radius(&style_screen_1_slider_1_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_1_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_1_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_1_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_1_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_1, LV_SLIDER_PART_INDIC, &style_screen_1_slider_1_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_1
	static lv_style_t style_screen_1_slider_1_bg;
	lv_style_reset(&style_screen_1_slider_1_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_1_bg
	lv_style_set_radius(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_1_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_1, LV_SLIDER_PART_BG, &style_screen_1_slider_1_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_1
	static lv_style_t style_screen_1_slider_1_knob;
	lv_style_reset(&style_screen_1_slider_1_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_1_knob
	lv_style_set_radius(&style_screen_1_slider_1_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_1_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_1_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_1_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_1_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_1, LV_SLIDER_PART_KNOB, &style_screen_1_slider_1_knob);
	lv_obj_set_pos(ui->screen_1_slider_1, 50, 80);
	lv_obj_set_size(ui->screen_1_slider_1, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_1,0, 100);
	lv_slider_set_value(ui->screen_1_slider_1,50,false);

	//Write codes screen_1_slider_2
	ui->screen_1_slider_2 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_2
	static lv_style_t style_screen_1_slider_2_indic;
	lv_style_reset(&style_screen_1_slider_2_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_2_indic
	lv_style_set_radius(&style_screen_1_slider_2_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_2_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_2_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_2_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_2_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_2, LV_SLIDER_PART_INDIC, &style_screen_1_slider_2_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_2
	static lv_style_t style_screen_1_slider_2_bg;
	lv_style_reset(&style_screen_1_slider_2_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_2_bg
	lv_style_set_radius(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_2_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_2, LV_SLIDER_PART_BG, &style_screen_1_slider_2_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_2
	static lv_style_t style_screen_1_slider_2_knob;
	lv_style_reset(&style_screen_1_slider_2_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_2_knob
	lv_style_set_radius(&style_screen_1_slider_2_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_2_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_2_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_2_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_2_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_2, LV_SLIDER_PART_KNOB, &style_screen_1_slider_2_knob);
	lv_obj_set_pos(ui->screen_1_slider_2, 50, 150);
	lv_obj_set_size(ui->screen_1_slider_2, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_2,0, 100);
	lv_slider_set_value(ui->screen_1_slider_2,50,false);

	//Write codes screen_1_slider_3
	ui->screen_1_slider_3 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_3
	static lv_style_t style_screen_1_slider_3_indic;
	lv_style_reset(&style_screen_1_slider_3_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_3_indic
	lv_style_set_radius(&style_screen_1_slider_3_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_3_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_3_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_3_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_3_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_3, LV_SLIDER_PART_INDIC, &style_screen_1_slider_3_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_3
	static lv_style_t style_screen_1_slider_3_bg;
	lv_style_reset(&style_screen_1_slider_3_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_3_bg
	lv_style_set_radius(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_3_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_3, LV_SLIDER_PART_BG, &style_screen_1_slider_3_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_3
	static lv_style_t style_screen_1_slider_3_knob;
	lv_style_reset(&style_screen_1_slider_3_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_3_knob
	lv_style_set_radius(&style_screen_1_slider_3_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_3_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_3_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_3_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_3_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_3, LV_SLIDER_PART_KNOB, &style_screen_1_slider_3_knob);
	lv_obj_set_pos(ui->screen_1_slider_3, 50, 220);
	lv_obj_set_size(ui->screen_1_slider_3, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_3,0, 100);
	lv_slider_set_value(ui->screen_1_slider_3,50,false);

	//Write codes screen_1_slider_4
	ui->screen_1_slider_4 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_4
	static lv_style_t style_screen_1_slider_4_indic;
	lv_style_reset(&style_screen_1_slider_4_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_4_indic
	lv_style_set_radius(&style_screen_1_slider_4_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_4_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_4_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_4_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_4_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_4, LV_SLIDER_PART_INDIC, &style_screen_1_slider_4_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_4
	static lv_style_t style_screen_1_slider_4_bg;
	lv_style_reset(&style_screen_1_slider_4_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_4_bg
	lv_style_set_radius(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_4_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_4, LV_SLIDER_PART_BG, &style_screen_1_slider_4_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_4
	static lv_style_t style_screen_1_slider_4_knob;
	lv_style_reset(&style_screen_1_slider_4_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_4_knob
	lv_style_set_radius(&style_screen_1_slider_4_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_4_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_4_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_4_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_4_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_4, LV_SLIDER_PART_KNOB, &style_screen_1_slider_4_knob);
	lv_obj_set_pos(ui->screen_1_slider_4, 50, 290);
	lv_obj_set_size(ui->screen_1_slider_4, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_4,0, 100);
	lv_slider_set_value(ui->screen_1_slider_4,50,false);

	//Write codes screen_1_slider_5
	ui->screen_1_slider_5 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_5
	static lv_style_t style_screen_1_slider_5_indic;
	lv_style_reset(&style_screen_1_slider_5_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_5_indic
	lv_style_set_radius(&style_screen_1_slider_5_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_5_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_5_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_5_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_5_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_5, LV_SLIDER_PART_INDIC, &style_screen_1_slider_5_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_5
	static lv_style_t style_screen_1_slider_5_bg;
	lv_style_reset(&style_screen_1_slider_5_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_5_bg
	lv_style_set_radius(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_5_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_5, LV_SLIDER_PART_BG, &style_screen_1_slider_5_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_5
	static lv_style_t style_screen_1_slider_5_knob;
	lv_style_reset(&style_screen_1_slider_5_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_5_knob
	lv_style_set_radius(&style_screen_1_slider_5_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_5_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_5_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_5_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_5_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_5, LV_SLIDER_PART_KNOB, &style_screen_1_slider_5_knob);
	lv_obj_set_pos(ui->screen_1_slider_5, 50, 360);
	lv_obj_set_size(ui->screen_1_slider_5, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_5,0, 100);
	lv_slider_set_value(ui->screen_1_slider_5,50,false);

	//Write codes screen_1_slider_6
	ui->screen_1_slider_6 = lv_slider_create(ui->screen_1, NULL);

	//Write style LV_SLIDER_PART_INDIC for screen_1_slider_6
	static lv_style_t style_screen_1_slider_6_indic;
	lv_style_reset(&style_screen_1_slider_6_indic);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_6_indic
	lv_style_set_radius(&style_screen_1_slider_6_indic, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_6_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_6_indic, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_6_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_6_indic, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_6, LV_SLIDER_PART_INDIC, &style_screen_1_slider_6_indic);

	//Write style LV_SLIDER_PART_BG for screen_1_slider_6
	static lv_style_t style_screen_1_slider_6_bg;
	lv_style_reset(&style_screen_1_slider_6_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_6_bg
	lv_style_set_radius(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_color(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_left(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_slider_6_bg, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_slider_6, LV_SLIDER_PART_BG, &style_screen_1_slider_6_bg);

	//Write style LV_SLIDER_PART_KNOB for screen_1_slider_6
	static lv_style_t style_screen_1_slider_6_knob;
	lv_style_reset(&style_screen_1_slider_6_knob);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_slider_6_knob
	lv_style_set_radius(&style_screen_1_slider_6_knob, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_slider_6_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_color(&style_screen_1_slider_6_knob, LV_STATE_DEFAULT, lv_color_make(0x02, 0xa2, 0xb1));
	lv_style_set_bg_grad_dir(&style_screen_1_slider_6_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_slider_6_knob, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_slider_6, LV_SLIDER_PART_KNOB, &style_screen_1_slider_6_knob);
	lv_obj_set_pos(ui->screen_1_slider_6, 50, 430);
	lv_obj_set_size(ui->screen_1_slider_6, 240, 20);
	lv_slider_set_range(ui->screen_1_slider_6,0, 100);
	lv_slider_set_value(ui->screen_1_slider_6,50,false);

	//Write codes screen_1_label_3
	ui->screen_1_label_3 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_3, "舵机2");
	lv_label_set_long_mode(ui->screen_1_label_3, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_3, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_3
	static lv_style_t style_screen_1_label_3_main;
	lv_style_reset(&style_screen_1_label_3_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_3_main
	lv_style_set_radius(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_3_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_3_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_3_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_3, LV_LABEL_PART_MAIN, &style_screen_1_label_3_main);
	lv_obj_set_pos(ui->screen_1_label_3, 310, 150);
	lv_obj_set_size(ui->screen_1_label_3, 100, 0);

	//Write codes screen_1_label_4
	ui->screen_1_label_4 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_4, "舵机3");
	lv_label_set_long_mode(ui->screen_1_label_4, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_4, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_4
	static lv_style_t style_screen_1_label_4_main;
	lv_style_reset(&style_screen_1_label_4_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_4_main
	lv_style_set_radius(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_4_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_4_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_4_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_4_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_4_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_4_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_4, LV_LABEL_PART_MAIN, &style_screen_1_label_4_main);
	lv_obj_set_pos(ui->screen_1_label_4, 310, 220);
	lv_obj_set_size(ui->screen_1_label_4, 100, 0);

	//Write codes screen_1_label_5
	ui->screen_1_label_5 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_5, "舵机4");
	lv_label_set_long_mode(ui->screen_1_label_5, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_5, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_5
	static lv_style_t style_screen_1_label_5_main;
	lv_style_reset(&style_screen_1_label_5_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_5_main
	lv_style_set_radius(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_5_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_5_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_5_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_5_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_5_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_5_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_5, LV_LABEL_PART_MAIN, &style_screen_1_label_5_main);
	lv_obj_set_pos(ui->screen_1_label_5, 310, 290);
	lv_obj_set_size(ui->screen_1_label_5, 100, 0);

	//Write codes screen_1_label_6
	ui->screen_1_label_6 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_6, "舵机5");
	lv_label_set_long_mode(ui->screen_1_label_6, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_6, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_6
	static lv_style_t style_screen_1_label_6_main;
	lv_style_reset(&style_screen_1_label_6_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_6_main
	lv_style_set_radius(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_6_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_6_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_6_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_6_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_6_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_6_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_6, LV_LABEL_PART_MAIN, &style_screen_1_label_6_main);
	lv_obj_set_pos(ui->screen_1_label_6, 310, 360);
	lv_obj_set_size(ui->screen_1_label_6, 100, 0);

	//Write codes screen_1_label_7
	ui->screen_1_label_7 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_7, "底座");
	lv_label_set_long_mode(ui->screen_1_label_7, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_7, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_7
	static lv_style_t style_screen_1_label_7_main;
	lv_style_reset(&style_screen_1_label_7_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_7_main
	lv_style_set_radius(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_7_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_7_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_7_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_7_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_7_main, LV_STATE_DEFAULT, &lv_font_simkai_25);
	lv_style_set_text_letter_space(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_7_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_7, LV_LABEL_PART_MAIN, &style_screen_1_label_7_main);
	lv_obj_set_pos(ui->screen_1_label_7, 310, 430);
	lv_obj_set_size(ui->screen_1_label_7, 100, 0);

	//Write codes screen_1_btn_1
	ui->screen_1_btn_1 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_1
	static lv_style_t style_screen_1_btn_1_main;
	lv_style_reset(&style_screen_1_btn_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_1_main
	lv_style_set_radius(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0x17, 0xde, 0x24));
	lv_style_set_bg_grad_color(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0x5c, 0xea, 0x1a));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_1, LV_BTN_PART_MAIN, &style_screen_1_btn_1_main);
	lv_obj_set_pos(ui->screen_1_btn_1, 570, 200);
	lv_obj_set_size(ui->screen_1_btn_1, 100, 50);
	ui->screen_1_btn_1_label = lv_label_create(ui->screen_1_btn_1, NULL);
	lv_label_set_text(ui->screen_1_btn_1_label, "保存");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_btn_2
	ui->screen_1_btn_2 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_2
	static lv_style_t style_screen_1_btn_2_main;
	lv_style_reset(&style_screen_1_btn_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_2_main
	lv_style_set_radius(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0xe9, 0x07, 0x07));
	lv_style_set_bg_grad_color(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_2, LV_BTN_PART_MAIN, &style_screen_1_btn_2_main);
	lv_obj_set_pos(ui->screen_1_btn_2, 679, 21);
	lv_obj_set_size(ui->screen_1_btn_2, 100, 50);
	ui->screen_1_btn_2_label = lv_label_create(ui->screen_1_btn_2, NULL);
	lv_label_set_text(ui->screen_1_btn_2_label, "返回");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_btn_3
	ui->screen_1_btn_3 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_3
	static lv_style_t style_screen_1_btn_3_main;
	lv_style_reset(&style_screen_1_btn_3_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_3_main
	lv_style_set_radius(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0x16, 0xd4, 0xbe));
	lv_style_set_bg_grad_color(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_3, LV_BTN_PART_MAIN, &style_screen_1_btn_3_main);
	lv_obj_set_pos(ui->screen_1_btn_3, 570, 100);
	lv_obj_set_size(ui->screen_1_btn_3, 100, 50);
	ui->screen_1_btn_3_label = lv_label_create(ui->screen_1_btn_3, NULL);
	lv_label_set_text(ui->screen_1_btn_3_label, "自动执行");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_3_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_3_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_ddlist_1
	ui->screen_1_ddlist_1 = lv_dropdown_create(ui->screen_1, NULL);
	lv_dropdown_set_options(ui->screen_1_ddlist_1, "list1\nlist2\nlist3\nlist4\nlist5\nlist6\nlist7\nlist8");

	//Write style LV_DROPDOWN_PART_MAIN for screen_1_ddlist_1
	static lv_style_t style_screen_1_ddlist_1_main;
	lv_style_reset(&style_screen_1_ddlist_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_ddlist_1_main
	lv_style_set_radius(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, 3);
	lv_style_set_bg_color(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, lv_color_make(0xe1, 0xe6, 0xee));
	lv_style_set_border_width(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, 1);
	lv_style_set_text_color(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_line_space(&style_screen_1_ddlist_1_main, LV_STATE_DEFAULT, 20);
	lv_obj_add_style(ui->screen_1_ddlist_1, LV_DROPDOWN_PART_MAIN, &style_screen_1_ddlist_1_main);

	//Write style LV_DROPDOWN_PART_SELECTED for screen_1_ddlist_1
	static lv_style_t style_screen_1_ddlist_1_selected;
	lv_style_reset(&style_screen_1_ddlist_1_selected);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_ddlist_1_selected
	lv_style_set_radius(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, 3);
	lv_style_set_bg_color(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
	lv_style_set_bg_grad_color(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
	lv_style_set_bg_grad_dir(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, lv_color_make(0xe1, 0xe6, 0xee));
	lv_style_set_border_width(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, 1);
	lv_style_set_text_color(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_1_ddlist_1_selected, LV_STATE_DEFAULT, &lv_font_simsun_12);
	lv_obj_add_style(ui->screen_1_ddlist_1, LV_DROPDOWN_PART_SELECTED, &style_screen_1_ddlist_1_selected);

	//Write style LV_DROPDOWN_PART_LIST for screen_1_ddlist_1
	static lv_style_t style_screen_1_ddlist_1_list;
	lv_style_reset(&style_screen_1_ddlist_1_list);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_ddlist_1_list
	lv_style_set_radius(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, 3);
	lv_style_set_bg_color(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, lv_color_make(0xe1, 0xe6, 0xee));
	lv_style_set_border_width(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, 1);
	lv_style_set_text_color(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_screen_1_ddlist_1_list, LV_STATE_DEFAULT, &lv_font_simsun_12);
	lv_obj_add_style(ui->screen_1_ddlist_1, LV_DROPDOWN_PART_LIST, &style_screen_1_ddlist_1_list);
	lv_obj_set_pos(ui->screen_1_ddlist_1, 680, 100);
	lv_obj_set_width(ui->screen_1_ddlist_1, 100);

	//Write codes screen_1_btn_4
	ui->screen_1_btn_4 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_4
	static lv_style_t style_screen_1_btn_4_main;
	lv_style_reset(&style_screen_1_btn_4_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_4_main
	lv_style_set_radius(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, lv_color_make(0xf4, 0x06, 0x06));
	lv_style_set_bg_grad_color(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, lv_color_make(0xeb, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_4_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_4, LV_BTN_PART_MAIN, &style_screen_1_btn_4_main);
	lv_obj_set_pos(ui->screen_1_btn_4, 570, 300);
	lv_obj_set_size(ui->screen_1_btn_4, 100, 50);
	ui->screen_1_btn_4_label = lv_label_create(ui->screen_1_btn_4, NULL);
	lv_label_set_text(ui->screen_1_btn_4_label, "删除");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_4_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_4_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_label_8
	ui->screen_1_label_8 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_8, "90");
	lv_label_set_long_mode(ui->screen_1_label_8, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_8, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_8
	static lv_style_t style_screen_1_label_8_main;
	lv_style_reset(&style_screen_1_label_8_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_8_main
	lv_style_set_radius(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_8_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_8_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_8_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_8_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_8_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_8_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_8, LV_LABEL_PART_MAIN, &style_screen_1_label_8_main);
	lv_obj_set_pos(ui->screen_1_label_8, 470, 80);
	lv_obj_set_size(ui->screen_1_label_8, 30, 0);

	//Write codes screen_1_label_9
	ui->screen_1_label_9 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_9, "90");
	lv_label_set_long_mode(ui->screen_1_label_9, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_9, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_9
	static lv_style_t style_screen_1_label_9_main;
	lv_style_reset(&style_screen_1_label_9_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_9_main
	lv_style_set_radius(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_9_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_9_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_9_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_9_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_9_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_9_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_9, LV_LABEL_PART_MAIN, &style_screen_1_label_9_main);
	lv_obj_set_pos(ui->screen_1_label_9, 470, 150);
	lv_obj_set_size(ui->screen_1_label_9, 30, 0);

	//Write codes screen_1_label_10
	ui->screen_1_label_10 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_10, "90");
	lv_label_set_long_mode(ui->screen_1_label_10, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_10, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_10
	static lv_style_t style_screen_1_label_10_main;
	lv_style_reset(&style_screen_1_label_10_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_10_main
	lv_style_set_radius(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_10_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_10_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_10_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_10_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_10_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_10_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_10, LV_LABEL_PART_MAIN, &style_screen_1_label_10_main);
	lv_obj_set_pos(ui->screen_1_label_10, 470, 220);
	lv_obj_set_size(ui->screen_1_label_10, 30, 0);

	//Write codes screen_1_label_11
	ui->screen_1_label_11 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_11, "90");
	lv_label_set_long_mode(ui->screen_1_label_11, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_11, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_11
	static lv_style_t style_screen_1_label_11_main;
	lv_style_reset(&style_screen_1_label_11_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_11_main
	lv_style_set_radius(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_11_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_11_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_11_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_11_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_11_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_11_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_11, LV_LABEL_PART_MAIN, &style_screen_1_label_11_main);
	lv_obj_set_pos(ui->screen_1_label_11, 470, 290);
	lv_obj_set_size(ui->screen_1_label_11, 30, 0);

	//Write codes screen_1_label_12
	ui->screen_1_label_12 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_12, "90");
	lv_label_set_long_mode(ui->screen_1_label_12, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_12, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_12
	static lv_style_t style_screen_1_label_12_main;
	lv_style_reset(&style_screen_1_label_12_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_12_main
	lv_style_set_radius(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_12_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_12_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_12_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_12_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_12_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_12_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_12, LV_LABEL_PART_MAIN, &style_screen_1_label_12_main);
	lv_obj_set_pos(ui->screen_1_label_12, 470, 360);
	lv_obj_set_size(ui->screen_1_label_12, 30, 0);

	//Write codes screen_1_btn_5
	ui->screen_1_btn_5 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_5
	static lv_style_t style_screen_1_btn_5_main;
	lv_style_reset(&style_screen_1_btn_5_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_5_main
	lv_style_set_radius(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, lv_color_make(0xf1, 0x04, 0x04));
	lv_style_set_bg_grad_color(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_5_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_5, LV_BTN_PART_MAIN, &style_screen_1_btn_5_main);
	lv_obj_set_pos(ui->screen_1_btn_5, 400, 80);
	lv_obj_set_size(ui->screen_1_btn_5, 60, 40);
	ui->screen_1_btn_5_label = lv_label_create(ui->screen_1_btn_5, NULL);
	lv_label_set_text(ui->screen_1_btn_5_label, "add");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_5_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_5_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_6
	ui->screen_1_btn_6 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_6
	static lv_style_t style_screen_1_btn_6_main;
	lv_style_reset(&style_screen_1_btn_6_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_6_main
	lv_style_set_radius(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, lv_color_make(0x13, 0x21, 0xe7));
	lv_style_set_bg_grad_color(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_6_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_6, LV_BTN_PART_MAIN, &style_screen_1_btn_6_main);
	lv_obj_set_pos(ui->screen_1_btn_6, 500, 80);
	lv_obj_set_size(ui->screen_1_btn_6, 60, 40);
	ui->screen_1_btn_6_label = lv_label_create(ui->screen_1_btn_6, NULL);
	lv_label_set_text(ui->screen_1_btn_6_label, "sub");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_6_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_6_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_7
	ui->screen_1_btn_7 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_7
	static lv_style_t style_screen_1_btn_7_main;
	lv_style_reset(&style_screen_1_btn_7_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_7_main
	lv_style_set_radius(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0x00, 0x00));
	lv_style_set_bg_grad_color(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_7_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_7, LV_BTN_PART_MAIN, &style_screen_1_btn_7_main);
	lv_obj_set_pos(ui->screen_1_btn_7, 400, 150);
	lv_obj_set_size(ui->screen_1_btn_7, 60, 40);
	ui->screen_1_btn_7_label = lv_label_create(ui->screen_1_btn_7, NULL);
	lv_label_set_text(ui->screen_1_btn_7_label, "add");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_7_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_7_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_8
	ui->screen_1_btn_8 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_8
	static lv_style_t style_screen_1_btn_8_main;
	lv_style_reset(&style_screen_1_btn_8_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_8_main
	lv_style_set_radius(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, lv_color_make(0x04, 0x00, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_8_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_8, LV_BTN_PART_MAIN, &style_screen_1_btn_8_main);
	lv_obj_set_pos(ui->screen_1_btn_8, 500, 150);
	lv_obj_set_size(ui->screen_1_btn_8, 60, 40);
	ui->screen_1_btn_8_label = lv_label_create(ui->screen_1_btn_8, NULL);
	lv_label_set_text(ui->screen_1_btn_8_label, "sub");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_8_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_8_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_9
	ui->screen_1_btn_9 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_9
	static lv_style_t style_screen_1_btn_9_main;
	lv_style_reset(&style_screen_1_btn_9_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_9_main
	lv_style_set_radius(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, lv_color_make(0xf5, 0x00, 0x00));
	lv_style_set_bg_grad_color(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_9_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_9, LV_BTN_PART_MAIN, &style_screen_1_btn_9_main);
	lv_obj_set_pos(ui->screen_1_btn_9, 400, 220);
	lv_obj_set_size(ui->screen_1_btn_9, 60, 40);
	ui->screen_1_btn_9_label = lv_label_create(ui->screen_1_btn_9, NULL);
	lv_label_set_text(ui->screen_1_btn_9_label, "add");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_9_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_9_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_10
	ui->screen_1_btn_10 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_10
	static lv_style_t style_screen_1_btn_10_main;
	lv_style_reset(&style_screen_1_btn_10_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_10_main
	lv_style_set_radius(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0x1e, 0xf9));
	lv_style_set_bg_grad_color(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_10_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_10, LV_BTN_PART_MAIN, &style_screen_1_btn_10_main);
	lv_obj_set_pos(ui->screen_1_btn_10, 500, 220);
	lv_obj_set_size(ui->screen_1_btn_10, 60, 40);
	ui->screen_1_btn_10_label = lv_label_create(ui->screen_1_btn_10, NULL);
	lv_label_set_text(ui->screen_1_btn_10_label, "sub");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_10_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_10_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_11
	ui->screen_1_btn_11 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_11
	static lv_style_t style_screen_1_btn_11_main;
	lv_style_reset(&style_screen_1_btn_11_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_11_main
	lv_style_set_radius(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0x00, 0x00));
	lv_style_set_bg_grad_color(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_11_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_11, LV_BTN_PART_MAIN, &style_screen_1_btn_11_main);
	lv_obj_set_pos(ui->screen_1_btn_11, 400, 290);
	lv_obj_set_size(ui->screen_1_btn_11, 60, 40);
	ui->screen_1_btn_11_label = lv_label_create(ui->screen_1_btn_11, NULL);
	lv_label_set_text(ui->screen_1_btn_11_label, "add");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_11_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_11_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_12
	ui->screen_1_btn_12 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_12
	static lv_style_t style_screen_1_btn_12_main;
	lv_style_reset(&style_screen_1_btn_12_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_12_main
	lv_style_set_radius(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, lv_color_make(0x09, 0x2f, 0xec));
	lv_style_set_bg_grad_color(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_12_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_12, LV_BTN_PART_MAIN, &style_screen_1_btn_12_main);
	lv_obj_set_pos(ui->screen_1_btn_12, 500, 290);
	lv_obj_set_size(ui->screen_1_btn_12, 60, 40);
	ui->screen_1_btn_12_label = lv_label_create(ui->screen_1_btn_12, NULL);
	lv_label_set_text(ui->screen_1_btn_12_label, "sub");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_12_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_12_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_13
	ui->screen_1_btn_13 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_13
	static lv_style_t style_screen_1_btn_13_main;
	lv_style_reset(&style_screen_1_btn_13_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_13_main
	lv_style_set_radius(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, lv_color_make(0xf4, 0x01, 0x01));
	lv_style_set_bg_grad_color(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_13_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_13, LV_BTN_PART_MAIN, &style_screen_1_btn_13_main);
	lv_obj_set_pos(ui->screen_1_btn_13, 400, 360);
	lv_obj_set_size(ui->screen_1_btn_13, 60, 40);
	ui->screen_1_btn_13_label = lv_label_create(ui->screen_1_btn_13, NULL);
	lv_label_set_text(ui->screen_1_btn_13_label, "add");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_13_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_13_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_14
	ui->screen_1_btn_14 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_14
	static lv_style_t style_screen_1_btn_14_main;
	lv_style_reset(&style_screen_1_btn_14_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_14_main
	lv_style_set_radius(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0x3d, 0xef));
	lv_style_set_bg_grad_color(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_14_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_14, LV_BTN_PART_MAIN, &style_screen_1_btn_14_main);
	lv_obj_set_pos(ui->screen_1_btn_14, 500, 360);
	lv_obj_set_size(ui->screen_1_btn_14, 60, 40);
	ui->screen_1_btn_14_label = lv_label_create(ui->screen_1_btn_14, NULL);
	lv_label_set_text(ui->screen_1_btn_14_label, "sub");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_14_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_14_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_1_btn_15
	ui->screen_1_btn_15 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_15
	static lv_style_t style_screen_1_btn_15_main;
	lv_style_reset(&style_screen_1_btn_15_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_15_main
	lv_style_set_radius(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0x00, 0x00));
	lv_style_set_bg_grad_color(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_15_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_15, LV_BTN_PART_MAIN, &style_screen_1_btn_15_main);
	lv_obj_set_pos(ui->screen_1_btn_15, 400, 430);
	lv_obj_set_size(ui->screen_1_btn_15, 60, 40);
	ui->screen_1_btn_15_label = lv_label_create(ui->screen_1_btn_15, NULL);
	lv_label_set_text(ui->screen_1_btn_15_label, "前进");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_15_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_15_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_btn_16
	ui->screen_1_btn_16 = lv_btn_create(ui->screen_1, NULL);

	//Write style LV_BTN_PART_MAIN for screen_1_btn_16
	static lv_style_t style_screen_1_btn_16_main;
	lv_style_reset(&style_screen_1_btn_16_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_btn_16_main
	lv_style_set_radius(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, lv_color_make(0x16, 0x06, 0xf4));
	lv_style_set_bg_grad_color(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_1_btn_16_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_1_btn_16, LV_BTN_PART_MAIN, &style_screen_1_btn_16_main);
	lv_obj_set_pos(ui->screen_1_btn_16, 480, 430);
	lv_obj_set_size(ui->screen_1_btn_16, 60, 40);
	ui->screen_1_btn_16_label = lv_label_create(ui->screen_1_btn_16, NULL);
	lv_label_set_text(ui->screen_1_btn_16_label, "后退");
	lv_obj_set_style_local_text_color(ui->screen_1_btn_16_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_1_btn_16_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_20);

	//Write codes screen_1_label_13
	ui->screen_1_label_13 = lv_label_create(ui->screen_1, NULL);
	lv_label_set_text(ui->screen_1_label_13, "0");
	lv_label_set_long_mode(ui->screen_1_label_13, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_1_label_13, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_1_label_13
	static lv_style_t style_screen_1_label_13_main;
	lv_style_reset(&style_screen_1_label_13_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_1_label_13_main
	lv_style_set_radius(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_1_label_13_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_1_label_13_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_1_label_13_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_1_label_13_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_1_label_13_main, LV_STATE_DEFAULT, &lv_font_simsun_30);
	lv_style_set_text_letter_space(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_1_label_13_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_1_label_13, LV_LABEL_PART_MAIN, &style_screen_1_label_13_main);
	lv_obj_set_pos(ui->screen_1_label_13, 570, 430);
	lv_obj_set_size(ui->screen_1_label_13, 100, 0);

	//Init events for screen
	events_init_screen_1(ui);
}
