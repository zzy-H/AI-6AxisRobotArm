/*
 * Copyright 2026 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
//#include "custom.h"


void setup_scr_screen_2(lv_ui *ui){

	//Write codes screen_2
	ui->screen_2 = lv_obj_create(NULL, NULL);

	//Write style LV_OBJ_PART_MAIN for screen_2
	static lv_style_t style_screen_2_main;
	lv_style_reset(&style_screen_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_2_main
	lv_style_set_bg_color(&style_screen_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_opa(&style_screen_2_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_2, LV_OBJ_PART_MAIN, &style_screen_2_main);

	//Write codes screen_2_label_1
	ui->screen_2_label_1 = lv_label_create(ui->screen_2, NULL);
	lv_label_set_text(ui->screen_2_label_1, "自动控制");
	lv_label_set_long_mode(ui->screen_2_label_1, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_2_label_1, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_2_label_1
	static lv_style_t style_screen_2_label_1_main;
	lv_style_reset(&style_screen_2_label_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_2_label_1_main
	lv_style_set_radius(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_2_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&style_screen_2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_2_label_1_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_2_label_1_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_2_label_1, LV_LABEL_PART_MAIN, &style_screen_2_label_1_main);
	lv_obj_set_pos(ui->screen_2_label_1, 70, 20);
	lv_obj_set_size(ui->screen_2_label_1, 100, 0);

	//Write codes screen_2_btn_1
	ui->screen_2_btn_1 = lv_btn_create(ui->screen_2, NULL);

	//Write style LV_BTN_PART_MAIN for screen_2_btn_1
	static lv_style_t style_screen_2_btn_1_main;
	lv_style_reset(&style_screen_2_btn_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_2_btn_1_main
	lv_style_set_radius(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_2_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_2_btn_1, LV_BTN_PART_MAIN, &style_screen_2_btn_1_main);
	lv_obj_set_pos(ui->screen_2_btn_1, 690, 10);
	lv_obj_set_size(ui->screen_2_btn_1, 100, 50);
	ui->screen_2_btn_1_label = lv_label_create(ui->screen_2_btn_1, NULL);
	lv_label_set_text(ui->screen_2_btn_1_label, "返回");
	lv_obj_set_style_local_text_color(ui->screen_2_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xf2, 0x07, 0x07));
	lv_obj_set_style_local_text_font(ui->screen_2_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Init events for screen
	events_init_screen_2(ui);
}