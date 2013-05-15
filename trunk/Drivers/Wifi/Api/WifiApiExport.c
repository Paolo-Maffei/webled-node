
#include "Project.h"

extern UCHAR ucNetworkPktBuffer[KBytes(1)];

int WiFiApi_CheckCurrentProfile ()
{
	int nRet = WIFICMD_RESULT_SUCCESS;

	nRet = WiFiCmd_RunCommand("get ssid");
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	nRet = WiFiCmd_RunCommand("get networktype");
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	nRet = WiFiCmd_RunCommand("get authmode");
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	nRet = WiFiCmd_RunCommand("get encrymode");
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	nRet = WiFiCmd_RunCommand("get keyascii");
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	return nRet;
}

int WiFiApi_CardSetEncryMode (ENCRYMODE mode)
{
	USHORT nMacControl;
	CMD_STATUS stat;
	int nRet = WIFICMD_RESULT_SUCCESS;

	switch (mode) {
		case EncryMode_Disable:
		{
			// Save current profile encryption mode
			g_pWlanAdapter->m_CurProfile.EncMode = mode;
			// Set MAC control value to firmware
			nMacControl = g_pWlanAdapter->nCurMacControl & ~HostCmd_ACT_MAC_WEP_ENABLE;
			stat = CmdMacControl(g_pWlanAdapter, nMacControl);
			if (stat == CMD_ERROR_SUCCESS)
				g_pWlanAdapter->nCurMacControl = nMacControl;
			break;
		}
		case EncryMode_WEP:
		{
			// Save current profile encryption mode
			g_pWlanAdapter->m_CurProfile.EncMode = mode;
			// Set MAC control value to firmware
			nMacControl = g_pWlanAdapter->nCurMacControl | HostCmd_ACT_MAC_WEP_ENABLE;
			if (g_pWlanAdapter->m_KeyMaterial.ucKeyLength == WEP_KEY_104_BIT_LEN)
				nMacControl |= HostCmd_ACT_MAC_WEP_TYPE;
			else
				nMacControl &= ~HostCmd_ACT_MAC_WEP_TYPE;
			stat = CmdMacControl(g_pWlanAdapter, nMacControl);
			if (stat == CMD_ERROR_SUCCESS)
				g_pWlanAdapter->nCurMacControl = nMacControl;
			break;
		}
		default:
			nRet = WIFICMD_RESULT_ERRORPARAM;
			break;
	}

	return nRet;
}

int WiFiApi_AdapterSetKey (char *pszKey)
{
	int nRet = WIFICMD_RESULT_SUCCESS;

	if ((g_pWlanAdapter->m_CurProfile.AuthMode == AuthMode_Open)
		|| (g_pWlanAdapter->m_CurProfile.AuthMode == AuthMode_Shared))
	{
		UCHAR ucKeyMaterial[32], ucSize;

		ucSize = WlanMisc_ASCII2WEPKey(pszKey, ucKeyMaterial);
		if (!ucSize)
			nRet = WIFICMD_RESULT_ERRORPARAM;
		else {
			CopyMemory(g_pWlanAdapter->m_KeyMaterial.ucMaterial, ucKeyMaterial, ucSize);
			g_pWlanAdapter->m_KeyMaterial.ucKeyLength = ucSize;
		}
	}
	else
	{
		TRACE("Unsupport authmode.\r\n");
		nRet = WIFICMD_RESULT_ERRORPARAM;
	}

	return nRet;
}

int WiFiApi_DoConnect ()
{
	CMD_STATUS stat = CMD_ERROR_SUCCESS;

	if (WIFI_GET_FLAG(g_pWlanAdapter, FLAG_MEDIA_CONNECTED)) {
		if (g_pWlanAdapter->m_CurApInfo.Bssid.InfrastructureMode == Ndis802_11IBSS)
			CmdAdHocStop(g_pWlanAdapter);
		else
			CmdDeauthenticate(g_pWlanAdapter, &g_pWlanAdapter->m_CurApInfo);

		WIFI_CLR_FLAG(g_pWlanAdapter, FLAG_MEDIA_CONNECTED);
	}

	if (g_pWlanAdapter->m_CurProfile.NetworkType == NetworkType_AdHoc) {
		stat = CmdAdHocStart(g_pWlanAdapter, &g_pWlanAdapter->m_CurApInfo);
		if (stat == CMD_ERROR_SUCCESS)
			stat = CmdSetWepKey(g_pWlanAdapter, HostCmd_ACT_ADD);

	}
	else {
		stat = CmdSsidScan(g_pWlanAdapter, g_pWlanAdapter->m_CurProfile.szSSID, &g_pWlanAdapter->m_CurApInfo);
		if (stat == CMD_ERROR_SUCCESS)
			stat = CmdAssociateExt(g_pWlanAdapter, &g_pWlanAdapter->m_CurApInfo);
		if (stat == CMD_ERROR_SUCCESS)
			stat = CmdSetWepKey(g_pWlanAdapter, HostCmd_ACT_ADD);
	}

	if (stat != CMD_ERROR_SUCCESS) {
		TRACE("WiFiApi_DoConnect: CMD_CTRL_CONN failed\r\n");
		return -1;
	}

	return stat;
}

int WiFiApi_ProcessAssoc()
{
	int nRet = WIFICMD_RESULT_SUCCESS; 

	// Check all fields of user profile are correct?
	nRet = WiFiApi_CheckCurrentProfile ();
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	// Read user profile into adapter
	Flash_Read(WIFI_PROFILE_ADDR, (PBYTE)&g_pWlanAdapter->m_CurProfile, sizeof(WiFiProfile));
        Console_Print("wifi_profile read:%d \n",sizeof(WiFiProfile));
	
	nRet = WiFiApi_CardSetEncryMode (g_pWlanAdapter->m_CurProfile.EncMode);
	if (nRet != WIFICMD_RESULT_SUCCESS)
		return nRet;

	nRet = WiFiApi_AdapterSetKey (g_pWlanAdapter->m_CurProfile.szKey);
	if (nRet != WIFICMD_RESULT_SUCCESS) {
		Console_Print("Key error, please check.\r\n");
		return nRet;
	}

	nRet = WiFiApi_DoConnect();

	return nRet;
}
