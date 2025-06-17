/*
 * dev_adc.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef DEV_ADC_H_
#define DEV_ADC_H_

float look1_iflf_binlxpw(float u0, const float bp0[], const float table[], uint32_t maxIndex);

void DevADC1Func_Init(void);
void DevADC1Func_Main(void);

#endif /* DEV_ADC_H_ */
