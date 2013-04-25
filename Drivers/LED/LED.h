/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#ifndef __LED_H__
#define __LED_H__
//
// Macros definitions
//

// Alias name
#define LED1			GPIO_Pin_6
#define LED2			GPIO_Pin_15
#define LED3			GPIO_Pin_14
#define LED4			GPIO_Pin_13

extern uint16_t TimerPeriod;
//
// Exported functions declare
//
BOOL LED_Init (void);
BOOL LED_TurnOn (UINT16 usLed);
BOOL LED_TurnOff (UINT16 usLed);
BOOL LED_Update(char *status);
BOOL LED_Flash (UINT16 usLed);

#endif // __LED_H__

