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
 * @brief	AD9833_SendPhase
 * @param   ch:超声驱动的通道号
 * 			txdata:要发送的数据
 * @retval  none
 * @func	向AD9833发送指令
 */
static void AD9833_SendPhase(uint8_t ch,uint16_t reg, uint16_t val)
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
static void AD9833_SendWave(uint8_t ch,uint32_t WaveMode,uint32_t Freq_SFR,uint32_t Phase_SFR)
{
    uint32_t val = 0;
    val = (val | WaveMode | Freq_SFR | Phase_SFR);
    AD9833_SendData(ch,val);
}

/*
 * @brief	AD9833_SendFrequency
 * @param   ch:超声驱动的通道号
 * @param	WaveMode:输出波形
 * @param	Freq_SFR:设置相位寄存器
 * @retval  none
 * @func	向AD9833发送指令
 */
static void AD9833_SendFrequency(uint8_t ch,unsigned short reg, double fout)
{
	int frequence_LSB,frequence_MSB;
	double   frequence_mid,frequence_DATA;
	long int frequence_hex;
	//Fout = Fmclk / 2^28 * FREQREG
	frequence_mid=268435456/11.059;
	frequence_DATA=fout;
	frequence_DATA=frequence_DATA/1000000;
	frequence_DATA=frequence_DATA*frequence_mid;
	frequence_hex=frequence_DATA;
	frequence_LSB=frequence_hex;
	frequence_LSB=frequence_LSB&0x3fff;
	frequence_MSB=frequence_hex>>14;
	frequence_MSB=frequence_MSB&0x3fff;
	frequence_LSB=frequence_LSB|reg;
	frequence_MSB=frequence_MSB|reg;
	AD9833_SendData(ch,0x2100);
	//设置频率是需要连续写入两次寄存器的值
    //第一次写入包含14个LSB
	//第二次写入包含14个MSB
	AD9833_SendData(ch,frequence_LSB);
	AD9833_SendData(ch,frequence_MSB);
}

/*
 * @brief	AD9833_InitIo
 * @param   ch:超声驱动的通道号
 * @retval  none
 * @func	初始化AD9833
 */
static void DevAD9833_Reset(void)
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
 * @brief	AD9833_SetPara
 * @param   ch:超声驱动的通道号
 * @param	Freq_SFR:设置频率寄存器
 * @param	Freq:输出频率
 * @param	Phase_SFR:相位寄存器
 * @param	Phase:相位
 * @param	WaveMode:波形类型
 * @retval  none
 * @func	向AD9833发送参数指令
 */
static void DevAD9833_SetPara(uint8_t ch,uint32_t Freq_SFR,double Freq,uint32_t Phase_SFR,uint32_t Phase,uint32_t WaveMode)
{
	unsigned int Fsel,Psel;
	//第一次设置数据时要先复位
	AD9833_SendData(ch,0x0100);
	AD9833_SendData(ch,0x2100);
	AD9833_SendFrequency(ch,Freq_SFR,Freq);
	AD9833_SendPhase(ch,Phase_SFR,Phase);
	if(Freq_SFR == AD9833_REG_FREQ0)
	{
		Fsel = AD9833_FSEL0;
	}
	else
	{
		Fsel = AD9833_FSEL1;
	}
	if(Phase_SFR == AD9833_REG_PHASE0)
	{
		Psel = AD9833_PSEL0;
	}
	else
	{
		Psel = AD9833_PSEL1;
	}
	AD9833_SendWave(ch,WaveMode,Fsel,Psel);
}

void DevAD9833Set_Init(void)
{
	uint8_t tag_i = 0;
	uint16_t ultra_param = 625;
	uint32_t freq = 0;

	DevAD9833_Reset();
	if((DevParamRead.UltraParam.UltraParamChangeFlg[0] == 0xdddd) &&
	   (DevParamRead.UltraParam.UltraParamChangeFlg[1] == 0xeeee) &&
	   (DevParamRead.UltraParam.UltraParamChangeFlg[2] == 0xffff))
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			freq = AD9833STEP * DevParamRead.UltraParam.Freq_Param[tag_i];
			UltraParam_Old[tag_i] = DevParamRead.UltraParam.Freq_Param[tag_i];

			DevAD9833_SetPara(tag_i,AD9833_REG_FREQ0,freq,AD9833_REG_PHASE1,2048,AD9833_OUT_TRIANGLE);
		}
	}else
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			freq = AD9833STEP * ultra_param;
			UltraParam_Old[tag_i] = ultra_param;

			DevAD9833_SetPara(tag_i,AD9833_REG_FREQ0,freq,AD9833_REG_PHASE1,2048,AD9833_OUT_TRIANGLE);
		}
	}
}

void DevAd9833Set_Main(void)
{
	uint8_t tag_i = 0;
	uint32_t freq = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		if(UltraParam_Old[tag_i] != DevParamRead.UltraParam.Freq_Param[tag_i])
		{
			freq = DevParamRead.UltraParam.Freq_Param[tag_i] * AD9833STEP;
			UltraParam_Old[tag_i] = DevParamRead.UltraParam.Freq_Param[tag_i];

			DevAD9833_SetPara(tag_i,AD9833_REG_FREQ0,freq,AD9833_REG_PHASE1,2048,AD9833_OUT_TRIANGLE);
		}else
		{
		}
	}
}



