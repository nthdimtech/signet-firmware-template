#include <memory.h>

#include "usb_raw_hid.h"
#include "usb_driver.h"
#include "usb.h"

int usb_raw_hid_rx_default(volatile usbw_t *data, int count)
{
	return 0;
}

void usb_raw_hid_tx_default()
{
}
