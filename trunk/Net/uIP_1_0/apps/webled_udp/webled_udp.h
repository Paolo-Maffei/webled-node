/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#ifndef _WEBLED_UDP_H_
#define _WEBLED_UDP_H_

#define RESULT_SUCCESS  0
#define RESULT_FAILED   1

//#define UIP_UDP_APPCALL  WebLED_App_Init
#define WEBLED_UDP_SEND_PORT        3133
#define WEBLED_UDP_RCV_PORT         3132

#define UDP_SBUF_SIZE   500   //发送缓冲区的大小

void WebLED_App_Init();
void WebLED_UDP_APPCALL(void);


#endif