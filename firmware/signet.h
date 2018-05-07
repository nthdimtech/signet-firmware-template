#ifndef SIGNET_H
#define SIGENT_H
void setup(void);
void idle(void);
void button_press(void);
void button_release(void);

void delay(int ms);
void led_on();
void led_off();

extern int ms_count;
#endif
