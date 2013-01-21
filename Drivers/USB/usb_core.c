/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_core.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Standard protocol processing (USB v2.0)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "HidDfuApp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ValBit(VAR,Place)    (VAR & (1 << Place))
#define SetBit(VAR,Place)    (VAR |= (1 << Place))
#define ClrBit(VAR,Place)    (VAR &= ((1 << Place) ^ 255))

#define Send0LengthData() { _SetEPTxCount(ENDP0, 0); \
		vSetEPTxStatus(EP_TX_VALID); \
	}

#define vSetEPRxStatus(st) (SaveRState = st)
#define vSetEPTxStatus(st) (SaveTState = st)

#define USB_StatusIn() Send0LengthData()
#define USB_StatusOut() vSetEPRxStatus(EP_RX_VALID)

#define StatusInfo0 StatusInfo.bw.bb1 /* Reverse bb0 & bb1 */
#define StatusInfo1 StatusInfo.bw.bb0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16_u8 StatusInfo;
BOOL Data_Mul_MaxPacketSize = FALSE;
/* Private function prototypes -----------------------------------------------*/
static void DataStageOut(void);
static void DataStageIn(void);
static void NoData_Setup0(void);
static void Data_Setup0(void);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Standard_GetConfiguration.
* Description    : Return the current configuration variable address.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 1 , if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
UINT8 *Standard_GetConfiguration(UINT16 Length)
{
	if (Length == 0) {
		g_pInformation->Ctrl_Info.Usb_wLength =
		    sizeof(g_pInformation->Current_Configuration);
		return 0;
	}
	g_pUserStandardRequests->User_GetConfiguration();
	return (UINT8 *)&g_pInformation->Current_Configuration;
}

/*******************************************************************************
* Function Name  : Standard_SetConfiguration.
* Description    : This routine is called to set the configuration value
*                  Then each class should configure device themself.
* Input          : None.
* Output         : None.
* Return         : Return USB_SUCCESS, if the request is performed.
*                  Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetConfiguration(void)
{
	TRACE("+Standard_SetConfiguration()\r\n");

	if ((g_pInformation->USBwValue0 <=
		g_pDeviceTable->Total_Configuration) && (g_pInformation->USBwValue1 == 0)
		&& (g_pInformation->USBwIndex == 0)) { /*call Back usb spec 2.0*/
		if (!g_pInformation->Current_Configuration)	// add by ydgong 2010/12/19
			g_pInformation->Current_Configuration = g_pInformation->USBwValue0;
		g_pUserStandardRequests->User_SetConfiguration();
		return USB_SUCCESS;
	}
	else {
		return USB_UNSUPPORT;
	}
}

/*******************************************************************************
* Function Name  : Standard_GetInterface.
* Description    : Return the Alternate Setting of the current interface.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
UINT8 *Standard_GetInterface(UINT16 Length)
{
	TRACE("+Standard_GetInterface()\r\n");

	if (Length == 0) {
		g_pInformation->Ctrl_Info.Usb_wLength =
		    sizeof(g_pInformation->Current_AlternateSetting);
		return 0;
	}
	g_pUserStandardRequests->User_GetInterface();
	return (UINT8 *)&g_pInformation->Current_AlternateSetting;
}

/*******************************************************************************
* Function Name  : Standard_SetInterface.
* Description    : This routine is called to set the interface.
*                  Then each class should configure the interface them self.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetInterface(void)
{
	RESULT Re;
	/*Test if the specified Interface and Alternate Setting are supported by
	  the application Firmware*/
	Re = (*g_pDeviceProperty->Class_Get_Interface_Setting)(g_pInformation->USBwIndex0, g_pInformation->USBwValue0);

	if (g_pInformation->Current_Configuration != 0) {
		if ((Re != USB_SUCCESS) || (g_pInformation->USBwIndex1 != 0)
		    || (g_pInformation->USBwValue1 != 0)) {
			return  USB_UNSUPPORT;
		}
		else if (Re == USB_SUCCESS) {
			g_pUserStandardRequests->User_SetInterface();
			g_pInformation->Current_Interface = g_pInformation->USBwIndex0;
			g_pInformation->Current_AlternateSetting = g_pInformation->USBwValue0;
			return USB_SUCCESS;
		}

	}

	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_GetStatus.
* Description    : Copy the device request data to "StatusInfo buffer".
* Input          : - Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is at end of data block,
*                  or is invalid when "Length" is 0.
*******************************************************************************/
UINT8 *Standard_GetStatus(UINT16 Length)
{
	if (Length == 0) {
		g_pInformation->Ctrl_Info.Usb_wLength = 2;
		return 0;
	}

	StatusInfo.w = 0;
	/* Reset Status Information */

	if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
		/*Get Device Status */
		UINT8 Feature = g_pInformation->Current_Feature;

		/* Remote Wakeup enabled */
		if (ValBit(Feature, 5)) {
			SetBit(StatusInfo0, 1);
		}

		/* Bus-powered */
		if (ValBit(Feature, 6)) {
			ClrBit(StatusInfo0, 0);
		}
		else { /* Self-powered */
			SetBit(StatusInfo0, 0);
		}
	}
	/*Interface Status*/
	else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
		return (UINT8 *)&StatusInfo;
	}
	/*Get EndPoint Status*/
	else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) {
		UINT8 Related_Endpoint;
		UINT8 wIndex0 = g_pInformation->USBwIndex0;

		Related_Endpoint = (wIndex0 & 0x0f);
		if (ValBit(wIndex0, 7)) {
			/* IN endpoint */
			if (_GetTxStallStatus(Related_Endpoint)) {
				SetBit(StatusInfo0, 0); /* IN Endpoint stalled */
			}
		}
		else {
			/* OUT endpoint */
			if (_GetRxStallStatus(Related_Endpoint)) {
				SetBit(StatusInfo0, 0); /* OUT Endpoint stalled */
			}
		}

	}
	else {
		return NULL;
	}
	g_pUserStandardRequests->User_GetStatus();
	return (UINT8 *)&StatusInfo;
}

/*******************************************************************************
* Function Name  : Standard_ClearFeature.
* Description    : Clear or disable a specific feature.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_ClearFeature(void)
{
	UINT32     Type_Rec = Type_Recipient;
	UINT32     Status;

	if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
		/*Device Clear Feature*/
		ClrBit(g_pInformation->Current_Feature, 5);
		return USB_SUCCESS;
	}
	else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) {
		/*EndPoint Clear Feature*/
		DEVICE* pDev;
		UINT32 Related_Endpoint;
		UINT32 wIndex0;
		UINT32 rEP;

		if ((g_pInformation->USBwValue != ENDPOINT_STALL)
		    || (g_pInformation->USBwIndex1 != 0)) {
			return USB_UNSUPPORT;
		}

		pDev = g_pDeviceTable;
		wIndex0 = g_pInformation->USBwIndex0;
		rEP = wIndex0 & ~0x80;
		Related_Endpoint = ENDP0 + rEP;

		if (ValBit(g_pInformation->USBwIndex0, 7)) {
			/*Get Status of endpoint & stall the request if the related_ENdpoint
			is Disabled*/
			Status = _GetEPTxStatus(Related_Endpoint);
		}
		else {
			Status = _GetEPRxStatus(Related_Endpoint);
		}

		if ((rEP >= pDev->Total_Endpoint) || (Status == 0)
		    || (g_pInformation->Current_Configuration == 0)) {
			return USB_UNSUPPORT;
		}

		if (wIndex0 & 0x80) {
			/* IN endpoint */
			if (_GetTxStallStatus(Related_Endpoint)) {
				ClearDTOG_TX(Related_Endpoint);
				SetEPTxStatus(Related_Endpoint, EP_TX_VALID);
			}
		}
		else {
			/* OUT endpoint */
			if (_GetRxStallStatus(Related_Endpoint)) {
				if (Related_Endpoint == ENDP0) {
					/* After clear the STALL, enable the default endpoint receiver */
					SetEPRxCount(Related_Endpoint, g_pDeviceProperty->MaxPacketSize);
					_SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
				}
				else {
					ClearDTOG_RX(Related_Endpoint);
					_SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
				}
			}
		}
		g_pUserStandardRequests->User_ClearFeature();
		return USB_SUCCESS;
	}

	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_SetEndPointFeature
* Description    : Set or enable a specific feature of EndPoint
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetEndPointFeature(void)
{
	UINT32    wIndex0;
	UINT32    Related_Endpoint;
	UINT32    rEP;
	UINT32   Status;

	wIndex0 = g_pInformation->USBwIndex0;
	rEP = wIndex0 & ~0x80;
	Related_Endpoint = ENDP0 + rEP;

	if (ValBit(g_pInformation->USBwIndex0, 7)) {
		/* get Status of endpoint & stall the request if the related_ENdpoint
		is Disabled*/
		Status = _GetEPTxStatus(Related_Endpoint);
	}
	else {
		Status = _GetEPRxStatus(Related_Endpoint);
	}

	if (Related_Endpoint >= g_pDeviceTable->Total_Endpoint
	    || g_pInformation->USBwValue != 0 || Status == 0
	    || g_pInformation->Current_Configuration == 0) {
		return USB_UNSUPPORT;
	}
	else {
		if (wIndex0 & 0x80) {
			/* IN endpoint */
			_SetEPTxStatus(Related_Endpoint, EP_TX_STALL);
		}

		else {
			/* OUT endpoint */
			_SetEPRxStatus(Related_Endpoint, EP_RX_STALL);
		}
	}
	g_pUserStandardRequests->User_SetEndPointFeature();
	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_SetDeviceFeature.
* Description    : Set or enable a specific feature of Device.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetDeviceFeature(void)
{
	SetBit(g_pInformation->Current_Feature, 5);
	g_pUserStandardRequests->User_SetDeviceFeature();
	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_GetDescriptorData.
* Description    : Standard_GetDescriptorData is used for descriptors transfer.
*                : This routine is used for the descriptors resident in Flash
*                  or RAM
*                  pDesc can be in either Flash or RAM
*                  The purpose of this routine is to have a versatile way to
*                  response descriptors request. It allows user to generate
*                  certain descriptors with software or read descriptors from
*                  external storage part by part.
* Input          : - Length - Length of the data in this transfer.
*                  - pDesc - A pointer points to descriptor struct.
*                  The structure gives the initial address of the descriptor and
*                  its original size.
* Output         : None.
* Return         : Address of a part of the descriptor pointed by the Usb_
*                  wOffset The buffer pointed by this address contains at least
*                  Length bytes.
*******************************************************************************/
UINT8 *Standard_GetDescriptorData(UINT16 Length, ONE_DESCRIPTOR *pDesc)
{
	UINT32  wOffset;

	wOffset = g_pInformation->Ctrl_Info.Usb_wOffset;
	if (Length == 0) {
		g_pInformation->Ctrl_Info.Usb_wLength = pDesc->Descriptor_Size - wOffset;
		return 0;
	}

	return pDesc->Descriptor + wOffset;
}

/*******************************************************************************
* Function Name  : DataStageOut.
* Description    : Data stage of a Control Write Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void DataStageOut(void)
{
	ENDPOINT_INFO *pEPinfo = &g_pInformation->Ctrl_Info;
	UINT32 save_rLength;

	save_rLength = pEPinfo->Usb_rLength;

	if (pEPinfo->CopyData && save_rLength) {
		UINT8 *Buffer;
		UINT32 Length;

		Length = pEPinfo->PacketSize;
		if (Length > save_rLength) {
			Length = save_rLength;
		}

		Buffer = (*pEPinfo->CopyData)(Length);
		pEPinfo->Usb_rLength -= Length;
		pEPinfo->Usb_rOffset += Length;

		PMAToUserBufferCopy(Buffer, GetEPRxAddr(ENDP0), Length);
	}

	if (pEPinfo->Usb_rLength != 0) {
		vSetEPRxStatus(EP_RX_VALID);/* re-enable for next data reception */
		SetEPTxCount(ENDP0, 0);
		vSetEPTxStatus(EP_TX_VALID);/* Expect the host to abort the data OUT stage */
	}
	/* Set the next State*/
	if (pEPinfo->Usb_rLength >= pEPinfo->PacketSize) {
		g_pInformation->ControlState = OUT_DATA;
	}
	else {
		if (pEPinfo->Usb_rLength > 0) {
			g_pInformation->ControlState = LAST_OUT_DATA;
		}
		else if (pEPinfo->Usb_rLength == 0) {
			g_pInformation->ControlState = WAIT_STATUS_IN;
			USB_StatusIn();
		}
	}
}

/*******************************************************************************
* Function Name  : DataStageIn.
* Description    : Data stage of a Control Read Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void DataStageIn(void)
{
	ENDPOINT_INFO *pEPinfo = &g_pInformation->Ctrl_Info;
	UINT32 save_wLength = pEPinfo->Usb_wLength;
	UINT32 ControlState = g_pInformation->ControlState;

	UINT8 *DataBuffer;
	UINT32 Length;

	if ((save_wLength == 0) && (ControlState == LAST_IN_DATA)) {
		if (Data_Mul_MaxPacketSize == TRUE) {
			/* No more data to send and empty packet */
			Send0LengthData();
			ControlState = LAST_IN_DATA;
			Data_Mul_MaxPacketSize = FALSE;
		}
		else {
			/* No more data to send so STALL the TX Status*/
			ControlState = WAIT_STATUS_OUT;
			vSetEPTxStatus(EP_TX_STALL);
		}

		goto Expect_Status_Out;
	}

	Length = pEPinfo->PacketSize;
	ControlState = (save_wLength <= Length) ? LAST_IN_DATA : IN_DATA;

	if (Length > save_wLength) {
		Length = save_wLength;
	}

	DataBuffer = (*pEPinfo->CopyData)(Length);
	UserToPMABufferCopy(DataBuffer, GetEPTxAddr(ENDP0), Length);

	SetEPTxCount(ENDP0, Length);

	pEPinfo->Usb_wLength -= Length;
	pEPinfo->Usb_wOffset += Length;
	vSetEPTxStatus(EP_TX_VALID);

	USB_StatusOut();/* Expect the host to abort the data IN stage */

Expect_Status_Out:
	g_pInformation->ControlState = ControlState;
}

/*******************************************************************************
* Function Name  : NoData_Setup0.
* Description    : Proceed the processing of setup request without data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NoData_Setup0(void)
{
	RESULT Result = USB_UNSUPPORT;
	UINT32 RequestNo = g_pInformation->USBbRequest;
	UINT32 ControlState;

	if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
		/* Device Request*/
		/* SET_CONFIGURATION*/
		if (RequestNo == SET_CONFIGURATION) {
			Result = Standard_SetConfiguration();
		}

		/*SET ADDRESS*/
		else if (RequestNo == SET_ADDRESS) {
			if ((g_pInformation->USBwValue0 > 127) || (g_pInformation->USBwValue1 != 0)
			    || (g_pInformation->USBwIndex != 0)
			    /* || (g_pInformation->Current_Configuration != 0) Commented by ydgong 2010/12/19 */ )
				/* Device Address should be 127 or less*/
			{
				ControlState = STALLED;
				goto exit_NoData_Setup0;
			}
			else {
				Result = USB_SUCCESS;
			}
		}
		/*SET FEATURE for Device*/
		else if (RequestNo == SET_FEATURE) {
			if ((g_pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP)
			    && (g_pInformation->USBwIndex == 0)
			    && (ValBit(g_pInformation->Current_Feature, 5))) {
				Result = Standard_SetDeviceFeature();
			}
			else {
				Result = USB_UNSUPPORT;
			}
		}
		/*Clear FEATURE for Device */
		else if (RequestNo == CLEAR_FEATURE) {
			if (g_pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP
			    && g_pInformation->USBwIndex == 0
			    && ValBit(g_pInformation->Current_Feature, 5)) {
				Result = Standard_ClearFeature();
			}
			else {
				Result = USB_UNSUPPORT;
			}
		}

	}

	/* Interface Request*/
	else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
		/*SET INTERFACE*/
		if (RequestNo == SET_INTERFACE) {
			Result = Standard_SetInterface();
		}
	}

	/* EndPoint Request*/
	else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) {
		/*CLEAR FEATURE for EndPoint*/
		if (RequestNo == CLEAR_FEATURE) {
			Result = Standard_ClearFeature();
		}
		/* SET FEATURE for EndPoint*/
		else if (RequestNo == SET_FEATURE) {
			Result = Standard_SetEndPointFeature();
		}
	}
	else {
		Result = USB_UNSUPPORT;
	}

	if (Result != USB_SUCCESS) {
		Result = (*g_pDeviceProperty->Class_NoData_Setup)(RequestNo);
		if (Result == USB_NOT_READY) {
			ControlState = PAUSE;
			goto exit_NoData_Setup0;
		}
	}

	if (Result != USB_SUCCESS) {
		ControlState = STALLED;
		goto exit_NoData_Setup0;
	}

	ControlState = WAIT_STATUS_IN;/* After no data stage SETUP */

	USB_StatusIn();

exit_NoData_Setup0:
	g_pInformation->ControlState = ControlState;
	return;
}

/*******************************************************************************
* Function Name  : Data_Setup0.
* Description    : Proceed the processing of setup request with data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Data_Setup0(void)
{
	UINT8 *(*CopyRoutine)(UINT16);
	RESULT Result;
	UINT32 Request_No = g_pInformation->USBbRequest;

	UINT32 Related_Endpoint, Reserved;
	UINT32 wOffset, Status;

	CopyRoutine = NULL;
	wOffset = 0;

	if (Request_No == GET_DESCRIPTOR) {
		if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
			UINT8 wValue1 = g_pInformation->USBwValue1;
			if (wValue1 == DEVICE_DESCRIPTOR) {
				CopyRoutine = g_pDeviceProperty->GetDeviceDescriptor;
			}
			else if (wValue1 == CONFIG_DESCRIPTOR) {
				CopyRoutine = g_pDeviceProperty->GetConfigDescriptor;
			}
			else if (wValue1 == STRING_DESCRIPTOR) {
				CopyRoutine = g_pDeviceProperty->GetStringDescriptor;
			}  /* End of GET_DESCRIPTOR */
		}
	}
	/*GET STATUS*/
	else if ((Request_No == GET_STATUS) && (g_pInformation->USBwValue == 0)
	         && (g_pInformation->USBwLength == 0x0002)
	         && (g_pInformation->USBwIndex1 == 0)) {
		/* GET STATUS for Device*/
		if ((Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
		    && (g_pInformation->USBwIndex == 0)) {
			CopyRoutine = Standard_GetStatus;
		}

		/* GET STATUS for Interface*/
		else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
			if (((*g_pDeviceProperty->Class_Get_Interface_Setting)(g_pInformation->USBwIndex0, 0) == USB_SUCCESS)
			    && (g_pInformation->Current_Configuration != 0)) {
				CopyRoutine = Standard_GetStatus;
			}
		}

		/* GET STATUS for EndPoint*/
		else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) {
			Related_Endpoint = (g_pInformation->USBwIndex0 & 0x0f);
			Reserved = g_pInformation->USBwIndex0 & 0x70;

			if (ValBit(g_pInformation->USBwIndex0, 7)) {
				/*Get Status of endpoint & stall the request if the related_ENdpoint
				is Disabled*/
				Status = _GetEPTxStatus(Related_Endpoint);
			}
			else {
				Status = _GetEPRxStatus(Related_Endpoint);
			}

			if ((Related_Endpoint < g_pDeviceTable->Total_Endpoint) && (Reserved == 0)
			    && (Status != 0)) {
				CopyRoutine = Standard_GetStatus;
			}
		}

	}
	/*GET CONFIGURATION*/
	else if (Request_No == GET_CONFIGURATION) {
		if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
			CopyRoutine = Standard_GetConfiguration;
		}
	}
	/*GET INTERFACE*/
	else if (Request_No == GET_INTERFACE) {
		if ((Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
		    && (g_pInformation->Current_Configuration != 0) && (g_pInformation->USBwValue == 0)
		    && (g_pInformation->USBwIndex1 == 0) && (g_pInformation->USBwLength == 0x0001)
		    && ((*g_pDeviceProperty->Class_Get_Interface_Setting)(g_pInformation->USBwIndex0, 0) == USB_SUCCESS)) {
			CopyRoutine = Standard_GetInterface;
		}
	}

	if (CopyRoutine) {
		g_pInformation->Ctrl_Info.Usb_wOffset = wOffset;
		g_pInformation->Ctrl_Info.CopyData = CopyRoutine;
		/* sb in the original the cast to word was directly */
		/* now the cast is made step by step */
		(*CopyRoutine)(0);
		Result = USB_SUCCESS;
	}
	else {
		Result = (*g_pDeviceProperty->Class_Data_Setup)(g_pInformation->USBbRequest);
		if (Result == USB_NOT_READY) {
			g_pInformation->ControlState = PAUSE;
			return;
		}
	}

	if (g_pInformation->Ctrl_Info.Usb_wLength == 0xFFFF) {
		/* Data is not ready, wait it */
		g_pInformation->ControlState = PAUSE;
		return;
	}
	if ((Result == USB_UNSUPPORT) || (g_pInformation->Ctrl_Info.Usb_wLength == 0)) {
		/* Unsupported request */
		g_pInformation->ControlState = STALLED;
		return;
	}

	if (ValBit(g_pInformation->USBbmRequestType, 7)) {
		/* Device ==> Host */
		vu32 wLength = g_pInformation->USBwLength;

		/* Restrict the data length to be the one host asks */
		if (g_pInformation->Ctrl_Info.Usb_wLength > wLength) {
			g_pInformation->Ctrl_Info.Usb_wLength = wLength;
		}

		else if (g_pInformation->Ctrl_Info.Usb_wLength < g_pInformation->USBwLength) {
			if (g_pInformation->Ctrl_Info.Usb_wLength < g_pDeviceProperty->MaxPacketSize) {
				Data_Mul_MaxPacketSize = FALSE;
			}
			else if ((g_pInformation->Ctrl_Info.Usb_wLength % g_pDeviceProperty->MaxPacketSize) == 0) {
				Data_Mul_MaxPacketSize = TRUE;
			}
		}

		g_pInformation->Ctrl_Info.PacketSize = g_pDeviceProperty->MaxPacketSize;
		DataStageIn();
	}
	else {
		g_pInformation->ControlState = OUT_DATA;
		vSetEPRxStatus(EP_RX_VALID); /* enable for next data reception */
	}

	return;
}

/*******************************************************************************
* Function Name  : Setup0_Process
* Description    : Get the device request data and dispatch to individual process.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
UINT8 Setup0_Process(void)
{
	union {
		UINT8* b;
		UINT16* w;
	} pBuf;

	pBuf.b = PMAAddr + (UINT8 *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr */

	if (g_pInformation->ControlState != PAUSE) {
		g_pInformation->USBbmRequestType = *pBuf.b++; /* bmRequestType */
		g_pInformation->USBbRequest = *pBuf.b++; /* bRequest */
		pBuf.w++;  /* word not accessed because of 32 bits addressing */
		g_pInformation->USBwValue = ByteSwap(*pBuf.w++); /* wValue */
		pBuf.w++;  /* word not accessed because of 32 bits addressing */
		g_pInformation->USBwIndex  = ByteSwap(*pBuf.w++); /* wIndex */
		pBuf.w++;  /* word not accessed because of 32 bits addressing */
		g_pInformation->USBwLength = *pBuf.w; /* wLength */
	}
/*
	{ // Copy data for Ps3UsbKey hub
		extern s8 usb_ep0_rx_buffer[8];
		s8 *pEp0RxBuf = (s8*)g_pInformation;

		usb_ep0_rx_buffer[0] = pEp0RxBuf[0];
		usb_ep0_rx_buffer[1] = pEp0RxBuf[1];
		usb_ep0_rx_buffer[2] = pEp0RxBuf[3];
		usb_ep0_rx_buffer[3] = pEp0RxBuf[2];
		usb_ep0_rx_buffer[4] = pEp0RxBuf[5];
		usb_ep0_rx_buffer[5] = pEp0RxBuf[4];
		usb_ep0_rx_buffer[6] = pEp0RxBuf[6];
		usb_ep0_rx_buffer[7] = pEp0RxBuf[7];


		TRACE("rx0: 0x%04X\r\n", usb_ep0_rx_buffer[0]&0xFF);
		TRACE("rx1: 0x%04X\r\n", usb_ep0_rx_buffer[1]&0xFF);
		TRACE("rx2: 0x%04X\r\n", usb_ep0_rx_buffer[2]&0xFF);
		TRACE("rx3: 0x%04X\r\n", usb_ep0_rx_buffer[3]&0xFF);
		TRACE("rx4: 0x%04X\r\n", usb_ep0_rx_buffer[4]&0xFF);
		TRACE("rx5: 0x%04X\r\n", usb_ep0_rx_buffer[5]&0xFF);
		TRACE("rx6: 0x%04X\r\n", usb_ep0_rx_buffer[6]&0xFF);
		TRACE("rx7: 0x%04X\r\n", usb_ep0_rx_buffer[7]&0xFF);
	}
*/
	g_pInformation->ControlState = SETTING_UP;
	if (g_pInformation->USBwLength == 0) {
		/* Setup with no data stage */
		NoData_Setup0();
	}
	else {
		/* Setup with data stage */
		Data_Setup0();
	}
	return Post0_Process();
}

/*******************************************************************************
* Function Name  : In0_Process
* Description    : Process the IN token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
UINT8 In0_Process(void)
{
	UINT32 ControlState = g_pInformation->ControlState;

	if ((ControlState == IN_DATA) || (ControlState == LAST_IN_DATA)) {
		DataStageIn();
		/* ControlState may be changed outside the function */
		ControlState = g_pInformation->ControlState;
	}
	else if (ControlState == WAIT_STATUS_IN) {
		if ((g_pInformation->USBbRequest == SET_ADDRESS) &&
		    (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))) {
			SetDeviceAddress(g_pInformation->USBwValue0);
			g_pUserStandardRequests->User_SetDeviceAddress();
		}
		(*g_pDeviceProperty->Process_Status_IN)();
		ControlState = STALLED;
	}
	else {
		ControlState = STALLED;
	}

	g_pInformation->ControlState = ControlState;

	return Post0_Process();
}

/*******************************************************************************
* Function Name  : Out0_Process
* Description    : Process the OUT token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
UINT8 Out0_Process(void)
{
	UINT32 ControlState = g_pInformation->ControlState;

	if ((ControlState == OUT_DATA) || (ControlState == LAST_OUT_DATA)) {
		DataStageOut();
		ControlState = g_pInformation->ControlState; /* may be changed outside the function */
	}

	else if (ControlState == WAIT_STATUS_OUT) {
		(*g_pDeviceProperty->Process_Status_OUT)();
		ControlState = STALLED;
	}

	else if ((ControlState == IN_DATA) || (ControlState == LAST_IN_DATA)) {
		/* host aborts the transfer before finish */
		ControlState = STALLED;
	}

	/* Unexpect state, STALL the endpoint */
	else {
		ControlState = STALLED;
	}

	g_pInformation->ControlState = ControlState;

	return Post0_Process();
}

/*******************************************************************************
* Function Name  : Post0_Process
* Description    : Stall the Endpoint 0 in case of error.
* Input          : None.
* Output         : None.
* Return         : - 0 if the control State is in PAUSE
*                  - 1 if not.
*******************************************************************************/
UINT8 Post0_Process(void)
{
	SetEPRxCount(ENDP0, g_pDeviceProperty->MaxPacketSize);

	if (g_pInformation->ControlState == STALLED) {
		vSetEPRxStatus(EP_RX_STALL);
		vSetEPTxStatus(EP_TX_STALL);
	}

	return (g_pInformation->ControlState == PAUSE);
}

/*******************************************************************************
* Function Name  : SetDeviceAddress.
* Description    : Set the device and all the used Endpoints addresses.
* Input          : - Val: device adress.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDeviceAddress(UINT8 Val)
{
	UINT32 i;
	UINT32 nEP = g_pDeviceTable->Total_Endpoint;

	TRACE("SetDevAddr: %d\r\n" , Val);

	/* set address in every used endpoint */
	for (i = 0; i < nEP; i++) {
		_SetEPAddress((UINT8)i, (UINT8)i);
	} /* for */
	_SetDADDR(Val | DADDR_EF); /* set device address and enable function */
}

/*******************************************************************************
* Function Name  : NOP_Process
* Description    : No operation function.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NOP_Process(void)
{
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
