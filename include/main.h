/*define typedef*/
#ifndef _MAIN_H
#define _MAIN_H
#include <stdint.h>
    #define ACTIVE 1
    #define DEACTIVE 0
    typedef struct {
        uint8_t on_hour;
        uint8_t on_min;
        uint8_t off_hour;
        uint8_t off_min; 
        uint8_t state = ACTIVE;
        uint8_t normal_value;
        uint8_t trig_time_out=30; // in second unit, max 255s, default 30s 
        uint8_t trig_value;
        uint8_t trig_state= DEACTIVE;
        uint8_t out_value;
        const uint16_t var_boundary[11]={23,59,23,59,23,59,1,100,120,100,1}; // the first 2 is for the RTC bound current time
    } timer_t;
    typedef struct{
        double volt;
        double cur;
        double pow;
        double temperature;
        uint16_t para_update_interval=250;
    } system_para_t;
    typedef enum {NO_EVENT,CCW,CW,PRESSED} enc_t; // encoder
    void enc_handle();
    void bt_handling();
#endif