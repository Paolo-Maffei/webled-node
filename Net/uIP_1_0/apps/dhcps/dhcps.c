/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#include "Project.h"

#include "uip.h"
#include "DHCPS.h"

static DHCPInfo dhcpInfo = {IP2DWORD(192, 168, 1, 108), IP2DWORD(192, 168, 1, 102)};
static UCHAR ucDhcpTransId[4];

///////////////////////////////////////////////////////////////////////////////////
// Privated functions
//
static PBYTE _AddMsgType(PBYTE pOpt, UCHAR ucType)
{
	*pOpt ++ = DHCP_OPTION_MSG_TYPE;
	*pOpt ++ = 1;
	*pOpt ++ = ucType;

	return pOpt;
}

static PBYTE _AddOfferOptions(PBYTE pOpt)
{
	UCHAR ucIpAddr[4];

	DWORD2IP(dhcpInfo.ulServerIp, ucIpAddr);

	*pOpt++ = DHCP_OPTION_SERVER_ID;
	*pOpt++ = 4;  //len
	*pOpt++ = ucIpAddr[0];
	*pOpt++ = ucIpAddr[1];
	*pOpt++ = ucIpAddr[2];
	*pOpt++ = ucIpAddr[3];

	*pOpt++ = DHCP_OPTION_SUBNET_MASK;
	*pOpt++ = 4;  //len
	*pOpt++ = 255;
	*pOpt++ = 255;	//note this is different from interface netmask and broadcast address
	*pOpt++ = 255;
	*pOpt++ = 0;

	*pOpt++ = DHCP_OPTION_LEASE_TIME;
	*pOpt++ = 4;  //len
	*pOpt++ = 0x00;
	*pOpt++ = 0x01;
	*pOpt++ = 0x51;
	*pOpt++ = 0x80; 	//1 day

	*pOpt++ = DHCP_OPTION_BROADCAST_ADDRESS;
	*pOpt++ = 4;  //len
	*pOpt++ = 255;
	*pOpt++ = 255;
	*pOpt++ = 255;
	*pOpt++ = 255;


	*pOpt++ = DHCP_OPTION_INTERFACE_MTU;
	*pOpt++ = 2;  //len
	*pOpt++ = 0x02;	//mtu of 576
	*pOpt++ = 0x40;

	*pOpt++ = DHCP_OPTION_PERFORM_ROUTER_DISCOVERY;
	*pOpt++ = 1;  //len
	*pOpt++ = 0x00; //dont do router discovery

	//disable microsoft netbios over tcp
	*pOpt++ = 43;	//vendor specific
	*pOpt++ = 6;	//length of embedded option

	*pOpt++ = 0x01;	//vendor specific (microsoft disable netbios over tcp)
	*pOpt++ = 4;  //len
	*pOpt++ = 0x00;
	*pOpt++ = 0x00;
	*pOpt++ = 0x00;
	*pOpt++ = 0x02; 	//disable=0x02,  enable = 0x00

	return pOpt;
}

static PBYTE _AddEnd(PBYTE pOpt)
{
	*pOpt++ = DHCP_OPTION_END;

	return pOpt;
}

static void _CreateMsg(PDHCPMSG pDhcpMsg)
{
	ULONG ulClientIp = ENDIANCHANGE32(dhcpInfo.ulClientIp);

	pDhcpMsg->op = DHCP_REPLY;
	pDhcpMsg->htype = DHCP_HTYPE_ETHERNET;
	pDhcpMsg->hlen = 6;  //mac id length
	pDhcpMsg->hops = 0;
	CopyMemory(pDhcpMsg->xid, ucDhcpTransId, 4);
	pDhcpMsg->secs = 0;
	pDhcpMsg->flags = 0;//ENDIANCHANGE16(BOOTP_BROADCAST); /*  Broadcast bit. */
	CopyMemory((char *)pDhcpMsg->yiaddr, (char *)&ulClientIp, sizeof(pDhcpMsg->yiaddr));
	ZeroMemory((char *)pDhcpMsg->ciaddr, sizeof(pDhcpMsg->ciaddr));
	ZeroMemory((char *)pDhcpMsg->siaddr, sizeof(pDhcpMsg->siaddr));
	ZeroMemory((char *)pDhcpMsg->giaddr, sizeof(pDhcpMsg->giaddr));
	ZeroMemory((char *)pDhcpMsg->sname, sizeof(pDhcpMsg->sname));
	ZeroMemory((char *)pDhcpMsg->file, sizeof(pDhcpMsg->file));
	ZeroMemory((char *)pDhcpMsg->options, sizeof(pDhcpMsg->options));
	CopyMemory((char *)pDhcpMsg->options, (char *) DHCP_MAGIC_COOKIE, 4);
}

static void _SendOffer(PDHCPMSG pDhcpMsg)
{
	PBYTE pEnd;

	TRACE("+Dhcp_SendOffer()\r\n");

	_CreateMsg(pDhcpMsg);

	pEnd = _AddMsgType(&pDhcpMsg->options[4], DHCP_TYPE_OFFER);
	pEnd = _AddOfferOptions(pEnd);
	pEnd = _AddEnd(pEnd);

	uip_send((void*)pDhcpMsg, sizeof(*pDhcpMsg));
}

static void _SendNak(PDHCPMSG pDhcpMsg)
{
	PBYTE pEnd;

	TRACE("+Dhcp_SendNak()\r\n");

	_CreateMsg(pDhcpMsg);

	pEnd = _AddMsgType(&pDhcpMsg->options[4], DHCP_TYPE_NAK);
	pEnd = _AddEnd(pEnd);

	uip_send((void*)pDhcpMsg, sizeof(*pDhcpMsg));
}

static void _SendAck(PDHCPMSG pDhcpMsg)
{
	PBYTE pEnd;

	TRACE("+Dhcp_SendAck()\r\n");

	_CreateMsg(pDhcpMsg);

	pEnd = _AddMsgType(&pDhcpMsg->options[4], DHCP_TYPE_ACK);
	pEnd = _AddOfferOptions(pEnd);
	pEnd = _AddEnd(pEnd);

	uip_send((void*)pDhcpMsg, sizeof(*pDhcpMsg));
}

static UCHAR _ParseOptions(PBYTE pOpt, USHORT usLen)
{
	PBYTE pEnd = pOpt + usLen;
	UCHAR ucType = 0, ucState = DHCP_STATE_IDLE;

	while (pOpt < pEnd) {
		switch ((USHORT) *pOpt) {
			case DHCP_OPTION_MSG_TYPE:
				ucType = *(pOpt + 2);
				break;
			case DHCP_OPTION_REQ_IPADDR:
			{
				ULONG ulClientIp = ENDIANCHANGE32(dhcpInfo.ulClientIp);

				if (EqualMemory( (char *)&ulClientIp, (char *)pOpt + 2, 4))
					ucState = DHCP_STATE_ACK;
				else
					ucState = DHCP_STATE_NAK;
				break;
			}
			case DHCP_OPTION_END:
				break;
		}
		pOpt += pOpt[1] + 2;
	}

	switch (ucType) {
		case DHCP_TYPE_DECLINE:
			ucState = DHCP_STATE_IDLE;
			break;
		case DHCP_TYPE_DISCOVER:
			ucState = DHCP_STATE_OFFER;
			break;
		case DHCP_TYPE_REQUEST:
			if ( !(ucState == DHCP_STATE_ACK || ucState == DHCP_STATE_NAK) ) {
				ucState = DHCP_STATE_NAK;
			}
			break;
		case DHCP_TYPE_RELEASE:
			ucState = DHCP_STATE_IDLE;
			break;
	}

	return ucState;
}

///////////////////////////////////////////////////////////////////////////////////
// Exported functions
//
void Dhcp_Init(const void *mac_addr, int mac_len)
{
	uip_ipaddr_t addr;
	struct uip_udp_conn *conn = NULL;

	uip_ipaddr(addr, 255,255,255,255);
	conn = uip_udp_new(&addr, HTONS(DHCPS_CLIENT_PORT));
	if(conn != NULL) {
		uip_udp_bind(conn, HTONS(DHCPS_SERVER_PORT));
	}
}

void Dhcp_PktHandler(PBYTE pInBuffer, USHORT usLen)
{
	PDHCPMSG pDhcpMsg = (PDHCPMSG)pInBuffer;

	if (EqualMemory( (char *) pDhcpMsg->options, (char *) DHCP_MAGIC_COOKIE, 4)) {
		CopyMemory(ucDhcpTransId, pDhcpMsg->xid, 4);
		switch (_ParseOptions(&pDhcpMsg->options[4], DHCPMSG_OPTION_LENGTH)) {
			case DHCP_STATE_OFFER:
				_SendOffer((PDHCPMSG)pInBuffer);
				break;
			case DHCP_STATE_ACK:
				_SendAck((PDHCPMSG)pInBuffer);
				break;
			case DHCP_STATE_NAK:
				_SendNak((PDHCPMSG)pInBuffer);
				break;
		}
	}
}
