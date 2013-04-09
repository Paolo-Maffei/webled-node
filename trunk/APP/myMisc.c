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