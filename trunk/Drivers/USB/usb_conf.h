/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_conf.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Joystick Mouse demo configuration file
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/
/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM     (3)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

#define BASEADDR_DATA   (BTABLE_ADDRESS + 0x00000040)

// ENP0
#define ENDP0_PACKETSIZE      0x40
#define ENDP0_RXADDR          BASEADDR_DATA
#define ENDP0_TXADDR          (ENDP0_RXADDR + ENDP0_PACKETSIZE)

// ENP1
#define ENDP1_PACKETSIZE      0x40
#define ENDP1_RXADDR          (ENDP0_TXADDR + ENDP0_PACKETSIZE)
#define ENDP1_TXADDR          (ENDP1_RXADDR + ENDP1_PACKETSIZE)

// ENP2
#define ENDP2_PACKETSIZE      0x40
#define ENDP2_RXADDR          (ENDP1_TXADDR + ENDP1_PACKETSIZE)
#define ENDP2_TXADDR          (ENDP2_RXADDR + ENDP2_PACKETSIZE)

// ENP3
#define ENDP3_PACKETSIZE      0x40
#define ENDP3_RXADDR          (ENDP2_TXADDR + ENDP2_PACKETSIZE)
#define ENDP3_TXADDR          (ENDP3_RXADDR + ENDP3_PACKETSIZE)

// ENP4
#define ENDP4_PACKETSIZE      0x40
#define ENDP4_RXADDR          (ENDP3_TXADDR + ENDP3_PACKETSIZE)
#define ENDP4_TXADDR          (ENDP4_RXADDR + ENDP4_PACKETSIZE)

// ENP5
#define ENDP5_PACKETSIZE      0x40
#define ENDP5_RXADDR          (ENDP4_TXADDR + ENDP4_PACKETSIZE)
#define ENDP5_TXADDR          (ENDP5_RXADDR + ENDP5_PACKETSIZE)

// ENP6
#define ENDP6_PACKETSIZE      0x40
#define ENDP6_RXADDR          (ENDP5_TXADDR + ENDP5_PACKETSIZE)
#define ENDP6_TXADDR          (ENDP6_RXADDR + ENDP6_PACKETSIZE)

// ENP7
#define ENDP7_PACKETSIZE      0x40
#define ENDP7_RXADDR          (ENDP6_TXADDR + ENDP6_PACKETSIZE)
#define ENDP7_TXADDR          (ENDP7_RXADDR + ENDP7_PACKETSIZE)

/* EP0  */
/* rx/tx buffer base address */
//#define ENDP0_RXADDR        (0x18)
//#define ENDP0_TXADDR        (0x58)

/* EP1  */
/* tx buffer base address */
//#define ENDP1_TXADDR        (0x100)

// EP2
//#define ENDP2_RXADDR        (0x140)
//#define ENDP2_TXADDR        (0x180)

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
//#define IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM \
//                | CNTR_ESOFM | CNTR_RESETM )

#define IMR_MSK (CNTR_CTRM | CNTR_RESETM )

/* CTR service routines */
/* associated to defined endpoints */
#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /*__USB_CONF_H*/

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

