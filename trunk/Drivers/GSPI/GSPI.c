/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#include "Project.h"

BOOL GSPI_Init(PSTM32SPI pSpi)
{
	if (pSpi->ucMode == GSPI_MODE_SPI) {
		SPI_InitTypeDef SPI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		// Enable GPIO clock
		if (pSpi->SPIx == SPI1) {
			RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_SPI1 |
						RCC_APB2Periph_GPIOA
						,
						ENABLE
						);
		}
		else if (pSpi->SPIx == SPI2) {
			RCC_APB1PeriphClockCmd(
						RCC_APB1Periph_SPI2
						,
						ENABLE
						);
			RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_GPIOB
						,
						ENABLE
						);
		}
		else {
			TRACE("Unsupport SPI port!!\r\n");
			return FALSE;
		}

		// Configure SPIx pins: SCK, MISO and MOSI as function pins
		GPIO_InitStructure.GPIO_Pin = pSpi->usClkPin | pSpi->usMosiPin | pSpi->usMisoPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(pSpi->pGPIOPort, &GPIO_InitStructure);

		// Config SPIx CS pin
		GPIO_InitStructure.GPIO_Pin = pSpi->usCsPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(pSpi->pGPIOPort, &GPIO_InitStructure);

		// SPI1 configuration
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
#ifdef GSPI_USE_8BIT
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
#else // else for 16BIT
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
#endif
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(pSpi->SPIx, &SPI_InitStructure);

		// Enable SPIx
		SPI_Cmd(pSpi->SPIx, ENABLE);
	}
	else {
		GPIO_InitTypeDef GPIO_InitStructure;

		// Enable GPIO clock
		if (pSpi->SPIx == SPI1) {
			RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_GPIOA
						,
						ENABLE
						);
		}
		else if (pSpi->SPIx == SPI2) {
			RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_GPIOB
						,
						ENABLE
						);
		}
		else {
			TRACE("Unsupport SPI port!!\r\n");
			return FALSE;
		}
		
		// Config SPIx CS pin
		GPIO_InitStructure.GPIO_Pin = pSpi->usCsPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(pSpi->pGPIOPort, &GPIO_InitStructure);
		
		// SPIx port config as GPIO
		// Pin CLK & MOSI config as output
		GPIO_InitStructure.GPIO_Pin = pSpi->usClkPin | pSpi->usMosiPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(pSpi->pGPIOPort, &GPIO_InitStructure);

		// Pin MISO config as input
		GPIO_InitStructure.GPIO_Pin = pSpi->usMisoPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(pSpi->pGPIOPort, &GPIO_InitStructure);
	}
	
#ifdef GSPI_USE_DMA
	// Enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif

	return TRUE;
}

UINT16 GSPI_WriteByte(PSTM32SPI pSpi, UINT16 usData)
{
	if (pSpi->ucMode == GSPI_MODE_SPI) {
		// Loop while DR register in not emplty
		while (SPI_I2S_GetFlagStatus(pSpi->SPIx, SPI_I2S_FLAG_TXE) == RESET);

		// Send byte through the SPI peripheral
		SPI_I2S_SendData(pSpi->SPIx, usData);

		// Wait to receive a byte
		while (SPI_I2S_GetFlagStatus(pSpi->SPIx, SPI_I2S_FLAG_TXE) == RESET);

		// Return the byte read from the SPI bus
		return SPI_I2S_ReceiveData(pSpi->SPIx);
	}
	else {
		UINT16 i;

		for (i = 0x80; i > 0; i >>= 1) {
			// Set data.
			if (usData & i)
				GSPI_GPIO_SET_PIN(pSpi, pSpi->usMosiPin);
			else
				GSPI_GPIO_CLR_PIN(pSpi, pSpi->usMosiPin);
			GSPI_GPIO_SET_PIN(pSpi, pSpi->usClkPin);
			GSPI_GPIO_CLR_PIN(pSpi, pSpi->usClkPin);
		}

		return 0;
	}
}

UINT16 GSPI_ReadByte(PSTM32SPI pSpi)
{
	if (pSpi->ucMode == GSPI_MODE_SPI)
		return GSPI_WriteByte(pSpi, 0xFFFF);
	else {
		UINT16 usData, i;

		for (i = 0x80, usData = 0; i > 0; i >>= 1) {
			if (GSPI_GPIO_GET_PIN(pSpi, pSpi->usMisoPin))
				usData |= i;
			GSPI_GPIO_SET_PIN(pSpi, pSpi->usClkPin);
			GSPI_GPIO_CLR_PIN(pSpi, pSpi->usClkPin);
		}

		return usData;
	}
}

#ifdef GSPI_USE_DMA
void GSPI_WriteDMA (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize)
{
	DMA_InitTypeDef    DMA_InitStructure;
	volatile WORD bDummy;

	DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_TE2 | DMA1_FLAG_TC3 | DMA1_FLAG_TE3);

	// DMA Channel2 configuration
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)&pSpi->SPIx->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (UINT32)&bDummy;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
#ifdef GSPI_USE_8BIT
	DMA_InitStructure.DMA_BufferSize = nSize;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
#else // else for 16BIT,now 16bit for dma is not OK
	DMA_InitStructure.DMA_BufferSize = nSize / 2;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
#endif
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel2, ENABLE);

	// DMA Channel3 configuration
	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)&pSpi->SPIx->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (UINT32)pBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
#ifdef GSPI_USE_8BIT
	DMA_InitStructure.DMA_BufferSize = nSize;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
#else // else for 16BIT,now 16bit for dma is not OK
	DMA_InitStructure.DMA_BufferSize = nSize / 2;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
#endif
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel3, ENABLE);

	// Start transfer and wait dma complete
	SPI_I2S_DMACmd(pSpi->SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
	while (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET);
	SPI_I2S_DMACmd(pSpi->SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}

void GSPI_ReadDMA (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize)
{
	DMA_InitTypeDef    DMA_InitStructure;
	volatile WORD bDummy;

	DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_TE2 | DMA1_FLAG_TC3 | DMA1_FLAG_TE3);

	// DMA Channel2 configuration
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)&pSpi->SPIx->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (UINT32)pBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
#ifdef GSPI_USE_8BIT
	DMA_InitStructure.DMA_BufferSize = nSize;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
#else // else for 16BIT,now 16bit for dma is not OK
	DMA_InitStructure.DMA_BufferSize = nSize / 2;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
#endif
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel2, ENABLE);

	// DMA Channel3 configuration
	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)&pSpi->SPIx->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (UINT32)&bDummy;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
#ifdef GSPI_USE_8BIT
	DMA_InitStructure.DMA_BufferSize = nSize;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
#else // else for 16BIT,now 16bit for dma is not OK
	DMA_InitStructure.DMA_BufferSize = nSize / 2;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
#endif
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel3, ENABLE);

	// Start transfer and wait dma complete
	SPI_I2S_DMACmd(pSpi->SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
	while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
	SPI_I2S_DMACmd(pSpi->SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
#endif

void GSPI_Write (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize)
{
#ifdef GSPI_USE_DMA
	GSPI_WriteDMA (pBuffer, nSize);
#else

	UINT32 i;
#ifdef GSPI_USE_8BIT
	PBYTE p = pBuffer;
#else // else for 16BIT
	PWORD p = (PWORD)pBuffer;

	nSize >>= 1;
#endif

	for (i = 0; i < nSize; i ++)
		GSPI_WriteByte (pSpi, p[i]);
#endif
}

void GSPI_Read (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize)
{
#ifdef GSPI_USE_DMA
	GSPI_ReadDMA (pBuffer, nSize);
#else
	UINT32 i;
#ifdef GSPI_USE_8BIT
	PBYTE p = pBuffer;
#else // else for 16BIT
	PWORD p = (PWORD)pBuffer;

	nSize >>= 1;
#endif

	for (i = 0; i < nSize; i ++)
		p[i] = GSPI_ReadByte (pSpi);
#endif
}
