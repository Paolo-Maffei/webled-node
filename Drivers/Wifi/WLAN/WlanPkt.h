
#ifndef __WLAN_PKT_H__
#define __WLAN_PKT_H__
//
// Macros definition
//

//
// Structs definition
//

//
// Exported functions declare
//
BOOL WlanPkt_RcvePkt(PWLAN_ADAPTER pAdapter, PVOID pBuf, PUINT pnLen);
BOOL WlanPkt_SendPkt(PWLAN_ADAPTER pAdapter, PVOID pBuf, UINT nBufLen);
void WlanPkt_OnPktSend(PWLAN_ADAPTER pAdapter);
void WlanPkt_OnPktRcve(PWLAN_ADAPTER pAdapter);

#endif // __WLAN_PKT_H__
