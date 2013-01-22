/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "webled_udp.h"

void UDP_App_Init()
{
	struct uip_udp_conn *conn = NULL;

	conn = uip_udp_new(&uip_hostaddr, HTONS(0));
	if(conn != NULL) {
		uip_udp_bind(conn, HTONS(3132));
	}
}

void WebLED_UDP_APPCALL(void)
{
		if (uip_newdata())
		{
				switch (HTONS(uip_udp_conn->lport))
				{
//					case DHCPS_SERVER_PORT:
//						Dhcp_PktHandler(uip_appdata, uip_len);
					  break;
//					case LINGTONE_UDPAPP_SERVER_PORT:
	//					LT_UdpApp_PktHandler(uip_appdata, uip_len);
  					break;
				}
		}
}

void GetInfo_Server()
{
	
}

void SetInfo_Server()
{
	
}

void Ctrl_Server() //LED控制服务器
{
	
}

