/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

//
// Created by csword 2011/12/08
// Project: UartWifi
// All rights reserved
//
#ifndef __FLASH_H__
#define __FLASH_H__
//
// Macros definitions
//
#if (defined(STM32F10X_LD) || defined(STM32F10X_MD))
#define FLASH_PAGE_SIZE    KBytes(1) //1024  1K
#elif (defined(STM32F10X_HD) || defined(STM32F10X_CL))
#define FLASH_PAGE_SIZE    KBytes(2) //2048  2K
#else
#error
#endif

//
// Exported functions declare
//
BOOL Flash_Init(void);
BOOL Flash_UnLock(void);
BOOL Flash_Read(UINT32 param_flashbase, PBYTE pRomData, UINT32 NumByteToRead);
BOOL Flash_Write(UINT32 param_flashbase, PBYTE pRomData, UINT32 NumByteToWrite);

#endif // __FLASH_H__

