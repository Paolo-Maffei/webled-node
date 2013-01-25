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

/* task stack size */ 
#define STARTUP_TASK_STK_SIZE 80  
#define NET_TASK_STK_SIZE     1024

static OS_STK  Startup_Task_STK[STARTUP_TASK_STK_SIZE];
static OS_STK  Net_Task_STK[NET_TASK_STK_SIZE];



static void Startup_Task(void* p_arg)
{
	// Bsp Init
	Set_System();
	TraceInit();

	Board_Init ();
	// Drivers init
	Flash_Init();
        EE_Init(); 	//Flash_Unlock already done in Flash_Init()
	KEY_Init();
	LED_Init ();
	Console_Init ();
	
	Console_Print("WiFi Hello world!!\r\n");
	

#if(OS_TASK_STAT_EN>0)
	OSStatInit(); 
#endif

	/*TODO:Create Application Tasks Here*/
	OSTaskCreate(Net_Task,(void*)0,
				&Net_Task_STK[NET_TASK_STK_SIZE-1],
				NET_TASK_PRIO);	
	 
	OSTaskDel(OS_PRIO_SELF);  //删除启动任务
}

int main(void) 
{
#ifdef BUILD_FOR_WRITEFIRMWARE
	If_WriteFirmware ();
	while (1);
#else
	OSInit();//系统初始化	
   	
	OSTaskCreate(Startup_Task,(void*)0,
				&Startup_Task_STK[STARTUP_TASK_STK_SIZE-1],
				STARTUP_TASK_PRIO);

	OSStart(); //任务开始运行  
#endif

	while(1);
	
  return 0;
}
