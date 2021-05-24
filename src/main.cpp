#include <Arduino.h>
#include <RTClib.h>
#include "GPIO.h"
#include "main.h"
#include "lcd.h"
#include "timer_eeprom.h"
#include <Adafruit_INA219.h>
#include <Wire.h>
Adafruit_INA219 ina219;
RTC_DS3231 rtc; // object for the RTC
// prototype function
void enc_handle(); // interrupt callback for encoder
//uint8_t timer_check(const uint8_t sensor_state,uint8_t cur_hour,uint8_t cur_min,uint8_t cur_sec,const timer_t * timer);
/*check if current time is in range between on time and off time*/
uint8_t check_time(uint8_t cur_hour, uint8_t cur_min, uint8_t on_hour, uint8_t on_min, uint8_t off_hour, uint8_t off_min);
/*generic function to display and change a variable*/
uint8_t variable_setup(const char * title, uint8_t changing_var, const uint8_t upper_bound);
void menu_setup(DateTime current_time,timer_t * setting_timer);
void eeprom_intialize(timer_t * timer);
uint8_t sensor_state(); // get the current state of motion sensor
void system_get_para(system_para_t * para); // get all the parameter of system
// global variable
volatile uint8_t lcd_update=0; // the flag tell the main loop to update the LCD
volatile enc_t enc_event=NO_EVENT; // holding last state of encoder


// setup function
void setup() {
  delay(1000);
  gpio_intialize();
  lcd_initialize();
  delay(2000);// RTC need 2s to stablize
  if(!rtc.begin()){ // fail to intialize the RTC
    lcd_RTC_init_fail();
    while(1); // hang in there
  }
  if(!ina219.begin()){
      lcd_INA_init_fail();
      while(1);
  }
  if(rtc.lostPower()){ // the RTC was not intialized 
    lcd_RTC_first_init();
    rtc.adjust(DateTime(2021, 3, 9, 22, 11, 36)); // the time this code was written
    delay(2000);
  } 
    eeprom_intialize();
}

void loop() {
    DateTime now = rtc.now(); // get current time from RTC
    static timer_t my_timer;
    static uint8_t screen;
    system_para_t my_sys_para;
    uint32_t last_update_millis=0;
    uint8_t lcd_backlight_elapsed=LCD_BACKLIGHT_TIMEOUT;
    uint8_t last_second=0;
    uint8_t has_new_second=0; 
    uint8_t timer_trigger_elapsed=0;
    eeprom_read_timer(&my_timer); //first boot, read data from eeprom save to timer
    while(1){
        /*we dont want to update system para too frequently*/
        if(abs(millis()-last_update_millis)>=(my_sys_para.para_update_interval)){
            now=rtc.now();
            system_get_para(&my_sys_para);
            last_update_millis=millis();
            if(now.second()!=last_second){
                last_second=now.second();
                has_new_second=1;
            }
        }
        if(my_timer.state==ACTIVE){ // the timer must be active to do anything
            if(sensor_state() && my_timer.trig_state==ACTIVE){ //the sensor is trigger, overide the output
                my_timer.out_value=my_timer.trig_value;
                timer_trigger_elapsed=my_timer.trig_time_out;
            }
            else{
                if(timer_trigger_elapsed>0 && my_timer.trig_state==ACTIVE){ // the trigger duration is not finished
                    my_timer.out_value=my_timer.trig_value;
                    if(has_new_second) timer_trigger_elapsed--;
                }
                else{
                    /*check if current time satisfy timer condition*/
                   if(check_time(now.hour(),now.minute(),my_timer.on_hour,my_timer.on_min,my_timer.off_hour,my_timer.off_min)){
                    my_timer.out_value=my_timer.normal_value;
                   }
                   else my_timer.out_value=0;// Current time not satisfy timer condition, turn off output
                }
            }
        }
        else my_timer.out_value=0; //turn off output if timer is not actived
        output_write(my_timer.out_value);   
        /*check the lcd backlight*/
        if(lcd_backlight_elapsed>0){
            lcd_backlight_on();
            if(has_new_second) lcd_backlight_elapsed--;
        }
        else lcd_backlight_off();
        if(has_new_second || lcd_update){ // does LCD need to be update now ?
            switch(enc_event){  // handle the encoder
                case CCW:
                    if(screen==0) screen=MAX_MAIN_SCREEN-1;
                    else screen--;
                    break;
                case CW:
                    if(screen>=MAX_MAIN_SCREEN-1) screen=0;
                    else screen++;
                    break;
                case PRESSED:
                    lcd_backlight_on(); // we don't want to change menu in dark, dont we ?
                    menu_setup(now,&my_timer);
                    lcd_backlight_elapsed=LCD_BACKLIGHT_TIMEOUT; //after return from menu we want the backlight stay on for a while
                    break;
                default:
                    break;
            }
            if(enc_event!=NO_EVENT) lcd_backlight_elapsed=LCD_BACKLIGHT_TIMEOUT; // any input from user turn on lcd backlight
            lcd_main_screen(screen,now,&my_timer,&my_sys_para,sensor_state());
            lcd_update=0;
            enc_event=NO_EVENT;
            if(has_new_second)has_new_second=0; // sometime the lcd_update caused by user input
        }
    }
}

/*helper function*/
uint8_t sensor_state(){
    if(digitalRead(SENSOR_PIN) == SENSOR_ACTIVE_STATE) return ACTIVE;
    else return DEACTIVE;
}
uint8_t check_time(uint8_t cur_hour, uint8_t cur_min, uint8_t on_hour, uint8_t on_min, uint8_t off_hour, uint8_t off_min){
    /*determine the case of setting time( off time > on time or off time < on time)*/
    /*case off time > on time*/
    if(off_hour > on_hour || (off_hour == on_hour && off_min > on_min)){
        //printf("Case 1");
        if((cur_hour>=on_hour && cur_min >=on_min) &&((cur_hour<off_hour) || (cur_hour==off_hour&&cur_min<off_min))){
             return 1;
        }
    }
    /*case off time <on time*/
    else{
        if(off_hour<on_hour || (off_hour==on_hour && off_min<on_min)){
           // printf("Case 2");
            if(cur_hour > on_hour) return 1;
            if(cur_hour < off_hour) return 1;
            if(cur_hour == on_hour && cur_min >= on_min) return 1;
            if(cur_hour == off_hour && cur_min < off_min) return 1;
            }
        /*last case is off_time=on_time*/
        //else printf("case 3");
    }
    if(on_hour==cur_hour && on_min==cur_min){
        return 1;
    }
    //printf("fail");
    return 0;
}

uint8_t time_is_after(uint8_t h0, uint8_t m0, uint8_t h1, uint8_t m1){
    if(h1>h0) return 1; 
    if(h1<h0) return 0;
    if(m1>m0) return 1;
    if(m1<m0) return 0;
    return 2; // return 2 if equal
}
/*TODO: IMPLEMENT VELOCITY CONTROL ENCODER*/
void enc_handle(){
    static int8_t enc_count; 
    if(enc_event!=NO_EVENT) return; // the last interrupt is not processed
    if((PIND>>EN_B)&0x1){  // digitalRead is not fast enough, so we use direct register access
       if(enc_count<0) enc_count=0;
       else enc_count++;
    }
    else{
        if(enc_count>0){
        enc_count=0;
        }
        else enc_count--;
    }
    if(enc_count>=ENC_COUNT_PER_STEP){
        enc_count=0;
        enc_event=CW;
        lcd_update=1;
    }
    if( (0-enc_count)>=ENC_COUNT_PER_STEP){
        enc_count=0;
        enc_event=CCW;
        lcd_update=1;
    }
}
void bt_handling(){
    static uint32_t last_millis=millis(); // software debouncing
    if(millis()-last_millis>300 &&enc_event==NO_EVENT){
        enc_event=PRESSED;
        last_millis=millis();
        lcd_update=1;
    }
}
/* dear future me, FU*/
void menu_setup(DateTime current_time,timer_t * setting_timer){
    uint8_t menu_screen=0;
    uint8_t changing_var[MAX_MENU_SCREEN]; // temporary variable
    uint8_t has_change_current_time=0; // to tell if we touch the current time setting, if not we dont have to modify RTC
    for(int i=0;i<MAX_MENU_SCREEN;i++){
        changing_var[i]=0;
    }
    uint8_t finish_setting=0; // to break out our while loop, 2 to save changed, 1 to discard changing
    //lcd_update=1;
    const char * menu_title[MAX_MENU_SCREEN]={"    Set Hour","   Set Minute"," Timer On Hour","Timer On Minute","Timer Off Hour","Timer Off Minute",
                                              "  Timer Enable?","Timer On Output","  Trig Timeout"," Trig On Output","  Trig Enable?",
                                              "  Save Change?","Discard Changes?"};
    /*save data to changing var first*/
        changing_var[0]=current_time.hour();
        changing_var[1]=current_time.minute();
        changing_var[2]=setting_timer->on_hour;
        changing_var[3]=setting_timer->on_min;
        changing_var[4]=setting_timer->off_hour;
        changing_var[5]=setting_timer->off_min;
        changing_var[6]=setting_timer->state;
        changing_var[7]=setting_timer->normal_value;
        changing_var[8]=setting_timer->trig_time_out;
        changing_var[9]=setting_timer->trig_value;
        changing_var[10]=setting_timer->trig_state;
    enc_event=NO_EVENT; // clear the interrupt
    lcd_update=1;
    while(1){ // we will spend enternally(just like ours) in this loop until we told it to break
        if(lcd_update){
            switch(enc_event){ // check the event make lcd update
                case PRESSED:
                    if(menu_screen==0 || menu_screen==1) has_change_current_time=1; // the first 2 screen is RTC setting
                    if(menu_screen==MAX_MENU_SCREEN-1) finish_setting=1; // discard change
                    else{
                        if(menu_screen==MAX_MENU_SCREEN-2) finish_setting=2; // save all change
                        else changing_var[menu_screen]=variable_setup(menu_title[menu_screen],changing_var[menu_screen],setting_timer->var_boundary[menu_screen]);
                    }
                    break;
                case CCW:
                    if(menu_screen<=0) menu_screen=MAX_MENU_SCREEN-1;
                    else menu_screen--;
                    break;
                case CW:
                    if(menu_screen>=MAX_MENU_SCREEN-1) menu_screen=0;
                    else menu_screen++;
                    break;
                default:
                    break;
            }
            lcd_menu_screen(menu_title[menu_screen],menu_screen); // display the menu
            enc_event=NO_EVENT;
            lcd_update=0;
        }
        if(finish_setting!=0) break; // breakout the infinity loop
    }
    /*determine to save or discard*/
    if(finish_setting==2){
        if(has_change_current_time) rtc.adjust(DateTime(2021,3,9,changing_var[0],changing_var[1],0)); // set the current time if necessary
        setting_timer->on_hour=changing_var[2];
        setting_timer->on_min=changing_var[3];
        setting_timer->off_hour=changing_var[4];
        setting_timer->off_min=changing_var[5];
        setting_timer->state=changing_var[6];
        setting_timer->normal_value=changing_var[7];
        setting_timer->trig_time_out=changing_var[8];
        setting_timer->trig_value=changing_var[9];
        setting_timer->trig_state=changing_var[10];
        eeprom_write_timer(setting_timer); // save to EEPROM
    }
}
//generic function to display and change a variable
uint8_t variable_setup(const char * title, uint8_t changing_var, const uint8_t upper_bound){
    enc_event=NO_EVENT; //clear any waiting interrupt
    lcd_update=1;
    while(1){
        if(lcd_update){
            switch(enc_event){
                case PRESSED:
                    return changing_var;
                    break;
                case CCW:
                    if(changing_var<=0) changing_var=upper_bound;
                    else (changing_var)--;
                    break;
                case CW:
                    if(changing_var>=(upper_bound)) changing_var=0;
                    else (changing_var)++;
                    break;
                default:
                    break;
            }
            lcd_variable_setup_display(title,changing_var,upper_bound);
            enc_event=NO_EVENT;
            lcd_update=0;
        }
    }
}
/*get all the parameter of system*/
void system_get_para(system_para_t * para){
    para->volt=ina219.getBusVoltage_V()+(ina219.getShuntVoltage_mV()/1000.0);
    para->cur=ina219.getCurrent_mA();
    para->pow=ina219.getPower_mW();
    para->temperature=rtc.getTemperature();

}
