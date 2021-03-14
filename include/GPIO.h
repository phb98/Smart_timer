/* Header file contain define gpio used in project*/
#ifndef GPIO_H
  #include <stdint.h>
  #define GPIO_H
  #define ENC_COUNT_PER_STEP 2
  /*gpio for encoder*/
  #define EN_A 2 
  #define EN_B 4 // which pin in PORTD, does not support other port since it does not use digitalRead
  #define EN_BT 3 // encoder button
  /*gpio for output*/
  #define MOSFET 9
  /*gpio for motion sensor*/
  #define SENSOR_PIN 5
  #define SENSOR_ACTIVE_STATE 0 // change this if you need to invert the input of motion sensor
  /* setup pin mode for gpio */
  void gpio_intialize();
  void output_write(uint8_t val); // output to the mosfet
#endif 