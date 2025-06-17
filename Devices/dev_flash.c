/*
 * dev_flash.c
 *
 *  Created on: Jun 11, 2025
 *      Author: szt
 */
#include "App.h"

uint8_t FlashSaveFlg = 0;
static uint32_t FlashOffset = 0;

extern Device_Param DevParamRead;

HAL_StatusTypeDef DevFlash_Write(uint32_t addr,uint16_t *buff,uint8_t len)
{
	HAL_StatusTypeDef status = HAL_BUSY;
	uint8_t tag_i= 0;
	uint16_t writedata = 0;
	uint32_t writeaddr = addr;

	HAL_FLASH_Unlock();

	for(tag_i = 0;tag_i < len;tag_i++)
	{
		writedata = buff[tag_i];
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, writeaddr, (uint64_t)writedata);
		if(status != HAL_OK)
		{
			HAL_FLASH_Lock();
			return status;
		}else
		{
			writeaddr += 2;
		}
	}
	HAL_FLASH_Lock();

	return status;
}

void DevFlash_Read(uint32_t addr,uint16_t* buff,uint16_t len)
{
	uint8_t tag_i = 0;
	uint32_t readaddr = 0;

	HAL_FLASH_Unlock();

	readaddr = addr;
	for(tag_i = 0;tag_i < len;tag_i++)
	{
		*(buff + tag_i) = *(volatile uint32_t *)readaddr;
		readaddr += 2;
	}

	HAL_FLASH_Lock();
}

void DevParam_Init(void)
{
	uint32_t read_addr = FLASH_SLAVE_ULTRAPARAM;

	while(read_addr < (FLASH_SLAVE_ULTRAPARAM + PAGE_SIZE))
	{
		DevFlash_Read(read_addr,DevParamRead.UltraParamBuff,ULTRAPARAMSIZE);
		if(DevParamRead.UltraParam.WriteCount != 0xFFFFFFFF)
		{
			FlashOffset += (ULTRAPARAMSIZE * 2);
			read_addr = FLASH_SLAVE_ULTRAPARAM + FlashOffset;
		}else
		{
			if(read_addr == FLASH_SLAVE_ULTRAPARAM)
			{
				DevParamRead.UltraParam.WriteCount = 0;
				break;
			}
			FlashOffset -= (ULTRAPARAMSIZE * 2);
			read_addr = FLASH_SLAVE_ULTRAPARAM + FlashOffset;
			DevFlash_Read(read_addr,DevParamRead.UltraParamBuff,ULTRAPARAMSIZE);
			break;
		}
	}
}

void DevFlashWrite_Main(void)
{
	uint32_t sectorError = 0;
	uint32_t write_addr = FLASH_SLAVE_ULTRAPARAM;
	FLASH_EraseInitTypeDef eraseInitStruct;

	if(FlashSaveFlg == 1)
	{
		DevParamRead.UltraParam.WriteCount += 1;

		if(DevParamRead.UltraParam.WriteCount != 1)
		{
			FlashOffset += (ULTRAPARAMSIZE * 2);
			write_addr += FlashOffset;
		}
		if((write_addr + ULTRAPARAMSIZE * 2) > (FLASH_SLAVE_ULTRAPARAM + PAGE_SIZE))
		{
			HAL_FLASH_Unlock();

			eraseInitStruct.Banks = FLASH_BANK_2;
			eraseInitStruct.NbPages = 2;
			eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
			eraseInitStruct.PageAddress = FLASH_SLAVE_ULTRAPARAM;

			HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);

			write_addr = FLASH_SLAVE_ULTRAPARAM;
			FlashOffset = 0;
		}

		DevFlash_Write(write_addr,DevParamRead.UltraParamBuff,ULTRAPARAMSIZE);
		FlashSaveFlg = 0;
	}
}
