#include "usb_serial.h"

#include "types.h"
#include "usb.h"
#include "print.h"
#include "regmap.h"

int usb_serial_rx(volatile usbw_t *data, int count) __attribute__ ((weak, alias("usb_serial_rx_def")));
void usb_serial_tx() __attribute__ ((weak, alias("usb_serial_tx_def")));

void usb_serial_line_state(int rx_enable, int tx_enable) __attribute__ ((weak, alias("usb_serial_line_state_def")));

extern struct serial_line_coding g_serial_line_coding __attribute__ ((weak, alias("g_serial_line_coding_def")));

struct serial_line_coding g_serial_line_coding_def = {
	115200, 0, 0, 8
};


void usb_serial_tx_def()
{
}

int usb_serial_rx_def(volatile usbw_t *data, int count)
{
	return 1;
}

void usb_serial_line_state_def(int rx_enable, int tx_enable)
{
	dprint_s("USB CDC/ACM: SET_CONTROL_LINE_STATE: rx =  ");
	dprint_dec(rx_enable);
	dprint_s(", tx = ");
	dprint_dec(tx_enable);
	dprint_s("\r\n");
}
