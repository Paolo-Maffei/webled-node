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
*   函数名：UipPro
*   输  入:
*   输  出:
*   功能说明：中断触发读取网络接收缓存
*   注意：当使用uCOS时，不适合用中断。请务必谨记。
********************************************************************/
void UipPro(void)
{
	if (1) {
		uip_len = tapdev_read();    //从网络设备读取一个IP包,返回数据长度
		if (uip_len > 0) {          //收到数据时长度会变化
			/* 处理IP数据包(只有校验通过的IP包才会被接收) */
			if (BUF->type == htons(UIP_ETHTYPE_IP)) { //收到IP包
				uip_arp_ipin();        //去除以太网头结构，更新ARP表
				uip_input();           //IP包处理
				/*
				    因为uip_input();函数调用了回调函数UIP_APPCALL(),即tcp_server_appcall
				    当上面的函数执行后，如果回调函数需要发送数据，则全局变量 uip_len > 0
				    需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
				*/
				if (uip_len > 0) {      //有带外回应数据
					uip_arp_out();      //加以太网头结构，在主动连接时可能要构造ARP请求
					tapdev_send();      //发送数据到以太网（设备驱动程序）
				}
			}
			/* 处理arp报文 */
			else if (BUF->type == htons(UIP_ETHTYPE_ARP)) { //是ARP请求包
				uip_arp_arpin();        //如是是ARP回应，更新ARP表；如果是请求，构造回应数据包
				/*
				    当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
				    需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
				*/
				if (uip_len > 0) {      //是ARP请求，要发送回应
					tapdev_send();      //发ARP回应到以太网上
				}
			}
		}
	}
}

/*******************************************************************************
*   函数名：InitNet
*   输  入:
*   输  出:
*   功能说明：初始化网络硬件、UIP协议栈、配置本机IP地址
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
