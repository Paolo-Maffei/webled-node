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

static UartIrqHandler pUart1IrqHandler = NULL, pUart2IrqHandler = NULL, pUart3IrqHandler = NULL;

//----------------------------------------------------------
// For stm32f10x_it.c
//
void USART1_IRQHandler (void)
{
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();	
	/*TODO:Write IRQ Code Here*/
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		UCHAR ucChar;		

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		ucChar = USART_ReceiveData(USART1);
		pUart1IrqHandler(UART_INT_TYPE_RX, ucChar);
	}
	OSIntExit();
}

void USART2_IRQHandler (void)
{
	//pUart2IrqHandler();
}

void USART3_IRQHandler (void)
{
	//pUart3IrqHandler();
}

//-----------------------------------------------------------
// Exported functions
//
BOOL Uart_Init(PSTM32UART pUart)
{
	// Enable clock
	if (pUart->UARTx == USART1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	else if (pUart->UARTx == USART2)
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB2Periph_GPIOD, ENABLE);
	else if (pUart->UARTx == USART3)
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOC, ENABLE);
	else {
		TRACE("Unsupport UART port!\r\n");
		return FALSE;
	}

	if (1) { // GPIO config
		GPIO_InitTypeDef GPIO_InitStructure;

		// Configure USARTx_Tx as alternate push-pull
		GPIO_InitStructure.GPIO_Pin = pUart->usTxPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(pUart->pGPIOPort, &GPIO_InitStructure);

		// Configure USARTx_Rx as input floating
		GPIO_InitStructure.GPIO_Pin = pUart->usRxPin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(pUart->pGPIOPort, &GPIO_InitStructure);
	}

	if (1) {
		USART_InitTypeDef USART_InitStructure;
		USART_ClockInitTypeDef USART_ClockInitStructure;

		// UART info
		USART_InitStructure.USART_BaudRate = pUart->dwBaud;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
		USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
		USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
		USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

		// Configure USART1
		USART_Init(pUart->UARTx, &USART_InitStructure);
		USART_ClockInit(pUart->UARTx, &USART_ClockInitStructure);
	}

	// Enable the USART1
	USART_Cmd(pUart->UARTx, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	if (pUart->ucMode & UART_MODE_INT) {
		NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
		/* Set the Vector Table base location at 0x20000000 */
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
		/* Set the Vector Table base location at 0x08000000 */
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

		/* Configure the NVIC Preemption Priority Bits */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

		/* Enable the USART1 Interrupt */
		if (pUart->UARTx == USART1)
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		else if (pUart->UARTx == USART2)
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		else if (pUart->UARTx == USART3)
			NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		else {
			TRACE("Unsupport UART port!\r\n");
			return FALSE;
		}

		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}

	// Install interrupt handlers
	if (pUart->UARTx == USART1)
		pUart1IrqHandler = pUart->pUartIrqHandler;
	else if (pUart->UARTx == USART2)
		pUart2IrqHandler = pUart->pUartIrqHandler;
	else if (pUart->UARTx == USART3)
		pUart3IrqHandler = pUart->pUartIrqHandler;
	else {
		TRACE("Unsupport UART port!\r\n");
		return FALSE;
	}
	
	return TRUE;
}


void Uart_SendByte(PSTM32UART pUart, USHORT usData)
{
		USART_SendData(pUart->UARTx, usData);
		while ((pUart->UARTx->SR & USART_FLAG_TXE) == RESET);
}

void Uart_SendBuffer(PSTM32UART pUart, PUCHAR pucBuffer, int nLen)
{
	int i;

	for (i = 0; i < nLen; i ++) {
		USART_SendData(pUart->UARTx, pucBuffer[i]);
		while ((pUart->UARTx->SR & USART_FLAG_TXE) == RESET);
	}
}

void Uart_SendString(PSTM32UART pUart, char* pszBuffer)
{
	int i = 0;

	while (pszBuffer[i]) {
		USART_SendData(pUart->UARTx, pszBuffer[i]);
		while ((pUart->UARTx->SR & USART_FLAG_TXE) == RESET);
		i ++;
	}
}

