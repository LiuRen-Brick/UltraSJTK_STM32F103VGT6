/*
 * dev_ad9833.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */
#include "App.h"

static uint16_t AD9833_FSYNC_GPIO[AD9833_MaxCh] =
{
	AD9833CH1_FSYNC_Pin, AD9833CH2_FSYNC_Pin, AD9833CH3_FSYNC_Pin, AD9833CH4_FSYNC_Pin,
	AD9833CH5_FSYNC_Pin, AD9833CH6_FSYNC_Pin, AD9833CH7_FSYNC_Pin, AD9833CH8_FSYNC_Pin
};
static uint16_t AD9833_SDATA_GPIO[AD9833_MaxCh] =
{
	AD9833CH1_SCLK_Pin, AD9833CH2_SCLK_Pin, AD9833CH3_SCLK_Pin, AD9833CH4_SCLK_Pin,
	AD9833CH5_SCLK_Pin, AD9833CH6_SCLK_Pin, AD9833CH7_SCLK_Pin, AD9833CH8_SCLK_Pin
};
static uint16_t AD9833_SCLK_GPIO[AD9833_MaxCh] =
{
	AD9833CH1_SDATA_Pin, AD9833CH2_SDATA_Pin, AD9833CH3_SDATA_Pin, AD9833CH4_SDATA_Pin,
	AD9833CH5_SDATA_Pin, AD9833CH6_SDATA_Pin, AD9833CH7_SDATA_Pin, AD9833CH8_SDATA_Pin
};
static GPIO_TypeDef * AD9833_FSYNC_PORT[AD9833_MaxCh] =
{
	AD9833CH1_FSYNC_GPIO_Port, AD9833CH2_FSYNC_GPIO_Port, AD9833CH3_FSYNC_GPIO_Port, AD9833CH4_FSYNC_GPIO_Port,
	AD9833CH5_FSYNC_GPIO_Port, AD9833CH6_FSYNC_GPIO_Port, AD9833CH7_FSYNC_GPIO_Port, AD9833CH8_FSYNC_GPIO_Port
};
static GPIO_TypeDef * AD9833_SDATA_PORT[AD9833_MaxCh] =
{
	AD9833CH1_SCLK_GPIO_Port, AD9833CH2_SCLK_GPIO_Port, AD9833CH3_SCLK_GPIO_Port, AD9833CH4_SCLK_GPIO_Port,
	AD9833CH5_SCLK_GPIO_Port, AD9833CH6_SCLK_GPIO_Port, AD9833CH7_SCLK_GPIO_Port, AD9833CH8_SCLK_GPIO_Port
};
static GPIO_TypeDef * AD9833_SCLK_PORT[AD9833_MaxCh]  =
{
	AD9833CH1_SDATA_GPIO_Port, AD9833CH2_SDATA_GPIO_Port, AD9833CH3_SDATA_GPIO_Port, AD9833CH4_SDATA_GPIO_Port,
	AD9833CH5_SDATA_GPIO_Port, AD9833CH6_SDATA_GPIO_Port, AD9833CH7_SDATA_GPIO_Port, AD9833CH8_SDATA_GPIO_Port
};


static uint16_t UltraParam_Old[ULTRACHNUM] = {0};

static void AD9833_Delay(void)
{
	uint8_t i = 2;
	while(i--);
}
/*
 * @brief	:AD9833_FSYNCSet
 * @param   ch:超声驱动的通道号
 * 			PinState:电平置位
 * @retval  none
 * @func	写入AD9833FSYNC脚的电平信号
 */
static void AD9833_FSYNCSet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD9833_FSYNC_PORT[ch], AD9833_FSYNC_GPIO[ch], PinState);
}

/*
 * @brief	:AD9833_SCLKSet
 * @param   ch:超声驱动的通道号
 * 			PinState:电平置位
 * @retval  none
 * @func	写入AD9833SCLK脚的电平信号
 */
static void AD9833_SCLKSet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD9833_SCLK_PORT[ch], AD9833_SCLK_GPIO[ch], PinState);
}

/*
 * @brief	:AD9833_SDATASet
 * @param   ch:超声驱动的通道号
 * 			PinState:电平置位
 * @retval  none
 * @func	写入AD9833SDATA脚的电平信号
 */
static void AD9833_SDATASet(uint8_t ch,GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(AD9833_SDATA_PORT[ch], AD9833_SDATA_GPIO[ch], PinState);
}

/*
 * @brief	DevAD9833IO_Init
 * @param   ch:超声驱动的通道号
 * @retval  none
 * @func	初始化AD9833 IO口
 */
static void DevAD9833IO_Init(void)
{
	uint8_t ch = 0;

	for(ch = 0; ch < AD9833_MaxCh;ch++)
	{
		AD9833_FSYNCSet(ch,GPIO_PIN_SET);
		AD9833_SCLKSet(ch,GPIO_PIN_SET);
		AD9833_SDATASet(ch,GPIO_PIN_SET);
	}
}

/*
 * @brief	:AD9833_SendData
 * @param   ch:超声驱动的通道号
 * 			txdata:要发送的数据
 * @retval  none
 * @func	向AD9833发送指令
 */
static void AD9833_SendData(uint8_t ch,uint32_t txdata)
{
	uint8_t tag_i = 0;

	/*发送指令之前AD9833的SCLK引脚使能，FSYNC引脚复位*/
	AD9833_SCLKSet(ch,SET);
	AD9833_FSYNCSet(ch,SET);
	AD9833_FSYNCSet(ch,RESET);

	for(tag_i=0;tag_i<16;tag_i++){
		if (txdata & 0x8000){
			AD9833_SDATASet(ch,SET);
		}
		else{
			AD9833_SDATASet(ch,RESET);
		}
		AD9833_Delay();
		AD9833_SCLKSet(ch,RESET);
		AD9833_Delay();
		AD9833_SCLKSet(ch,SET);
		txdata<<=1;
	}
	AD9833_FSYNCSet(ch,SET);
}

/*
 * @brief	AD9833_SendFrequency
 * @param   ch:超声驱动的通道号
 * @param	WaveMode:输出波形
 * @param	Freq_SFR:设置相位寄存器
 * @retval  none
 * @func	向AD9833发送指令
 */
static void AD9833_SetFreq(uint8_t ch,uint16_t reg, uint32_t freq)
{
	uint32_t freq_data;
	uint32_t freq_data_lsb,freq_data_msb;

	freq_data = 268435456 / 11059 * (freq / 1000);
	freq_data_lsb = reg | (freq_data & 0x3FFF);
	freq_data_msb = reg | ((freq_data >> 14) & 0x3FFF);
	//设置频率是需要连续写入两次寄存器的值
    //第一次写入包含14个LSB
	//第二次写入包含14个MSB
	AD9833_SendData(ch,freq_data_lsb);
	AD9833_SendData(ch,freq_data_msb);
}

/*
 * @brief	AD9833_SendPhase
 * @param   ch:超声驱动的通道号
 * 			txdata:要发送的数据
 * @retval  none
 * @func	向AD9833发送指令
 */
static void AD9833_SetPhase(uint8_t ch,uint16_t reg, uint16_t val)
{
    uint16_t phase = reg;
    phase |= val;
    AD9833_SendData(ch,phase);
}

/*
 * @brief	:AD9833_SendWave
 * @param   ch:超声驱动的通道号
 * @param	WaveMode:输出波形
 *          AD9833_OUT_SINUS:正弦波
 *          AD9833_OUT_TRIANGLE:三角波
 *			AD9833_OUT_MSB:方波
 *			AD9833_OUT_MSB2:方波/2
 * @param	Freq_SFR:设置频率寄存器
 * @param	Freq_SFR:设置相位寄存器
 * @retval  none
 * @func	向AD9833发送波形输出指令
 */
static void AD9833_SetMode(uint8_t ch,uint16_t WaveMode)
{
    uint32_t val = 0;
    val = (WaveMode | 0x0000);
    AD9833_SendData(ch,val);
}

void DevAD9833Set_Init(void)
{
	uint8_t tag_i = 0;
	uint16_t ultra_param = 625;
	uint32_t freq = 0;

	DevAD9833IO_Init();
	if((DevParamRead.UltraParam.UltraParamChangeFlg[0] == 0xdddd) &&
	   (DevParamRead.UltraParam.UltraParamChangeFlg[1] == 0xeeee) &&
	   (DevParamRead.UltraParam.UltraParamChangeFlg[2] == 0xffff))
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			freq = AD9833STEP * DevParamRead.UltraParam.Freq_Param[tag_i];
			UltraParam_Old[tag_i] = DevParamRead.UltraParam.Freq_Param[tag_i];

			AD9833_SendData(tag_i,0x0100);						//芯片复位
			AD9833_SendData(tag_i,0x2100);						//配置寄存器连续写入模式
			AD9833_SetFreq(tag_i,AD9833_REG_FREQ0,freq);		//写入频率
			AD9833_SetPhase(tag_i,AD9833_REG_PHASE0,2048);			//写入相位
			AD9833_SetMode(tag_i,AD9833_OUT_TRIANGLE);			//写入波形类型并启动
		}
	}else
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			freq = AD9833STEP * ultra_param;
			UltraParam_Old[tag_i] = ultra_param;

			AD9833_SendData(tag_i,0x0100);						//芯片复位
			AD9833_SendData(tag_i,0x2100);						//配置寄存器连续写入模式
			AD9833_SetFreq(tag_i,AD9833_REG_FREQ0,freq);		//写入频率
			AD9833_SetPhase(tag_i,AD9833_REG_PHASE0,2048);			//写入相位
			AD9833_SetMode(tag_i,AD9833_OUT_TRIANGLE);			//写入波形类型并启动
		}
	}
}

void DevAD9833Set_Main(void)
{
	uint8_t tag_i = 0;
	uint32_t freq = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		if(UltraParam_Old[tag_i] != DevParamRead.UltraParam.Freq_Param[tag_i])
		{
			freq = DevParamRead.UltraParam.Freq_Param[tag_i] * AD9833STEP;
			UltraParam_Old[tag_i] = DevParamRead.UltraParam.Freq_Param[tag_i];

			AD9833_SetFreq(tag_i,AD9833_REG_FREQ0,freq);		//写入频率
		}else
		{
		}
	}
}
