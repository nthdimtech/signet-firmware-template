#include "types.h"
#include "regmap.h"
#include "usart.h"
#include "usb.h"
#include "print.h"
#include "flash.h"
#include "usb_keyboard.h"
#include "stm_aes.h"
#include "rtc_rand.h"
#include "gpio.h"
#include "irq.h"
#include "rng.h"
#include "crc.h"
#include "config.h"
#include "main.h"
#include "usb_driver.h"

#define USE_USB 1
#define USE_RNG 1
#define USE_FLASH 1
#define USE_RTC 1
#define USE_UART 1
#define USE_CRC 1

void setup(void) __attribute__ ((weak, alias ("default_event_handler")));
void idle(void) __attribute__ ((weak, alias ("default_event_handler")));
void button_press(void) __attribute__ ((weak, alias ("default_event_handler")));
void button_release(void) __attribute__ ((weak, alias ("default_event_handler")));
void long_button_press(void) __attribute__ ((weak, alias ("default_event_handler")));
void usb_keyboard_typing_done() __attribute__ ((weak, alias("default_event_handler")));
void rtc_rand_update() __attribute__ ((weak, alias("default_event_handler")));
void rng_rand_update() __attribute__ ((weak, alias("default_event_handler")));

volatile int ms_count = 0;
static int ms_last_pressed = 0;
int button_state = 0;

void led_on()
{
#if STATUS_LED_COUNT > 0
	STATUS_LED1_PORT->BSRR = 1 << (STATUS_LED1_PIN);
#if STATUS_LED_COUNT > 1
	STATUS_LED2_PORT->BSRR = 1 << (STATUS_LED2_PIN);
#endif
#endif
}

void led_off()
{
#if STATUS_LED_COUNT > 0
	STATUS_LED1_PORT->BSRR = 1 << (STATUS_LED1_PIN + 16);
#if STATUS_LED_COUNT > 1
	STATUS_LED2_PORT->BSRR = 1 << (STATUS_LED2_PIN + 16);
#endif
#endif
}

void systick_handler()
{
	ms_count++;
}

void delay(int ms)
{
	int ms_target = ms_count + ms;
	while(ms_count < ms_target);
}

void default_event_handler()
{
}

int press_pending = 0;

void BUTTON_HANDLER()
{
	ms_last_pressed = ms_count;
	press_pending = 1;
	EXTI_PR = (1<<BUTTON_PIN);
}

void loop()
{
	__asm__("cpsie i");
	if (!flash_writing()) {
		__asm__("wfi");
	}
	__asm__("cpsid i");
	flash_idle();
	if (press_pending) {
		press_pending = 0;
		if (!button_state) {
			button_state = 1;
			button_press();
			goto end;
		}
	}
	int current_button_state = (BUTTON_PORT->IDR & (1<<BUTTON_PIN)) ? 0 : 1;
	if (!current_button_state && button_state && (ms_count - ms_last_pressed) > 100) {
		button_state = 0;
		button_release();
		goto end;
	}
	idle();
end:
	__asm__("cpsie i");
}

u8 _storage[96][2048];

int main()
{
	RCC_CFGR = (RCC_CFGR & ~(RCC_CFGR_HPRE_MASK)) |
		(RCC_CFGR_HPRE_FACTOR * (7 + HCLK_DIVISOR_LOG2));

	while (!(RCC_CR & RCC_CR_MSIRDY));
	RCC_CR = (RCC_CR & ~RCC_CR_MSIRANGE_MASK) | RCC_CR_MSIRGSEL | (RCC_CR_MSIRANGE_FACTOR * 11);
	while (!(RCC_CR & RCC_CR_MSIRDY));

	u32 ahb1enr_val = 0;
	u32 ahb2enr_val = RCC_AHB2ENR_IOP_AEN | RCC_AHB2ENR_IOP_BEN | RCC_AHB2ENR_IOP_CEN |
		RCC_AHB2ENR_IOP_DEN;
	u32 apb2enr_val = RCC_APB2ENR_SYSCFGEN;
	u32 apb1enr1_val = 0;

#if USE_RTC
	apb1enr1_val |= RCC_APB1ENR1_PWREN;
#endif

#if USE_CRC
	ahb1enr_val |= RCC_AHB1ENR_CRCEN;
#endif

#if USE_USB
	RCC_CCIPR |= RCC_CCIPR_CLK48SEL_HSI48;
	RCC_CRRCR |= RCC_CRRCR_HSI48ON;
	while (!(RCC_CRRCR & RCC_CRRCR_HSI48RDY));
	apb1enr1_val |=	RCC_APB1ENR1_CRSEN | RCC_APB1ENR1_PWREN;
#endif

	STK_RVR = SYSTICK/1000;
	STK_CVR = 0;
	STK_CSR = 7;
	//Set SysTick priority to zero
	SCB_SHPR3 = SCB_SHPR3 & 0xffffff;

	//Enable fault handlers
	SCB_SHCSR |= 7 << 16;

	delay(4);

#if USE_USB
	set_irq_priority(USB_FS_IRQ, 128);
	EXTI_IMR |= (1<<USB_EXTI_LINE);
	EXTI_RTSR |= (1<<USB_EXTI_LINE);
	enable_irq(USB_FS_IRQ);
	apb1enr1_val |= RCC_APB1ENR1_USBFSEN;
#endif

	set_irq_priority(BUTTON_IRQ, 64);
	enable_irq(BUTTON_IRQ);

#if USE_FLASH
	set_irq_priority(FLASH_IRQ, 192);
	enable_irq(FLASH_IRQ);
#endif
#if USE_RTC
	set_irq_priority(RTC_WKUP_IRQ, 128);
	enable_irq(RTC_WKUP_IRQ);
	EXTI_IMR |= (1<<RTC_EXTI_LINE);
	EXTI_RTSR |= (1<<RTC_EXTI_LINE);
	apb1enr1_val |= RCC_APB1ENR1_PWREN;
#endif
#if USE_RNG
	set_irq_priority(RNG_IRQ, 128);
	enable_irq(RNG_IRQ);
	ahb2enr_val |= RCC_AHB2ENR_RNGEN;
#endif
#if USE_UART
	enable_irq(USART1_IRQ);
	set_irq_priority(USART1_IRQ, 128);
	apb2enr_val |= RCC_APB2ENR_USART1EN;
#endif

	RCC_APB2ENR |= apb2enr_val;
	RCC_APB1ENR1 |= apb1enr1_val;
	RCC_AHB1ENR |= ahb1enr_val;
	RCC_AHB2ENR |= ahb2enr_val;

	delay(2);

#if USE_UART
	//USART2 init
	usart_init(&usart1,
		HCLK, 115200,
		PORTA,
		9 /* pin 9-10 */,
		7 /* AF7 */);

	struct print usart1_print = {
		(void *)&usart1,
		(void (*)(void *, char c))usart_print_char
	};

	dbg_print = &usart1_print;
#endif

	dprint_s("Hello world!\r\n");

	//Button pin
	SYSCFG_EXTICR(BUTTON_PIN/4) = (BUTTON_PORT_NUM << (4*(BUTTON_PIN % 4)));
	gpio_set_in(BUTTON_PORT, BUTTON_PIN, 1);
	EXTI_FTSR |= (1<<BUTTON_PIN);
	EXTI_IMR |= (1<<BUTTON_PIN);

	//Led pins

#if STATUS_LED_COUNT > 0
	gpio_set_out(STATUS_LED1_PORT, STATUS_LED1_PIN);
	gpio_out_set(STATUS_LED1_PORT, STATUS_LED1_PIN);
#if STATUS_LED_COUNT > 1
	gpio_set_out(STATUS_LED2_PORT, STATUS_LED2_PIN);
	gpio_out_set(STATUS_LED2_PORT, STATUS_LED2_PIN);
#endif
#endif
	led_on();


#if USE_FLASH
	if (FLASH_CR & FLASH_CR_LOCK)
		flash_unlock();
	FLASH_CR |= FLASH_CR_EOPIE | FLASH_CR_ERRIE;
#endif

#if USE_RNG
	rng_init();
#endif

#if USE_CRC
	crc_init();
#endif

#if USE_RTC
	PWR_CR |= PWR_CR_DPB;
	RCC_CSR |= RCC_CSR_LSION;
	while (!(RCC_CSR & RCC_CSR_LSIRDY));

	RCC_BDCR = (RCC_BDCR & ~(RCC_BDCR_RTCSEL_MASK)) |
		RCC_BDCR_RTCEN |
		RCC_BDCR_RTCSEL_LSI;
	rtc_rand_init(0x7f);
#endif

#if USE_USB
	PWR_CR2 |= PWR_CR2_USV;
	CRS_CR |= CRS_CR_AUTOTRIMEN;
	gpio_enable_af(USB_DM_PORT, USB_DM_PIN, USB_DM_AF);
	gpio_enable_af(USB_DP_PORT, USB_DP_PIN, USB_DP_AF);
	usb_reset_device();
#endif
	dprint_s("Entering main loop\r\n");
	setup();
	while(1)
		loop();
	return 0;
}
