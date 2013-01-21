
#ifndef __LT_UDP_APP_H__
#define __LT_UDP_APP_H__
//
// Macros definition
//
#define LINGTONE_UDPAPP_CLIENT_PORT	1000
#define LINGTONE_UDPAPP_SERVER_PORT	1001

#define CTRL_MSG_OBJECT_LED			'L'
#define CTRL_MSG_OBJECT_BUZZER		'B'
#define CTRL_MSG_OBJECT_RELAY			'R'

#define CTRL_MSG_INDEX_1				'1'
#define CTRL_MSG_INDEX_2				'2'
#define CTRL_MSG_INDEX_3				'3'
#define CTRL_MSG_INDEX_4				'4'

#define CTRL_MSG_STATUS_ON			'1'
#define CTRL_MSG_STATUS_OFF			'0'

//
// Structs definition
//
typedef struct _LTUDPAPP_CTRL_MSG {
	UCHAR ucObject;
	UCHAR ucIndex;
	UCHAR ucStatus;
} LTUdpAppCtrlMsg, *PLTUDPAPPCTRLMSG;

//
// Exported functions declare
//
void LT_UdpApp_Init(const void *mac_addr, int mac_len);
void LT_UdpApp_PktHandler(PBYTE pInBuffer, USHORT usLen);

#endif // __LT_UDP_APP_H__
