/* ----------- keypad.c ------------ */
#include "main.h"
#include "keypad.h"

void keypad_init(void) {
  /* -- Configure GPIO D -- */
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIODEN);		//Enable GPIO D Clock

  KEYPAD->MODER &= ~(GPIO_MODER_MODE0 		// Set MODE[0:7][1:0] to 0
		  | GPIO_MODER_MODE1					// Will keep pins 4-7 as 0 for input mode
		  | GPIO_MODER_MODE2
		  | GPIO_MODER_MODE3
		  | GPIO_MODER_MODE4
		  | GPIO_MODER_MODE5
		  | GPIO_MODER_MODE6
		  | GPIO_MODER_MODE7);

  KEYPAD->MODER |= (GPIO_MODER_MODE0_0 		// Set MODE[0:3][0] to 1
		  | GPIO_MODER_MODE1_0				// Set as output pins
		  | GPIO_MODER_MODE2_0
		  | GPIO_MODER_MODE3_0);

  KEYPAD->OTYPER &= ~(GPIO_OTYPER_OT0 		// Set OTYPE[0:3] to 0
		  | GPIO_OTYPER_OT1					// Set as push-pull
		  | GPIO_OTYPER_OT2
		  | GPIO_OTYPER_OT3);

  KEYPAD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 // Set OSPEED[0:3] to 0
		  | GPIO_OSPEEDR_OSPEED1				// Set output speed as low
		  | GPIO_OSPEEDR_OSPEED2
		  | GPIO_OSPEEDR_OSPEED3);

  KEYPAD->PUPDR &= ~(GPIO_PUPDR_PUPD0 		// Set PUPD[0:7][1:0] to 0
		  | GPIO_PUPDR_PUPD1					// Will keep pins 0-3 at 0
		  | GPIO_PUPDR_PUPD2
		  | GPIO_PUPDR_PUPD3
		  | GPIO_PUPDR_PUPD4
		  | GPIO_PUPDR_PUPD5
		  | GPIO_PUPDR_PUPD6
		  | GPIO_PUPDR_PUPD7);

  KEYPAD->PUPDR |= (GPIO_PUPDR_PUPD4_1 		// Set PUPD[4:7][1] to 1
		  | GPIO_PUPDR_PUPD5_1				// Enables pull-down resistors
		  | GPIO_PUPDR_PUPD6_1
		  | GPIO_PUPDR_PUPD7_1);

  KEYPAD->ODR &= ~(KEYPAD_ROW_MASK);			// Set rows to high
  KEYPAD->ODR |= (KEYPAD_ROW_MASK);
}

// READ KEYPAD FUNCTION
uint8_t keypad_read(void) {

	/* -- Initialize variables -- */

	// rows, cols - keep track of row # and column #
	// button - saves key pressed on keypad
	// row_output - enables rows to be turned high one at a time
	uint8_t rows, cols, button, row_output = 1;

	cols = KEYPAD->IDR & KEYPAD_COL_MASK;	// Read input register and only save columns
	if (cols == 0)							// If no keys pressed (cols = 0), return KEYPAD_NO_PRESS
		return KEYPAD_NO_PRESS;

	/* -- Row Incrementer -- */
	for(rows = 0; rows < 4; rows++) {
		KEYPAD->ODR &= ~(KEYPAD_ROW_MASK);
		KEYPAD->ODR |= row_output;						// Set ROW0 high
		row_output *= 2;									// Multiply row by 2, output to ROW1,2,3
		cols = (KEYPAD->IDR & KEYPAD_COL_MASK);			// Read columns
		if (cols != 0) {									// If no column high, skip loop

			while (KEYPAD->IDR & KEYPAD_COL_MASK){
				//	Delay while buttons are pressed
			}

			button = keypad_calc(cols,rows);				// Calc key press from row and col value
														// and save to button
			KEYPAD->ODR &= ~(KEYPAD_ROW_MASK);			// Set rows high
			KEYPAD->ODR |= (KEYPAD_ROW_MASK);
			return button;								// Return button


		}

	}
	KEYPAD->ODR &= ~(KEYPAD_ROW_MASK);				// Set rows high
	KEYPAD->ODR |= (KEYPAD_ROW_MASK);
	return KEYPAD_NO_PRESS;							// Return 0xFF
}

// KEYPAD CALCULATE WHAT BUTTON WAS PRESSED
uint8_t keypad_calc(uint8_t cols, uint8_t rows) {
	uint8_t key_press;								// Initialize variable to save key value

	cols = (cols >> 5);								// Shift column value 5 bits, allows columns to
													// be numbered 0,1,2,4

	if ((cols < 4) && (rows < 3)) {					// 4x4 KEYPAD, check IF value is number 1-9
		key_press = ((rows*3)+(cols*1)+ 1);			// Use eqn to calculate number key pressed and
													// add 0x30 to convert to ASCII value

	} else if (cols == 4) {							// If a value in column 4 was pressed
		key_press = KEYPAD_A + rows;					// Add rows to KEYPAD_A (10),
													// if row = 0, key_press = 10 (A),
													// if row = 1, key_press = 11 (B), etc.

	} else if (rows == 3 && cols == 0) {				// If bottom left button, key pressed = *
		key_press = KEYPAD_STAR;

	} else if (rows == 3 && cols == 1) {				// If bottom middle button, key pressed = 0
		key_press = KEYPAD_0;

	} else if (rows == 3 && cols == 2) {				// If bottom right button, key pressed = #
		key_press = KEYPAD_POUND;

	} else
		key_press = KEYPAD_NO_PRESS;					// If nothing pressed, return 0xFF

	return key_press;								// Return key_press value
}

uint8_t readKeys(void){
	uint8_t keypress;							// Initialize Variable
	keypress = keypad_read();					// Read keypad
	while (keypress == KEYPAD_NO_PRESS)			// Wait until button has been pressed
		keypress = keypad_read();
	return keypress;							// Return button pressed
}

// USED TO CONVERT DIGIT 0-9 TO ASCII CHAR VALUE
uint8_t convertNum (uint8_t num) {
	num += 0x30;		// Add hex 0x30 and save as num.
	return num;			// Return num
}
