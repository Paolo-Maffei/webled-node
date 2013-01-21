#include "Project.h"

#include "uIP_App.h"
#include "uip.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"
#include "dhcps.h"
#include "lingtone_udp_app.h"

/*uIP  stack*/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

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
	uip_ipaddr_t ipaddr;

	WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
	uip_init();
	Dhcp_Init(uip_ethaddr.addr, 6);
	LT_UdpApp_Init(uip_ethaddr.addr, 6);

	uip_ipaddr(ipaddr, 192, 168, 1, 108);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 192, 168, 1, 1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255, 255, 255, 0);
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(1200));
	uip_listen(HTONS(80));
}
