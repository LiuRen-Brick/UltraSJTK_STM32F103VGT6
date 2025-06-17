/*
 * App.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#include "App.h"
#include "gpio.h"

uint8_t WorkSta;
uint8_t ParamRefresh;
uint8_t UltraEnFlg[8] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
uint32_t UltraWorkTime;
Device_Param DevParamRead;

static uint8_t MotorEnableFlg = 1;
static uint16_t StimuTime = 500;
static uint16_t IdleTime = 1500;

static GPIO_TypeDef * Ultra_Gpio_Port[ULTRACHNUM]  =
{
	UltraEn_CH1_GPIO_Port, UltraEn_CH2_GPIO_Port, UltraEn_CH3_GPIO_Port, UltraEn_CH4_GPIO_Port,
	UltraEn_CH5_GPIO_Port, UltraEn_CH6_GPIO_Port, UltraEn_CH7_GPIO_Port, UltraEn_CH8_GPIO_Port
};

static GPIO_TypeDef * Motor_Gpio_Port[ULTRACHNUM]  =
{
	MotorEn_CH1_GPIO_Port, MotorEn_CH2_GPIO_Port, MotorEn_CH3_GPIO_Port, MotorEn_CH4_GPIO_Port,
	MotorEn_CH5_GPIO_Port, MotorEn_CH6_GPIO_Port, MotorEn_CH7_GPIO_Port, MotorEn_CH8_GPIO_Port
};

static uint16_t Ultra_Gpio_Pin[ULTRACHNUM]  =
{
	UltraEn_CH1_Pin, UltraEn_CH2_Pin, UltraEn_CH3_Pin, UltraEn_CH4_Pin,
	UltraEn_CH5_Pin, UltraEn_CH6_Pin, UltraEn_CH7_Pin, UltraEn_CH8_Pin
};

static uint16_t Motor_Gpio_Pin[ULTRACHNUM]  =
{
	MotorEn_CH1_Pin, MotorEn_CH2_Pin, MotorEn_CH3_Pin, MotorEn_CH4_Pin,
	MotorEn_CH5_Pin, MotorEn_CH6_Pin, MotorEn_CH7_Pin, MotorEn_CH8_Pin
};


void DevSystem_Init(void)
{
	DevParam_Init();
	DevAD9833Set_Init();
	DevADC1Func_Init();
	DevAD9833Set_Init();
	DevPwmFunc_Init();

	HAL_GPIO_WritePin(V36CH1_4_EN_GPIO_Port, V36CH1_4_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(V36CH5_8_EN_GPIO_Port, V36CH5_8_EN_Pin, GPIO_PIN_SET);
}

void AppMainFunc(void)
{
	uint8_t tag_i = 0;

	if(WorkSta == DEVWORKSTART)
	{
		if(UltraWorkTime < (StimuTime * 10))
		{
			for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
			{
				if((tag_i % 2) == 0)
				{
					HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], (GPIO_PinState)UltraEnFlg[tag_i]);
					if(DevParamRead.UltraParam.VibraEnableFlg == 1)
					{
						HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_SET);
					}
				}else
				{
					HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
					HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
				}
			}
		}else if((UltraWorkTime >= (StimuTime * 10)) && (UltraWorkTime < ((IdleTime + StimuTime) * 5)))
		{
			for(tag_i = 0;tag_i < (ULTRACHNUM / 2);tag_i++)
			{
				HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i * 2], Motor_Gpio_Pin[tag_i * 2], GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i * 2], Ultra_Gpio_Pin[tag_i * 2], GPIO_PIN_RESET);
			}
		}else if((UltraWorkTime >= ((IdleTime + StimuTime) * 5)) && (UltraWorkTime < (StimuTime * 15 + IdleTime * 5)))
		{
			for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
			{
				if((tag_i % 2) == 1)
				{
					HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], (GPIO_PinState)UltraEnFlg[tag_i]);
					if(DevParamRead.UltraParam.VibraEnableFlg == 1)
					{
						HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_SET);
					}
				}else
				{
					HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
					HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
				}
			}
		}else if((UltraWorkTime >= (StimuTime * 15 + IdleTime * 5)) && (UltraWorkTime < (StimuTime * 10 + IdleTime * 10)))
		{
			for(tag_i = 0;tag_i < (ULTRACHNUM / 2);tag_i++)
			{
				HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i * 2 + 1], Motor_Gpio_Pin[tag_i * 2 + 1], GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i * 2 + 1], Ultra_Gpio_Pin[tag_i * 2 + 1], GPIO_PIN_RESET);
			}
		}else
		{
			UltraWorkTime = 0;
		}

	}else
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
		}
	}

	UltraParamReload();
}

void UltraParamReload(void)
{
	if(ParamRefresh == 1)
	{
		MotorEnableFlg = DevMotor_StaGet();
		StimuTime = DevWork_StimuTimeGet();
		IdleTime = DevWork_IdleTimeGet();

		ParamRefresh = 0;
	}
}
