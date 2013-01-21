/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
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
