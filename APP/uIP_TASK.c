

/******************************************************************************
* �ɶ��ﴴ�Ƽ�
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
/*uIP��Ӧ�ó�����ɺ���ָ�룬�Ա�ʵ���ڿ���ʱ�������ý��벻ͬӦ��*/
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
    /* ����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) */
        if(BUF->type == htons(UIP_ETHTYPE_IP)) 
        { //�յ�IP��
          uip_arp_ipin();        //ȥ����̫��ͷ�ṹ������ARP��
          uip_input();           //IP������
          if (uip_len > 0) 
          {
            Random_Delay(10); 
            uip_arp_out();      //����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
            tapdev_send();      //�������ݵ���̫�����豸��������
          }
        }
        /* ����arp���� */
        else if (BUF->type == htons(UIP_ETHTYPE_ARP)) 
        { //��ARP�����
          uip_arp_arpin();        //������ARP��Ӧ������ARP����������󣬹����Ӧ���ݰ�
          /*
          ������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
          ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
          */
          if (uip_len > 0) 
          {      //��ARP����Ҫ���ͻ�Ӧ
            Random_Delay(10); 
            tapdev_send();      //��ARP��Ӧ����̫����
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
      uip_len = tapdev_read();    //�������豸��ȡһ��IP��,�������ݳ���
      OS_EXIT_CRITICAL();
      
      if (uip_len > 0)          //�յ�����ʱ���Ȼ�仯
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
    for(int j =0; j <3; j++)  //����3��
    {
      NetTickStatus = NET_TICK_STATUS_SEND;
      
#if UIP_UDP    //������ѯ���ӷ��������ݰ�
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
        break; //�յ���Ӧ�����ݰ�
      else
      {
        if(2 <= j)//��3����δ�յ��ظ�����Ϊ�����Ѷϣ������ڵ�����
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
        else   //С���������������
          continue;
      }
     }
  }
}

void Periodic_Task(void *pdata)  //��ʱpoll����
{
  while(1)
  {
#if UIP_UDP    //��ʱʱ����������
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
    NodeAttr_SetStatus(status); //�ƹ������ϣ����µƹ��PWM״̬
    FreeMemory(status);         //�ͷ���Ϣ����Ķ�̬�ڴ�
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
    WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);//��ȡMAC��ַ
  }while( 0 == (uip_ethaddr.addr[5] | uip_ethaddr.addr[4] | uip_ethaddr.addr[3] | uip_ethaddr.addr[2]) );
    
  WiFiCmd_RunCommand ("set ssid CDDC");
  WiFiCmd_RunCommand ("set networktype infra");
  WiFiCmd_RunCommand ("set authmode open");
  WiFiCmd_RunCommand ("set encrymode wep");
  WiFiCmd_RunCommand ("set keyascii 12345");
  do{
      //�����ʱ
    Random_Delay(10); 
    WiFiCmd_RunCommand ("ctrl connect");
  }while( FLAG_MEDIA_CONNECTED != WIFI_GET_FLAG(g_pWlanAdapter, FLAG_MEDIA_CONNECTED) );
  
  //��ʼ��ʼ��uip
  uip_ipaddr_t ipaddr;
  uip_init();
  
  if(!NodeAttr_Is_Configed())  //ID��IP��ַ��Ϣδ����
  {
    //����MAC��ַ���ɲ�����ID
    int id = ((int)uip_ethaddr.addr[2]<<24) + ((int)uip_ethaddr.addr[3]<<16) + ((int)uip_ethaddr.addr[4]<<8) + ((int)uip_ethaddr.addr[5]);
    NodeAttr_SetID(id);
    //����IP��ַ��Ϣ
    p_udp_appcall = dhcpc_appcall;
    dhcpc_init(uip_ethaddr.addr,6);
    
    OSTaskCreate(Periodic_Task,(void*)0,      //����Periodic_Task������ѯ��������
               &Periodic_Task_STK[Periodic_TASK_STK_SIZE-1],
               Periodic_TASK_PRIO);	
  }
  else
  {

    p_udp_appcall = WebLED_UDP_APPCALL; //����WebLEDӦ��
    WebLED_App_Init();                  //��ʼ��WEBLED UDPӦ��
    
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
  
  NodeAttr_Set_Config_Status();  //��ǵ�ַ�Ѿ�����
  
  //��ַ������ϣ�����LEDӦ��
  uip_ipaddr_t ipaddr;
  p_udp_appcall = WebLED_UDP_APPCALL; //����WebLEDӦ��
  WebLED_App_Init();                  //��ʼ��WEBLED UDPӦ��

  uip_ipaddr(ipaddr, node_info.ipaddr[0],node_info.ipaddr[1],node_info.ipaddr[2],node_info.ipaddr[3]);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, node_info.gateway[0],node_info.gateway[1],node_info.gateway[2],node_info.gateway[3]);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, node_info.netmask[0],node_info.netmask[1],node_info.netmask[2],node_info.netmask[3]);
  uip_setnetmask(ipaddr);
  
  OSTaskDel(Periodic_TASK_PRIO);  //ɾ��Poll_Task����
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