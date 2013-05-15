
#ifndef __WIFI_IF_H__
#define __WIFI_IF_H__
//
// Headers include
//
#include "Wifi_If_Gspi.h"

//
// Macros definitions
//
#define WLAN_INTR_ENABLE_BITS   (HIS_TxDnLdRdy | HIS_RxUpLdRdy | HIS_CmdDnLdRdy | HIS_CardEvent | HIS_CmdUpLdRdy)

//-------------------------------------------------------------------------------
// Wlan CallBack function type
//-------------------------------------------------------------------------------
typedef void(*WlanInterruptCallback)(PWLAN_ADAPTER);

//
// Exported functions declare
//
BOOL If_Init(PWLAN_ADAPTER Adapter);
void If_Deinit(PWLAN_ADAPTER Adapter);
BOOL If_StartDevice(PWLAN_ADAPTER Adapter);
int If_ReadRegister(int nRegNo, PVOID pBuf);
int If_WriteRegister(int nRegNo, WORD nData);
int If_ReadRegister2(int nRegNo, PVOID pBuf, int nSize);
int If_WriteRegister2(int nRegNo, PVOID pBuf, int nSize);
void If_HostIntrEnable();
void If_HostIntrDisable();
BOOL If_IsFirmwareLoaded();
BOOL If_WaitforHostIntr(int nTimeOut);
BOOL If_FirmwareImageProg(PBYTE pImage, int nImgSize);
BOOL If_WlanImageProg(PBYTE pImage, int nImgSize);
BOOL If_FirmwareDownload(void);
BOOL If_ExitDeepSleep(void);
BOOL If_ResetDeepSleep(void);
void If_OnIoIntr(PWLAN_ADAPTER pAdapter);
void NOP_Process(PWLAN_ADAPTER pAdapter);

#ifdef BUILD_FOR_WRITEFIRMWARE
void If_WriteFirmware(void);
#endif

//
// Exported variables declare
//
extern UCHAR ucSpiDummyClk;
extern WlanInterruptCallback pWlanInterruptCallback;

#endif // __WIFI_IF_H__
