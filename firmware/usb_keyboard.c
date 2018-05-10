#include "usb.h"
#include "regmap.h"
#include "usb_keyboard.h"
#include "print.h"
#include "signet.h"

static u8 type_buffer[2];
static int typing;
static int ms_type_next = -1;
static int scancode_typing;
static const char *type_str;
static int type_pos;

struct key {
	unsigned char ascii_key;
	u8 scancode;
	u8 modifier;
};

static struct key keymap_en_us[] = {
	{'a', 4 , 0} , {'A', 4 , 2},
	{'b', 5 , 0} , {'B', 5 , 2},
	{'c', 6 , 0} , {'C', 6 , 2},
	{'d', 7 , 0} , {'D', 7 , 2},
	{'e', 8 , 0} , {'E', 8 , 2},
	{'f', 9 , 0} , {'F', 9 , 2},
	{'g', 10, 0} , {'G', 10, 2},
	{'h', 11, 0} , {'H', 11, 2},
	{'i', 12, 0} , {'I', 12, 2},
	{'j', 13, 0} , {'J', 13, 2},
	{'k', 14, 0} , {'K', 14, 2},
	{'l', 15, 0} , {'L', 15, 2},
	{'m', 16, 0} , {'M', 16, 2},
	{'n', 17, 0} , {'N', 17, 2},
	{'o', 18, 0} , {'O', 18, 2},
	{'p', 19, 0} , {'P', 19, 2},
	{'q', 20, 0} , {'Q', 20, 2},
	{'r', 21, 0} , {'R', 21, 2},
	{'s', 22, 0} , {'S', 22, 2},
	{'t', 23, 0} , {'T', 23, 2},
	{'u', 24, 0} , {'U', 24, 2},
	{'v', 25, 0} , {'V', 25, 2},
	{'w', 26, 0} , {'W', 26, 2},
	{'x', 27, 0} , {'X', 27, 2},
	{'y', 28, 0} , {'Y', 28, 2},
	{'z', 29, 0} , {'Z', 29, 2},

	{'1', 30, 0} , {'!', 30, 2},
	{'2', 31, 0} , {'@', 31, 2},
	{'3', 32, 0} , {'#', 32, 2},
	{'4', 33, 0} , {'$', 33, 2},
	{'5', 34, 0} , {'%', 34, 2},
	{'6', 35, 0} , {'^', 35, 2},
	{'7', 36, 0} , {'&', 36, 2},
	{'8', 37, 0} , {'*', 37, 2},
	{'9', 38, 0} , {'(', 38, 2},
	{'0', 39, 0} , {')', 39, 2},
	{'\n', 40, 0} ,

	{'\t', 43, 0} ,
	{' ' , 44, 0} ,
	{'-' , 45, 0} , {'_', 45, 2},
	{'=' , 46, 0} , {'+', 46, 2},
	{'[' , 47, 0} , {'{', 47, 2},
	{']' , 48, 0} , {'}', 48, 2},
	{'\\', 49, 0} , {'|', 49, 2},

	{';' , 51, 0} , {':', 51, 2},
	{'\'', 52, 0} , {'"', 52, 2},
	{'`' , 53, 0} , {'~', 53, 2},
	{',' , 54, 0} , {'<', 54, 2},
	{'.' , 55, 0} , {'>', 55, 2},
	{'/' , 56, 0} , {'?', 56, 2}
};

static u8 keymap_inv[256][2];

static void usb_keyboard_type(u8 modifier, u8 scancode)
{
	scancode_typing = scancode;
	type_buffer[0] = modifier;
	type_buffer[1] = scancode;
	if (typing)
		return;
	typing = 1;
	usb_send_bytes(KEYBOARD_ENDPOINT, type_buffer, 2);
}

void usb_tx_keyboard()
{
	if (typing) {
		typing = 0;
		if (!scancode_typing) {
			ms_type_next = ms_count + 100;
		} else {
			usb_keyboard_type(0, 0);
		}
	}
}

void usb_keyboard_type_string(const char *str)
{
	type_str = str;
	type_pos = 0;
	ms_type_next = ms_count;
}

void usb_keyboard_stop_typing()
{
	ms_type_next = -1;
	type_str = NULL;
	type_pos = 0;	
}

void usb_keyboard_setup()
{
	int keymap_en_us_n_keys = sizeof(keymap_en_us) / sizeof(struct key);
	for (int i = 0; i < keymap_en_us_n_keys; i++) {
		keymap_inv[keymap_en_us[i].ascii_key][0] = keymap_en_us[i].scancode;
		keymap_inv[keymap_en_us[i].ascii_key][1] = keymap_en_us[i].modifier;
	}
}

void usb_keyboard_idle()
{
	if (ms_count > ms_type_next && ms_type_next > 0) {
		unsigned char c = type_str[type_pos];
		if (c) {
			usb_keyboard_type(keymap_inv[c][1], keymap_inv[c][0]);
			type_pos++;
		} else {
			type_pos = 0;
			type_str = NULL;
			usb_keyboard_typing_done();
		}
		ms_type_next = -1;
	}
}
