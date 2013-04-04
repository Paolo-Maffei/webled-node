

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
          {      //�д����Ӧ����
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
            tapdev_send();      //��ARP��Ӧ����̫����
          }
        }

        break;
    case UIP_MBOX_POLL:
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
	udp_packdata(i);
	/* If the above function invocation resulted in data that
        should be sent out on the network, the global variable
        uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }

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

void Poll_Task(void *pdata)  //��ʱpoll����
{
  while(1)
  {
#if UIP_UDP    //��ʱʱ����������
      for(int i = 0; i < UIP_UDP_CONNS; i++) 
      {
	uip_udp_periodic(i);
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }
       OSTimeDlyHMSM(0,0,1,0);  
#endif /* UIP_UDP */
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
  
  WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);//��ȡMAC��ַ
  if(0xFFFFFFFF == node_info.id)      //����MAC��ַ���ɽڵ�ID��ֻ�ڽڵ��һ���ϵ�ʱִ��
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
  
  if(0xEE != node_info.ip_status)  //��ַδ����
  {
    p_udp_appcall = dhcpc_appcall;
    dhcpc_init(uip_ethaddr.addr,6);
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
  
 printf("Got IP address %d.%d.%d.%d\n",
 uip_ipaddr1(s.ipaddr), uip_ipaddr2(s.ipaddr),
 uip_ipaddr3(s.ipaddr), uip_ipaddr4(s.ipaddr));
 printf("Got netmask %d.%d.%d.%d\n",
 uip_ipaddr1(s.netmask), uip_ipaddr2(s.netmask),
 uip_ipaddr3(s.netmask), uip_ipaddr4(s.netmask));
 printf("Got DNS server %d.%d.%d.%d\n",
 uip_ipaddr1(s.dnsaddr), uip_ipaddr2(s.dnsaddr),
 uip_ipaddr3(s.dnsaddr), uip_ipaddr4(s.dnsaddr));
 printf("Got default router %d.%d.%d.%d\n",
 uip_ipaddr1(s.default_router), uip_ipaddr2(s.default_router),
 uip_ipaddr3(s.default_router), uip_ipaddr4(s.default_router));
 printf("Lease expires in %ld seconds\n",
 ntohs(s.lease_time[0])*65536ul + ntohs(s.lease_time[1]));
 
}
