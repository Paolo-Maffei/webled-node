

/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"

/*uIP  stack*/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
/*uIP的应用程序，设成函数指针，以便实现在开机时根据设置进入不同应用*/
void (*p_appcall)(void);
void (*p_udp_appcall)(void);

OS_EVENT *uip_mbox;

void Net_Task(void* p_arg)
{
  Set_uIP();
  char err, msg;
  while(1) 
  {
    msg = (char)OSMboxPend(uip_mbox,0,&err);
    switch(msg){
    case UIP_MBOX_RX:
    /* 处理IP数据包(只有校验通过的IP包才会被接收) */
        if(BUF->type == htons(UIP_ETHTYPE_IP)) 
        { //收到IP包
          uip_arp_ipin();        //去除以太网头结构，更新ARP表
          uip_input();           //IP包处理
          if (uip_len > 0) 
          {      //有带外回应数据
            uip_arp_out();      //加以太网头结构，在主动连接时可能要构造ARP请求
            tapdev_send();      //发送数据到以太网（设备驱动程序）
          }
        }
        /* 处理arp报文 */
        else if (BUF->type == htons(UIP_ETHTYPE_ARP)) 
        { //是ARP请求包
          uip_arp_arpin();        //如是是ARP回应，更新ARP表；如果是请求，构造回应数据包
          /*
          当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
          需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
          */
          if (uip_len > 0) 
          {      //是ARP请求，要发送回应
            tapdev_send();      //发ARP回应到以太网上
          }
        }
//#if UIP_UDP    //超时时主动发数据
//      for(int i = 0; i < UIP_UDP_CONNS; i++) 
//      {
//	uip_udp_periodic(i);
//	/* If the above function invocation resulted in data that
//	   should be sent out on the network, the global variable
//	   uip_len is set to a value > 0. */
//	if(uip_len > 0) {
//	  uip_arp_out();
//	  tapdev_send();
//	}
//      }
//#endif /* UIP_UDP */
        break;
    case UIP_MBOX_POLL:

      break;
      
    default:
    }
//    if(g_Console.ucState & CONSOLE_STATE_RECEIVED) 
//    {
//      WiFiCmd_RunCommand ((char*)g_Console.ucConsoleRxBuffer);
//      Console_ResetRx();
//    }
  }		
}

void Wifi_RX_Task(void *pdata)
{   
  while(1)
  {
    if(WIFI_GET_FLAG(g_pWlanAdapter, FLAG_PKTRCVED)) 
    {
      WIFI_CLR_FLAG(g_pWlanAdapter, FLAG_PKTRCVED);
      uip_len = tapdev_read();    //从网络设备读取一个IP包,返回数据长度
      if (uip_len > 0)          //收到数据时长度会变化
        OSMboxPost(uip_mbox,(void *)UIP_MBOX_RX);
    }
  }
}

static void Set_uIP()
{
  BOOL bStatus = FALSE;
  // Wifi start!
  bStatus = Wlan_Init(g_pWlanAdapter);
  if (!bStatus) 
  {
    TRACE("Wlan init failed!!\r\n");
    return;
  }
  
  uip_ipaddr_t ipaddr;
  
  WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
  if(0xFFFFFFFF == node_info.id)      //根据MAC地址生成节点ID，只在节点第一次上电时执行
  {
    int id = (int)uip_ethaddr.addr[5] + ((int)uip_ethaddr.addr[4]<<8) + ((int)uip_ethaddr.addr[3]<<16) + ((int)uip_ethaddr.addr[2]<<24);
    NodeAttr_SetID(id);
  }
  
  WiFiCmd_RunCommand ("set ssid CDDC");
  WiFiCmd_RunCommand ("set networktype infra");
  WiFiCmd_RunCommand ("set authmode open");
  WiFiCmd_RunCommand ("set encrymode wep");
  WiFiCmd_RunCommand ("set keyascii 12345");
  WiFiCmd_RunCommand ("ctrl connect");
  
  uip_init();
  
  if(0xFF == node_info.ipaddr[0] && 0xFF == node_info.ipaddr[1] && 0xFF == node_info.ipaddr[2] && 0xFF == node_info.ipaddr[3])
  {
    p_udp_appcall = dhcpc_appcall;
    dhcpc_init(uip_ethaddr.addr,6);
  }
  else
  {
    p_udp_appcall = WebLED_UDP_APPCALL; //启动WebLED应用
    WebLED_App_Init();                  //初始化WEBLED UDP应用
    
    uip_ipaddr(ipaddr, 192, 168, 1, 5);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 192, 168, 1, 1);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);
    
  }
}

void dhcpc_configured(const struct dhcpc_state *s)
{
  
  
}