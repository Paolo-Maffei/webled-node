/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_core.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Standard protocol processing functions prototypes
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CORE_H
#define __USB_CORE_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _CONTROL_STATE {
	WAIT_SETUP,       /* 0 */
	SETTING_UP,       /* 1 */
	IN_DATA,          /* 2 */
	OUT_DATA,         /* 3 */
	LAST_IN_DATA,     /* 4 */
	LAST_OUT_DATA,    /* 5 */
	WAIT_STATUS_IN,   /* 7 */
	WAIT_STATUS_OUT,  /* 8 */
	STALLED,          /* 9 */
	PAUSE             /* 10 */
} CONTROL_STATE;    /* The state machine states of a control pipe */

typedef struct OneDescriptor {
	UINT8 *Descriptor;
	UINT16 Descriptor_Size;
}
ONE_DESCRIPTOR, *PONE_DESCRIPTOR;
/* All the request process routines return a value of this type
   If the return value is not SUCCESS or NOT_READY,
   the software will STALL the correspond endpoint */
typedef enum _RESULT {
	USB_SUCCESS = 0,    /* Process sucessfully */
	USB_ERROR,
	USB_UNSUPPORT,
	USB_NOT_READY       /* The process has not been finished, endpoint will be
                         NAK to further rquest */
} RESULT;


/*-*-*-*-*-*-*-*-*-*-* Definitions for endpoint level -*-*-*-*-*-*-*-*-*-*-*-*/
typedef struct _ENDPOINT_INFO {
	/* When send data out of the device,
	 CopyData() is used to get data buffer 'Length' bytes data
	 if Length is 0,
	  CopyData() returns the total length of the data
	  if the request is not supported, returns 0
	  (NEW Feature )
	   if CopyData() returns -1, the calling routine should not proceed
	   further and will resume the SETUP process by the class device
	 if Length is not 0,
	  CopyData() returns a pointer to indicate the data location
	 Usb_wLength is the data remain to be sent,
	 Usb_wOffset is the Offset of original data
	When receive data from the host,
	 CopyData() is used to get user data buffer which is capable
	 of Length bytes data to copy data from the endpoint buffer.
	 if Length is 0,
	  CopyData() returns the available data length,
	 if Length is not 0,
	  CopyData() returns user buffer address
	 Usb_rLength is the data remain to be received,
	 Usb_rPointer is the Offset of data buffer
	*/
	UINT16  Usb_wLength;
	UINT16  Usb_wOffset;
	UINT16  PacketSize;
	UINT8   *(*CopyData)(UINT16 Length);
} ENDPOINT_INFO;

/*-*-*-*-*-*-*-*-*-*-*-* Definitions for device level -*-*-*-*-*-*-*-*-*-*-*-*/

typedef struct _DEVICE {
	UINT8 Total_Endpoint;     /* Number of endpoints that are used */
	UINT8 Total_Configuration;/* Number of configuration available */
}
DEVICE;

typedef union {
	UINT16 w;
	struct BW {
		UINT8 bb1;
		UINT8 bb0;
	}
	bw;
} u16_u8;

typedef struct _DEVICE_INFO {
	UINT8 USBbmRequestType;       /* bmRequestType */
	UINT8 USBbRequest;            /* bRequest */
	u16_u8 USBwValues;         /* wValue */
	u16_u8 USBwIndexs;         /* wIndex */
	u16_u8 USBwLengths;        /* wLength */

	UINT8 ControlState;           /* of type CONTROL_STATE */
	UINT8 Current_Feature;
	UINT8 Current_Configuration;   /* Selected configuration */
	UINT8 Current_Interface;       /* Selected interface of current configuration */
	UINT8 Current_AlternateSetting;/* Selected Alternate Setting of current
                                     interface*/

	ENDPOINT_INFO Ctrl_Info;
} DEVICE_INFO;

typedef struct _DEVICE_PROP {
	void (*Init)(void);        /* Initialize the device */
	void (*Reset)(void);       /* Reset routine of this device */

	/* Device dependent process after the status stage */
	void (*Process_Status_IN)(void);
	void (*Process_Status_OUT)(void);

	/* Procedure of process on setup stage of a class specified request with data stage */
	/* All class specified requests with data stage are processed in Class_Data_Setup
	 Class_Data_Setup()
	  responses to check all special requests and fills ENDPOINT_INFO
	  according to the request
	  If IN tokens are expected, then wLength & wOffset will be filled
	  with the total transferring bytes and the starting position
	  If OUT tokens are expected, then rLength & rOffset will be filled
	  with the total expected bytes and the starting position in the buffer

	  If the request is valid, Class_Data_Setup returns SUCCESS, else UNSUPPORT

	 CAUTION:
	  Since GET_CONFIGURATION & GET_INTERFACE are highly related to
	  the individual classes, they will be checked and processed here.
	*/
	RESULT(*Class_Data_Setup)(UINT8 RequestNo);

	/* Procedure of process on setup stage of a class specified request without data stage */
	/* All class specified requests without data stage are processed in Class_NoData_Setup
	 Class_NoData_Setup
	  responses to check all special requests and perform the request

	 CAUTION:
	  Since SET_CONFIGURATION & SET_INTERFACE are highly related to
	  the individual classes, they will be checked and processed here.
	*/
	RESULT(*Class_NoData_Setup)(UINT8 RequestNo);

	/*Class_Get_Interface_Setting
	 This function is used by the file usb_core.c to test if the selected Interface
	 and Alternate Setting (UINT8 Interface, UINT8 AlternateSetting) are supported by
	 the application.
	 This function is writing by user. It should return "SUCCESS" if the Interface
	 and Alternate Setting are supported by the application or "UNSUPPORT" if they
	 are not supported. */

	RESULT(*Class_Get_Interface_Setting)(UINT8 Interface, UINT8 AlternateSetting);

	UINT8*(*GetDeviceDescriptor)(UINT16 Length);
	UINT8*(*GetConfigDescriptor)(UINT16 Length);
	UINT8*(*GetStringDescriptor)(UINT16 Length);

	UINT8* RxEP_buffer;
	UINT8 MaxPacketSize;

} DEVICE_PROP;

typedef struct _USER_STANDARD_REQUESTS {
	void (*User_GetConfiguration)(void);       /* Get Configuration */
	void (*User_SetConfiguration)(void);       /* Set Configuration */
	void (*User_GetInterface)(void);           /* Get Interface */
	void (*User_SetInterface)(void);           /* Set Interface */
	void (*User_GetStatus)(void);              /* Get Status */
	void (*User_ClearFeature)(void);           /* Clear Feature */
	void (*User_SetEndPointFeature)(void);     /* Set Endpoint Feature */
	void (*User_SetDeviceFeature)(void);       /* Set Device Feature */
	void (*User_SetDeviceAddress)(void);       /* Set Device Address */
}
USER_STANDARD_REQUESTS;

/* Exported constants --------------------------------------------------------*/
#define Type_Recipient (g_pInformation->USBbmRequestType & (REQUEST_TYPE | RECIPIENT))

#define Usb_rLength Usb_wLength
#define Usb_rOffset Usb_wOffset

#define USBwValue USBwValues.w
#define USBwValue0 USBwValues.bw.bb0
#define USBwValue1 USBwValues.bw.bb1
#define USBwIndex USBwIndexs.w
#define USBwIndex0 USBwIndexs.bw.bb0
#define USBwIndex1 USBwIndexs.bw.bb1
#define USBwLength USBwLengths.w
#define USBwLength0 USBwLengths.bw.bb0
#define USBwLength1 USBwLengths.bw.bb1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
UINT8 Setup0_Process(void);
UINT8 Post0_Process(void);
UINT8 Out0_Process(void);
UINT8 In0_Process(void);

RESULT Standard_SetEndPointFeature(void);
RESULT Standard_SetDeviceFeature(void);

UINT8 *Standard_GetConfiguration(UINT16 Length);
RESULT Standard_SetConfiguration(void);
UINT8 *Standard_GetInterface(UINT16 Length);
RESULT Standard_SetInterface(void);
UINT8 *Standard_GetDescriptorData(UINT16 Length, PONE_DESCRIPTOR pDesc);

UINT8 *Standard_GetStatus(UINT16 Length);
RESULT Standard_ClearFeature(void);
void SetDeviceAddress(UINT8);
void NOP_Process(void);

/* cells saving status during interrupt servicing */
extern UINT16 SaveRState;
extern UINT16 SaveTState;

#endif /* __USB_CORE_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
