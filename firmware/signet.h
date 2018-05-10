#ifndef SIGNET_H
#define SIGENT_H

#include "types.h"

//API
void led_on();
void led_off();
extern int ms_count;
extern u8 _storage[][2048];

//Callbacks
void setup(void);
void idle(void);
void button_press(void);
void button_release(void);

//Internal
void delay(int ms);
#endif
