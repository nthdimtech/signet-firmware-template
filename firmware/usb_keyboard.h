#ifndef USB_KEYBOARD_H
#define USB_KEYBOARD_H

#include "types.h"

//API
void usb_keyboard_setup();
void usb_keyboard_idle();
void usb_keyboard_type_string(const char *str);
void usb_keyboard_stop_typing();

//Callback
void usb_keyboard_typing_done();

//Internal
void usb_tx_keyboard();

#endif
