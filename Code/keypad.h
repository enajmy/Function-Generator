/* ----------- keypad.h ------------ */
#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_

// Include #defines
#define ROW0 0x01									// Define ROW0 as 'pin 1'
#define ROW1 0x02									// Define ROW1 as 'pin 2'
#define ROW2 0x04									// Define ROW2 as 'pin 3'
#define ROW3 0x08									// Define ROW3 as 'pin 4'
#define COL0 0x10									// Define COL0 as 'pin 5'
#define COL1 0x20									// Define COL1 as 'pin 6'
#define COL2 0x40									// Define COL2 as 'pin 7'
#define COL3 0x80									// Define COL3 as 'pin 8'
#define KEYPAD_ROW_MASK (ROW0 | ROW1 | ROW2 | ROW3)	// KEYPAD_ROW_MASK = 0x0F
													// - when ANDed, only gives ROW values
#define KEYPAD_COL_MASK (COL0 | COL1 | COL2 | COL3) // KEYPAD_COL_MASK = 0xF0
													// - when ANDed, only gives COL values
#define KEYPAD_NO_PRESS 0xF3							// Return 0b11110011 ('?') when no key is pressed
#define KEYPAD GPIOD									// Define keypad as GPIOD

#define KEYPAD_0 0x00
#define KEYPAD_A 0x41
#define KEYPAD_B 0x42
#define KEYPAD_C 0x43
#define KEYPAD_D 0x44
#define KEYPAD_STAR 0x2A
#define KEYPAD_POUND 0x23

// Include function declarations / prototypes
void keypad_init(void);
uint8_t keypad_read(void);
uint8_t keypad_calc(uint8_t, uint8_t);
uint8_t convertNum(uint8_t);
uint8_t readKeys(void);

#endif /* SRC_KEYPAD_H_ */
