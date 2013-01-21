/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#ifndef __CONSOLE_H__
#define __CONSOLE_H__
//
// Macros define
//
#define CONSOLE_RX_BUFFER_SIZE			128
#define CONSOLE_TX_BUFFER_SIZE			128

#define CONSOLE_STATE_RECEIVED			(1 << 0)
#define CONSOLE_STATE_SENT				(1 << 1)

//
// Structs define
//
typedef struct _Console {
	UCHAR ucConsoleRxBuffer[CONSOLE_RX_BUFFER_SIZE];
	UCHAR ucConsoleTxBuffer[CONSOLE_TX_BUFFER_SIZE];
	UCHAR ucRxCnt;
	UCHAR ucTxCnt;
	UCHAR ucState;
} Console, *PCONSOLE;

//
// Exported functions declare
//
BOOL Console_Init();
void Console_Print(const char *fmt, ...);
BOOL Console_IsReceived();
BOOL Console_IsSent();
BOOL Console_ResetRx();
BOOL Console_ResetTx();

//
// Exported variables declare
//
extern Console g_Console;

#endif // __CONSOLE_H__
