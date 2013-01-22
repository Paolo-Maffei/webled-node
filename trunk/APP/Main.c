/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "uIP_App.h"
#include "tapdev.h"
#include "clock-arch.h"

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
		WiFiCmd_RunCommand ("set encrymode disable");
		WiFiCmd_RunCommand ("set keyascii 12345");
	}
	else{ //work mode,use UDP
		// uIP Init
		uip_ipaddr_t ipaddr;

		WlanAdapter_MacAddr(g_pWlanAdapter, uip_ethaddr.addr, FALSE);
		uip_init();
		
    UDP_App_Init();

		uip_ipaddr(ipaddr, 192, 168, 1, 2);
		uip_sethostaddr(ipaddr);
		uip_ipaddr(ipaddr, 192, 168, 1, 1);
		uip_setdraddr(ipaddr);
		uip_ipaddr(ipaddr, 255, 255, 255, 0);
		uip_setnetmask(ipaddr);

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
