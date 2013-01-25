/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "tapdev.h"

void App_TaskCreateHook(OS_TCB *ptcb){}
void App_TaskDelHook(OS_TCB *ptcb){}
void App_TaskStatHook(void){}
void App_TCBInitHook(OS_TCB * os_tcb){}
void App_TaskSwHook(void){}
void App_TimeTickHook(void){}
			
void App_TaskIdleHook(void)
{
  if(!WifiInitFinished)
    return;
  uip_len = tapdev_read();    //从网络设备读取一个IP包,返回数据长度
  if (uip_len > 0)          //收到数据时长度会变化
  {
    MboxMsg = UIP_MBOX_RCV;
    OSMboxPost(uIP_MBOX,&MboxMsg);
  }
}

