/* ----------- DAC.h ------------ */
#ifndef SRC_DAC_H_
#define SRC_DAC_H_

// Include #defines
#define DACC GPIOA

// Include function definitions / prototypes

void DAC_init(void);
void DAC_write(uint16_t);
uint16_t DAC_volt_conv(uint16_t);

#endif /* SRC_DAC1_H_ */
