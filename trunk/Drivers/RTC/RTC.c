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
// Created by YDGong 2011/12/08
// Project: BDU
// All rights reserved
//
#include "Project.h"

static SYSTEMTIME g_RealTime;

const UINT8 TAB_DATE[12] = {6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4,};
const UINT8 Month2Day_Tab[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;

BOOL Rtc_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	Rtc_IntrInit ();
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
		BKP_DeInit();
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
		RTC_SetPrescaler(32776);
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		RCC_ClearFlag();

		return FALSE;
	}
	else {
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();

		Rtc_ToTime();

		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();

		return TRUE;
	}
}

BOOL Rtc_IntrInit (void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return TRUE;
}

void Rtc_GetTime(PDATETIME pTime)
{
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	RTC_WaitForLastTask();

	// Copy time
	CopyMemory(pTime, &g_RealTime, sizeof(SYSTEMTIME));
	
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}

UINT8 Rtc_GetDate(PDATETIME pTime)
{
	// 1 for Sunday, 2 for Monday, 3 for Tuesday, 4 for Wednesday, 5 for Thursday,  6 for Friday, 7 for Saturday
	if (pTime->wMonth ==1 || pTime->wMonth == 2)   
		return (pTime->wDay + 2*(pTime->wMonth+12) + 3*(pTime->wMonth+13)/5 + pTime->wYear + ((pTime->wYear-1)/4) - (pTime->wYear-1)/100 + (pTime->wYear-1)/400) % 7 + 1;   
	else   
		return (pTime->wDay + 1 + 2*pTime->wMonth + 3*(pTime->wMonth+1)/5 + pTime->wYear + (pTime->wYear/4) - pTime->wYear/100 + pTime->wYear/400) % 7 + 1; 
}
void Rtc_GetDateTime(PDATETIME pTime)
{
	volatile UINT8 nDayOfWeek;
	
	Rtc_GetTime (pTime);
	nDayOfWeek = Rtc_GetDate (pTime);
	pTime->wDayOfWeek = nDayOfWeek;
}

void Rtc_ToTime(void)
{
	UINT32 count;
	UINT8 tmp, change = 0;

	g_RealTime.wYear = BKP_ReadBackupRegister(BKP_DR2);
	g_RealTime.wMonth = BKP_ReadBackupRegister(BKP_DR3);
	g_RealTime.wDay = BKP_ReadBackupRegister(BKP_DR4);
	g_RealTime.wDayOfWeek = BKP_ReadBackupRegister(BKP_DR5);

	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	count = RTC_GetCounter();

	while (count >= 0x0001517f) {
		change = 1;
		count -= 0x0001517f;
		if ((++g_RealTime.wDayOfWeek) >= 8)
			g_RealTime.wDayOfWeek = 1;

		if (g_RealTime.wMonth == 2) {
			if (g_RealTime.wYear % 4)
				tmp = 28;
			else
				tmp = 29;
		}
		else {
			tmp = Month2Day_Tab[g_RealTime.wMonth-1];
		}
		if ((++g_RealTime.wDay) > tmp) {
			g_RealTime.wDay = 1;

			if ((++g_RealTime.wMonth) > 12) {
				g_RealTime.wMonth = 1;

				if ((++g_RealTime.wYear) >= 100) {
					g_RealTime.wYear = 0;
				}
			}
		}
	}
	g_RealTime.wHour = count / 3600;
	g_RealTime.wMinute = (count % 3600) / 60;
	g_RealTime.wSecond = (count % 3600) % 60;

	if (change) {
		RTC_SetCounter(count);
		BKP_WriteBackupRegister(BKP_DR5, g_RealTime.wDayOfWeek);
		BKP_WriteBackupRegister(BKP_DR4, g_RealTime.wDay);
		BKP_WriteBackupRegister(BKP_DR3, g_RealTime.wMonth);
		BKP_WriteBackupRegister(BKP_DR2, g_RealTime.wYear);
	}
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}

BOOL Rtc_IsOneHourLater (PDATETIME pTime1, PDATETIME pTime2)
{
	BOOL bDayChanged = FALSE;
	int nMinutes = 0;

	if ((pTime2->wYear > pTime1->wYear) ||
		(pTime2->wMonth > pTime1->wMonth) ||
		(pTime2->wDay > pTime1->wDay))
	{
		bDayChanged = TRUE;
	}

	if (bDayChanged)
		nMinutes = (24 - pTime1->wHour - 1) * 60 + pTime2->wHour * 60 + pTime2->wMinute + (60 - pTime1->wMinute);
	else if (pTime2->wHour > pTime1->wHour)
		nMinutes = (pTime2->wHour - pTime1->wHour - 1) * 60 + pTime2->wMinute + (60 - pTime1->wMinute);

	return (nMinutes >= 60) ? TRUE : FALSE;
}

void Rtc_SetTime(PDATETIME pTime)
{
	UINT32 count;
	RTC_ITConfig(RTC_IT_SEC, DISABLE);

	RTC_WaitForLastTask();

	CopyMemory(&g_RealTime, pTime, sizeof(SYSTEMTIME));

	pTime->wDayOfWeek = g_RealTime.wDayOfWeek = Rtc_GetDate(pTime);

	BKP_WriteBackupRegister(BKP_DR5, g_RealTime.wDayOfWeek);
//        RTC_WaitForLastTask();
	BKP_WriteBackupRegister(BKP_DR4, g_RealTime.wDay);
//        RTC_WaitForLastTask();
	BKP_WriteBackupRegister(BKP_DR3, g_RealTime.wMonth);
//        RTC_WaitForLastTask();
	BKP_WriteBackupRegister(BKP_DR2, g_RealTime.wYear);
//        RTC_WaitForLastTask();

	count = g_RealTime.wHour * 3600 + g_RealTime.wMinute * 60 + g_RealTime.wSecond;
	RTC_WaitForLastTask();
	RTC_SetCounter(count);
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}

void Rtc_Tick(void)
{
	UINT8 tmp;

	if ((++g_RealTime.wSecond) > 59) {
		g_RealTime.wSecond = 0;
		if ((++g_RealTime.wMinute) > 59) {
			g_RealTime.wMinute = 0;
			if ((++g_RealTime.wHour) > 23) {
				g_RealTime.wHour = 0;
				if ((++g_RealTime.wDayOfWeek) >= 8)
					g_RealTime.wDayOfWeek = 1;
				BKP_WriteBackupRegister(BKP_DR5, g_RealTime.wDayOfWeek);

				if (g_RealTime.wMonth == 2) {
					if (g_RealTime.wYear % 4)
						tmp = 28;
					else
						tmp = 29;
				}
				else {
					tmp = Month2Day_Tab[g_RealTime.wMonth-1];
				}
				if ((++g_RealTime.wDay) > tmp) {
					g_RealTime.wDay = 1;
					if ((++g_RealTime.wMonth) > 12) {
						g_RealTime.wMonth = 1;

						if ((++g_RealTime.wYear) > 99) {
							g_RealTime.wYear = 0;
						}
						BKP_WriteBackupRegister(BKP_DR2, g_RealTime.wYear);
					}
					BKP_WriteBackupRegister(BKP_DR3, g_RealTime.wMonth);
				}
				BKP_WriteBackupRegister(BKP_DR4, g_RealTime.wDay);
			}
		}
	}
}
