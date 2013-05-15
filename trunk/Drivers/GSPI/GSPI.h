/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#ifndef __GSPI_H__
#define __GSPI_H__
//
// Macros define
//

// Compile config

//#define GSPI_USE_DMA
#define GSPI_USE_8BIT

#define GSPI_CS_ENABLE(pSpi)				GPIO_ResetBits(pSpi->pGPIOPort, pSpi->usCsPin)
#define GSPI_CS_DISABLE(pSpi)				GPIO_SetBits(pSpi->pGPIOPort, pSpi->usCsPin)

#define GSPI_GPIO_GET_PIN(pSpi, pin)			pSpi->pGPIOPort->IDR&pin
#define GSPI_GPIO_SET_PIN(pSpi, pin)			pSpi->pGPIOPort->BSRR=pin
#define GSPI_GPIO_CLR_PIN(pSpi, pin)			pSpi->pGPIOPort->BRR=pin

//
// Structs define
//
#define GSPI_MODE_SPI						0
#define GSPI_MODE_GPIO						1

typedef struct _STM32SPI {
	SPI_TypeDef* SPIx;
	GPIO_TypeDef* pGPIOPort;
	USHORT	usCsPin;
	USHORT	usClkPin;
	USHORT	usMisoPin;
	USHORT	usMosiPin;
	UCHAR ucMode;
} Stm32Spi, *PSTM32SPI;

//
// Exported functions declare
//
BOOL GSPI_Init(PSTM32SPI pSpi);
UINT16 GSPI_WriteByte(PSTM32SPI pSpi, UINT16 usData);
UINT16 GSPI_ReadByte(PSTM32SPI pSpi);
#ifdef GSPI_USE_DMA
void GSPI_WriteDMA (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize);
void GSPI_ReadDMA (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize);
#endif
void GSPI_Write (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize);
void GSPI_Read (PSTM32SPI pSpi, PBYTE pBuffer, UINT32 nSize);

#endif // __GSPI_H__
