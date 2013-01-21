#ifndef __LINGTONE_APP_H__
#define __LINGTONE_APP_H__
//
// Macros definitions
//
#define UIP_UDP_APPCALL  WQUDP_PktHandler

//
// Exported functions declare
//
void WQUDP_PktHandler(void);
void tcp_test_appcall(void);
void myudp_send(char *str, short n);
void UDP_newdata(void);
void etherdev_init(void);
void tcp_server_appcall(void);
void uip_log(char *m);

//
// Exported varialbes declare
//

#endif // __LINGTONE_APP_H__

