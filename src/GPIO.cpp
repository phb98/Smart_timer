#include "GPIO.h"
#include <Arduino.h>
#include <stdint.h>
#include "main.h"
void gpio_intialize(){
    pinMode(EN_A,INPUT_PULLUP);
    pinMode(EN_B,INPUT_PULLUP);
    pinMode(EN_BT,INPUT_PULLUP);
    pinMode(SENSOR_PIN,INPUT_PULLUP);
    attachInterrupt(0,enc_handle,FALLING);
    attachInterrupt(1,bt_handling,FALLING);
}
void output_write(uint8_t val){
    analogWrite(MOSFET,map(val,0,100,0,255)); //scale from percentage to uint8_t
}