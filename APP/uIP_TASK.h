
//uIP_TASK.h
#ifndef _UIP_TASK_H_
#define _UIP_TASK_H_

#include  "ucos_ii.h"

static void Set_uIP(void);
void Net_Task(void* p_arg);
void Wifi_RX_Task(void *pdata);

extern void (*p_appcall)(void);

extern OS_EVENT *uip_mbox;


#define UIP_MBOX_RX		0x01
#define UIP_MBOX_POLL		0x02

#endif