/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

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

