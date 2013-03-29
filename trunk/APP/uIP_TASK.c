

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

volatile int32_t uIP_RunTime = 0;//__IO == volatile //��������Ӷ�ʱ��֧��
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
      uip_len = tapdev_read();    //�������豸��ȡһ��IP��,�������ݳ���
      if (uip_len > 0)          //�յ�����ʱ���Ȼ�仯
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
  { // ����ʱKEY1���½���ģ�������ģʽ
    
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
    
    UDP_App_Init(); //��ʼ��UDPӦ��
    
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