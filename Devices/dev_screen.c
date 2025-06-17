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

typedef void (*DevScreenCmdFuncType)(uint8_t *buff);

static void DevScreen_StartorStop(uint8_t *buff);
static void DevScreen_SetUltraFreq(uint8_t *buff);
static void DevScreen_SetUltraVibra(uint8_t *buff);
static void DevScreen_SetUltraDuty(uint8_t *buff);
static void DevScreen_SetUltraPeriod(uint8_t *buff);
static void DevScreen_SetUltraWork(uint8_t *buff);

static const DevScreenCmdFuncType dev_screen_cmd[DevScreenCmdMax] =
{
	[DevScreenCmd_WorkStart] = DevScreen_StartorStop,
	[DevScreenCmd_SetUltraFreq] = DevScreen_SetUltraFreq,
	[DevScreenCmd_SetUltraVibra] = DevScreen_SetUltraVibra,
	[DevScreenCmd_SetUltraDuty] = DevScreen_SetUltraDuty,
	[DevScreenCmd_SetUltraPeriod] = DevScreen_SetUltraPeriod,
	[DevScreenCmd_SetUltraWork] = DevScreen_SetUltraWork,
};

static void DevScreen_StartorStop(uint8_t *buff)
{
	WorkSta = (buff[0] == 0) ? DEVWORKSTOP : DEVWORKSTART;
	UltraWorkTime = 0;
}

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

static void DevScreen_SetUltraDuty(uint8_t *buff)
{
	uint8_t tag_i;
	uint8_t pluse_duty_temp = 0;

	for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
	{
		pluse_duty_temp = buff[tag_i];

		if(pluse_duty_temp != DevParamRead.UltraParam.PluseDuty[tag_i])
		{
			DevParamRead.UltraParam.PluseDuty[tag_i] = pluse_duty_temp;
			FlashSaveFlg = 1;
		}
	}
}

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

		FlashSaveFlg = (DevParamRead.UltraParam.PlusePeriod[tag_i] != period_temp) ? 1 : FlashSaveFlg;
		DevParamRead.UltraParam.PlusePeriod[tag_i] = period_temp;
	}
}

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
