
#include "Project.h"

char name1[]="hellonode";
char name2[]="zhouqiang";

static void LED_Delay(t)
{
  __IO uint32_t i = 0;
  for(i ; i < t; i++)
  {
  }
}

char buf[8];
void main(void)
{
		// Bsp Init
	Set_System();

	TraceInit();
	Console_Init ();
	Flash_Init();
  
        Init_ADC();
        while(1)
        {
          ADC_GetValue(buf);
        }
        
        GPIO_InitTypeDef GPIO_InitStructure;
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        /* GPIO Configuration: LED1 */
	GPIO_InitStructure.GPIO_Pin = LED1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init (GPIOC, &GPIO_InitStructure);
        
        /* GPIO Configuration: LED2¡¢LED3¡¢LED4 */
        GPIO_InitStructure.GPIO_Pin = LED2 | LED3 | LED4;
        GPIO_Init (GPIOB, &GPIO_InitStructure);
        
        while(1)
        {
          LED_TurnOn(LED1);
          LED_Delay(5000);
          
          LED_TurnOff(LED1);
          LED_Delay(5000);
        }
        
//	GroupTable_Init();
//	GroupTable_Add(0xFFFFFFFF);
//	GroupTable_Add(0x87654321);
//	GroupTable_Init();
//        
////        Init_NodeAttr();
////        NodeAttr_SetID(255);
////        NodeAttr_SetName(name1,sizeof(name1));
////        NodeAttr_SetMode(12);
////        NodeAttr_SetStatus(22);
////         NodeAttr_SetName(name2,sizeof(name2));
////        int tmp = NodeAttr_GetID();
////        
//        
//        
//	if(0 != GroupTable_Exist(0xFFFFFFFF))
//		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	else
//		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	
//	if(0 != GroupTable_Exist(0x87654321))
//		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	else
//		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	
//	GroupTable_Del(0x87654321);
//  if(0 != GroupTable_Exist(0x87654321))
//		Console_Print("0x1234 Existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	else
//		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//
//  if(0 != GroupTable_Exist(0xFFFFFFFF))
//		Console_Print("0x1234 Existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	else
//		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	
//	GroupTable_Add(0x12433448);
//	GroupTable_Init();
//	if(0 != GroupTable_Exist(0x12433448))
//		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
//	else
//		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	
	while (1);
}
