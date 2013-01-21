#include "Project.h"

BOOL KEY_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO clock
	RCC_APB2PeriphClockCmd(
							RCC_APB2Periph_GPIOB,
							ENABLE
	                      );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	return TRUE;
}

BOOL Get_KEY1()
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
}
BOOL Get_KEY2()
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
}