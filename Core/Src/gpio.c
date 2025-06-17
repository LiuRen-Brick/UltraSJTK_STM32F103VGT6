/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, AD5160CH1_DATA_Pin|AD5160CH1_CLK_Pin|V36CH1_4_EN_Pin|UltraEn_CH5_Pin
                          |UltraEn_CH1_Pin|UltraEn_CH2_Pin|UltraEn_CH3_Pin|UltraEn_CH4_Pin
                          |AD5160CH2_CS_Pin|AD5160CH1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MotorEn_CH1_Pin|MotorEn_CH2_Pin|MotorEn_CH3_Pin|MotorEn_CH4_Pin
                          |AD9833CH5_SCLK_Pin|AD9833CH5_SDATA_Pin|AD9833CH5_FSYNC_Pin|UltraEn_CH6_Pin
                          |V36CH5_8_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MotorEn_CH5_Pin|MotorEn_CH6_Pin|AD9833CH6_SCLK_Pin|AD9833CH6_SDATA_Pin
                          |AD9833CH6_FSYNC_Pin|AD9833CH4_SCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MotorEn_CH7_Pin|MotorEn_CH8_Pin|AD5160CH2_DATA_Pin|AD5160CH2_CLK_Pin
                          |UltraEn_CH8_Pin|UltraEn_CH7_Pin|AD9833CH1_SCLK_Pin|AD9833CH1_SDATA_Pin
                          |AD9833CH1_FSYNC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, AD9833CH8_SCLK_Pin|AD9833CH8_SDATA_Pin|AD9833CH8_FSYNC_Pin|AD9833CH7_SCLK_Pin
                          |AD9833CH7_SDATA_Pin|AD9833CH7_FSYNC_Pin|AD9833CH4_SDATA_Pin|AD9833CH4_FSYNC_Pin
                          |AD9833CH3_SCLK_Pin|AD9833CH3_SDATA_Pin|AD9833CH3_FSYNC_Pin|AD9833CH2_SCLK_Pin
                          |AD9833CH2_SDATA_Pin|AD9833CH2_FSYNC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : AD5160CH1_DATA_Pin AD5160CH1_CLK_Pin AD5160CH2_CS_Pin AD5160CH1_CS_Pin */
  GPIO_InitStruct.Pin = AD5160CH1_DATA_Pin|AD5160CH1_CLK_Pin|AD5160CH2_CS_Pin|AD5160CH1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : V36CH1_4_EN_Pin UltraEn_CH5_Pin UltraEn_CH1_Pin UltraEn_CH2_Pin
                           UltraEn_CH3_Pin UltraEn_CH4_Pin */
  GPIO_InitStruct.Pin = V36CH1_4_EN_Pin|UltraEn_CH5_Pin|UltraEn_CH1_Pin|UltraEn_CH2_Pin
                          |UltraEn_CH3_Pin|UltraEn_CH4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : MotorEn_CH1_Pin MotorEn_CH2_Pin MotorEn_CH3_Pin MotorEn_CH4_Pin
                           UltraEn_CH6_Pin V36CH5_8_EN_Pin */
  GPIO_InitStruct.Pin = MotorEn_CH1_Pin|MotorEn_CH2_Pin|MotorEn_CH3_Pin|MotorEn_CH4_Pin
                          |UltraEn_CH6_Pin|V36CH5_8_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MotorEn_CH5_Pin MotorEn_CH6_Pin */
  GPIO_InitStruct.Pin = MotorEn_CH5_Pin|MotorEn_CH6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MotorEn_CH7_Pin MotorEn_CH8_Pin UltraEn_CH8_Pin UltraEn_CH7_Pin */
  GPIO_InitStruct.Pin = MotorEn_CH7_Pin|MotorEn_CH8_Pin|UltraEn_CH8_Pin|UltraEn_CH7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : AD5160CH2_DATA_Pin AD5160CH2_CLK_Pin AD9833CH1_SCLK_Pin AD9833CH1_SDATA_Pin
                           AD9833CH1_FSYNC_Pin */
  GPIO_InitStruct.Pin = AD5160CH2_DATA_Pin|AD5160CH2_CLK_Pin|AD9833CH1_SCLK_Pin|AD9833CH1_SDATA_Pin
                          |AD9833CH1_FSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : AD9833CH8_SCLK_Pin AD9833CH8_SDATA_Pin AD9833CH8_FSYNC_Pin AD9833CH7_SCLK_Pin
                           AD9833CH7_SDATA_Pin AD9833CH7_FSYNC_Pin AD9833CH4_SDATA_Pin AD9833CH4_FSYNC_Pin
                           AD9833CH3_SCLK_Pin AD9833CH3_SDATA_Pin AD9833CH3_FSYNC_Pin AD9833CH2_SCLK_Pin
                           AD9833CH2_SDATA_Pin AD9833CH2_FSYNC_Pin */
  GPIO_InitStruct.Pin = AD9833CH8_SCLK_Pin|AD9833CH8_SDATA_Pin|AD9833CH8_FSYNC_Pin|AD9833CH7_SCLK_Pin
                          |AD9833CH7_SDATA_Pin|AD9833CH7_FSYNC_Pin|AD9833CH4_SDATA_Pin|AD9833CH4_FSYNC_Pin
                          |AD9833CH3_SCLK_Pin|AD9833CH3_SDATA_Pin|AD9833CH3_FSYNC_Pin|AD9833CH2_SCLK_Pin
                          |AD9833CH2_SDATA_Pin|AD9833CH2_FSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : AD9833CH6_SCLK_Pin AD9833CH6_SDATA_Pin AD9833CH6_FSYNC_Pin AD9833CH4_SCLK_Pin */
  GPIO_InitStruct.Pin = AD9833CH6_SCLK_Pin|AD9833CH6_SDATA_Pin|AD9833CH6_FSYNC_Pin|AD9833CH4_SCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AD9833CH5_SCLK_Pin AD9833CH5_SDATA_Pin AD9833CH5_FSYNC_Pin */
  GPIO_InitStruct.Pin = AD9833CH5_SCLK_Pin|AD9833CH5_SDATA_Pin|AD9833CH5_FSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
