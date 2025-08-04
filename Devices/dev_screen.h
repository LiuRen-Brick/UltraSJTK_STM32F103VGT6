/*
 * dev_screen.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef DEV_SCREEN_H_
#define DEV_SCREEN_H_

#include "main.h"

#define DEV_SCREEN_UART_HANDLE   (&huart3)
#define DEV_SCREEN_RXBUFF_SIZE		64
#define DEV_SDREEN_TXBUFF_SZIE      16

extern uint8_t ScreenRxBuff[DEV_SCREEN_RXBUFF_SIZE];

typedef enum
{
	DevScreenCmd_WorkStart = 1,
	DevScreenCmd_SetUltraFreq,
	DevScreenCmd_SetUltraVibra,
	DevScreenCmd_SetUltraDuty,
	DevScreenCmd_SetUltraPeriod,
	DevScreenCmd_SetUltraWork,
	DevScreenCmd_SetUltraChEn,
	DevScreenCmd_SetUltraModule,
	DevScreenCmd_SetUltraPower,
	DevScreenCmdMax,
}DevScreenCmdLable;

void DevUart_Init(void);
void HAL_UART_IdleCallback(UART_HandleTypeDef *huart);
void DevScreenSendData(uint8_t cmd,uint8_t val);

uint8_t DevMotor_StaGet(void);
uint8_t DevUltra_StatGet(uint8_t ch);
uint16_t DevWork_StimuTimeGet(void);
uint16_t DevWork_IdleTimeGet(void);
uint8_t DevWork_ModuleGet(void);

#endif /* DEV_SCREEN_H_ */
