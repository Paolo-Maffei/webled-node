

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
OS_EVENT *led_mbox;
OS_EVENT *net_tick_mbox;

void Net_Task(void* p_arg)
{
//  struct timer periodic_timer, arp_timer;
//  timer_set(&periodic_timer, CLOCK_SECOND / 2);
//  timer_set(&arp_timer, CLOCK_SECOND * 10);
  
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
          {
            Random_Delay(10); 
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
            Random_Delay(10); 
            tapdev_send();      //发ARP回应到以太网上
          }
        }
        break;
    case UIP_MBOX_POLL:
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
	uip_udp_periodic(i);
	if(uip_len > 0)
        {
	  uip_arp_out();              
	  tapdev_send();
	}
      }
      break;
      
    default:
      break;
    }
  }		
}

void Wifi_RX_Task(void *pdata)
{   
  while(1)
  {
    if(WIFI_GET_FLAG(g_pWlanAdapter, FLAG_PKTRCVED)) 
    {
      OS_CPU_SR cpu_sr;
      OS_ENTER_CRITICAL();      
      WIFI_CLR_FLAG(g_pWlanAdapter, FLAG_PKTRCVED);
      uip_len = tapdev_read();    //从网络设备读取一个IP包,返回数据长度
      OS_EXIT_CRITICAL();
      
      if (uip_len > 0)          //收到数据时长度会变化
        OSMboxPost(uip_mbox,(void *)UIP_MBOX_RX);
    }
  }
}

unsigned char NetTickStatus = NET_TICK_STATUS_INIT;
void Net_Tick_Task(void *pdata)
{
  char err;
  while(1)
  {
     OSTimeDly(OS_TICKS_PER_SEC*20);
    //OSTimeDlyHMSM(0,0,20,0);
    for(int j =0; j <3; j++)  //重试3次
    {
      NetTickStatus = NET_TICK_STATUS_SEND;
      
#if UIP_UDP    //主动轮询连接发心跳数据包
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
        uip_udp_periodic(i);
        if(uip_len > 0) {
          Random_Delay(10); 
          uip_arp_out();
          tapdev_send();
        }
      }
#endif /* UIP_UDP */
      
      NetTickStatus = (char)OSMboxPend(net_tick_mbox,OS_TICKS_PER_SEC*j,&err);
      if( NET_TICK_STATUS_RECV == NetTickStatus )
        break; //收到回应的数据包
      else
      {
        if(2 <= j)//第3次仍未收到回复，认为网络已断，重启节点网络
        {
          BOOL bStatus = FALSE;
          do{
            bStatus = Wlan_Init(g_pWlanAdapter);
          }while(!bStatus); 

          do{
            WiFiCmd_RunCommand ("ctrl connect");
          }while( FLAG_MEDIA_CONNECTED != WIFI_GET_FLAG(g_pWlanAdapter, FLAG_MEDIA_CONNECTED) );
          
          break;
        }
        else   //小于两次则继续发包
          continue;
      }
     }
  }
}

void Periodic_Task(void *pdata)  //定时poll任务
{
  while(1)
  {
#if UIP_UDP    //超时时主动发数据
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
	uip_udp_periodic(i);
	if(uip_len > 0) {
          Random_Delay(10); 
	  uip_arp_out();
	  tapdev_send();
	}
      }
#endif /* UIP_UDP */
      OSTimeDlyHMSM(0,0,1,0);
  }
}

void LED_Task(void *pdata)
{
  char *status,err;
  while(1)
  {
    status = (char *)OSMboxPend(led_mbox,0,&err);  //get pwm info
    for(unsigned char i=1;i<5;i++)
    {
      if(status[i] != node_info.status[i])
      {
        LED_Transit(i,node_info.status[i],status[i]);
      }
    }
    NodeAttr_SetStatus(status); //灯光操作完毕，更新灯光和PWM状态
    FreeMemory(status);         //释放消息邮箱的动态内存
  }
}

static void Set_uIP()
{
  BOOL bStatus = FALSE;
  // Wifi start!
  do{
    bStatus = Wlan_Init(g_pWlanAdapter);
  }while(!bStatus); 
  
  do{
    WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);//获取MAC地址
  }while( 0 == (uip_ethaddr.addr[5] | uip_ethaddr.addr[4] | uip_ethaddr.addr[3] | uip_ethaddr.addr[2]) );
    
  WiFiCmd_RunCommand ("set ssid CDDC");
  WiFiCmd_RunCommand ("set networktype infra");
  WiFiCmd_RunCommand ("set authmode open");
  WiFiCmd_RunCommand ("set encrymode wep");
  WiFiCmd_RunCommand ("set keyascii 12345");
  do{
      //随机延时
    Random_Delay(10); 
    WiFiCmd_RunCommand ("ctrl connect");
  }while( FLAG_MEDIA_CONNECTED != WIFI_GET_FLAG(g_pWlanAdapter, FLAG_MEDIA_CONNECTED) );
  
  //开始初始化uip
  uip_ipaddr_t ipaddr;
  uip_init();
  
  if(!NodeAttr_Is_Configed())  //ID、IP地址信息未配置
  {
    //根据MAC地址生成并配置ID
    int id = ((int)uip_ethaddr.addr[2]<<24) + ((int)uip_ethaddr.addr[3]<<16) + ((int)uip_ethaddr.addr[4]<<8) + ((int)uip_ethaddr.addr[5]);
    NodeAttr_SetID(id);
    //配置IP地址信息
    p_udp_appcall = dhcpc_appcall;
    dhcpc_init(uip_ethaddr.addr,6);
    
    OSTaskCreate(Periodic_Task,(void*)0,      //创建Periodic_Task主动轮询发送任务
               &Periodic_Task_STK[Periodic_TASK_STK_SIZE-1],
               Periodic_TASK_PRIO);	
  }
  else
  {

    p_udp_appcall = WebLED_UDP_APPCALL; //启动WebLED应用
    WebLED_App_Init();                  //初始化WEBLED UDP应用
    
    uip_ipaddr(ipaddr, node_info.ipaddr[0],node_info.ipaddr[1],node_info.ipaddr[2],node_info.ipaddr[3]);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, node_info.gateway[0],node_info.gateway[1],node_info.gateway[2],node_info.gateway[3]);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, node_info.netmask[0],node_info.netmask[1],node_info.netmask[2],node_info.netmask[3]);
    uip_setnetmask(ipaddr);
  }
}
void dhcpc_configured(const struct dhcpc_state *s)
{
  char ip[4],gateway[4],netmask[4];
  
  ip[0] = uip_ipaddr1(s->ipaddr);
  ip[1] = uip_ipaddr2(s->ipaddr);
  ip[2] = uip_ipaddr3(s->ipaddr);
  ip[3] = uip_ipaddr4(s->ipaddr);
  gateway[0] = uip_ipaddr1(s->default_router);
  gateway[1] = uip_ipaddr2(s->default_router);
  gateway[2] = uip_ipaddr3(s->default_router);
  gateway[3] = uip_ipaddr4(s->default_router);  
  netmask[0] = uip_ipaddr1(s->netmask);
  netmask[1] = uip_ipaddr2(s->netmask);
  netmask[2] = uip_ipaddr3(s->netmask);
  netmask[3] = uip_ipaddr4(s->netmask);
  NodeAttr_SetIP(ip,gateway,netmask);
  
  NodeAttr_Set_Config_Status();  //标记地址已经配置
  
  //地址配置完毕，启动LED应用
  uip_ipaddr_t ipaddr;
  p_udp_appcall = WebLED_UDP_APPCALL; //启动WebLED应用
  WebLED_App_Init();                  //初始化WEBLED UDP应用

  uip_ipaddr(ipaddr, node_info.ipaddr[0],node_info.ipaddr[1],node_info.ipaddr[2],node_info.ipaddr[3]);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, node_info.gateway[0],node_info.gateway[1],node_info.gateway[2],node_info.gateway[3]);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, node_info.netmask[0],node_info.netmask[1],node_info.netmask[2],node_info.netmask[3]);
  uip_setnetmask(ipaddr);
  
  OSTaskDel(Periodic_TASK_PRIO);  //删除Poll_Task任务
}


//      
//        WORD TX_FRAME_STATUS,HOST_INT_CTRL,HOST_INT_CAUSE,HOST_INT_STATUS,HOST_INT_EVENT,HOST_INT_EVENT_MASK,HOST_INT_STATUS_MASK,HOST_INT_RESET_SELECT;
//        If_ReadRegister(TX_FRAME_STATUS_REG,&TX_FRAME_STATUS);
//        If_ReadRegister(HOST_INT_CTRL_REG,&HOST_INT_CTRL);
//        
//	If_ReadRegister(HOST_INT_CAUSE_REG, &HOST_INT_CAUSE);
//        If_ReadRegister(HOST_INT_STATUS_REG, &HOST_INT_EVENT);
//        If_ReadRegister(HOST_INT_EVENT_REG, &HOST_INT_STATUS);
//        If_ReadRegister(HOST_INT_EVENT_MASK_REG, &HOST_INT_EVENT_MASK);
//        If_ReadRegister(HOST_INT_STATUS_MASK_REG, &HOST_INT_STATUS_MASK);
//        If_ReadRegister(HOST_INT_RESET_SELECT_REG, &HOST_INT_RESET_SELECT);
//        
//        WORD CARD_INT_CAUSE,CARD_INT_STATUS,CARD_INT_EVENT,CARD_INT_EVENT_MASK,CARD_INT_STATUS_MASK,CARD_INT_RESET_SELECT;
//	If_ReadRegister(CARD_INT_CAUSE_REG, &CARD_INT_CAUSE);
//        If_ReadRegister(CARD_INT_STATUS_REG, &CARD_INT_EVENT);
//        If_ReadRegister(CARD_INT_EVENT_REG, &CARD_INT_STATUS);
//        If_ReadRegister(CARD_INT_EVENT_MASK_REG, &CARD_INT_EVENT_MASK);
//        If_ReadRegister(CARD_INT_STATUS_MASK_REG, &CARD_INT_STATUS_MASK);
//        If_ReadRegister(CARD_INT_RESET_SELECT_REG, &CARD_INT_RESET_SELECT);