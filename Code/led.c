/* ---------- led.c ---------- */

#include "main.h"
#include "led.h"

void LED_init(void) {

	// Initialize GPIO Clocks
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);

	// Set pin 1 and 2 to output mode
	LED->MODER &= ~(GPIO_MODER_MODE7);
	LED->MODER |= (GPIO_MODER_MODE7_0);
	//LED->MODER &= ~(GPIO_MODER_MODE1);
	//LED->MODER |= (GPIO_MODER_MODE1_0);

	LEDB->MODER &= ~(GPIO_MODER_MODE7);
	LEDB->MODER |= (GPIO_MODER_MODE7_0);
	LEDB->MODER &= ~(GPIO_MODER_MODE14);
	LEDB->MODER |= (GPIO_MODER_MODE14_0);

	// Set pin 1 and 2 to PP
	LED->OTYPER &= ~(GPIO_OTYPER_OT7);
	LEDB->OTYPER &= ~(GPIO_OTYPER_OT7);
	LEDB->OTYPER &= ~(GPIO_OTYPER_OT14);

	// Set pin 1 and 2 to low speed
	LED->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7);
	LEDB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7);
	LEDB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED14);

	// Set pin 1 and 2 to no PUPD
	LED->PUPDR &= ~(GPIO_PUPDR_PUPD7);
	LEDB->PUPDR &= ~(GPIO_PUPDR_PUPD7);
	LEDB->PUPDR &= ~(GPIO_PUPDR_PUPD14);
}

// Turn LED on
void LED_on(void) {
	// LED3 has hex value 0x01, giving a 1 at bit 1
	LED->ODR &= ~(LED1);
	LED->ODR |= (LED1);
}

// Turn LED off
void LED_off(void) {
	LED->ODR &= ~(LED1);
}

void LED_toggle(void) {
	LED->ODR ^= (LED1);
}
void LED_on2(void) {
	LED->ODR &= ~(LED2);
	LED->ODR |= (LED2);
}

void LED_off2(void) {
	LED->ODR &= ~(LED2);
}
