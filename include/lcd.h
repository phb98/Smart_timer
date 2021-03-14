#ifndef _LCD_h
#define _LCD_h
#define MAX_MAIN_SCREEN 5
#define MAX_MENU_SCREEN 13
#define LCD_ADDR 0x27
#define LCD_UPDATE_INTERVAL 200
#define LCD_BACKLIGHT_TIMEOUT 30
#include <stdio.h>
#include <RTClib.h>
#include "main.h"
    void lcd_initialize();
    void lcd_main_screen(const uint8_t screen,const DateTime &cur_time, const timer_t*timer,const system_para_t* sys_para,uint8_t trigger_input);
    void lcd_RTC_init_fail();
    void lcd_INA_init_fail();
    void lcd_display();
    void lcd_RTC_first_init();
    void lcd_menu_screen(const char * title,uint8_t screen);
    void lcd_variable_setup_display(const char * title, uint8_t var,uint8_t b);
    void lcd_backlight_on();
    void lcd_backlight_off();
#endif