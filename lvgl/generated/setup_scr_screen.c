/*
 * Copyright 2026 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
//#include "custom.h"


void setup_scr_screen(lv_ui *ui){

	//Write codes screen
	ui->screen = lv_obj_create(NULL, NULL);

	//Write style LV_OBJ_PART_MAIN for screen
	static lv_style_t style_screen_main;
	lv_style_reset(&style_screen_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_main
	lv_style_set_bg_color(&style_screen_main, LV_STATE_DEFAULT, lv_color_make(0xe5, 0xd7, 0xd7));
	lv_style_set_bg_opa(&style_screen_main, LV_STATE_DEFAULT, 218);
	lv_obj_add_style(ui->screen, LV_OBJ_PART_MAIN, &style_screen_main);

	//Write codes screen_btn_1
	ui->screen_btn_1 = lv_btn_create(ui->screen, NULL);

	//Write style LV_BTN_PART_MAIN for screen_btn_1
	static lv_style_t style_screen_btn_1_main;
	lv_style_reset(&style_screen_btn_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_btn_1_main
	lv_style_set_radius(&style_screen_btn_1_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xce, 0xf2, 0x1c));
	lv_style_set_bg_grad_color(&style_screen_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_btn_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_btn_1_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_btn_1_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_btn_1_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_btn_1, LV_BTN_PART_MAIN, &style_screen_btn_1_main);
	lv_obj_set_pos(ui->screen_btn_1, 20, 300);
	lv_obj_set_size(ui->screen_btn_1, 100, 150);
	ui->screen_btn_1_label = lv_label_create(ui->screen_btn_1, NULL);
	lv_label_set_text(ui->screen_btn_1_label, "手动控制");
	lv_obj_set_style_local_text_color(ui->screen_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x23, 0xe6, 0x0a));
	lv_obj_set_style_local_text_font(ui->screen_btn_1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_btn_2
	ui->screen_btn_2 = lv_btn_create(ui->screen, NULL);

	//Write style LV_BTN_PART_MAIN for screen_btn_2
	static lv_style_t style_screen_btn_2_main;
	lv_style_reset(&style_screen_btn_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_btn_2_main
	lv_style_set_radius(&style_screen_btn_2_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0x2e, 0xb7, 0xe5));
	lv_style_set_bg_grad_color(&style_screen_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_btn_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_btn_2_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_btn_2_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_btn_2_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_btn_2, LV_BTN_PART_MAIN, &style_screen_btn_2_main);
	lv_obj_set_pos(ui->screen_btn_2, 350, 300);
	lv_obj_set_size(ui->screen_btn_2, 100, 150);
	ui->screen_btn_2_label = lv_label_create(ui->screen_btn_2, NULL);
	lv_label_set_text(ui->screen_btn_2_label, "自动控制");
	lv_obj_set_style_local_text_color(ui->screen_btn_2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x2a, 0xff));
	lv_obj_set_style_local_text_font(ui->screen_btn_2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simsun_20);

	//Write codes screen_btn_3
	ui->screen_btn_3 = lv_btn_create(ui->screen, NULL);

	//Write style LV_BTN_PART_MAIN for screen_btn_3
	static lv_style_t style_screen_btn_3_main;
	lv_style_reset(&style_screen_btn_3_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_btn_3_main
	lv_style_set_radius(&style_screen_btn_3_main, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_screen_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_btn_3_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_style_set_border_color(&style_screen_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
	lv_style_set_border_width(&style_screen_btn_3_main, LV_STATE_DEFAULT, 2);
	lv_style_set_border_opa(&style_screen_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_style_set_outline_color(&style_screen_btn_3_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
	lv_style_set_outline_opa(&style_screen_btn_3_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_btn_3, LV_BTN_PART_MAIN, &style_screen_btn_3_main);
	lv_obj_set_pos(ui->screen_btn_3, 680, 300);
	lv_obj_set_size(ui->screen_btn_3, 100, 150);
	ui->screen_btn_3_label = lv_label_create(ui->screen_btn_3, NULL);
	lv_label_set_text(ui->screen_btn_3_label, "帮助");
	lv_obj_set_style_local_text_color(ui->screen_btn_3_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xf5, 0x00, 0x00));
	lv_obj_set_style_local_text_font(ui->screen_btn_3_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_simkai_27);

	//Write codes screen_label_1
	ui->screen_label_1 = lv_label_create(ui->screen, NULL);
	lv_label_set_text(ui->screen_label_1, "23时00分00秒 ");
	lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_label_1, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_label_1
	static lv_style_t style_screen_label_1_main;
	lv_style_reset(&style_screen_label_1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_1_main
	lv_style_set_radius(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xb7, 0x24, 0xe0));
	lv_style_set_bg_grad_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_1_main, LV_STATE_DEFAULT, &lv_font_simkai_40);
	lv_style_set_text_letter_space(&style_screen_label_1_main, LV_STATE_DEFAULT, 10);
	lv_style_set_pad_left(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_label_1, LV_LABEL_PART_MAIN, &style_screen_label_1_main);
	lv_obj_set_pos(ui->screen_label_1, 30, 130);
	lv_obj_set_size(ui->screen_label_1, 740, 0);

	//Write codes screen_label_2
	ui->screen_label_2 = lv_label_create(ui->screen, NULL);
	lv_label_set_text(ui->screen_label_2, "2025年11月00日 ");
	lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_label_2, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_label_2
	static lv_style_t style_screen_label_2_main;
	lv_style_reset(&style_screen_label_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_2_main
	lv_style_set_radius(&style_screen_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xc3, 0x10, 0xda));
	lv_style_set_bg_grad_color(&style_screen_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xce, 0xc8, 0x22));
	lv_style_set_bg_grad_dir(&style_screen_label_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_screen_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_2_main, LV_STATE_DEFAULT, &lv_font_simkai_40);
	lv_style_set_text_letter_space(&style_screen_label_2_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_label_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_label_2_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_label_2, LV_LABEL_PART_MAIN, &style_screen_label_2_main);
	lv_obj_set_pos(ui->screen_label_2, 30, 60);
	lv_obj_set_size(ui->screen_label_2, 740, 0);

	//Write codes screen_label_3
	ui->screen_label_3 = lv_label_create(ui->screen, NULL);
	lv_label_set_text(ui->screen_label_3, "FPS:000");
	lv_label_set_long_mode(ui->screen_label_3, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_label_3, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_label_3
	static lv_style_t style_screen_label_3_main;
	lv_style_reset(&style_screen_label_3_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_3_main
	lv_style_set_radius(&style_screen_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_label_3_main, LV_STATE_DEFAULT, lv_color_make(0x1a, 0xdb, 0xa4));
	lv_style_set_bg_grad_color(&style_screen_label_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_label_3_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_3_main, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_screen_label_3_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_3_main, LV_STATE_DEFAULT, &lv_font_simsun_20);
	lv_style_set_text_letter_space(&style_screen_label_3_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_label_3_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_label_3_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_label_3, LV_LABEL_PART_MAIN, &style_screen_label_3_main);
	lv_obj_set_pos(ui->screen_label_3, 673, 7);
	lv_obj_set_size(ui->screen_label_3, 100, 0);

	//Init events for screen
	events_init_screen(ui);
}
