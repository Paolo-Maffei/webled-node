
#ifndef __WIFIAPI_EXPORT_H__
#define __WIFIAPI_EXPORT_H__
//
// Macros definition
//

//
// Structs definition
//

//
// Exported functions declare
//
int WiFiApi_CheckCurrentProfile ();
int WiFiApi_CardSetEncryMode (ENCRYMODE mode);
int WiFiApi_AdapterSetKey (char *pszKey);
int WiFiApi_DoConnect ();
int WiFiApi_ProcessAssoc();
int WiFiApi_ProcessPacket (PUCHAR pucBuffer, UINT nLen);

#endif // __WIFIAPI_EXPORT_H__

