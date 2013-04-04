/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "uIP_TASK.h"
#include "tapdev.h"
#include "clock-arch.h"

/* task priority */ 
#define STARTUP_TASK_PRIO     4
#define NET_TASK_PRIO         6
#define Wifi_RX_TASK_PRIO     8
#define Poll_TASK_PRIO        7

/* task stack size */ 
#define STARTUP_TASK_STK_SIZE  80  
#define NET_TASK_STK_SIZE      1024
#define Wifi_RX_TASK_STK_SIZE  256
#define Poll_TASK_STK_SIZE     100

static OS_STK  Startup_Task_STK[STARTUP_TASK_STK_SIZE];
static OS_STK  Net_Task_STK[NET_TASK_STK_SIZE];
static OS_STK  Wifi_RX_Task_STK[Wifi_RX_TASK_STK_SIZE];
static OS_STK  Poll_Task_STK[Poll_TASK_STK_SIZE];

static void Startup_Task(void* p_arg)
{
#if(OS_TASK_STAT_EN>0)
  OSStatInit(); 
#endif
  
  uip_mbox = OSMboxCreate((void *)0);
  
  /*TODO:Create Application Tasks Here*/
  OSTaskCreate(Net_Task,(void*)0,
               &Net_Task_STK[NET_TASK_STK_SIZE-1],
               NET_TASK_PRIO);	
  OSTaskCreate(Wifi_RX_Task,(void*)0,
               &Wifi_RX_Task_STK[Wifi_RX_TASK_STK_SIZE-1],
               Wifi_RX_TASK_PRIO);
  OSTaskCreate(Poll_Task,(void*)0,
               &Poll_Task_STK[Poll_TASK_STK_SIZE-1],
               Poll_TASK_PRIO);	
  
  OSTaskDel(OS_PRIO_SELF);  //删除启动任务
}

int main(void) 
{
  SystemInit();
  // Bsp Init
  Set_System();
  TraceInit();
  
  //Driver init
  KEY_Init();
  LED_Init ();
  Console_Init ();	
//  Console_Print("WiFi LED node!!\r\n");
   
  // Flash Storage init
  Flash_Init();
  //  EE_Init(); //Flash_Unlock already done in Flash_Init()
  GroupTable_Init();
  Init_NodeAttr();
  
  OSInit();//系统初始化	
  
  OSTaskCreate(Startup_Task,(void*)0,
               &Startup_Task_STK[STARTUP_TASK_STK_SIZE-1],
               STARTUP_TASK_PRIO);
  
  OSStart(); //任务开始运行  
  
  while(1);
}
