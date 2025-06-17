/*
 * dev_pwm.h
 *
 *  Created on: Jun 12, 2025
 *      Author: szt
 */

#ifndef DEV_PWM_H_
#define DEV_PWM_H_


typedef struct
{
	TIM_HandleTypeDef *Handle;
	uint32_t Channel;
}DevUltraPwmHandle;

typedef enum
{
	DevUltra_Ch1 = 0,
	DevUltra_Ch2,
	DevUltra_Ch3,
	DevUltra_Ch4,
	DevUltra_Ch5,
	DevUltra_Ch6,
	DevUltra_Ch7,
	DevUltra_Ch8,
	DevUltraChMax,
}DevUltraChannelLableType;

void DevPwm_LoadDuty(uint8_t ch,uint8_t duty);
void DevPwm_LoadPeriod(uint8_t ch,uint8_t period);
void DevPwmFunc_Init(void);
void DevPwmFunc_Main(void);

#endif /* DEV_PWM_H_ */
