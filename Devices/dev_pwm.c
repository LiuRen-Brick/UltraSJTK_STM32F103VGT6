/*
 * dev_pwm.c
 *
 *  Created on: Jun 12, 2025
 *      Author: szt
 */

#include "App.h"
#include "tim.h"

#define ULTRA_CH1		&htim11, TIM_CHANNEL_1,
#define ULTRA_CH2		&htim10, TIM_CHANNEL_1,
#define ULTRA_CH3		&htim4,  TIM_CHANNEL_2,
#define ULTRA_CH4		&htim3,  TIM_CHANNEL_2,
#define ULTRA_CH5		&htim8,  TIM_CHANNEL_1,
#define ULTRA_CH6		&htim12, TIM_CHANNEL_2,
#define ULTRA_CH7		&htim2,  TIM_CHANNEL_4,
#define ULTRA_CH8		&htim1,  TIM_CHANNEL_4,

#define MOTOR_HANDLE	&htim9
#define MOTOR_CHANNEL	TIM_CHANNEL_2
#define MOTOR_STEP		216

#define FAN_HANDLE		&htim5
#define FAN_CHANNEL		TIM_CHANNEL_3
#define FAN_PULSE		5000

#define TIM3_CLK 			72000000
#define MIN_OUTPUT_FREQ		2
#define MAX_OUTPUT_FREQ 	10000

static uint8_t MotorLevel = 0xA;
static uint8_t pwm_duty[DevUltraChMax] = {0};
static uint8_t pwm_duty_old[DevUltraChMax] = {0};
static uint32_t pwm_period[DevUltraChMax] = {0};
static uint32_t pwm_period_old[DevUltraChMax] = {0};
static uint32_t pwm_pluse[DevUltraChMax] = {0};

static const DevUltraPwmHandle dev_ultra_handle[DevUltraChMax] =
{
	[DevUltra_Ch1] = {ULTRA_CH1},
	[DevUltra_Ch2] = {ULTRA_CH2},
	[DevUltra_Ch3] = {ULTRA_CH3},
	[DevUltra_Ch4] = {ULTRA_CH4},
	[DevUltra_Ch5] = {ULTRA_CH5},
	[DevUltra_Ch6] = {ULTRA_CH6},
	[DevUltra_Ch7] = {ULTRA_CH7},
	[DevUltra_Ch8] = {ULTRA_CH8},
};

static void DevPwm_SetPulseDuty(uint8_t ch);
static uint32_t DevPwm_SetPulseFreq(uint8_t ch,uint32_t pulsetime,uint8_t duty);

void DevPwm_LoadDuty(uint8_t ch,uint8_t duty)
{
	if(ch < DevUltraChMax)
	{
		pwm_duty[ch] = duty;
	}
}

void DevPwm_LoadPeriod(uint8_t ch,uint8_t period)
{
	if(ch < DevUltraChMax)
	{
		pwm_period[ch] = period;
	}
}

void DevPwmFunc_Init(void)
{
	uint8_t tag_i = 0;
	uint8_t motor_level = 0xA;

	for(tag_i = 0;tag_i < DevUltraChMax;tag_i++)
	{
		pwm_duty[tag_i] = DevParamRead.UltraParam.PluseDuty[tag_i];
		pwm_period[tag_i] = DevParamRead.UltraParam.PlusePeriod[tag_i];

		pwm_pluse[tag_i] = DevPwm_SetPulseFreq(tag_i,pwm_period[tag_i],pwm_duty[tag_i]);
		pwm_duty_old[tag_i] = pwm_duty[tag_i];
		pwm_period_old[tag_i] = pwm_period[tag_i];
	}

	motor_level = DevParamRead.UltraParam.Vibra_Param;
	MotorLevel = motor_level;

	HAL_TIM_Base_Start(FAN_HANDLE);
	HAL_TIM_PWM_Start(FAN_HANDLE, FAN_CHANNEL);

	HAL_TIM_Base_Start(MOTOR_HANDLE);
	HAL_TIM_PWM_Start(MOTOR_HANDLE, MOTOR_CHANNEL);
	__HAL_TIM_SetCompare(MOTOR_HANDLE,MOTOR_CHANNEL,motor_level * MOTOR_STEP);
}

void DevPwmFunc_Main(void)
{
	uint8_t tag_i = 0;

	for(tag_i = 0;tag_i < DevUltraChMax;tag_i++)
	{
		if(pwm_period_old[tag_i] != pwm_period[tag_i])
		{
			pwm_period_old[tag_i] = pwm_period[tag_i];
			pwm_pluse[tag_i] = DevPwm_SetPulseFreq(tag_i,pwm_period[tag_i],pwm_duty[tag_i]);
		}

		if(pwm_duty_old[tag_i] != pwm_duty[tag_i])
		{
			pwm_duty_old[tag_i] = pwm_duty[tag_i];
			DevPwm_SetPulseDuty(tag_i);
		}else
		{
		}
	}

	if(MotorLevel != DevParamRead.UltraParam.Vibra_Param)
	{
		MotorLevel = DevParamRead.UltraParam.Vibra_Param;
		__HAL_TIM_SetCompare(MOTOR_HANDLE,MOTOR_CHANNEL,MotorLevel * MOTOR_STEP);
	}

	if(SampleTemp > 45.0f)
	{
		__HAL_TIM_SetCompare(FAN_HANDLE,FAN_CHANNEL,FAN_PULSE);
	}else if(SampleTemp < 40.0f)
	{
		__HAL_TIM_SetCompare(FAN_HANDLE,FAN_CHANNEL,0);
	}
}

static void DevPwm_SetPulseDuty(uint8_t ch)
{
	uint32_t pulse = 0;
	uint32_t channel;
	TIM_HandleTypeDef *p_handle;

	channel = dev_ultra_handle[ch].Channel;
	p_handle = dev_ultra_handle[ch].Handle;

	pulse = pwm_duty[ch] * pwm_pluse[ch] / 100;
	__HAL_TIM_SetCompare(p_handle,channel,pulse);
}
/*
 * @brief
 * @param 	pulsetime:脉冲周期时间
 * @param	dutycycle:脉冲占空比
 * @retval	none
 * @func	调节脉冲时间和占空比
 */
static uint32_t DevPwm_SetPulseFreq(uint8_t ch,uint32_t pulsetime,uint8_t duty)
{
	uint32_t input_freq = 0;
	uint32_t out_prescaler_value = 0;
	uint32_t out_count_value = 0;
	uint32_t channel;
	uint32_t pluse = 0;
	TIM_HandleTypeDef *p_handle;

	channel = dev_ultra_handle[ch].Channel;
	p_handle = dev_ultra_handle[ch].Handle;

	input_freq = 10000 / pulsetime;

	if((input_freq > MAX_OUTPUT_FREQ) || (input_freq < MIN_OUTPUT_FREQ))
	{
		return 0;
	}

	if((input_freq >= 20) && (input_freq <= 10000))
	{
		out_prescaler_value = 720-1;
		out_count_value = 1000000 / input_freq;
	}else if((input_freq >= 2) && (input_freq < 20))
	{
		out_prescaler_value = 7200-1;
		out_count_value = 100000 / input_freq;
	}else
	{
		return 0;
	}

	// 停止定时器
	HAL_TIM_PWM_Stop(p_handle, channel);
	HAL_TIM_Base_Stop(p_handle);

	__HAL_TIM_SET_PRESCALER(p_handle, out_prescaler_value);
	__HAL_TIM_SetAutoreload(p_handle, out_count_value);

	pluse = out_count_value * duty / 100;
	__HAL_TIM_SetCompare(p_handle, channel, pluse); // 假设使用通道1	//PWM_Ultra_B

    // 更新并启动定时器
	HAL_TIM_Base_Start(p_handle);
    HAL_TIM_PWM_Start(p_handle, channel);

    return out_count_value;
}

