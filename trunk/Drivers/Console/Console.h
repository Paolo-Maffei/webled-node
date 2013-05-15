/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

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
