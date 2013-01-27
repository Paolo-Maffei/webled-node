/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "webled_udp.h"

char udp_send_buf[UDP_SBUF_SIZE];
char udp_send_len;

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
            udp_send_len = 0;
 //           u16_t len = uip_datalen();
            char *dataptr = (char *)uip_appdata;
            if(*(int *)(dataptr+1) != NodeAttr_GetID())  //目的ID和本节点ID不同
              break;
            switch(*dataptr){
            case 0x10:
              break;
            case 0x11:
              udp_send_buf[0] = dataptr[0];
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&dataptr[1],4);
              CopyMemory(&udp_send_buf[9],&node_info.id,4);
              udp_send_len = 13;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x12:
              udp_send_buf[0] = dataptr[0];
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&dataptr[1],4);
              udp_send_buf[9] = node_info.name_length;
              CopyMemory(&udp_send_buf[10],node_info.name,node_info.name_length);
              udp_send_len = node_info.name_length + 10;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x13:
              break;
            case 0x14:
              break;
            case 0x15:
              break;
            case 0x16:
              break;
            default:
              break;
          }
          if(uip_poll())
          {
          }
          break;
          
        default:
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

