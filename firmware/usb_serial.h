#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include "types.h"
#include "regmap.h"
#include "usb.h"

int usb_serial_rx(volatile usbw_t *data, int count);
void usb_serial_tx();

void usb_serial_line_state(int rx_enable, int tx_enable);

struct serial_line_coding {
	u32 baud;
	u8 stop_bits;
	u8 parity;
	u8 data_bits;
};

extern struct serial_line_coding g_serial_line_coding;

#define usb_serial_write(b,l) usb_send_bytes(CDC_TX_ENDPOINT, (b), (l))

#endif
