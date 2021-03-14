#include "timer_eeprom.h"
#include <Arduino.h>
#include "EEPROM.h"
void eeprom_intialize(){
    EEPROM.begin();
}
void eeprom_read_timer(timer_t * timer){
   timer->normal_value=EEPROM.read(EEPROM_ADDR);
    timer->off_hour=EEPROM.read(EEPROM_ADDR+1);
    timer->off_min=EEPROM.read(EEPROM_ADDR+2);
    timer->on_hour=EEPROM.read(EEPROM_ADDR+3);
    timer->on_min=EEPROM.read(EEPROM_ADDR+4);
    //timer->out_value=EEPROM.read(EEPROM_ADDR+5);
    timer->state=EEPROM.read(EEPROM_ADDR+6);
    timer->trig_state=EEPROM.read(EEPROM_ADDR+7);
    timer->trig_time_out=EEPROM.read(EEPROM_ADDR+8);
    timer->trig_value=EEPROM.read(EEPROM_ADDR+9);
}
void eeprom_write_timer(const timer_t * timer){
    EEPROM.write(EEPROM_ADDR,timer->normal_value);
    EEPROM.write(EEPROM_ADDR+1,timer->off_hour);
    EEPROM.write(EEPROM_ADDR+2,timer->off_min);
    EEPROM.write(EEPROM_ADDR+3,timer->on_hour);
    EEPROM.write(EEPROM_ADDR+4,timer->on_min);
    //EEPROM.write(EEPROM_ADDR+1,timer->out_value);
    EEPROM.write(EEPROM_ADDR+6,timer->state);
    EEPROM.write(EEPROM_ADDR+7,timer->trig_state);
    EEPROM.write(EEPROM_ADDR+8,timer->trig_time_out);
    EEPROM.write(EEPROM_ADDR+9,timer->trig_value);
}