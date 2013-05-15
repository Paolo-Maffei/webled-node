

#ifndef __UART_H__
#define __UART_H__
//
// Macros define
//
#define UART_MODE_POLL						0	// 普通模式
#define UART_MODE_INT						1	// 中断模式

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
