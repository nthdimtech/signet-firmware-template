#ifndef USB_H
#include "types.h"
#include "regmap.h"

void usb_send_bytes(int ep, const u8 *data, int length);

#define LANGID_US_ENGLISH 0x409

#define USB_VENDOR_ID (0x5E2A)
#define USB_SIGNET_DESKTOP_PRODUCT_ID (0x0001)
#define USB_SIGNET_MOBILE_PRODUCT_ID (0x0001)
#define USB_REV_ID (0x0483)
#define USB_RAW_HID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#define USB_RAW_HID_USAGE	0x0200	// recommended: 0x0100 to 0xFFFF
#define RAW_HID_PACKET_SIZE 64
#define RAW_HID_HEADER_SIZE 1
#define RAW_HID_PAYLOAD_SIZE (RAW_HID_PACKET_SIZE - RAW_HID_HEADER_SIZE)

#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1
#define RAW_HID_INTERFACE 2
#define KEYBOARD_INTERFACE 3
#define NUM_INTERFACES 4

#define CONTROL_ENDPOINT 0
#define CDC_ACM_ENDPOINT 1
#define CDC_TX_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define RAW_HID_TX_ENDPOINT 4
#define RAW_HID_RX_ENDPOINT 5
#define KEYBOARD_ENDPOINT 6

#define CDC_ACM_PACKET_SIZE 16
#define CDC_TX_SIZE 64
#define CDC_RX_SIZE 64
#define RAW_HID_TX_SIZE RAW_HID_PACKET_SIZE
#define RAW_HID_TX_INTERVAL 2
#define RAW_HID_RX_SIZE RAW_HID_PACKET_SIZE
#define RAW_HID_RX_INTERVAL 8
#define CTRL_RX_SIZE 64
#define CTRL_TX_SIZE 64
#define KEYBOARD_PACKET_SIZE 8
#define KEYBOARD_INTERVAL 10

#define HID_REQ_SET_IDLE (10)
#define HID_REQ_GET_REPORT (1)

void usb_tx_ctrl();

void usb_rx(int id, int setup, volatile usbw_t *data, int count);
void usb_tx(int id);
void usb_copyfrom(void *dest, const usbw_t *src, int len);
void usb_copyto(usbw_t *dest, const void *src, int len);
void usb_set_mobile_mode();
extern int usb_device_addr;

#endif
