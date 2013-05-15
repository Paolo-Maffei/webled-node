
#include "Project.h"
#include "Firmware.h"

UCHAR ucSpiDummyClk = 0x05;
WlanInterruptCallback pWlanInterruptCallback = NOP_Process;

BOOL If_Init(PWLAN_ADAPTER Adapter)
{
	BOOL rc = Hw_Init();

	if (!rc)
		If_Deinit(Adapter);

	pWlanInterruptCallback = If_OnIoIntr;

	return rc;
}

void If_Deinit(PWLAN_ADAPTER Adapter)
{
	TRACE("+If_Deinit()\r\n");

	ZeroMemory(Adapter, sizeof(WLAN_ADAPTER));
	Hw_Deinit();

	TRACE("-If_Deinit()\r\n");
}

BOOL If_StartDevice(PWLAN_ADAPTER Adapter)
{
	BOOL rc = TRUE;

	TRACE("+If_StartDevice()\r\n");

	if (rc) {
		Hw_PowerOn();

		ucSpiDummyClk = 5;
		for (rc = 8; rc > 0; rc --) {
			if (If_FirmwareDownload())
				break;
			Hw_Reset();
		}
		ucSpiDummyClk = 1; // for CfgBus in HalStartDevice()

		if (!rc)
			Hw_PowerOff();
	}
	if (rc) {
		if (1) {
			USHORT nDummyClk = 1;

			CmdGSpiBusCfg(Adapter, CMD_ACT_SET, &nDummyClk, &nDummyClk);
		}
		/*
		        CmdGetHwSepc();

		        if(EqualMemory(&m_nic.maCurrent, 0, sizeof(m_nic.maCurrent)) ||
		            EqualMemory(&m_nic.maCurrent,sizeof(m_nic.maCurrent),0xFF))
		        {
		            MoveMemory(&m_nic.maCurrent,&m_nic.maPermanent,sizeof(m_nic.maCurrent));
		        }
		        else if(!EqualMemory(&m_nic.maCurrent,&m_nic.maPermanent,sizeof(m_nic.maCurrent)))
		        {
		            CmdMacAddress(CMD_ACT_SET,&m_nic.maCurrent);
		        }
		*/
	}

	return rc;
}

int If_ReadRegister(int nRegNo, PVOID pBuf)
{
	return Hw_ReadRegister(nRegNo, pBuf, 2);
}

int If_WriteRegister(int nRegNo, WORD nData)
{
	return Hw_WriteRegister(nRegNo, &nData, 2);
}

int If_ReadRegister2(int nRegNo, PVOID pBuf, int nSize)
{
	return Hw_ReadRegister(nRegNo, pBuf, nSize);
}

int If_WriteRegister2(int nRegNo, PVOID pBuf, int nSize)
{
	return Hw_WriteRegister(nRegNo, pBuf, nSize);
}

void If_HostIntrEnable()
{
	WORD i;

	If_WriteRegister(HOST_INT_STATUS_MASK_REG, 0x1f);
	If_ReadRegister(HOST_INT_CTRL_REG, &i);
	If_WriteRegister(HOST_INT_CTRL_REG, i&~(0x01e0));
}

void If_HostIntrDisable()
{
	If_WriteRegister(HOST_INT_STATUS_MASK_REG, 0x00);
}

BOOL If_IsFirmwareLoaded()
{
	WORD nValue = 0;

	If_ReadRegister(SCRATCH_4_REG, &nValue);

	return (nValue == (WORD)FIRMWARE_DNLD_OK);
}

BOOL If_WaitforHostIntr(int nTimeOut)
{
	int i;
	WORD nState;

	for (i = 0; i < nTimeOut; i++) {
		nState = 0xffff;
		If_ReadRegister(HOST_INT_STATUS_REG, &nState);
		if (nState != 0xffff && (nState & CIC_CmdDnLdOvr))
			return TRUE;
		Sleep(1);// stall for 100 us
	}

	return FALSE;
}

BOOL If_FirmwareImageProg(PBYTE pImage, int nImgSize)
{
	PBYTE pCur, pEnd = pImage + nImgSize;

	for (pCur = pImage; pCur < pEnd; pCur += SPI_FW_DOWNLOAD_PKTCNT) {
		If_WriteRegister(SCRATCH_1_REG, SPI_FW_DOWNLOAD_PKTCNT);
		if (!If_WaitforHostIntr(200)) {
			TRACE("Helper Firmware download died ......\r\n");

			return FALSE;
		}

		If_WriteRegister2(CMD_RDWRPORT_REG, pCur, SPI_FW_DOWNLOAD_PKTCNT);
		If_WriteRegister(HOST_INT_STATUS_REG, 0x0000);
		If_WriteRegister(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
	}
	TRACE("Download %d bytes of helper Image\r\n", nImgSize);

	// Writing 0 to Scr1 is to indicate the end of Firmware dwld
	If_WriteRegister(SCRATCH_1_REG, FIRMWARE_DNLD_END);
	If_WriteRegister(HOST_INT_STATUS_REG, 0x0000);
	If_WriteRegister(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);

	return TRUE;
}

#define DLIMAGE_LEN 1024
BOOL If_WlanImageProg(PBYTE pImage, int nImgSize)
{
	WORD nLen;
	DWORD nCnt = 0;

	do {
		nLen = 0;
		Sleep(2);
		If_ReadRegister(SCRATCH_1_REG, &nLen);
		if (++nCnt > 100) {
			TRACE("WlanImageProg: Cann't get the packet size\r\n");
			return FALSE;
		}
	}
	while (!nLen);

	nCnt = 0;
	while (1) {
		if (!If_WaitforHostIntr(200)) {
			TRACE("Firmware download died ......\r\n");
			return FALSE;
		}
		If_ReadRegister(SCRATCH_1_REG, &nLen);
		if (nLen == 0)
			break;
		if (nLen > DLIMAGE_LEN) {
			TRACE("WlanImageProg: PacketSize(%d) error\r\n", nLen);
			return FALSE;
		}
		if (nLen & 1) {
			TRACE("WlanImageProg: CRC error(nLen=%d)\r\n", nLen);
			nLen &= ~1;
		}

		If_WriteRegister2(CMD_RDWRPORT_REG, (pImage + nCnt), nLen);
		If_WriteRegister(HOST_INT_STATUS_REG, 0x0000);
		If_WriteRegister(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
		nCnt += nLen;
	}

	if (nImgSize != nCnt) {
		TRACE("WlanImageProg: nImgSize[%d]!=nWriteLen[%d])\r\n", nImgSize, nLen);
		return FALSE;
	}

	TRACE("Download %d bytes of Wlan Image\r\n", nImgSize);

	return TRUE;
}

BOOL If_FirmwareDownload(void)
{
	BOOL rc;

	If_HostIntrDisable();
	Hw_CloseInterrupts();

#ifdef FIRMWARE_IN_FLASH
	rc = If_FirmwareImageProg((PBYTE)WIFI_FW_HELPER_ADDR, sizeof(helperimage));
	if (rc) {
		Sleep(10);
		rc = If_WlanImageProg((PBYTE)WIFI_FW_FW_ADDR, sizeof(fmimage));
	}
#else
	rc = If_FirmwareImageProg((PBYTE)helperimage, sizeof(helperimage));
	if (rc) {
		Sleep(10);
		rc = If_WlanImageProg((PBYTE)fmimage, sizeof(fmimage));
	}
#endif

	if (rc) {
		int i = 10;

		do {
			Sleep(10);
			rc = If_IsFirmwareLoaded();
		}
		while (!rc && --i >= 0);
		if (!rc) {
			TRACE("Image Down has some error\r\n");
		}
	}
	if (rc) {
		WORD nValue;

		If_ReadRegister(HOST_INT_STATUS_REG, &nValue);
		If_WriteRegister(HOST_INT_STATUS_REG, ~nValue);
		If_ReadRegister(HOST_INT_STATUS_REG, &nValue);
		If_ReadRegister(HOST_INT_CAUSE_REG, &nValue);
		If_HostIntrEnable();
		Hw_OpenInterrupts();
	}

	TRACE("FirmwareDownlown rc=%d\r\n", rc);

	return rc;
}

BOOL If_ExitDeepSleep(void)
{
	if (If_WriteRegister(HOST_INT_CTRL_REG, HIC_DEFAULT_VALUE | HIC_WakeUp) == 2)
		return TRUE;

	return FALSE;
}

BOOL If_ResetDeepSleep(void)
{
	return If_WriteRegister(HOST_INT_CTRL_REG, HIC_DEFAULT_VALUE & (~HIC_WakeUp));
}

void If_OnIoIntr(PWLAN_ADAPTER Adapter)
{
	WORD nIntrStatus;

	If_WriteRegister(HOST_INT_STATUS_MASK_REG, 0);

	if (!If_ReadRegister(HOST_INT_STATUS_REG, &nIntrStatus) || nIntrStatus == MAXWORD)
		nIntrStatus = 0;

	TRACE("If_OnIoIntr: IoIntr %04x\r\n", nIntrStatus);
	if (nIntrStatus) {
		If_WriteRegister(HOST_INT_STATUS_REG, ~nIntrStatus & 0x1f);
		if (nIntrStatus & HIS_TxDnLdRdy)
			WlanPkt_OnPktSend(Adapter);
		if (nIntrStatus & HIS_RxUpLdRdy)
			WlanPkt_OnPktRcve(Adapter);
		if (nIntrStatus & HIS_CmdDnLdRdy)
			;//CmdOnIntrCmdSent();
		if (nIntrStatus & HIS_CardEvent)
			;//CeOnIntrCardEvent();
		if (nIntrStatus & HIS_CmdUpLdRdy)
			CmdOnCmdUpLdRdy(Adapter);
		WIFI_SET_FLAG(Adapter, FLAG_INTRETURN);
	}

	If_WriteRegister(HOST_INT_STATUS_MASK_REG, WLAN_INTR_ENABLE_BITS);
}

void NOP_Process(PWLAN_ADAPTER pAdapter)
{
	TRACE("Wifi interrupt!!\r\n");
}

#ifdef BUILD_FOR_WRITEFIRMWARE

BYTE ucBuffer[FLASH_PAGE_SIZE];

void If_WriteFirmware(void)
{
	Flash_Init();

	if (1) {
		UINT32 ReadAddrOffset, AddrBase, DataSize, i;
		PBYTE pOriginalData;
		UINT32 usLength;

		if (1) {
			// Write data to flash
			AddrBase = WIFI_FW_HELPER_ADDR;
			Flash_Write (AddrBase, (UINT8*)helperimage, sizeof (helperimage));
			// Read back data for checking...
			ReadAddrOffset = 0;
			pOriginalData = (PBYTE)helperimage;
			DataSize = sizeof (helperimage);
			while (ReadAddrOffset < DataSize) {
				if (ReadAddrOffset + FLASH_PAGE_SIZE < DataSize)
					usLength = FLASH_PAGE_SIZE;
				else
					usLength = DataSize - ReadAddrOffset;
				Flash_Read(AddrBase + ReadAddrOffset, ucBuffer, usLength);
				for (i = 0; i < usLength; i ++) {
					if (ucBuffer[i] != pOriginalData[ReadAddrOffset + i])
						break;
				}
				if (i != usLength)
					TRACE("Data Verify failed1!\r\n");
				ReadAddrOffset += usLength;
			}
		}

		if (1) {
			// Write data to flash
			AddrBase = WIFI_FW_FW_ADDR;
			Flash_Write (AddrBase, (UINT8*)fmimage, sizeof (fmimage));
			// Read back data for checking...
			ReadAddrOffset = 0;
			pOriginalData = (PBYTE)fmimage;
			DataSize = sizeof (fmimage);
			while (ReadAddrOffset < DataSize) {
				if (ReadAddrOffset + FLASH_PAGE_SIZE < DataSize)
					usLength = FLASH_PAGE_SIZE;
				else
					usLength = DataSize - ReadAddrOffset;
				Flash_Read(AddrBase + ReadAddrOffset, ucBuffer, usLength);
				for (i = 0; i < usLength; i ++) {
					if (ucBuffer[i] != pOriginalData[ReadAddrOffset + i])
						break;
				}
				if (i != usLength)
					TRACE("Data Verify failed1!2\r\n");
				ReadAddrOffset += usLength;
			}
		}
	}

	while (1);
}

#endif
