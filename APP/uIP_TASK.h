
//uIP_TASK.h
#ifndef _UIP_TASK_H_
#define _UIP_TASK_H_

#include  "ucos_ii.h"

/* task priority */ 
#define STARTUP_TASK_PRIO     4
#define NET_TASK_PRIO         6
#define Periodic_TASK_PRIO    7
#define LED_TASK_PRIO         8
#define Wifi_RX_TASK_PRIO     9

/* task stack size */ 
#define STARTUP_TASK_STK_SIZE  80  
#define NET_TASK_STK_SIZE      1024
#define Wifi_RX_TASK_STK_SIZE  256
#define Periodic_TASK_STK_SIZE   100
#define LED_TASK_STK_SIZE      100

static OS_STK  Startup_Task_STK[STARTUP_TASK_STK_SIZE];
static OS_STK  Net_Task_STK[NET_TASK_STK_SIZE];
static OS_STK  Wifi_RX_Task_STK[Wifi_RX_TASK_STK_SIZE];
static OS_STK  Periodic_Task_STK[Periodic_TASK_STK_SIZE];
static OS_STK  LED_Task_STK[LED_TASK_STK_SIZE];


static void Set_uIP(void);
void Net_Task(void* p_arg);
void Wifi_RX_Task(void *pdata);
void Periodic_Task(void *pdata);
void LED_Task(void *pdata);

extern void (*p_appcall)(void);
extern void (*p_udp_appcall)(void);

extern OS_EVENT *uip_mbox;
extern OS_EVENT *led_mbox;

#define UIP_MBOX_RX		0x01
#define UIP_MBOX_POLL		0x02

#endif