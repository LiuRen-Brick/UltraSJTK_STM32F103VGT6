/*
 * dev_ad9833.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef DEV_AD9833_H_
#define DEV_AD9833_H_

#include "main.h"

//#define AD9833_OUT_SINUS    ((0 << 5) | (0 << 1) | (0 << 3))
//#define AD9833_OUT_TRIANGLE ((0 << 5) | (1 << 1) | (0 << 3))
//#define AD9833_OUT_MSB      ((1 << 5) | (0 << 1) | (1 << 3))
//#define AD9833_OUT_MSB2     ((1 << 5) | (0 << 1) | (0 << 3))
//#define AD9833_REG_CMD      (0 << 14)
//#define AD9833_REG_FREQ0    (1 << 14)
//#define AD9833_REG_FREQ1    (2 << 14)
//#define AD9833_REG_PHASE0   (6 << 13)
//#define AD9833_REG_PHASE1   (7 << 13)
//#define AD9833_FSEL0        (0 << 11)
//#define AD9833_FSEL1        (1 << 11)
//#define AD9833_PSEL0        (0 << 10)
//#define AD9833_PSEL1        (1 << 10)

#define AD9833_OUT_SINUS		0x00		//正弦波
#define AD9833_OUT_TRIANGLE		0x02		//三角波
#define AD9833_OUT_MSB			0x20		//全幅方波
#define AD9833_OUT_MSB2			0x28		//半幅方波

#define AD9833_REG_FREQ0		0x4000		//频率寄存器FREQ0
#define AD9833_REG_FREQ1		0x8000		//频率寄存器FREQ1

#define AD9833_REG_PHASE0		0xC000		//相位寄存器PHASE0
#define AD9833_REG_PHASE1		0xE000		//相位寄存器PHASE1

#define AD9833STEP		2000

typedef enum
{
	AD9833_Ch1,
	AD9833_Ch2,
	AD9833_Ch3,
	AD9833_Ch4,
	AD9833_Ch5,
	AD9833_Ch6,
	AD9833_Ch7,
	AD9833_Ch8,
	AD9833_MaxCh,
}AD9833_MaxTable;

void DevAD9833Set_Init(void);
void DevAD9833Set_Main(void);

#endif /* DEV_AD9833_H_ */
