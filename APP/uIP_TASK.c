

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

volatile int32_t uIP_RunTime = 0;//__IO == volatile //��������Ӷ�ʱ��֧��

OS_EVENT  *uIP_MBOX;

/*uIP  stack*/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

/*uIP��Ӧ�ó�����ɺ���ָ�룬�Ա�ʵ���ڿ���ʱ�������ý��벻ͬӦ��*/
void (*p_appcall)(void);

void Net_Task(void* p_arg)
{
	FlagStatus *WIFI_INT;
	BOOL bStatus = FALSE;
	
	//
	// Wifi start!
	//
	bStatus = Wlan_Init(g_pWlanAdapter);
	if (!bStatus) {
		TRACE("Wlan init failed!!\r\n");
		return;
	}

	if (!Get_KEY1()) { // ����ʱKEY1���½���ģ�������ģʽ
		
		// uIP Init
		uip_ipaddr_t ipaddr;

		WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
		uip_init();
		
		uip_ipaddr(ipaddr, 192, 168, 1, 1);
		uip_sethostaddr(ipaddr);
		uip_ipaddr(ipaddr, 192, 168, 1, 1);
		uip_setdraddr(ipaddr);
		uip_ipaddr(ipaddr, 255, 255, 255, 0);
		uip_setnetmask(ipaddr);
			
		p_appcall = httpd_appcall;
		httpd_init();
			
		WiFiCmd_RunCommand ("set ssid WEBLED");
		WiFiCmd_RunCommand ("set networktype adhoc");
		WiFiCmd_RunCommand ("set authmode open");
		WiFiCmd_RunCommand ("set encrymode disable");
		WiFiCmd_RunCommand ("set keyascii 12345");
	}
	else{ //work mode,use UDP
		// uIP Init
		uip_ipaddr_t ipaddr;

		WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
		uip_init();
		
    UDP_App_Init();

		uip_ipaddr(ipaddr, 192, 168, 1, 2);
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
	
	uIP_MBOX = OSMboxCreate (NULL);
		
	while(1) {
			if (WIFI_GET_FLAG(g_pWlanAdapter, FLAG_PKTRCVED)) {
				WIFI_CLR_FLAG(g_pWlanAdapter, FLAG_PKTRCVED);
				UipPro();
			}
			if (g_Console.ucState & CONSOLE_STATE_RECEIVED) {
				WiFiCmd_RunCommand ((char*)g_Console.ucConsoleRxBuffer);
				Console_ResetRx();
			}
		}		
}

/*******************************************************************************
*   ��������UipPro
*   ��  ��:
*   ��  ��:
*   ����˵�����жϴ�����ȡ������ջ���
*   ע�⣺��ʹ��uCOSʱ�����ʺ����жϡ�����ؽ��ǡ�
********************************************************************/
void UipPro(void)
{
	
	if (1) {
		uip_len = tapdev_read();    //�������豸��ȡһ��IP��,�������ݳ���
		if (uip_len > 0) {          //�յ�����ʱ���Ȼ�仯
			/* ����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) */
			if (BUF->type == htons(UIP_ETHTYPE_IP)) { //�յ�IP��
				uip_arp_ipin();        //ȥ����̫��ͷ�ṹ������ARP��
				uip_input();           //IP������
				/*
				    ��Ϊuip_input();���������˻ص�����UIP_APPCALL(),��tcp_server_appcall
				    ������ĺ���ִ�к�����ص�������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
				    ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
				*/
				if (uip_len > 0) {      //�д����Ӧ����
					uip_arp_out();      //����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
					tapdev_send();      //�������ݵ���̫�����豸��������
				}
			}
			/* ����arp���� */
			else if (BUF->type == htons(UIP_ETHTYPE_ARP)) { //��ARP�����
				uip_arp_arpin();        //������ARP��Ӧ������ARP����������󣬹����Ӧ���ݰ�
				/*
				    ������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
				    ��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
				*/
				if (uip_len > 0) {      //��ARP����Ҫ���ͻ�Ӧ
					tapdev_send();      //��ARP��Ӧ����̫����
				}
			}
		}
	}
}

/*******************************************************************************
*   ��������InitNet
*   ��  ��:
*   ��  ��:
*   ����˵������ʼ������Ӳ����UIPЭ��ջ�����ñ���IP��ַ
************************************************************/
void uIP_Net_Init(void)
{
}