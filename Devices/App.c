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
uint8_t WorkModule = SYNCMODE;
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

static void UltraWorkModule_SyncMode(void);
static void UltraWorkModule_AlternateMode(void);
static void UltraWorkModule_CycleMode(void);
static void UltraWorkModule_AlternateMode2(void);

/*系统初始化*/
void DevSystem_Init(void)
{
	DevParam_Init();
	DevAD9833Set_Init();
	DevAD5160Func_Init();
	DevADC1Func_Init();
	DevUart_Init();
	DevPwmFunc_Init();

	HAL_GPIO_WritePin(V36CH1_4_EN_GPIO_Port, V36CH1_4_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(V36CH5_8_EN_GPIO_Port, V36CH5_8_EN_Pin, GPIO_PIN_SET);
}

void AppMainFunc(void)
{
	uint8_t tag_i = 0;
	uint8_t mode = 0;

	if(WorkSta == DEVWORKSTART)
	{
#if 0
		for(tag_i = 0;tag_i < 8;tag_i++)
		{
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], UltraEnFlg[tag_i]);
			HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
		}
#else
		mode = WorkModule;

		switch(mode)
		{
		case SYNCMODE:
			/*同步模式 8探头同时工作*/
//			UltraWorkModule_SyncMode();
			break;

		case ALTERMODE:
			/*交替模式 四探头为一组，两两交替*/
			UltraWorkModule_AlternateMode();
			break;

		case CYCLEMODE:
			/*循环模式 8探头循环交替工作*/
//			UltraWorkModule_CycleMode();
			break;

		case ALTERMODE2:
			/*交替模式2 2探头为一组，交替运行*/
//			UltraWorkModule_AlternateMode2();
			break;

		default:
			UltraWorkModule_AlternateMode();
			break;
		}
#endif
	}else
	{
		/*震动马达、超声复位*/
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
		}
	}

	UltraParamReload();			//超声参数装载
}

void UltraParamReload(void)
{
	uint8_t tag_i = 0;

	/*参数修改后进行刷新*/
	if(ParamRefresh == 1)
	{
		MotorEnableFlg = DevMotor_StaGet();
		StimuTime = DevWork_StimuTimeGet();
		IdleTime = DevWork_IdleTimeGet();
		WorkModule =  DevWork_ModuleGet();

		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			UltraEnFlg[tag_i] = DevUltra_StatGet(tag_i);
		}

		ParamRefresh = 0;
	}
}

/*
 *
 *			8探头同步工作模式
 *
 */
static void UltraWorkModule_SyncMode(void)
{
	uint8_t tag_i = 0;

	if(UltraWorkTime < (StimuTime * 10))
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], UltraEnFlg[tag_i]);

			if(MotorEnableFlg == 1)
			{
				HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_SET);
			}
		}
	}else if((UltraWorkTime >= StimuTime * 10) && (UltraWorkTime < (StimuTime + IdleTime) * 10))
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
		}
	}else
	{
		UltraWorkTime = 0;
	}
}

/*
 *
 *			8探头两组交替工作模式(4个为一组)
 *
 */

static void UltraWorkModule_AlternateMode(void)
{
	uint8_t tag_i = 0;

	if(UltraWorkTime < (StimuTime * 10))
	{
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i++)
		{
			if((tag_i % 2) == 0)
			{
//				HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], (GPIO_PinState)UltraEnFlg[tag_i]);
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
//				HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], (GPIO_PinState)UltraEnFlg[tag_i]);
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
}

/*
 *
 *			8探头四组交替工作模式(2个为一组)
 *
 */

static void UltraWorkModule_AlternateMode2(void)
{
	uint8_t tag_i = 0;
	uint8_t group_id = 0;
	uint32_t idle_start,idle_stop;
	uint32_t alter_cycle = 0;

	alter_cycle = (IdleTime + StimuTime) / 4;
	group_id = UltraWorkTime / alter_cycle;
	idle_start = ((group_id + 4) * StimuTime / 4) + group_id * IdleTime;
	idle_stop = alter_cycle * (group_id + 1);

	if((UltraWorkTime > idle_start) && (UltraWorkTime <= idle_stop))
	{
		group_id = 4;
	}

	switch(group_id)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i ++)
		{
			if((tag_i == (group_id * 2)) || (tag_i == (group_id * 2 + 1)))
			{
				HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], (GPIO_PinState)UltraEnFlg[tag_i]);
				if(MotorEnableFlg == 1)
				{
					HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_SET);
				}
			}else
			{
				HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
			}
		}
		break;
	case 4:
		for(tag_i = 0;tag_i < ULTRACHNUM;tag_i ++)
		{
			HAL_GPIO_WritePin(Motor_Gpio_Port[tag_i], Motor_Gpio_Pin[tag_i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Ultra_Gpio_Port[tag_i], Ultra_Gpio_Pin[tag_i], GPIO_PIN_RESET);
		}
		break;
	default:
		break;
	}

	if(UltraWorkTime > IdleTime + StimuTime)
	{
		UltraWorkTime;
	}

}
/*
 *
 *			8探头循环工作模式
 *
 */

static void UltraWorkModule_CycleMode(void)
{
	uint32_t sum_time,ave_time = 0;
	static uint8_t sCycleCh = 0;

	sum_time = StimuTime + IdleTime;
	ave_time = sum_time / ULTRACHNUM;

	if(UltraWorkTime < StimuTime)
	{
		HAL_GPIO_WritePin(Ultra_Gpio_Port[sCycleCh], Ultra_Gpio_Pin[sCycleCh], UltraEnFlg[sCycleCh]);
		if(MotorEnableFlg == 1)
		{
			HAL_GPIO_WritePin(Motor_Gpio_Port[sCycleCh], Motor_Gpio_Pin[sCycleCh], GPIO_PIN_SET);
		}
	}else if((UltraWorkTime >= StimuTime) && (UltraWorkTime < ave_time))
	{
		HAL_GPIO_WritePin(Ultra_Gpio_Port[sCycleCh], Ultra_Gpio_Pin[sCycleCh], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Motor_Gpio_Port[sCycleCh], Motor_Gpio_Pin[sCycleCh], GPIO_PIN_RESET);
	}else
	{
		HAL_GPIO_WritePin(Ultra_Gpio_Port[sCycleCh], Ultra_Gpio_Pin[sCycleCh], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Motor_Gpio_Port[sCycleCh], Motor_Gpio_Pin[sCycleCh], GPIO_PIN_RESET);
		UltraWorkTime = 0;
		sCycleCh++;
	}

	if(sCycleCh >= ULTRACHNUM)
	{
		sCycleCh = 0;
	}
}
