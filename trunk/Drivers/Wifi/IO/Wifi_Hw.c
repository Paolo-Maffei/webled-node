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

static UCHAR ucSpiBuf[HW_IODATA_SIZE];
const Stm32Spi Spi = {SPI1, GPIOA, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GSPI_MODE_SPI};
#define pSpi	((PSTM32SPI)&Spi)

static int Hw_Write(int nSize)
{
	GSPI_Write (pSpi, ucSpiBuf, nSize);

	return nSize;
}

static int Hw_Read(int nSize)
{
	GSPI_Read (pSpi, ucSpiBuf, nSize);

	return nSize;
}

BOOL Hw_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GSPI_Init (pSpi);
	
	// Enable GPIO clock
	RCC_APB2PeriphClockCmd(
				RCC_APB2Periph_GPIOB |
				RCC_APB2Periph_GPIOC,
				ENABLE
	                      );
	// PDN: PB0
	GPIO_InitStructure.GPIO_Pin = WIFI_CTRL2_PDN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(WIFI_CTRL2_PORT, &GPIO_InitStructure);

	// RST: PC5
	GPIO_InitStructure.GPIO_Pin = WIFI_CTRL3_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(WIFI_CTRL3_PORT, &GPIO_InitStructure);

	if (Hw_IntrInit() != TRUE)
		return FALSE;
	if (Hw_DmaInit() != TRUE)
		return FALSE;

	TRACE("-Hw_Init()\r\n");

	return TRUE;
}

BOOL Hw_Deinit(void)
{
	TRACE("+Hw_Deinit()\r\n");

	return TRUE;
}

BOOL Hw_IntrInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;

	TRACE("+Hw_IntrInit()\r\n");

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO , ENABLE);

	// SPI interrupt pin PC4 GPIO config
	GPIO_InitStructure.GPIO_Pin = WIFI_CTRL3_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(WIFI_CTRL3_PORT, &GPIO_InitStructure);

	// Ext interrupt config
#ifdef  VECT_TAB_RAM
	// Set the Vector Table base location at 0x20000000
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
	// Set the Vector Table base location at 0x08000000
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Configure EXTI Line4 to generate an interrupt on falling edge

	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
	EXTI_GenerateSWInterrupt(EXTI_Line4);
	// Disable hw interrupt first
	Hw_CloseInterrupts();

	TRACE("-Hw_IntrInit()\r\n");

	return TRUE;
}

BOOL Hw_DmaInit(void)
{
	TRACE("+Hw_DmaInit()\r\n");

	return TRUE;
}

BOOL Hw_PowerOn()
{
	TRACE("+Hw_PowerOn()\r\n");

	// WIFI power up.
	WIFI_POWER_UP();
	Sleep(1);
	// Chipselect Enable.
	GSPI_CS_ENABLE(pSpi);
	Sleep(1);
	Hw_Reset();
	// Chipselect disable first.
	GSPI_CS_DISABLE(pSpi);
#ifdef GSPI_GPIO_MODE
	GSPI_GPIO_CLR_PIN(GSPI_GPIO_CLK_PIN);
#endif
#if 1
	TRACE("SPI testing...\r\nPlease confirm that the value read is equal with write...\r\n");
	{
		int i;
		WORD stat, stat0 = 0;

		for (i = 0 ; i < 100; i ++) {
			stat = stat0;
			TRACE("Write: 0x%04X\r\n", stat);
			Hw_WriteRegister(0x64, (PVOID)&stat, 2);
			stat = 0;
			Hw_ReadRegister(0x64, (PVOID)&stat, 2);
			TRACE("Read: 0x%04X\r\n", stat);

			if (stat != stat0) {
				TRACE("SPI test failed!");
				break;
			}
			stat0 ++;
		}
	}
	TRACE("SPI test done.\r\n");
#endif

	TRACE("-Hw_PowerOn()\r\n");

	return TRUE;
}

BOOL Hw_PowerOff()
{
	WIFI_POWER_DOWN();

	return TRUE;
}

int Hw_WriteRegister(int nRegNo, PVOID pBuf, int nSize)
{
	PBYTE pCur, pEnd, pBufPos;
	PBYTE aData = ucSpiBuf;
	int nBufLen, nDmaLen, nResidue;

	GSPI_CS_ENABLE(pSpi);

	nResidue = nSize;
	pBufPos = (PBYTE)pBuf;

#if (/*defined(SPI_USE_DMA) || */defined(GSPI_USE_8BIT))
	aData[0] = HIBYTE(nRegNo) | 0x80;
	aData[1] = LOBYTE(nRegNo);
#else // else for 16BIT
	aData[1] = HIBYTE(nRegNo) | 0x80;
	aData[0] = LOBYTE(nRegNo);
#endif
	pCur = aData + 2;
	while (nResidue > 0) {
		pEnd = aData + HW_IODATA_SIZE;
		nBufLen = pEnd - pCur;
		if (nResidue < nBufLen) {
			nBufLen = nResidue;
			pEnd = pCur + nResidue;
		}
		while (pCur < pEnd) {
#if (/*defined(SPI_USE_DMA) || */defined(GSPI_USE_8BIT))
			pCur[1] = *pBufPos ++;
			pCur[0] = *pBufPos ++;
#else // else for 16BIT
			pCur[0] = *pBufPos ++;
			pCur[1] = *pBufPos ++;
#endif
			pCur += 2;
		}
		nDmaLen = pEnd - aData;
		if (Hw_Write(nDmaLen) != nDmaLen)
			break;
		nResidue -= nBufLen;
		pCur = aData;
	}

	GSPI_CS_DISABLE(pSpi);

	return nSize - nResidue;
}

int Hw_ReadRegister(int nRegNo, PVOID pBuf, int nSize)
{
	PBYTE pCur,pEnd,pBufPos;
	PBYTE aData = ucSpiBuf;
	int nBufLen,nDmaLen,nResidue;

	GSPI_CS_ENABLE(pSpi);

	nResidue = nSize;
	nDmaLen = (int)ucSpiDummyClk * 2 + 2;
	FillMemory(aData, 0xFF, nDmaLen);
#if (/*defined(SPI_USE_DMA) || */defined(GSPI_USE_8BIT))
	aData[0] = HIBYTE(nRegNo);
	aData[1] = LOBYTE(nRegNo);
#else // else for 16BIT
	aData[1] = HIBYTE(nRegNo);
	aData[0] = LOBYTE(nRegNo);
#endif
	if(Hw_Write(nDmaLen) == nDmaLen) {
		pBufPos = (PBYTE)pBuf;
		pCur = aData;
		while(nResidue > 0) {
			pEnd = aData + HW_IODATA_SIZE;
			nBufLen = pEnd - pCur;
			if(nResidue < nBufLen) {
				nBufLen = nResidue;
				pEnd = pCur + nResidue;
			}
			nDmaLen = pEnd - aData;
#ifndef GSPI_GPIO_MODE
			if(Hw_Read(nDmaLen) != nDmaLen)
				break;
#else
			while (nDmaLen --)
				*pCur ++ = (BYTE)GSPI_ReadByte(&Spi);
			pCur = aData;
#endif
			while(pCur < pEnd) {
#if (/*defined(SPI_USE_DMA) || */defined(GSPI_USE_8BIT))
				*pBufPos ++ = pCur[1];
				*pBufPos ++ = pCur[0];
#else // else for 16BIT
				*pBufPos ++ = pCur[0];
				*pBufPos ++ = pCur[1];
#endif
				pCur += 2;
			}
			nResidue -= nBufLen;
			pCur = aData;
		}
	}

	GSPI_CS_DISABLE(pSpi);

	return nSize;
}

BOOL Hw_OpenInterrupts(void)
{
	TRACE("+Hw_OpenInterrupts()\r\n");

	// STM32 IMR write 1 is ENABLE!!!!!,0 is DISABLE!!!!
	EXTI->IMR |= (1 << 4);

	return TRUE;
}

BOOL Hw_CloseInterrupts(void)
{
	TRACE("+Hw_CloseInterrupts()\r\n");

	// STM32 IMR write 1 is ENABLE!!!!!,0 is DISABLE!!!!
	EXTI->IMR &= ~(1 << 4);

	return TRUE;
}

BOOL Hw_Reset(void)
{
	TRACE("+Hw_Reset()\r\n");

	WIFI_GSPICLK_RESET();
	Sleep(10);
	WIFI_GSPICLK_RELEASE_RESETING();
	Sleep(10);

	TRACE("-Hw_Reset()\r\n");

	return TRUE;
}
