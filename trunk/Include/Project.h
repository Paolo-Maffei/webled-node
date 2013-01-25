
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

#include "AppTask.h"
#include "uIP_TASK.h"
//
// Base driver headers
//
#include "GSPI.h"
#include "Flash.h"
#include "EEPROM.h"
#include "KEY.h"
#include "Led.h"
#include "Uart.h"
#include "Console.h"

#include "Group.h"

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
