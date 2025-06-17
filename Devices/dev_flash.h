/*
 * dev_flash.h
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */

#ifndef DEV_FLASH_H_
#define DEV_FLASH_H_

#define PAGE_SIZE					2048*2
#define FLASH_SLAVE_ULTRAPARAM		0x08080000			//脉冲周期、占空比调整参数;超声配置参数--驱动频率、震动等级、参数锁定;功能选择

HAL_StatusTypeDef DevFlash_Write(uint32_t addr,uint16_t *buff,uint8_t len);
void DevFlash_Read(uint32_t addr,uint16_t* buff,uint16_t len);

void DevParam_Init(void);
void DevFlashWrite_Main(void);

#endif /* DEV_FLASH_H_ */
