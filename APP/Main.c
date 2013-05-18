/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "uIP_TASK.h"
#include "tapdev.h"
#include "clock-arch.h"

static void Startup_Task(void* p_arg)
{
#if(OS_TASK_STAT_EN>0)
  OSStatInit(); 
#endif
  
  uip_mbox = OSMboxCreate((void *)0);
  led_mbox = OSMboxCreate((void *)0);
  
  /*TODO:Create Application Tasks Here*/
  OSTaskCreate(Net_Task,(void*)0,
               &Net_Task_STK[NET_TASK_STK_SIZE-1],
               NET_TASK_PRIO);	
  OSTaskCreate(Wifi_RX_Task,(void*)0,
               &Wifi_RX_Task_STK[Wifi_RX_TASK_STK_SIZE-1],
               Wifi_RX_TASK_PRIO);
  OSTaskCreate(LED_Task,(void*)0,
               &LED_Task_STK[LED_TASK_STK_SIZE-1],
               LED_TASK_PRIO);
  
  OSTaskDel(OS_PRIO_SELF);  //删除启动任务
}

int main(void) 
{
  SystemInit();
  // Bsp Init
  Set_System();
  
  //Driver init
#ifdef  NODE_TYPE_PANEL    
  KEY_Init();  
#endif  //NODE_TYPE_PANEL 
  // Flash Storage init
  Flash_Init(); //Flash_Unlock done in Flash_Init()
  GroupTable_Init();
  Init_NodeAttr();

#ifndef  NODE_TYPE_PANEL   
  Init_ADC();
  LED_Init ();
#endif 
  OSInit();//系统初始化	
  
  OSTaskCreate(Startup_Task,(void*)0,
               &Startup_Task_STK[STARTUP_TASK_STK_SIZE-1],
               STARTUP_TASK_PRIO);
  
  OSStart(); //任务开始运行  
  
  while(1);
}
