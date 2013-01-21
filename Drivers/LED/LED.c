/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#include "Project.h"

BOOL LED_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED1 | LED2 | LED3 | LED4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init (GPIOC, &GPIO_InitStructure);

	// Turn off all LED first
	LED_TurnOff (LED1);
	LED_TurnOff (LED2);
	LED_TurnOff (LED3);
	LED_TurnOff (LED4);

	return TRUE;
}

BOOL LED_TurnOn (UINT16 usLed)
{
	GPIO_ResetBits (GPIOC, usLed);

	return TRUE;
}

BOOL LED_TurnOff (UINT16 usLed)
{
	GPIO_SetBits (GPIOC, usLed);

	return TRUE;
}

BOOL LED_Flash (UINT16 usLed)
{
	if (GPIO_ReadOutputDataBit(GPIOC, usLed) == Bit_RESET)
		GPIO_SetBits(GPIOC, usLed);
	else
		GPIO_ResetBits(GPIOC, usLed);

	return TRUE;
}
