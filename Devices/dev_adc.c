/*
 * dev_adc.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#include "App.h"
#include "adc.h"

#define DEV_ADC			&hadc1
#define DEV_LEN			5
#define ADC_DMA_CH		DMA_FLAG_TC1
#define TEMPTURENUM		161
#define FILTERNUM		15

const float INP_HWTemp_R[TEMPTURENUM] =
{
	   324.0f,    333.0f,    342.0f,    351.0f,    361.0f,    371.0f,    382.0f,	393.0f,	   404.0f,    415.0f,
	   427.0f,    439.0f,    451.0f,    464.0f,    478.0f,    491.0f,    505.0f,    520.0f,    535.0f,    551.0f,
	   547.0f,    584.0f,    601.0f,    619.0f,    637.0f,    657.0f,    676.0f,    697.0f,    718.0f,    740.0f,
	   763.0f,    787.0f,    811.0f,    837.0f,    863.0f,    890.0f,    919.0f,    948.0f,    979.0f,   1011.0f,
	  1044.0f,   1078.0f,   1113.0f,   1150.0f,   1189.0f,   1228.0f,   1270.0f,   1313.0f,   1358.0f,   1404.0f,
	  1452.0f,   1503.0f,   1555.0f,   1610.0f,   1666.0f,   1725.0f,   1786.0f,   1850.0f,   1971.0f,   1986.0f,
	  2058.0f,   2133.0f,   2211.0f,   2293.0f,   2377.0f,   2466.0f,   2558.0f,   2654.0f,   2754.0f,   2859.0f,
	  2968.0f,   3081.0f,   3200.0f,   3324.0f,   3453.0f,   3588.0f,   3728.0f,   3875.0f,   4029.0f,   4189.0f,
	  4357.0f,   4532.0f,   4716.0f,   4907.0f,   5108.0f,   5318.0f,   5537.0f,   5765.0f,   6008.0f,   6259.0f,
	  6523.0f,   6800.0f,   7090.0f,   7394.0f,   7712.0f,   8047.0f,   8397.0f,   8765.0f,   9151.0f,   9557.0f,
	 10000.0f,  10431.0f,  10901.0f,  11395.0f,  11915.0f,  12461.0f,  13036.0f,  13641.0f,  14277.0f,  14947.0f,
	 15652.0f,  16395.0f,  17117.0f,  18000.0f,  18868.0f,  19783.0f,  20746.0f,  21762.0f,  22833.0f,  23962.0f,
	 25152.0f,  26408.0f,  27732.0f,  29128.0f,  30601.0f,  32116.0f,  33795.0f,  35524.0f,  37349.0f,  39274.0f,
	 41306.0f,  43450.0f,  45712.0f,  48100.0f,  50620.0f,  53280.0f,  56087.0f,  59052.0f,  62183.0f,  65489.0f,
	 68982.0f,  72672.0f,  76574.0f,  80699.0f,  85063.0f,  89682.0f,  94573.0f,  99756.0f, 105252.0f, 111084.0f,
	117280.0f, 123866.0f, 130877.0f, 138347.0f, 146315.0f, 154827.0f, 163931.0f, 173681.0f, 184139.0f, 198372.0f,
	207454.0f,
};

const float OUT_HWTemp_T[TEMPTURENUM] =
{
	125.0f, 124.0f, 123.0f, 122.0f, 121.0f,	120.0f,	119.0f,	118.0f,	117.0f,	116.0f,
	115.0f,	114.0f,	113.0f,	112.0f,	111.0f,	110.0f,	109.0f,	108.0f,	107.0f,	106.0f,
	105.0f,	104.0f,	103.0f,	102.0f,	101.0f,	100.0f,	 99.0f,	 98.0f,	 97.0f,  96.0f,
	 95.0f,	 94.0f,	 93.0f,	 92.0f,	 91.0f,	 90.0f,	 89.0f,	 88.0f,  87.0f,	 86.0f,
	 85.0f,	 84.0f,	 83.0f,	 82.0f,	 81.0f,	 80.0f,	 79.0f,	 78.0f,	 77.0f,	 76.0f,
	 75.0f,	 74.0f,	 73.0f,	 72.0f,	 71.0f,	 70.0f,	 69.0f,	 68.0f,	 67.0f,	 66.0f,
	 65.0f,	 64.0f,	 63.0f,	 62.0f,	 61.0f,	 60.0f,	 59.0f,	 58.0f,	 57.0f,	 56.0f,
	 55.0f,	 54.0f,	 53.0f,  52.0f,	 51.0f,	 50.0f,	 49.0f,	 48.0f,	 47.0f,	 46.0f,
	 45.0f,	 44.0f,	 43.0f,	 42.0f,	 41.0f,	 40.0f,	 39.0f,	 38.0f,	 37.0f,	 36.0f,
	 35.0f,  34.0f,	 33.0f,	 32.0f,	 31.0f,	 30.0f,	 29.0f,	 28.0f,	 27.0f,	 26.0f,
	 25.0f,	 24.0f,	 23.0f,	 22.0f,	 21.0f,	 20.0f,	 19.0f,	 18.0f,	 17.0f,	 16.0f,
	 15.0f,	 14.0f,	 13.0f,	 12.0f,	 11.0f,	 10.0f,	  9.0f,	  8.0f,	  7.0f,	  6.0f,
	  5.0f,	  4.0f,	  3.0f,	  2.0f,	  1.0f,	  0.0f,	 -1.0f,	 -2.0f,  -3.0f,	 -4.0f,
	 -5.0f,	 -6.0f,	 -7.0f,	 -8.0f,	 -9.0f,	-10.0f,	-11.0f,	-12.0f,	-13.0f,	-14.0f,
	-15.0f,	-16.0f,	-17.0f, -18.0f,	-19.0f,	-20.0f, -21.0f,	-22.0f,	-23.0f,	-24.0f,
	-25.0f,	-26.0f,	-27.0f,	-28.0f,	-29.0f,	-30.0f,	-31.0f,	-32.0f,	-33.0f,	-34.0f,
	-35.0f,
};

static float TemptureVol_Temp[FILTERNUM] =
{
	1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f, 1.65f
};

static float CurrentFrontVol_Temp[FILTERNUM];
static float CurrentAfterVol_Temp[FILTERNUM];
static uint16_t ADC1SampleVal[DEV_LEN] = {0};

float SampleTemp;
float CurrentFront;
float CurrentAfter;
float CoreTempture;

static float SampleVolFilter(float* buff,uint8_t len);

float look1_iflf_binlxpw(float u0, const float bp0[], const float table[], uint32_t maxIndex)
{
  float frac;
  uint32_t iRght;
  uint32_t iLeft;
  uint32_t bpIdx;

  if (u0 <= bp0[0U])
  {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  }
  else if (u0 < bp0[maxIndex])
  {
    /* Binary Search */
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U)
    {
      if (u0 < bp0[bpIdx])
      {
        iRght = bpIdx;
      }
      else
      {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  }
  else
  {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  return (table[iLeft + 1U] - table[iLeft]) * frac + table[iLeft];
}

void DevADC1Func_Init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1SampleVal, DEV_LEN);
}

void DevADC1Func_Main(void)
{
	float gain = 1.0f;
	float TemptureVol,CurrentFrontVol,CurrentAfterVol,CoreVol_Temp,TemptureRes;
	static uint8_t sCount = 0;
	static uint8_t sTempOver = 0;

	gain = ADC1SampleVal[4] * 3.3 / 4095 / 1.2;
	TemptureVol_Temp[sCount] = (float)ADC1SampleVal[0] * 3.3f / 4095.0f;
	CurrentFrontVol_Temp[sCount] = (float)ADC1SampleVal[1] * 3.3f / 4095.0f  / gain;
	CurrentAfterVol_Temp[sCount] = (float)ADC1SampleVal[2] * 3.3f / 4095.0f  / gain;
	CoreVol_Temp =  (float)ADC1SampleVal[3] * 3.3f / 4095.0f / gain;

	sCount++;
	if(sCount >= 15)
	{
		sCount = 0;
	}

	TemptureVol = SampleVolFilter(TemptureVol_Temp,FILTERNUM);
	CurrentFrontVol = SampleVolFilter(CurrentFrontVol_Temp,FILTERNUM);
	CurrentAfterVol = SampleVolFilter(CurrentAfterVol_Temp,FILTERNUM);
	TemptureRes = (10000.0f * TemptureVol) / (3.3f - TemptureVol);

	SampleTemp = look1_iflf_binlxpw(TemptureRes,INP_HWTemp_R,OUT_HWTemp_T,TEMPTURENUM - 1);
	CurrentFront = (CurrentFrontVol - 1.14) / 50.0f * 100.0f;			//CurrentFrontVol * GAIN(50) / 10mΩ(0.01);
	CurrentAfter = CurrentAfterVol / 4.7f * 50.0f * 100.0f;
	CoreTempture = (1.43 - CoreVol_Temp) / 0.0043f + 25.0f;

	/*过温检测，超过80℃屏幕提示报警*/
	if((SampleTemp > 80.0f) && (sTempOver == 0))
	{
		sTempOver = 1;
		DevScreenSendData(0x30,sTempOver);
	}else if((SampleTemp < 77.0f) && (sTempOver == 1))
	{
		sTempOver = 0;
		DevScreenSendData(0x30,sTempOver);
	}
}

static float SampleVolFilter(float* buff,uint8_t len)
{
	float vol_max,vol_min;
	float vol_sum = 0;
	uint8_t tag_i = 0;

	vol_max = buff[0];
	vol_min = buff[0];

	for(tag_i = 0;tag_i < len;tag_i++)
	{
		if(buff[tag_i] < vol_min)
		{
			vol_min = buff[tag_i];
		}else if(buff[tag_i] > vol_max)
		{
			vol_max = buff[tag_i];
		}

		vol_sum += buff[tag_i];
	}

	return (vol_sum - vol_max - vol_min) / (len -2);
}

