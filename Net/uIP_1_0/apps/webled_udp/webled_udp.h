/******************************************************************************
* �ɶ��ﴴ�Ƽ�
* by lits @2013/01/22
*******************************************************************************/

#ifndef _WEBLED_UDP_H_
#define _WEBLED_UDP_H_

#define UIP_UDP_APPCALL  WebLED_UDP_APPCALL
#define WEBLED_UDP_SEND_PORT        3131
#define WEBLED_UDP_RCV_PORT         3132

void UDP_App_Init();
void WebLED_UDP_APPCALL(void);


#endif