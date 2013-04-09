/******************************
*******************************/

#ifndef __WIFI_DEFS_H__
#define __WIFI_DEFS_H__

//-----------------------------------------------------------------------------
// Get / Set flags
//-----------------------------------------------------------------------------
#define WIFI_SET_FLAG(Obj, flag)                	do{Obj->m_dwFlags|=flag;}while(0)
#define WIFI_GET_FLAG(Obj, flag)                	(Obj->m_dwFlags&flag)
#define WIFI_CLR_FLAG(Obj, flag)                	do{Obj->m_dwFlags&=~flag;}while(0)

#endif // __WIFI_DEFS_H__
