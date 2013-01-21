#include "HidDfuApp.h"

//
// Global variables
//
UINT8 EPindex;
UINT16 SaveState ;
UINT16  wInterrupt_Mask;
DEVICE_INFO Device_Info;

//
// Global pointers
//
USER_STANDARD_REQUESTS  *g_pUserStandardRequests;
DEVICE *g_pDeviceTable;
DEVICE_INFO *g_pInformation;
DEVICE_PROP *g_pDeviceProperty;

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config(FunctionalState NewState)
{
	if (NewState == DISABLE) {
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);
	}
	else {
	  GPIO_SetBits(GPIOC, GPIO_Pin_10);		
	}
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
	/* Select USBCLK source : 72MHz / 1.5 = 48MHz */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

	/* Enable USB clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : USB_Init
* Description    : USB system initialization
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Config USB Enable pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// PC10 used as USB Enable
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// USB cable Reset
	USB_Cable_Config(DISABLE);
	// Enable USB clock
	Set_USBClock();
	
	do {
		g_pInformation = &Device_Info;
		g_pInformation->ControlState = 2;

		g_pDeviceProperty = &HidDfuDeviceProperty;
		g_pUserStandardRequests = &HidDfuUserStandardRequests;
		g_pDeviceTable = &HidDfuDeviceTable;
		/* Initialize devices one by one */
		g_pDeviceProperty->Init();
	} while (FALSE);
}
