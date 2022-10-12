// EE 329 - 01, Ethan Najmy
// Project #2 - Function Generator

// -------- main.c --------//
#include "main.h"
#include "keypad.h"
#include "DAC.h"
#include <math.h>

#define TIM2_IRQn 28
#define DAC_3Volts 3540
#define DAC_0Volts 0000

#define CLK_40MHz 40000000

// Defined for a 50% duty cycle square wave
#define ARR_MAX 400000
#define CCR1_MAX 200000

// Will turn off ARR
#define ARR_ON 0xFFFFFFFF

// Calculated values
#define ARRAY_SIZE 2640
#define INDEX 152 			// Max Resolution

#define PI 3.14159265358979323846

// Declare functions
void SystemClock_Config(void);
uint16_t sineWave(uint16_t count);
uint16_t sawWave(uint16_t count);
uint16_t triWave(uint16_t count);

// Initialize variables
uint16_t sine_array[ARRAY_SIZE], tri_array[ARRAY_SIZE], saw_array[ARRAY_SIZE];

// Waveform = 4 for initial startup into square with 100Hz freq and 50% duty
uint8_t keypress, waveform = 4, freq = 1, duty = 5;
uint16_t count = 0;



int main(void){
	// Create FSM
	typedef enum {ST_WAVEFORM, ST_SINE, ST_TRI, ST_SAW, ST_SQUARE} State_Type;
	State_Type state = ST_SQUARE;

	// Create Lookup tables for each wave
	for (uint16_t sineCount = 0; sineCount < ARRAY_SIZE; sineCount++){
		sine_array[sineCount] = sineWave(sineCount);
	}
	for (uint16_t triCount = 0; triCount < ARRAY_SIZE; triCount++){
		tri_array[triCount] = triWave(triCount);
	}

	for (uint16_t sawCount = 0; sawCount < ARRAY_SIZE; sawCount++){
		saw_array[sawCount] = sawWave(sawCount);
	}

	// Initialize
	HAL_Init();
	SystemClock_Config();

	DAC_init();
	keypad_init();

	// Turn on Timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	// Enable Auto-reload preload
	TIM2->CR1 = (TIM_CR1_ARPE);

	// Set ARR and CCR1 for 100Hz, 50% duty cycle wave
	TIM2->ARR = (CLK_40MHz / (100 * freq));
	TIM2->CCR1 = ((TIM2->ARR * duty) / 10);

	// Enable interrupt flags
	TIM2->EGR |= (TIM_EGR_UG);
	TIM2->DIER |= (TIM_DIER_UIE);
	TIM2->DIER |= (TIM_DIER_CC1IE);


	// Enable global interrupts
	NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));
	__enable_irq();

	// Start timer
	TIM2->CR1 |= TIM_CR1_CEN;

	while (1) {

		// FSM!
		switch(state){

		// Used to switch between waveforms
		case ST_WAVEFORM:{

			// Disable all interrupts and clear flags
			TIM2->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
			TIM2->DIER &= ~(TIM_SR_UIF | TIM_SR_CC1IF);

			// If sine wave selected:
			if (keypress == 6){

				// Used to identify waveform in IRQ
				waveform = 1;

				// Turn off ARR and set CCR1 to max res.
				TIM2->ARR = ARR_ON;
				TIM2->CCR1 = INDEX;

				// Update interrupt event!!!!
				TIM2->EGR |= TIM_EGR_UG;
				count = 0;

				// Re-enable CCR1 ONLY
				TIM2->DIER |= (TIM_DIER_CC1IE);

				// Move to sine wave
				state = ST_SINE;
				break;

				// If triangle wave selected:
			} else if (keypress == 7){

				// All same comments as above
				waveform = 2;

				TIM2->ARR = ARR_ON;
				TIM2->CCR1 = INDEX;
				TIM2->EGR |= TIM_EGR_UG;
				count = 0;

				TIM2->DIER |= (TIM_DIER_CC1IE);

				state = ST_TRI;
				break;

				// If saw wave selected:
			} else if (keypress == 8){

				// All same comments as above
				waveform = 3;

				TIM2->ARR = ARR_ON;
				TIM2->CCR1 = INDEX;
				TIM2->EGR |= TIM_EGR_UG;
				count = 0;

				TIM2->DIER |= (TIM_DIER_CC1IE);

				state = ST_SAW;
				break;

				// If square wave selected
			} else if (keypress == 9){
				waveform = 4;

				// Set ARR and CCR using formulas according to
				// frequency and duty cycle selected.
				TIM2->ARR = (CLK_40MHz / (100 * freq));
				TIM2->CCR1 = ((TIM2->ARR * duty) / 10);
				TIM2->EGR |= TIM_EGR_UG;
				count = 0;

				// Enable both ARR and CCR
				TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);

				state = ST_SQUARE;
				break;

				// If for some reason no key was pressed
				// stay in the same state
			} else {
				state = state;
				break;
			}
			break;
		}

		// Sawtooth Wave
		case ST_SAW:{
			// Wait for keypress
			keypress = readKeys();

			// If a number between 1-5, change freq
			if (keypress > 0 && keypress < 6)
				freq = keypress;

			// If a number to change waves, go to ST_WAVEFORM
			if ((keypress >= 6 && keypress <= 7) || keypress == 9){
				state = ST_WAVEFORM;
				break;
			}
			break;
		}

		case ST_SINE:{

			// Same comments as above
			keypress = readKeys();

			if (keypress > 0 && keypress < 6)
				freq = keypress;

			if (keypress >= 7 && keypress <= 9){
				state = ST_WAVEFORM;
				break;
			}
			break;
		}

		case ST_TRI:{

			// Same comments as above
			keypress = readKeys();

			if (keypress > 0 && keypress < 6)
				freq = keypress;

			if (keypress == 6 || (keypress >= 8 && keypress <= 9)){
				state = ST_WAVEFORM;
				break;
			}
			break;
		}

		case ST_SQUARE:{
			keypress = readKeys();

			// If changing frequencies, update ARR and CCR
			if (keypress > 0 && keypress < 6){
				freq = keypress;
				TIM2->ARR = (CLK_40MHz / (100 * freq));
				TIM2->CCR1 = ((TIM2->ARR * duty) / 10);
			}

			// If wanting to decrement duty cycle, only adjust CCR
			if (keypress == KEYPAD_STAR){
				// decrement duty by 10%
				if (duty > 1){
					duty--;
					TIM2->CCR1 = ((TIM2->ARR * duty) / 10);
				} else {
					duty = 1;
				}
			}

			// If wanting to increment duty cycle, only adjust CCR
			if (keypress == KEYPAD_POUND){
				// increment duty by 10%
				if (duty < 9){
					duty++;
					TIM2->CCR1 = ((TIM2->ARR * duty) / 10);
				} else {
					duty = 9;
				}
			}

			// Will reset duty cycle
			if (keypress == KEYPAD_0){
				// reset duty to 50%
				duty = 5;
				TIM2->CCR1 = ((TIM2->ARR * duty) / 10);
			}

			// Change states if wave selected.
			if (keypress >= 6 && keypress <= 8){
				state = ST_WAVEFORM;
				break;
			}

			break;
		}
		}
	}
}

// INTERRUPT HANDLER
void TIM2_IRQHandler(void){

	// Switch statement to choose how to handle the
	// interrupt depending on waveform variable
	switch(waveform){

	// Waveform is SINE
	case 1:{
		// If CCR triggered, reset flag
		if (TIM2->SR & TIM_SR_CC1IF){
			TIM2->SR = ~(TIM_SR_CC1IF);

			// Write to the DAC using the array lookup
			DAC_write(sine_array[count]);

			// If count exceeded the array (minus freq
			// to obtain no jumping b/w points), reset
			// count to zero.
			if (count >= (ARRAY_SIZE - freq))
				count = 0;
			// Otherwise increment count by freq
			// (Increment by freq to change sampling rate
			// which allows for frequency changes without
			// changing CCR).
			else
				count += freq;

			// Keep incrementing CCR so it doesn't stop
			TIM2->CCR1 += INDEX;
		}
		break;
	}

	// Triangle Wave
	case 2:{

		// Same exact comments just with tri_array
		if (TIM2->SR & TIM_SR_CC1IF){
			TIM2->SR = ~(TIM_SR_CC1IF);
			DAC_write(tri_array[count]);
			if (count >= (ARRAY_SIZE))
				count = 0;
			else
				count += freq;
			TIM2->CCR1 += INDEX;
		}
		break;
	}

	// Sawtooth Wave
	case 3:{

		// Same exact comments just with saw_array
		if (TIM2->SR & TIM_SR_CC1IF){
			TIM2->SR = ~(TIM_SR_CC1IF);
			DAC_write(saw_array[count]);
			if (count >= (ARRAY_SIZE))
				count = 0;
			else
				count += freq;
			TIM2->CCR1 += INDEX;
		}
		break;
	}

	// Square Wave
	case 4:{

		// If ARR triggered, clear flag and write
		// 3V to the DAC
		if (TIM2->SR & TIM_SR_UIF){
			TIM2->SR &= ~(TIM_SR_UIF);
			DAC_write(DAC_3Volts);
		}
		// IF CCR triggered, clear the flag and
		// write 0V to the DAC
		if (TIM2->SR & TIM_SR_CC1IF){
			TIM2->SR &= ~(TIM_SR_CC1IF);
			DAC_write(DAC_0Volts);
		}
		break;
	}
	}
}

// Array lookup functions

uint16_t sineWave(uint16_t sinInput){
	uint16_t sine;

	// Use double to be as accurate as possible
	double DAC_in;

	// Formula for a sine wave adjusted for 3V exact output
	// Multiply and add by 1475 for 3Vpp, 1.5V offset
	// In the thousands because thats how the DAC interprets
	DAC_in = (1475 * sin(((2 * PI * sinInput) / ARRAY_SIZE)) + 1475);

	// Convert back to int
	DAC_in = (uint16_t)DAC_in;

	// Convert to voltage that can be written to DAC and return
	sine = DAC_volt_conv(DAC_in);
	return sine;
}

uint16_t sawWave(uint16_t sawInput){
	uint16_t saw, DAC_in;

	// Multiply count input by (3000/2640)
	// (3V out / array size) (rise / run)
	// Give scaling factor to ensure points
	// Reach 3V in size of array
	DAC_in = sawInput * 1.137666412;

	// Convert to DAC voltage
	saw = DAC_volt_conv(DAC_in);
	return saw;
}

uint16_t triWave(uint16_t triInput){
	uint16_t tri, DAC_in;

	// Same concept as above with the (3000/2640)
	// However, if we have reached over half the
	// array, begin inverting the values with a
	// negative slope and a 6000 offset so the
	// values begin at 3V rather than -3V
	if (triInput <= (ARRAY_SIZE/2)){
		DAC_in = (triInput * 2 * 1.137666412);
		tri = DAC_volt_conv(DAC_in);
		return tri;
	} else {
		DAC_in = (triInput * 2 * -1.137666412) + 6000;
		tri = DAC_volt_conv(DAC_in);
		return tri;
	}
}

// ---------------------------------------------------------
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
