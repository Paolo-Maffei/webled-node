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

BOOL Buzzer_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO clock
	RCC_APB2PeriphClockCmd(
							RCC_APB2Periph_GPIOE,
							ENABLE
	                      );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Buzzer_Off();

	return TRUE;
}

BOOL Buzzer_On ()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_0);
	
	return TRUE;
}

BOOL Buzzer_Off ()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_0);
	
	return TRUE;
}
