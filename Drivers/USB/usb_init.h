/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_init.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Initialization routines & global variables
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_INIT_H
#define __USB_INIT_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USB_Init();
void USB_Cable_Config(FunctionalState NewState);
void USB_Interrupts_Config(void);
void Set_USBClock(void);;


/* External variables --------------------------------------------------------*/
/*  The number of current endpoint, it will be used to specify an endpoint */
extern UINT8   EPindex;
/*  The number of current device, it is an index to the Device_Table */
/*extern UINT8 Device_no; */
/*  Points to the DEVICE_INFO structure of current device */
/*  The purpose of this register is to speed up the execution */
extern DEVICE_INFO* g_pInformation;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
extern DEVICE_PROP* g_pDeviceProperty;
/*  Temporary save the state of Rx & Tx status. */
/*  Whenever the Rx or Tx state is changed, its value is saved */
/*  in this variable first and will be set to the EPRB or EPRA */
/*  at the end of interrupt process */
extern USER_STANDARD_REQUESTS *g_pUserStandardRequests;
extern DEVICE *g_pDeviceTable;

extern UINT16  SaveState ;
extern UINT16 wInterrupt_Mask;

#endif /* __USB_INIT_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
