
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
#define WIFI_PROFILE_ADDR			  (SYSTEM_FLASH_ADDR + KBytes(400))	//400KB+

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
