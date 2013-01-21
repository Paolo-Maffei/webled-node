/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/
#include "Project.h"

#include "uip.h"
#include "lingtone_app.h"
#include "dhcps.h"
#include "lingtone_udp_app.h"

void WQUDP_PktHandler(void)
{
	if (uip_newdata()) {
		switch (HTONS(uip_udp_conn->lport)) {
			case DHCPS_SERVER_PORT:
				Dhcp_PktHandler(uip_appdata, uip_len);
				break;
			case LINGTONE_UDPAPP_SERVER_PORT:
				LT_UdpApp_PktHandler(uip_appdata, uip_len);
				break;
		}
	}
}

