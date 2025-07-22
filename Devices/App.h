/*
 * App.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef APP_H_
#define APP_H_

#include "dev_ad5160.h"
#include "dev_ad9833.h"
#include "dev_adc.h"
#include "dev_flash.h"
#include "dev_screen.h"
#include "dev_pwm.h"

#define DEVWORKSTOP		0
#define DEVWORKSTART	1

#define ULTRACHNUM		8
#define ULTRAPARAMSIZE  40

#define SYNCMODE	1
#define ALTERMODE	2
#define CYCLEMODE	3

typedef union
{
	struct
	{
		uint32_t WriteCount;
		uint16_t UltraParamChangeFlg[3];
		uint16_t Freq_Param[ULTRACHNUM];
		uint16_t VibraEnableFlg;
		uint16_t Vibra_Param;
		uint8_t  PluseDuty[ULTRACHNUM];				//脉冲占空比
		uint16_t PlusePeriod[ULTRACHNUM];		    //脉冲周期
		uint16_t StimuTime;							//刺激时间
		uint16_t IdleTime;							//空闲时间
		uint8_t  UltraEn[ULTRACHNUM];
		uint16_t UltraWorkModule;
		uint16_t UltraPowerLevel;
		uint16_t Reserved[5];
	}UltraParam;
	uint16_t UltraParamBuff[ULTRAPARAMSIZE];
}Device_Param;

extern uint8_t WorkSta;
extern uint8_t ParamRefresh;
extern uint32_t UltraWorkTime;
extern Device_Param DevParamRead;

extern float SampleTemp;

extern uint8_t FlashSaveFlg;

void DevSystem_Init(void);
void AppMainFunc(void);
void UltraParamReload(void);

#endif /* APP_H_ */
