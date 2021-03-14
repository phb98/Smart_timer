#include "lcd.h"
#include <LiquidCrystal_I2C.h>
#include "GPIO.h"
#include <RTClib.h>
#include <stdio.h>
#include "main.h"
#include <stdint.h>
LiquidCrystal_I2C lcd(LCD_ADDR,16,2);
char line0[17]; // buffer for lcd
char line1[17]; //buffer for lcd
void lcd_initialize(){
    lcd.init();
    lcd.backlight();
    sprintf(line0,"Light timer");
    sprintf(line1,"Ver 0.5 by PHB"); 
    lcd_display();
}
void lcd_main_screen(uint8_t screen,const DateTime &cur_time, const timer_t*timer,const system_para_t* sys_para,uint8_t trigger_input){
    lcd.clear();
    lcd.setCursor(0,0);
    switch(screen){ // which screen need to be displayed ?
        case 0:
            sprintf(line0,"    %02d:%02d:%02d",cur_time.hour(),cur_time.minute(),cur_time.second());
            sprintf(line1,"Set:%02d:%02d->%02d:%02d",timer->on_hour,timer->on_min,timer->off_hour,timer->off_min);
        break;
        case 1:
            sprintf(line1,"PWM Output:%d",timer->out_value);
            sprintf(line0,"Timer %s",(timer->state==ACTIVE? "Enabled":"Disabled"));
            break;
        case 2:
            sprintf(line0,"Trig %s",(timer->trig_state==ACTIVE? "Enabled":"Disabled"));
            sprintf(line1,"%s",trigger_input==ACTIVE?"Sensor Triggered":"Sensor Normal");
            break;
        case 3:
            sprintf(line0,"Temp:%.1f*C",sys_para->temperature);
            sprintf(line1,"Power:%.1fmW",sys_para->pow);
            break;
        case 4:
            sprintf(line0,"Volt:%.2fV",sys_para->volt);
            sprintf(line1,"Curr:%.1fmA",sys_para->cur);
            break;
        default:
            sprintf(line0,"screen:%d",screen);
        break;
    }
    lcd_display();
}
void lcd_RTC_init_fail(){
    sprintf(line0,"RTC FAIL");
    sprintf(line1,"Please Reset");
    lcd_display();
}
void lcd_INA_init_fail(){
    sprintf(line0,"INA219 FAIL");
    sprintf(line1,"Please Reset");
    lcd_display();
}
void lcd_RTC_first_init(){
    sprintf(line0,"RTC reset");
    sprintf(line1,"9/3/2021 22:11");
    lcd_display();
}
void lcd_variable_setup_display(const char * title, uint8_t var,uint8_t b){
    sprintf(line0,"%s",title);
    sprintf(line1,"%d",var);
    lcd_display();
}
void lcd_menu_screen(const char * title,uint8_t screen){
    sprintf(line0,"%s",title);
    sprintf(line1,"<    %02d/%02d     >",screen+1,MAX_MENU_SCREEN); // human dont use index 0, fu human
    lcd_display();
}
void lcd_backlight_on(){
    lcd.backlight();
}
void lcd_backlight_off(){
    lcd.noBacklight();
}
void lcd_display(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line0);
    lcd.setCursor(0,1);
    lcd.print(line1);
}
