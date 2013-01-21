/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

//
// Created by csword 2011/01/13
// All rights reserved
//
#include "Project.h"
#include "uip.h"
#include "uIP_App.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "lingtone_app.h"

static OS_STK  Startup_Task_STK[STARTUP_TASK_STK_SIZE];
static OS_STK  Net_Task_STK[NET_TASK_STK_SIZE];
volatile int32_t uIP_RunTime = 0;//__IO == volatile
OS_EVENT *WIFI_INT_Mbox;


static void Net_Task(void* p_arg)
{
	FlagStatus *WIFI_INT;
	BOOL bStatus = FALSE;
	
	//
	// Wifi start!
	//
	WIFI_INT_Mbox = OSMboxCreate((void *)0);
	bStatus = Wlan_Init(g_pWlanAdapter);
	if (!bStatus) {
		TRACE("Wlan init failed!!\r\n");
		return;
	}

	if (!Get_KEY1()) { // 启动时KEY1按下进入模块的设置模式
		
		// uIP Init
		uip_ipaddr_t ipaddr;

		WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
		uip_init();
		Dhcp_Init(uip_ethaddr.addr, 6);  //DHCP Server using UDP
		LT_UdpApp_Init(uip_ethaddr.addr, 6);

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
		WiFiCmd_RunCommand ("set encrymode wep");
		WiFiCmd_RunCommand ("set keyascii 12345");

	}
	else{
		// uIP Init
		uip_ipaddr_t ipaddr;

		WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
		uip_init();
		Dhcp_Init(uip_ethaddr.addr, 6);  //DHCP Server using UDP
		LT_UdpApp_Init(uip_ethaddr.addr, 6);

		uip_ipaddr(ipaddr, 192, 168, 1, 2);
		uip_sethostaddr(ipaddr);
		uip_ipaddr(ipaddr, 192, 168, 1, 1);
		uip_setdraddr(ipaddr);
		uip_ipaddr(ipaddr, 255, 255, 255, 0);
		uip_setnetmask(ipaddr);
			
		p_appcall = httpd_appcall;
		httpd_init();
		
		WiFiCmd_RunCommand ("set ssid CDDC");
		WiFiCmd_RunCommand ("set networktype infra");
		WiFiCmd_RunCommand ("set authmode open");
		WiFiCmd_RunCommand ("set encrymode wep");
		WiFiCmd_RunCommand ("set keyascii 12345");
	}
	WiFiCmd_RunCommand ("ctrl connect");
		
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

static void Startup_Task(void* p_arg)
{
	// Bsp Init
	Set_System();
	TraceInit();

	Board_Init ();

	// Drivers init
	Flash_Init();
  EE_Init(); 	//Flash_Unlock already done in Flash_Init()
	KEY_Init();
	Buzzer_Init ();
	Relay_Init ();
	LED_Init ();
	Console_Init ();

	Console_Print("WiFi Hello world!!\r\n");

#if(OS_TASK_STAT_EN>0)
	OSStatInit(); 
#endif

	/*TODO:Create Application Tasks Here*/
	OSTaskCreate(Net_Task,(void*)0,
				&Net_Task_STK[NET_TASK_STK_SIZE-1],
				NET_TASK_PRIO);	
	 
	OSTaskDel(OS_PRIO_SELF);  //删除启动任务
}

int main(void) 
{
#ifdef BUILD_FOR_WRITEFIRMWARE
	If_WriteFirmware ();
	while (1);
#else
	OSInit();//系统初始化	
   	
	OSTaskCreate(Startup_Task,(void*)0,
				&Startup_Task_STK[STARTUP_TASK_STK_SIZE-1],
				STARTUP_TASK_PRIO);

	OSStart(); //任务开始运行  
#endif

	while(1);
	
  	return 0;
}
