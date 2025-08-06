/*
 * dev_screen.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#include "App.h"
#include "usart.h"

uint8_t ScreenRxBuff1[DEV_SCREEN_RXBUFF_SIZE] = {0};
uint8_t ScreenRxBuff2[DEV_SCREEN_RXBUFF_SIZE] = {0};

uint8_t *active_buff = ScreenRxBuff1;
uint8_t *passive_buff = ScreenRxBuff2;

uint8_t ScreenTxBuff[9] = {0xEE,0xB5,0x30,0x00,0x00,0xFF,0xFC,0xFF,0xFF};

typedef void (*DevScreenCmdFuncType)(uint8_t *buff);

static void DevScreen_StartorStop(uint8_t *buff);
static void DevScreen_SetUltraFreq(uint8_t *buff);
static void DevScreen_SetUltraVibra(uint8_t *buff);
static void DevScreen_SetUltraDuty(uint8_t *buff);
static void DevScreen_SetUltraPeriod(uint8_t *buff);
static void DevScreen_SetUltraWork(uint8_t *buff);
static void DevScreen_SetUltraChEn(uint8_t *buff);
static void DevScreen_SetUltraModule(uint8_t *buff);
static void DevScreen_SetPowerLevel(uint8_t * buff);

static const DevScreenCmdFuncType dev_screen_cmd[DevScreenCmdMax] =
{
	[DevScreenCmd_WorkStart] = DevScreen_StartorStop,
	[DevScreenCmd_SetUltraFreq] = DevScreen_SetUltraFreq,
	[DevScreenCmd_SetUltraVibra] = DevScreen_SetUltraVibra,
	[DevScreenCmd_SetUltraDuty] = DevScreen_SetUltraDuty,
	[DevScreenCmd_SetUltraPeriod] = DevScreen_SetUltraPeriod,
	[DevScreenCmd_SetUltraWork] = DevScreen_SetUltraWork,
	[DevScreenCmd_SetUltraChEn] = DevScreen_SetUltraChEn,
	[DevScreenCmd_SetUltraModule] = DevScreen_SetUltraModule,
	[DevScreenCmd_SetUltraPower] = DevScreen_SetPowerLevel,
};

/* @brief:	屏幕控制超声输出
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_StartorStop(uint8_t *buff)
{
	WorkSta = (buff[0] == 0) ? DEVWORKSTOP : DEVWORKSTART;
	UltraWorkTime = 0;
}

/* @brief:	屏幕设置驱动频率
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraFreq(uint8_t *buff)
{
	uint8_t tag_i = 0;
	uint8_t temp = 0;
	uint16_t freq_param_temp = 0;

	DevParamRead.UltraParam.UltraParamChangeFlg[0] = 0xdddd;
	DevParamRead.UltraParam.UltraParamChangeFlg[1] = 0xeeee;
	DevParamRead.UltraParam.UltraParamChangeFlg[2] = 0xffff;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		if((tag_i % 2) == 0)
		{
			temp = tag_i / 2 * 3;
			freq_param_temp = ((uint16_t)buff[temp] << 4) | ((buff[temp + 1] >> 4) & 0x0f);
		}else
		{
			temp = tag_i / 2 * 3 + 1;
			freq_param_temp = ((uint16_t)(buff[temp] & 0x0f) << 8) | buff[temp+1];
		}

		if(freq_param_temp != 0)
		{
			FlashSaveFlg = (DevParamRead.UltraParam.Freq_Param[tag_i] != freq_param_temp) ? 1 : FlashSaveFlg;
			DevParamRead.UltraParam.Freq_Param[tag_i] = freq_param_temp;
		}
	}
}

/* @brief:	屏幕设置震动等级
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraVibra(uint8_t *buff)
{
	if((DevParamRead.UltraParam.VibraEnableFlg != (uint16_t)buff[0]) ||
	   (DevParamRead.UltraParam.Vibra_Param != (uint16_t)buff[1]))
	{
		FlashSaveFlg = 1;
		DevParamRead.UltraParam.VibraEnableFlg = (uint16_t)buff[0];
		DevParamRead.UltraParam.Vibra_Param = (uint16_t)buff[1];
	}
}

/* @brief:	屏幕设置脉冲占空比
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraDuty(uint8_t *buff)
{
	uint8_t tag_i;
	uint8_t pluse_duty_temp = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		pluse_duty_temp = buff[tag_i];

		if(pluse_duty_temp != DevParamRead.UltraParam.PluseDuty[tag_i])
		{
			FlashSaveFlg = 1;
			if(pluse_duty_temp == 100)
			{
				DevParamRead.UltraParam.PluseDuty[tag_i] = 101;    //保证占空比100%时,PWM输出会溢出
			}else
			{
				DevParamRead.UltraParam.PluseDuty[tag_i] = pluse_duty_temp;
			}
			DevPwm_LoadDuty(tag_i,DevParamRead.UltraParam.PluseDuty[tag_i]);
		}
	}
}

/* @brief:	屏幕设置脉冲周期
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraPeriod(uint8_t *buff)
{
	uint8_t tag_i = 0;
	uint8_t temp = 0;
	uint16_t period_temp = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		if((tag_i % 2) == 0)
		{
			temp = tag_i / 2 * 3;
			period_temp = ((uint16_t)buff[temp] << 4) | ((buff[temp + 1] >> 4) & 0x0f);
		}else
		{
			temp = tag_i / 2 * 3 + 1;
			period_temp = ((uint16_t)(buff[temp] & 0x0f) << 8) | buff[temp+1];
		}

		if(DevParamRead.UltraParam.PlusePeriod[tag_i] != period_temp)
		{
			FlashSaveFlg = 1;
			DevParamRead.UltraParam.PlusePeriod[tag_i] = period_temp;
			DevPwm_LoadPeriod(tag_i,period_temp);
		}
	}
}

/* @brief:	屏幕设置刺激时长和脉冲时长
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraWork(uint8_t *buff)
{
	uint16_t stimu_temp = 0;
	uint16_t idle_temp = 0;

	stimu_temp = ((uint16_t)buff[0] << 8) | buff[1];
	idle_temp = ((uint16_t)buff[2] << 8) | buff[3];
	if((DevParamRead.UltraParam.StimuTime != stimu_temp) || (idle_temp != DevParamRead.UltraParam.IdleTime))
	{
		FlashSaveFlg = 1;
		DevParamRead.UltraParam.StimuTime = stimu_temp;
		DevParamRead.UltraParam.IdleTime = idle_temp;
	}
}

/* @brief:	屏幕设置超声通道使能
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraChEn(uint8_t *buff)
{
	uint8_t tag_i = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		if(DevParamRead.UltraParam.UltraEn[tag_i] != buff[tag_i])
		{
			FlashSaveFlg = 1;
			DevParamRead.UltraParam.UltraEn[tag_i] = buff[tag_i];
		}
	}
}

/* @brief:	屏幕设置设备工作模式
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetUltraModule(uint8_t * buff)
{
	if(DevParamRead.UltraParam.UltraWorkModule != buff[0])
	{
		FlashSaveFlg = 1;
		DevParamRead.UltraParam.UltraWorkModule = (buff[0] == 0) ? 2 : buff[0];
	}

}

/* @brief:	屏幕设置超声驱动电压
 * @param:	buff-指令参数
 * @retval: none
 */
static void DevScreen_SetPowerLevel(uint8_t * buff)
{
	if(DevParamRead.UltraParam.UltraPowerLevel != buff[0])
	{
		FlashSaveFlg = 1;
		DevParamRead.UltraParam.UltraPowerLevel = (buff[0] > 5) ? 0 : buff[0];

		DevAD5160_ValueLoad(DevParamRead.UltraParam.UltraPowerLevel);
	}
}

void DevUart_Init(void)
{
	 HAL_UART_Receive_DMA(DEV_SCREEN_UART_HANDLE, active_buff, DEV_SCREEN_RXBUFF_SIZE);
	 __HAL_UART_ENABLE_IT(DEV_SCREEN_UART_HANDLE,UART_IT_IDLE);
}

void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
	uint8_t cmd_id = 0;
	if((huart->Instance == USART3) && (RESET != __HAL_UART_GET_FLAG(DEV_SCREEN_UART_HANDLE,UART_FLAG_IDLE)))
	{
		HAL_UART_DMAStop(DEV_SCREEN_UART_HANDLE);
		uint8_t *temp_buff = passive_buff;
		passive_buff = active_buff;
		active_buff = temp_buff;

		if((passive_buff[0] == 0xEE) &&
		   (passive_buff[1] == 0xB5) &&
		   (passive_buff[15] == 0xFF) &&
		   (passive_buff[16] == 0xFC) &&
		   (passive_buff[17] == 0xFF) &&
		   (passive_buff[18] == 0xFF))
		{
			cmd_id = passive_buff[2] - 0xB0;
			if((cmd_id >= DevScreenCmd_WorkStart) && (cmd_id < DevScreenCmdMax))
			{
				dev_screen_cmd[cmd_id](&passive_buff[3]);
			}
		}
		HAL_UART_Receive_DMA(DEV_SCREEN_UART_HANDLE, active_buff, DEV_SCREEN_RXBUFF_SIZE);

		ParamRefresh = 1;
	}
}

void DevScreenSendData(uint8_t cmd,uint8_t val)
{
	ScreenTxBuff[2] = cmd;
	ScreenTxBuff[3] = val;

	HAL_UART_Transmit_DMA(DEV_SCREEN_UART_HANDLE, ScreenTxBuff, sizeof(ScreenTxBuff));
}

inline uint8_t DevMotor_StaGet(void)
{
	uint8_t motor_sta = 0;

	motor_sta = (DevParamRead.UltraParam.VibraEnableFlg == 0xFFFF) ? 1 : (uint8_t)DevParamRead.UltraParam.VibraEnableFlg;
	return motor_sta;
}

inline uint16_t DevWork_StimuTimeGet(void)
{
	uint16_t stimu_time = 0;
	stimu_time = (DevParamRead.UltraParam.StimuTime == 0xFFFF) ? 1500 : DevParamRead.UltraParam.StimuTime;
	return stimu_time;
}

inline uint16_t DevWork_IdleTimeGet(void)
{
	uint16_t idle_time = 0;

	idle_time = (DevParamRead.UltraParam.IdleTime == 0xFFFF) ? 2500 : DevParamRead.UltraParam.IdleTime;
	return idle_time;
}

inline uint8_t DevWork_ModuleGet(void)
{
	uint8_t work_module = 0;

	work_module = (DevParamRead.UltraParam.UltraWorkModule == 0xFFFF) ? 2 : DevParamRead.UltraParam.UltraWorkModule;

	return work_module;
}

inline uint8_t DevUltra_StatGet(uint8_t ch)
{
	uint8_t sta = (DevParamRead.UltraParam.UltraEn[ch] == 0xFF) ? 1 : DevParamRead.UltraParam.UltraEn[ch];

	return sta;
}
