/******************************
成都达创
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

