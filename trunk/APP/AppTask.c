/******************************************************************************
* �ɶ��ﴴ�Ƽ�
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
  uip_len = tapdev_read();    //�������豸��ȡһ��IP��,�������ݳ���
  if (uip_len > 0)          //�յ�����ʱ���Ȼ�仯
  {
    MboxMsg = UIP_MBOX_RCV;
    OSMboxPost(uIP_MBOX,&MboxMsg);
  }
}

