#ifndef USB_RAW_HID_H
#define USB_RAW_HID_H

#include "usb.h"

int usb_raw_hid_rx(volatile usbw_t *data, int count);
void usb_raw_hid_tx();

#define usb_raw_hid_send(b,c) usb_send_bytes(RAW_HID_TX_ENDPOINT, b, c)

#endif
