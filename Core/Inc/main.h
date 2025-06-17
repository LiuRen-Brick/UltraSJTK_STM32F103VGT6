/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AD5160CH1_DATA_Pin GPIO_PIN_2
#define AD5160CH1_DATA_GPIO_Port GPIOE
#define AD5160CH1_CLK_Pin GPIO_PIN_3
#define AD5160CH1_CLK_GPIO_Port GPIOE
#define V36CH1_4_EN_Pin GPIO_PIN_4
#define V36CH1_4_EN_GPIO_Port GPIOE
#define UltraEn_CH5_Pin GPIO_PIN_6
#define UltraEn_CH5_GPIO_Port GPIOE
#define Temp_Sample_Pin GPIO_PIN_1
#define Temp_Sample_GPIO_Port GPIOC
#define Current_Front_Pin GPIO_PIN_2
#define Current_Front_GPIO_Port GPIOC
#define Current_After_Pin GPIO_PIN_3
#define Current_After_GPIO_Port GPIOC
#define MotorEn_CH1_Pin GPIO_PIN_4
#define MotorEn_CH1_GPIO_Port GPIOA
#define MotorEn_CH2_Pin GPIO_PIN_5
#define MotorEn_CH2_GPIO_Port GPIOA
#define MotorEn_CH3_Pin GPIO_PIN_6
#define MotorEn_CH3_GPIO_Port GPIOA
#define MotorEn_CH4_Pin GPIO_PIN_7
#define MotorEn_CH4_GPIO_Port GPIOA
#define MotorEn_CH5_Pin GPIO_PIN_4
#define MotorEn_CH5_GPIO_Port GPIOC
#define MotorEn_CH6_Pin GPIO_PIN_5
#define MotorEn_CH6_GPIO_Port GPIOC
#define MotorEn_CH7_Pin GPIO_PIN_0
#define MotorEn_CH7_GPIO_Port GPIOB
#define MotorEn_CH8_Pin GPIO_PIN_1
#define MotorEn_CH8_GPIO_Port GPIOB
#define UltraEn_CH1_Pin GPIO_PIN_8
#define UltraEn_CH1_GPIO_Port GPIOE
#define UltraEn_CH2_Pin GPIO_PIN_9
#define UltraEn_CH2_GPIO_Port GPIOE
#define UltraEn_CH3_Pin GPIO_PIN_10
#define UltraEn_CH3_GPIO_Port GPIOE
#define UltraEn_CH4_Pin GPIO_PIN_11
#define UltraEn_CH4_GPIO_Port GPIOE
#define AD5160CH2_CS_Pin GPIO_PIN_15
#define AD5160CH2_CS_GPIO_Port GPIOE
#define AD5160CH2_DATA_Pin GPIO_PIN_10
#define AD5160CH2_DATA_GPIO_Port GPIOB
#define AD5160CH2_CLK_Pin GPIO_PIN_12
#define AD5160CH2_CLK_GPIO_Port GPIOB
#define UltraEn_CH8_Pin GPIO_PIN_13
#define UltraEn_CH8_GPIO_Port GPIOB
#define UltraEn_CH7_Pin GPIO_PIN_14
#define UltraEn_CH7_GPIO_Port GPIOB
#define AD9833CH8_SCLK_Pin GPIO_PIN_8
#define AD9833CH8_SCLK_GPIO_Port GPIOD
#define AD9833CH8_SDATA_Pin GPIO_PIN_9
#define AD9833CH8_SDATA_GPIO_Port GPIOD
#define AD9833CH8_FSYNC_Pin GPIO_PIN_10
#define AD9833CH8_FSYNC_GPIO_Port GPIOD
#define AD9833CH7_SCLK_Pin GPIO_PIN_11
#define AD9833CH7_SCLK_GPIO_Port GPIOD
#define AD9833CH7_SDATA_Pin GPIO_PIN_12
#define AD9833CH7_SDATA_GPIO_Port GPIOD
#define AD9833CH7_FSYNC_Pin GPIO_PIN_13
#define AD9833CH7_FSYNC_GPIO_Port GPIOD
#define AD9833CH6_SCLK_Pin GPIO_PIN_7
#define AD9833CH6_SCLK_GPIO_Port GPIOC
#define AD9833CH6_SDATA_Pin GPIO_PIN_8
#define AD9833CH6_SDATA_GPIO_Port GPIOC
#define AD9833CH6_FSYNC_Pin GPIO_PIN_9
#define AD9833CH6_FSYNC_GPIO_Port GPIOC
#define AD9833CH5_SCLK_Pin GPIO_PIN_8
#define AD9833CH5_SCLK_GPIO_Port GPIOA
#define AD9833CH5_SDATA_Pin GPIO_PIN_9
#define AD9833CH5_SDATA_GPIO_Port GPIOA
#define AD9833CH5_FSYNC_Pin GPIO_PIN_10
#define AD9833CH5_FSYNC_GPIO_Port GPIOA
#define UltraEn_CH6_Pin GPIO_PIN_12
#define UltraEn_CH6_GPIO_Port GPIOA
#define V36CH5_8_EN_Pin GPIO_PIN_15
#define V36CH5_8_EN_GPIO_Port GPIOA
#define AD9833CH4_SCLK_Pin GPIO_PIN_12
#define AD9833CH4_SCLK_GPIO_Port GPIOC
#define AD9833CH4_SDATA_Pin GPIO_PIN_0
#define AD9833CH4_SDATA_GPIO_Port GPIOD
#define AD9833CH4_FSYNC_Pin GPIO_PIN_1
#define AD9833CH4_FSYNC_GPIO_Port GPIOD
#define AD9833CH3_SCLK_Pin GPIO_PIN_2
#define AD9833CH3_SCLK_GPIO_Port GPIOD
#define AD9833CH3_SDATA_Pin GPIO_PIN_3
#define AD9833CH3_SDATA_GPIO_Port GPIOD
#define AD9833CH3_FSYNC_Pin GPIO_PIN_4
#define AD9833CH3_FSYNC_GPIO_Port GPIOD
#define AD9833CH2_SCLK_Pin GPIO_PIN_5
#define AD9833CH2_SCLK_GPIO_Port GPIOD
#define AD9833CH2_SDATA_Pin GPIO_PIN_6
#define AD9833CH2_SDATA_GPIO_Port GPIOD
#define AD9833CH2_FSYNC_Pin GPIO_PIN_7
#define AD9833CH2_FSYNC_GPIO_Port GPIOD
#define AD9833CH1_SCLK_Pin GPIO_PIN_3
#define AD9833CH1_SCLK_GPIO_Port GPIOB
#define AD9833CH1_SDATA_Pin GPIO_PIN_4
#define AD9833CH1_SDATA_GPIO_Port GPIOB
#define AD9833CH1_FSYNC_Pin GPIO_PIN_6
#define AD9833CH1_FSYNC_GPIO_Port GPIOB
#define AD5160CH1_CS_Pin GPIO_PIN_1
#define AD5160CH1_CS_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
