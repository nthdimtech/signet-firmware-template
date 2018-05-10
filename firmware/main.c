#include "signet.h"
#include "flash.h"
#include "usb_keyboard.h"
#include "usb_raw_hid.h"
#include "usb_serial.h"

#include <string.h>

static int led_blinking;

//
// USB raw HID
//

int usb_raw_hid_rx(volatile usbw_t *data, int count)
{
	u8 msg[256];
	for (int i = 0; i < count && i < sizeof(msg); i++) {
		msg[i] = (data[i>>1] >> (8*(i&1))) & 0xff;
		if (msg[i] == 0) {
			flash_write_page(_storage[0], msg, i);
			break;
		}
	}
	return 1;
}

void usb_raw_hid_tx()
{
}

//
// USB keyboard
//

void usb_keyboard_typing_done()
{
}

//
// USB serial
//

int usb_serial_rx(volatile usbw_t *data, int count)
{
	static char line[256];
	static char line_echo[256];
	static int line_pos;
	for (int i = 0; i < count; i++) {
		u8 c = (data[i>>1] >> (8*(i&1))) & 0xff;
		line[line_pos] = c;
		if (c == '\r') {
			line[line_pos++] = '\r';
			line[line_pos++] = '\n';
			line[line_pos] = 0;
			line_pos = 0;
			strcpy(line_echo, line);
			usb_serial_write((void *)line_echo, strlen(line_echo));
		} else {
			line_pos++;
		}
	}
	return 1;
}


void usb_serial_tx()
{
}

void usb_serial_line_state(int rx_enable, int tx_enable)
{
}

struct serial_line_coding g_serial_line_coding = {
	115200, 0, 0, 8
};

//
// Button
//

void button_press()
{
	const char *str = NULL;
	int i;
	led_blinking = 1;
	for (i = 0; i < 2048; i++) {
		if (_storage[0][i] == 0) {
			str = (char *)_storage[0];
		}
	}
	if (!str) {
		str = "Hello world!";
	}
	usb_keyboard_type_string(str);
}

void button_release()
{
	led_blinking = 0;
	usb_keyboard_stop_typing();
}

//
// Flash
//
void flash_write_complete(void)
{
}

void flash_write_failed(void)
{
}

//
// RTC rand
//

void rtc_rand_update()
{
}

//
// Application
//

void setup()
{
	usb_keyboard_setup();
	led_on();
}

void idle()
{
	usb_keyboard_idle();
	if (((ms_count/200)%2) && led_blinking) {
		led_on();
	} else {
		led_off();
	}
}
