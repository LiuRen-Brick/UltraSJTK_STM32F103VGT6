/*
 * dev_ad5160.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#include "App.h"

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

static void AD5160_Delay(void)
{
	uint8_t i = 3;

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
	AD5160_CLKSet(ch,GPIO_PIN_SET);
	AD5160_CSSet(ch, GPIO_PIN_RESET);

	for(tag_i = 0;tag_i < 8;tag_i++)
	{
		if((data >> tag_i) & 0x01)
		{
			AD5160_DATASet(ch,GPIO_PIN_SET);
		}else
		{
			AD5160_DATASet(ch,GPIO_PIN_RESET);
		}
		AD5160_CLKSet(ch,GPIO_PIN_RESET);
		AD5160_Delay();
		AD5160_CLKSet(ch,GPIO_PIN_SET);
		AD5160_Delay();
	}

	AD5160_CSSet(ch, GPIO_PIN_SET);
}

void DevAD5160Func_Init(void)
{
	uint8_t res_data = 128;
	uint8_t tag_i;

	for(tag_i = 0;tag_i < AD5160_MaxCh;tag_i++)
	{
		AD5160_SendData(tag_i,res_data);
	}
}

void DevAD5160Func_Main(uint8_t ch,uint8_t data)
{
	AD5160_SendData(ch,data);
}
