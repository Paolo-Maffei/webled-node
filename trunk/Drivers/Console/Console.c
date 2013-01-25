/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#include "Project.h"

Console g_Console;
void Console_IRQHandler(UCHAR ucIntType, UCHAR ucCharRx);
const Stm32Uart uart = {USART1, GPIOA, GPIO_Pin_10, GPIO_Pin_9, 115200, UART_MODE_INT, Console_IRQHandler};
#define pUart	((PSTM32UART)&uart)

BOOL Console_Init()
{
	ZeroMemory(&g_Console, sizeof(Console));
	Uart_Init(pUart);
		
	return TRUE;
}

void Console_IRQHandler(UCHAR ucIntType, UCHAR ucCharRx)
{
	if(ucIntType == UART_INT_TYPE_RX) {
		switch (ucCharRx) {
			case '\r':	 // Enter		  
			case '\n':
				g_Console.ucConsoleRxBuffer[g_Console.ucRxCnt]  = '\0';
				Uart_SendString(pUart, "\r\n");
				g_Console.ucState |= CONSOLE_STATE_RECEIVED;
				break;
			case '\0': // nul			  
				break;
			case 0x03: // ^C - break		  
				g_Console.ucConsoleRxBuffer[0] = '\0';	// discard input
				g_Console.ucRxCnt = 0;
				break;
			case 0x15: // ^U - erase line
			{
				while (g_Console.ucRxCnt) {
					Uart_SendString (pUart, "\b \b");
					g_Console.ucRxCnt --;
				}
				break;
			}
			case 0x08: // ^H  - backspace	  
			case 0x7F: // DEL - backspace	  
			{
				Uart_SendString (pUart, "\b \b");
				if (g_Console.ucRxCnt)
					g_Console.ucRxCnt --;

				break;
			}
			default:
				if (ucCharRx >= 32 && ucCharRx <= 126)
				{
					if (g_Console.ucRxCnt < CONSOLE_RX_BUFFER_SIZE - 1) {
						g_Console.ucConsoleRxBuffer[g_Console.ucRxCnt]  = ucCharRx;
						g_Console.ucRxCnt ++;
						Uart_SendByte(pUart, ucCharRx);
					}
					else
						Uart_SendByte(pUart, '\a'); // Buffer full
				}
				break;
		}
	}
}

void Console_Print(const char *fmt, ...)
{
	char szBuffer[256];
	va_list args;

	va_start(args, fmt);
	vsprintf(szBuffer, fmt, args);
	va_end(args);

	Uart_SendString(pUart, szBuffer);
}

BOOL Console_IsReceived()
{
	return (g_Console.ucState & CONSOLE_STATE_RECEIVED ? TRUE : FALSE);
}

BOOL Console_IsSent()
{
	return (g_Console.ucState & CONSOLE_STATE_SENT ? TRUE : FALSE);
}

BOOL Console_ResetRx()
{
	g_Console.ucState &= ~CONSOLE_STATE_RECEIVED;
	g_Console.ucRxCnt = 0;
	
	return TRUE;
}

BOOL Console_ResetTx()
{
	g_Console.ucState &= ~CONSOLE_STATE_SENT;
	g_Console.ucTxCnt = 0;
		
	return TRUE;
}

BOOL Console_ProcCmd()
{
		
	return TRUE;
}

