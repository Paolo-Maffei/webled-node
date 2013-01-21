/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

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

