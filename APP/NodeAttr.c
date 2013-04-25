/*
  Wifi节点的属性定义，相关数据操作
*/

#include "Project.h"

NodeAttr node_info;

void Init_NodeAttr(void)  //call GroupTable_Init() before
{
  Flash_Read(NODEATTR_FLASH_ADDR,(PBYTE)&node_info,sizeof(NodeAttr));
  node_info.group_table = &Group_Match_Table;
//  if(0xFFFFFFFF == node_info.id)
//    NodeAttr_SetID(255);             //若未配置，默认ID为255
  if(0xFF == node_info.name_length)
    NodeAttr_SetName("未设置",sizeof("未设置"));
}

static void NodeAttr_Save(void)
{
  Flash_Write(NODEATTR_FLASH_ADDR,(PBYTE)&node_info,sizeof(NodeAttr)); 
}

void NodeAttr_SetID(int id)
{
  node_info.id = id;
  NodeAttr_Save();
}

int NodeAttr_GetID(void)
{
  return node_info.id;
}

void NodeAttr_Set_Config_Status(void)
{
  node_info.config_status = STATUS_CONFIGURED;
  NodeAttr_Save();
}

void NodeAttr_Clr_Config_Status(void)
{
  node_info.config_status = 0xFF;
  NodeAttr_Save();
}

char NodeAttr_Get_Config_Status(void)
{
  return node_info.config_status;
}

char NodeAttr_Is_Configed(void)
{
  return (STATUS_CONFIGURED == node_info.config_status);
}

void NodeAttr_SetIP(char *ip,char *gwip,char *netmask)
{
  CopyMemory(node_info.ipaddr,ip,4);
  CopyMemory(node_info.gateway,gwip,4);
  CopyMemory(node_info.netmask,netmask,4);
  NodeAttr_Save();
}

void NodeAttr_SetName(char *name,int length)
{
  CopyMemory(node_info.name,name,length);
  node_info.name_length = length;
  NodeAttr_Save();
}

int NodeAttr_GetName(char *name)
{
  CopyMemory(name,node_info.name,node_info.name_length);
  return node_info.name_length;
}

void NodeAttr_SetSSID(char *ssid,int length)
{
  CopyMemory(node_info.ssid,ssid,length);
  node_info.ssid_length = length;
  NodeAttr_Save();
}

int NodeAttr_GetSSID(char *ssid)
{
  CopyMemory(ssid,node_info.ssid,node_info.ssid_length);
  return node_info.ssid_length;
}

void NodeAttr_SetKey(char *key,int length)
{
  CopyMemory(node_info.key,key,length);
  node_info.key_length = length;
  NodeAttr_Save();
}

int NodeAttr_GetKey(char *key)
{
  CopyMemory(key,node_info.key,node_info.key_length);
  return node_info.key_length;
}

void NodeAttr_SetType(char type)
{
  node_info.type = type;
  NodeAttr_Save();
}

char NodeAttr_GetType(void)
{
  return node_info.type;
}

void NodeAttr_SetStatus(char *status)
{
  CopyMemory(node_info.status,status,5);
  NodeAttr_Save();
}

int NodeAttr_GetStatus(char *status)
{
  CopyMemory(status,node_info.status,5);
  return 5;
}