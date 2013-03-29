

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

volatile int32_t uIP_RunTime = 0;//__IO == volatile //待处理，添加定时器支持
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
#if UIP_UDP
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }
#endif /* UIP_UDP */
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
  //
  // Wifi start!
  //
  bStatus = Wlan_Init(g_pWlanAdapter);
  if (!bStatus) 
  {
    TRACE("Wlan init failed!!\r\n");
    return;
  }
  
  if (!Get_KEY1()) 
  { // 启动时KEY1按下进入模块的设置模式
    
    // uIP Init
    uip_ipaddr_t ipaddr;
    
    WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
    uip_init();
    
    uip_ipaddr(ipaddr, 192, 168, 1, 2);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 192, 168, 1, 1);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);
    
    p_appcall = httpd_appcall;
    httpd_init();
    
    WiFiCmd_RunCommand ("set ssid CDDC");
    WiFiCmd_RunCommand ("set networktype infra");
    WiFiCmd_RunCommand ("set authmode open");
    WiFiCmd_RunCommand ("set encrymode wep");
    WiFiCmd_RunCommand ("set keyascii 12345");
  }
  else{ //work mode,use UDP
    // uIP Init
    uip_ipaddr_t ipaddr;
    
    WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
    uip_init();
    
    UDP_App_Init(); //初始化UDP应用
    
    uip_ipaddr(ipaddr, 192, 168, 1, 5);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 192, 168, 1, 1);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);
    
    WiFiCmd_RunCommand ("set ssid CDDC");
    WiFiCmd_RunCommand ("set networktype infra");
    WiFiCmd_RunCommand ("set authmode open");
    WiFiCmd_RunCommand ("set encrymode wep");
    WiFiCmd_RunCommand ("set keyascii 12345");
  }
  WiFiCmd_RunCommand ("ctrl connect");
}