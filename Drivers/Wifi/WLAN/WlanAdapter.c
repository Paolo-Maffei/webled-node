/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#include "Project.h"

WLAN_ADAPTER g_WlanAdapter, *g_pWlanAdapter = &g_WlanAdapter;

BOOL WlanAdapter_MacAddr(PWLAN_ADAPTER pAdapter, UCHAR* pMa, BOOL bSet)
{
	if (bSet)
		return CmdMacAddress(pAdapter, CMD_ACT_SET, (NDIS_802_11_MAC_ADDRESS *)pMa);
	else
		return CmdMacAddress(pAdapter, CMD_ACT_GET, (NDIS_802_11_MAC_ADDRESS *)pMa);
}

BOOL WlanAdapter_SendPkt(PWLAN_ADAPTER pAdapter, PVOID pBuf, UINT nBufLen)
{
	return WlanPkt_SendPkt(pAdapter, pBuf, nBufLen);
}

BOOL WlanAdapter_RcvePkt(PWLAN_ADAPTER pAdapter, PVOID pBuf, PUINT pnLen)
{
	return WlanPkt_RcvePkt(pAdapter, pBuf, pnLen);
}

