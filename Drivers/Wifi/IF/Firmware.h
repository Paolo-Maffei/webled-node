
#ifndef _INC_FIRMWARE_H
#define _INC_FIRMWARE_H

#if (WIFI_MODULE == WM_G_MR_08)
#include "gspi8385.h"
#include "helper8385.h"
#elif (WIFI_MODULE == WM_G_MR_09)
#include "gspi8686_09mod.h"
#include "helper_09mod.h"
#else
#error "WIFI module not defined."
#endif

#endif
