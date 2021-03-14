#ifndef eeprom_h
#define eeprom_h
#define EEPROM_ADDR 0x10
#include "main.h"
void eeprom_intialize();
void eeprom_read_timer(timer_t * timer);
void eeprom_write_timer(const timer_t * timer);
#endif