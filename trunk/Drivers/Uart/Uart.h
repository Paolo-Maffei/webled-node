/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#ifndef __UART_H__
#define __UART_H__
//
// Macros define
//
#define UART_MODE_POLL						0	// ��ͨģʽ
#define UART_MODE_INT						1	// �ж�ģʽ

#define UART_INT_TYPE_RX					0
#define UART_INT_TYPE_TX					1
//
// Structs define
//
typedef void (*UartIrqHandler) (UCHAR ucType, UCHAR ucCharRx);

typedef struct _STM32UART {
	USART_TypeDef* UARTx;
	GPIO_TypeDef* pGPIOPort;
	USHORT	usRxPin;
	USHORT	usTxPin;
	DWORD dwBaud;
	UCHAR ucMode;
	UartIrqHandler pUartIrqHandler;
} Stm32Uart, *PSTM32UART;

//
// Exported functions declare
//
BOOL Uart_Init(PSTM32UART pUart);
void Uart_SendByte(PSTM32UART pUart, USHORT usData);
void Uart_SendBuffer(PSTM32UART pUart, PUCHAR pucBuffer, int nLen);
void Uart_SendString(PSTM32UART pUart, char* pszBuffer);

#endif // __UART_H__
