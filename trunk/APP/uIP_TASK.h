
//uIP_TASK.h
#ifndef _UIP_TASK_H_
#define _UIP_TASK_H_

#include  "ucos_ii.h"

static void Set_uIP(void);

void Net_Task(void* p_arg);

extern void (*p_appcall)(void);

#define UIP_MBOX_RCV 		0x01
#define UIP_MBOX_POLL		0x02

#endif