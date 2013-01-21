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
#include "lingtone_udp_app.h"

///////////////////////////////////////////////////////////////////////////////////
// Exported functions
//
void LT_UdpApp_Init(const void *mac_addr, int mac_len)
{
	struct uip_udp_conn *conn = NULL;

	conn = uip_udp_new(&uip_hostaddr, HTONS(0));
	if(conn != NULL) {
		uip_udp_bind(conn, HTONS(LINGTONE_UDPAPP_SERVER_PORT));
	}
}

void LT_UdpApp_PktHandler(PBYTE pInBuffer, USHORT usLen)
{
	PLTUDPAPPCTRLMSG pCtrlMsg = (PLTUDPAPPCTRLMSG)pInBuffer;

	switch (pCtrlMsg->ucObject) {
		case CTRL_MSG_OBJECT_LED:
			switch (pCtrlMsg->ucIndex) {
				case CTRL_MSG_INDEX_1:
					if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
						LED_TurnOn(LED1);
					else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
						LED_TurnOff(LED1);
					break;
				case CTRL_MSG_INDEX_2:
					if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
						LED_TurnOn(LED2);
					else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
						LED_TurnOff(LED2);
					break;
				case CTRL_MSG_INDEX_3:
					if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
						LED_TurnOn(LED3);
					else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
						LED_TurnOff(LED3);
					break;
				case CTRL_MSG_INDEX_4:
					if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
						LED_TurnOn(LED4);
					else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
						LED_TurnOff(LED4);
					break;
			}
			break;
		case CTRL_MSG_OBJECT_BUZZER:
			if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
				Buzzer_On();
			else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
				Buzzer_Off();
			break;
		case CTRL_MSG_OBJECT_RELAY:
			if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_ON)
				Relay_On();
			else if (pCtrlMsg->ucStatus == CTRL_MSG_STATUS_OFF)
				Relay_Off();
			break;
	}
}
