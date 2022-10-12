/* ---------- led.h ---------- */

#ifndef SRC_LED_H_
#define SRC_LED_H_


// Include Pound Defines
#define LED GPIOC	// Define LED as GPIOC
#define LEDB GPIOB
#define LED1 0x80 // C7
#define LED2 0x80 //B7
#define LED3 0x4000 //B14

// Include function definitions / prototypes
void LED_init(void);
void LED_on(void);
void LED_off(void);
void LED_toggle(void);
void LED_on2(void);
void LED_off2(void);

#endif
