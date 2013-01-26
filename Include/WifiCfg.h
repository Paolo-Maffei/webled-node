/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

// Compile option
//#define BUILD_FOR_WRITEFIRMWARE

#define NO_TRACE
#define NO_DEBUG

//------------------------------------------------------------------------------
// Firmware in flash
//------------------------------------------------------------------------------
#define SYSTEM_FLASH_ADDR			   0x08000000UL
#define WIFI_PROFILE_ADDR				(SYSTEM_FLASH_ADDR + KBytes(500))	// 500KB+

//now the firmware is located in data array
//#define FIRMWARE_IN_FLASH		1

//------------------------------------------------------------------------------
// Hw data buffer
//------------------------------------------------------------------------------
#define HW_IODATA_SIZE				KBytes(1)

//------------------------------------------------------------------------------
// Adapter cmd buffer
//------------------------------------------------------------------------------
#define ADAPTER_CMD_BUFFER_LENGTH	KBytes(1)

//------------------------------------------------------------------------------
// Packet buffers
//------------------------------------------------------------------------------
#define PKT_RCVE_BUFF_LENGTH		KBytes(4)
#define PKT_SEND_BUFF_LENGTH		KBytes(4)

//
// WIFI module select
//
#define WM_G_MR_08					1
#define WM_G_MR_09					2

#define WIFI_MODULE					WM_G_MR_08

#endif // __WIFI_CONFIG_H__
