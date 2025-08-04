/*
 * dev_ad5160.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#include "App.h"

#define POWERLEVELNUM	6

static uint8_t ResistCode[POWERLEVELNUM] =
{
	0xFF, 0xC3, 0xA5, 0x87, 0x69, 0x55
};

static uint16_t AD5160_CS_GPIO[AD5160_MaxCh] =
{
	AD5160CH1_CS_Pin, AD5160CH2_CS_Pin
};

static uint16_t AD5160_DATA_GPIO[AD5160_MaxCh] =
{
	AD5160CH1_DATA_Pin, AD5160CH2_DATA_Pin
};

static uint16_t AD5160_CLK_GPIO[AD5160_MaxCh] =
{
	AD5160CH1_CLK_Pin, AD5160CH2_CLK_Pin
};

static GPIO_TypeDef * AD5160_CS_PORT[AD5160_MaxCh] =
{
	AD5160CH1_CS_GPIO_Port, AD5160CH2_CS_GPIO_Port
};
static GPIO_TypeDef * AD5160_DATA_PORT[AD5160_MaxCh] =
{
	AD5160CH1_DATA_GPIO_Port, AD5160CH2_DATA_GPIO_Port
};
static GPIO_TypeDef * AD5160_CLK_PORT[AD5160_MaxCh]  =
{
	AD5160CH1_CLK_GPIO_Port, AD5160CH2_CLK_GPIO_Port
};

static uint8_t PowerLevel = 1;
static uint8_t PowerLevel_old = 1;

static void AD5160_Delay(void)
{
	uint8_t i = 30;

	while(i--);
}

static void AD5160_CSSet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD5160_CS_PORT[ch], AD5160_CS_GPIO[ch], PinState);
}

static void AD5160_CLKSet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD5160_CLK_PORT[ch], AD5160_CLK_GPIO[ch], PinState);
}

static void AD5160_DATASet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD5160_DATA_PORT[ch], AD5160_DATA_GPIO[ch], PinState);
}

static void AD5160_SendData(uint8_t ch,uint8_t data)
{
	uint8_t tag_i = 0;

	AD5160_CSSet(ch,GPIO_PIN_SET);
	AD5160_CLKSet(ch,GPIO_PIN_RESET);
	AD5160_CSSet(ch, GPIO_PIN_RESET);

	for(tag_i = 0;tag_i < 8;tag_i++)
	{
		AD5160_CLKSet(ch,GPIO_PIN_RESET);
		if((data << tag_i) & 0x80)
		{
			AD5160_DATASet(ch,GPIO_PIN_SET);
		}else
		{
			AD5160_DATASet(ch,GPIO_PIN_RESET);
		}
		AD5160_Delay();
		AD5160_CLKSet(ch,GPIO_PIN_SET);
		AD5160_Delay();
	}

	AD5160_CSSet(ch, GPIO_PIN_SET);
	AD5160_CLKSet(ch,GPIO_PIN_RESET);
}

void DevAD5160_ValueLoad(uint8_t power_level)
{
	if(power_level < POWERLEVELNUM)
	{
		PowerLevel = power_level;
	}
}

void DevAD5160Func_Init(void)
{
	uint8_t res_data = 165;
	uint8_t tag_i = 0;

	PowerLevel = DevParamRead.UltraParam.UltraPowerLevel;
	res_data = ResistCode[PowerLevel];
	for(tag_i = 0;tag_i < AD5160_MaxCh;tag_i++)
	{
		AD5160_SendData(tag_i,res_data);
	}
	PowerLevel_old = PowerLevel;
}

void DevAD5160Func_Main(void)
{
	uint8_t res_data = 255;
	uint8_t tag_i;

	if(PowerLevel != PowerLevel_old)
	{
		res_data = ResistCode[PowerLevel];
		for(tag_i = 0;tag_i < AD5160_MaxCh;tag_i++)
		{
			AD5160_SendData(tag_i,res_data);
		}
		PowerLevel_old = PowerLevel;
	}
}
