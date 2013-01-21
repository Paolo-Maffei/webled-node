/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#ifndef __PROJECT_H__
#define __PROJECT_H__
//
// Headers include
//

//
// Base Librarys
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include  "ucos_ii.h"

#include "stm32f10x.h"
#include "hw_config.h"

#include "BaseTypes.h"
#include "BaseDefs.h"

//
// OS layer headers
//
#include "OS.h"

//
// Base driver headers
//
#include "GSPI.h"
#include "Buzzer.h"
#include "Relay.h"
#include "Flash.h"
#include "EEPROM.h"
#include "KEY.h"
#include "Led.h"
#include "Uart.h"
#include "Console.h"

//
// Wifi reference headers
//
#include "WifiDefs.h"
#include "WifiCfg.h"

//
// NDIS layer headers
//
#include "Ndis.h"
#include "Ndisdef.h"

//
// Marvell defined headers
//
#include "MrvConstant.h"
#include "MrvStruct.h"
#include "MrvHostCmd.h"

//
// Wifi headers
//
#include "WlanAdapter.h"
#include "Wifi_Hw.h"
#include "Wifi_If.h"
#include "WlanCmd.h"
#include "WlanInit.h"
#include "WlanPkt.h"
#include "WlanMisc.h"

//
// Headers for user
//
#include "WifiApiExport.h"
#include "WIFICommand.h"

//
// For debug only
//
#include "UartTrace.h"

//
// Global variables declare
//
extern WLAN_ADAPTER g_Adapter;


#endif // __PROJECT_H__
