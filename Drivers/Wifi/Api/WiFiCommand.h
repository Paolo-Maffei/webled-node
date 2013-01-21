/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#ifndef __WIFI_COMMAND__
#define __WIFI_COMMAND__
//
// Macros definition
//
#define WIFICMD_MAXARGS				3

#define WIFICMD_RESULT_SUCCESS			(0)
#define WIFICMD_RESULT_UNKNOWCMD		(-1)
#define WIFICMD_RESULT_UNKNOWOPTION		(-2)
#define WIFICMD_RESULT_ERRORPARAM		(-3)
#define WIFICMD_RESULT_PARAMNOTSET		(-4)

//
// Structs definition
//

//
// Exported functions declare
//
int WiFiCmd_RunCommand (char *pLine);

#endif // __WIFI_COMMAND__

