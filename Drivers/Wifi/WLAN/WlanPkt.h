/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

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
