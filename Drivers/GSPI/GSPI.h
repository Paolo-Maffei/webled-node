/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

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
