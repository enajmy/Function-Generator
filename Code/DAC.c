/* ----------- DAC.c ------------ */
#include "main.h"
#include "DAC.h"

// Function to Initialize the DAC
void DAC_init(void){
	/* -- Configure GPIO PA4, PA5, PA7 for SPI Control -- */

	//Enable GPIO A Clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);

	// Set MODER to alternate function mode
	DACC->MODER &= ~(GPIO_MODER_MODE4
			| GPIO_MODER_MODE5
			| GPIO_MODER_MODE7);
	DACC->MODER |= (GPIO_MODER_MODE4_1
			| GPIO_MODER_MODE5_1
			| GPIO_MODER_MODE7_1);

	// Set AFRL to AF5 for SPI1 controller
	DACC->AFR[0] &= ~(GPIO_AFRL_AFSEL4
			| GPIO_AFRL_AFSEL5
			| GPIO_AFRL_AFSEL7);
	DACC->AFR[0] |= (GPIO_AFRL_AFSEL4_2
			| GPIO_AFRL_AFSEL4_0
			| GPIO_AFRL_AFSEL5_2
			| GPIO_AFRL_AFSEL5_0
			| GPIO_AFRL_AFSEL7_2
			| GPIO_AFRL_AFSEL7_0);

	// Set OTYPER to output push pull
	DACC->OTYPER &= ~(GPIO_OTYPER_OT4
			| GPIO_OTYPER_OT5
			| GPIO_OTYPER_OT7);

	// Set OSPEED to low speed
	DACC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4
			| GPIO_OSPEEDR_OSPEED5
			| GPIO_OSPEEDR_OSPEED7);

	// Set PUPDR to no pull up pull down
	DACC->PUPDR &= ~(GPIO_PUPDR_PUPD4
			| GPIO_PUPDR_PUPD5
			| GPIO_PUPDR_PUPD7);


	/* -- Enable SPI Mode -- */

	/* CR1 code is as follows:
	 * -- RXONLY = 0 (Enables Simplex Mode)
	 * -- SSM = 0 (Hardware Chip Select Management)
	 * -- LSBFIRST = 0 (Sending MSB first to DAC)
	 * -- BR = 0 (Baud rate = f/2 (highest))
	 * -- CPOL & CPHA = 0 (Sets clock phase and polarity
	 * 	   to have a low idle and send data on rising edge)
	 * -- MSTR = 1 (Set MCU to controller configuration) */
	SPI1->CR1 = (SPI_CR1_MSTR);

	/* CR2 code is as follows:
	 * -- DS = 0xF (16 bit communications)
	 * -- NSSP = 1 (chip select pulse mode)
	 * -- SSOE = 1 (chip select enable) */
	SPI1->CR2 = (SPI_CR2_DS
			| SPI_CR2_NSSP
			| SPI_CR2_SSOE);

	/* -- Lastly, enable SPI -- */
	SPI1->CR1 |= (SPI_CR1_SPE);
}

void DAC_write(uint16_t command){
	/* Set hiNibble to 0x3000, this is to ensure bits 15-12
	 * sent to the DAC are 0011 as required by the DAC */
	uint16_t hiNibble = 0x3000;

	// Remove upper 4 bits of command
	uint16_t loNibble = (command & 0x0FFF);

	/* Set the upper 4 bits of command = hiNibble and
	 * the lower 12 bits of command to loNibble, which
	 * in our case is the command (or voltage level) */
	command = hiNibble | loNibble;

	// Output to SPI data register
	SPI1->DR = command;
}

uint16_t DAC_volt_conv(uint16_t voltage){
	/* The following formula has been adapted from
	 * the DAC data sheet.
	 * The 1.2412 is used to scale the voltage entered
	 * into a range from 0-4096 (12 bits).
	 * 1.2412 = 4096/3300 (total bits/max voltage)
	 * The 1.0042 and -3.5727 are calibrations
	 * used from recording desired data vs actual output
	 * and came from an excel trendline as outlined
	 * in technical note 5 of lab manual.
	 */
	voltage = ((voltage * (1.2412)) / 1.0042) - 3.5727;
	return voltage;
}
