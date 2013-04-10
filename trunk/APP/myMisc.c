#include "Project.h"
#include "uip.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"

void Random_Delay(char seed)//�����ʱ
{  
  volatile unsigned long t;
  if(NodeAttr_Is_Configed()) //IP��ַ�Ѿ����ã�ʹ��IP��ַ���1�ֽ���ʱ
    t = 72*seed*node_info.ipaddr[3];
  else                       //IP��ַ��Ϣδ����,ʹ��MAC��ַ���1�ֽ���ʱ
    t = 72*seed*uip_ethaddr.addr[5];
  while(t--);
}

int Is_Wifi_Ready(void)
{
  	WORD reg_val;
	If_ReadRegister(HOST_INT_CTRL_REG, &reg_val);
        return reg_val;
}