/*
  Wifi节点的属性定义，相关数据操作
*/

#include "Project.h"

NodeAttr node_info;

#ifdef  NODE_TYPE_PANEL
char panel_status;  //当前按键的最新状态
#endif

void Init_NodeAttr(void)  //call GroupTable_Init() before
{
  Flash_Read(NODEATTR_FLASH_ADDR,(PBYTE)&node_info,sizeof(NodeAttr));
  node_info.group_table = &Group_Match_Table;
//  if(0xFFFFFFFF == node_info.id)
//    NodeAttr_SetID(255);             //若未配置，默认ID为255
  if(0xFF == node_info.name_length)
    NodeAttr_SetName("未设置",sizeof("未设置"));
  if(0xFF == node_info.status[0] && 0xFF == node_info.status[1])
  {
    char status[5] ={0,50,50,50,50};
    NodeAttr_SetStatus(status); 
  }
  if(0xff == node_info.type)
    NodeAttr_SetType(NODE_DEFAULT_TYPE); //设置节点类型
  
#ifdef  NODE_TYPE_PANEL
  panel_status = node_info.status[0];
#endif //NODE_TYPE_PANEL
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
  if(0 != CompareMemory(node_info.status,status,5))
  {
    CopyMemory(node_info.status,status,5);
    NodeAttr_Save();
  }
}

int NodeAttr_GetStatus(char *status)
{
  CopyMemory(status,node_info.status,5);
  return 5;
}

void NodeAttr_SetIO(char io)
{
  node_info.status[0] = io;
  NodeAttr_Save();
}

void NodeAttr_SetPWM1(char pwm1)
{
  node_info.status[1] = pwm1;
  NodeAttr_Save();
}

void NodeAttr_SetPWM2(char pwm2)
{
  node_info.status[2] = pwm2;
  NodeAttr_Save();
}

void NodeAttr_SetPWM3(char pwm3)
{
  node_info.status[3] = pwm3;
  NodeAttr_Save();
}

void NodeAttr_SetPWM4(char pwm4)
{
  node_info.status[4] = pwm4;
  NodeAttr_Save();
}

char NodeAttr_GetIO(void)
{
  return node_info.status[0];
}

char NodeAttr_GetPWM1(void)
{
  return node_info.status[1];
}

char NodeAttr_GetPWM2(void)
{
  return node_info.status[2];
}

char NodeAttr_GetPWM3(void)
{
  return node_info.status[3];
}

char NodeAttr_GetPWM4(void)
{
  return node_info.status[4];
}

#ifdef NODE_TYPE_PANEL 
int NodeAttr_GetPanelKeyID(char i)
{
  return node_info.panel_key[i];
}

char NodeAttr_SetPanelKeyID(char i,int id)
{
  node_info.panel_key[i] = id;
  NodeAttr_Save();
  return 0;
}
#endif //NODE_TYPE_PANEL