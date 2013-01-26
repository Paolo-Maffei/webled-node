/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "webled_udp.h"

void UDP_App_Init(void)
{
        uip_ipaddr_t addr;
	struct uip_udp_conn *conn = NULL;
        uip_ipaddr(addr,255,255,255,255);

	conn = uip_udp_new(&addr, HTONS(WEBLED_UDP_RCV_PORT));
	if(conn != NULL) {
		uip_udp_bind(conn, HTONS( WEBLED_UDP_RCV_PORT));
	}//收发采用同一个端口
}

void WebLED_UDP_APPCALL(void)
{
        switch(HTONS(uip_udp_conn->lport))
        {
        case WEBLED_UDP_RCV_PORT:
          if (uip_newdata())
          {
            uip_send("hello udp\n",20);
          }
          break;
          
        default:
          break;
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

