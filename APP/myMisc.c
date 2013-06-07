#include "Project.h"
#include "uip.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"

void Random_Delay(char seed)//随机延时
{  
  volatile unsigned long t;
  if(NodeAttr_Is_Configed()) //IP地址已经配置，使用IP地址最后1字节延时
    t = 72*seed*node_info.ipaddr[3];
  else                       //IP地址信息未配置,使用MAC地址最后1字节延时
    t = 72*seed*uip_ethaddr.addr[5];
  while(t--);
}

int Is_Wifi_Ready(void)
{
  	WORD reg_val;
	If_ReadRegister(HOST_INT_CTRL_REG, &reg_val);
        return reg_val;
}

static void _Delay(uint32_t t) //5000 per ms
{
  __IO uint32_t i = 0;
  for(i ; i < t; i++)
  {
  }
}

void DelayMS(unsigned int t)
{
  _Delay(2500*t);
}

void SoftReset(void) 
{  
  __set_FAULTMASK(1);      // 关闭所有中端
  NVIC_SystemReset();// 复位
}
