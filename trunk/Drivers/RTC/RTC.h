/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#ifndef __RTC_H__
#define __RTC_H__
//
// Macros definitions
//

//
// Structs definitions
//
#pragma pack(1)

typedef struct {
	UINT16 wYear;
	UINT16 wMonth;
	UINT16 wDay;
	UINT16 wHour;
	UINT16 wMinute;
	UINT16 wSecond;
	UINT16 wDayOfWeek;
} DateTime, *PDATETIME;

#pragma pack()

//
// Exported functions declare
//
BOOL Rtc_Init(void);
BOOL Rtc_IntrInit (void);
void Rtc_GetTime(PDATETIME pTime);
UINT8 Rtc_GetDate(PDATETIME pTime);
void Rtc_GetDateTime(PDATETIME pTime);
void Rtc_ToTime(void);
void Rtc_SetTime(PDATETIME pTime);
void Rtc_Tick(void);
BOOL Rtc_IsOneHourLater (PDATETIME pTime1, PDATETIME pTime2);

#endif // __RTC_H__

