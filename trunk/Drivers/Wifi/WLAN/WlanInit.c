
#include "Project.h"

VOID InitAdapterObject(
    PWLAN_ADAPTER Adapter
)
{
	// Reset all flags
	Adapter->m_dwFlags = 0;
	Adapter->nCurMacControl = MAC_CTRL_DEFAULT;

	return;
}

BOOL Wlan_Init(PWLAN_ADAPTER Adapter)
{
	// Zero out the adapter object space
	ZeroMemory(Adapter, sizeof(WLAN_ADAPTER));
	InitAdapterObject(Adapter);

	// interface initialize
	if (!If_Init(Adapter)) {
		TRACE("Interface init failed!!\r\n");
		return FALSE;
	}
	if (!If_StartDevice(Adapter)) {
		TRACE("Interface start device failed!!\r\n");
		return FALSE;
	}
	CmdSubscribeEvent(Adapter);

	return TRUE;
}
