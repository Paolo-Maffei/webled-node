/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#ifndef __WLAN_MISC_H__
#define __WLAN_MISC_H__
//
// Macros definition
//
enum KeySize {
	KeySizeValue_40_64bit_ASCII     =   5,
	KeySizeValue_40_64bit_HEX       =   10,
	KeySizeValue_104_128bit_ASCII   =   13,
	KeySizeValue_104_128bit_HEX     =   26
};

#define IP2DWORD(a,b,c,d)				((DWORD)(BYTE)(d)|(DWORD)(BYTE)(c)<<8|(DWORD)(BYTE)(b)<<16|(DWORD)(BYTE)(a)<<24)
#define DWORD2IP(val,array)			\
	do {							\
		array[0] = (BYTE)((val >> 24) & 0x000000FF); \
		array[1] = (BYTE)((val >> 16) & 0x000000FF); \
		array[2] = (BYTE)((val >> 8) & 0x000000FF); \
		array[3] = (BYTE)((val >> 0) & 0x000000FF); \
	} while(0)

//
// Exported functions declare
//
UCHAR WlanMisc_ASCII2WEPKey(CHAR *pszKeyASCII, UCHAR *pucKeyMaterial);
BOOL WlanMisc_String2Ip(char *pszString, ULONG *pulIp);
BOOL WlanMisc_String2Mac(char *pszString, PBYTE pbMac);

#endif // __WLAN_MISC_H__
