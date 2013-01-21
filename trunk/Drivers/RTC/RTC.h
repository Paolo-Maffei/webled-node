/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
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

