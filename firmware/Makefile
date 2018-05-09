all: fw.dfu

HTUPLE:=arm-none-eabi

CFLAGS=-DFIRMWARE -Wall -mcpu=cortex-m4 -mthumb -ffunction-sections -fdata-sections -O2 -I..
LDFLAGS=-Wl,"--gc-sections" -Wl,"-Tstm32l443xc.ld"

clean:
	rm -rf *.o *.d fw.elf fw.bin fw.dfu

%.o: %.c
	$(HTUPLE)-gcc  $(CFLAGS) $< -c -o $@
	@$(HTUPLE)-gcc  $(CFLAGS) $< -M -MF $@.d

SOURCES = main.c \
	  startup.c \
	  usb_fs_driver.c \
	  usart.c \
	  usb_serial.c \
	  usb_storage.c \
	  usb.c \
	  usb_keyboard.c \
	  print.c \
	  irq.c \
	  gpio.c \
	  crc.c \
	  rtc_rand.c \
	  usb_raw_hid.c \
	  rng_driver.c \
	  ivt_l443xc.c \
	  main_l443xc.c \
	  flash_l443xc.c

OBJECTS = $(SOURCES:.c=.o)
DEPFILES = $(SOURCES:.c=.o.d)

fw.elf: $(OBJECTS)
	$(HTUPLE)-gcc  $(CFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@

fw.bin: fw.elf
	$(HTUPLE)-objcopy $^ -O binary $@

fw.dfu: fw.bin
	cp $^ $@
	dfu-suffix -v 0x0483 -p 0xdf11 -a $@

-include $(DEPFILES)
