/*
 * dev_ad5160.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef DEV_AD5160_H_
#define DEV_AD5160_H_

#include <stdint.h>

typedef enum
{
	AD5160_Ch1,
	AD5160_Ch2,
	AD5160_MaxCh,
}AD5160_MaxTable;

void DevAD5160_ValueLoad(uint8_t power_level);
void DevAD5160Func_Init(void);
void DevAD5160Func_Main(void);

#endif /* DEV_AD5160_H_ */
