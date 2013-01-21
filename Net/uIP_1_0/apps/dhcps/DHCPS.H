/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
*******************************/

#ifndef __LNET_DHCPS_H__
#define __LNET_DHCPS_H__
//
// Macros definition
//
#define BOOTP_BROADCAST					0x8000

#define DHCP_REQUEST					1
#define DHCP_REPLY						2
#define DHCP_HTYPE_ETHERNET				1
#define DHCP_HLEN_ETHERNET				6
#define DHCP_MSG_LEN					236

#define DHCP_STATE_IDLE					0
#define DHCP_STATE_DISCOVER				1
#define DHCP_STATE_OFFER				2
#define DHCP_STATE_REQUEST				3
#define DHCP_STATE_DECLINE				4
#define DHCP_STATE_ACK					5
#define DHCP_STATE_NAK					6
#define DHCP_STATE_RELEASE				7

#define DHCP_TYPE_DISCOVER				1
#define DHCP_TYPE_OFFER					2
#define DHCP_TYPE_REQUEST				3
#define DHCP_TYPE_DECLINE				4
#define DHCP_TYPE_ACK					5
#define DHCP_TYPE_NAK					6
#define DHCP_TYPE_RELEASE				7

#define DHCP_OPTION_SUBNET_MASK			1
#define DHCP_OPTION_ROUTER				3
#define DHCP_OPTION_DNS_SERVER			6
#define DHCP_OPTION_REQ_IPADDR			50
#define DHCP_OPTION_LEASE_TIME			51
#define DHCP_OPTION_MSG_TYPE			53
#define DHCP_OPTION_SERVER_ID			54
#define DHCP_OPTION_INTERFACE_MTU		26
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY 31
#define DHCP_OPTION_BROADCAST_ADDRESS	28
#define DHCP_OPTION_REQ_LIST			55
#define DHCP_OPTION_END					255

#define DHCPS_SERVER_PORT				67
#define DHCPS_CLIENT_PORT				68

#define DHCP_MAGIC_COOKIE 				"\x63\x82\x53\x63"
#define DHCP_XID						"\xad\xde\x12\x23"

//
// Structs definition
//
#pragma pack(push, 1)
#define DHCPMSG_OPTION_LENGTH		312
typedef struct
{
	UINT8 op;
	UINT8 htype;
	UINT8 hlen;
	UINT8 hops;
	UINT8 xid[4];
	UINT16 secs;
	UINT16 flags;
	UINT8 ciaddr[4];
	UINT8 yiaddr[4];
	UINT8 siaddr[4];
	UINT8 giaddr[4];
	UINT8 chaddr[16];
	UINT8 sname[64];
	UINT8 file[128];
	UINT8 options[DHCPMSG_OPTION_LENGTH];
} DHCPMsg, *PDHCPMSG;

typedef struct
{
	ULONG ulServerIp;
	ULONG ulClientIp;
} DHCPInfo, *PDHCPINFO;

#pragma pack(pop)

//
// Exported functions declare
//
void Dhcp_Init(const void *mac_addr, int mac_len);
void Dhcp_PktHandler(PBYTE pInBuffer, USHORT usLen);

#endif // __LNET_DHCPS_H__
