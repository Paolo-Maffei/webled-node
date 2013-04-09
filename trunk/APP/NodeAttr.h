
#ifndef _NODEATTR_H_
#define _NODEATTR_H_

#define NODEATTR_FLASH_ADDR   (WIFI_PROFILE_ADDR+KBytes(4))

#define STATUS_CONFIGURED 0xEE

#define NODE_NAME_MAX_LENGTH  50

typedef struct{
  int id;      //标识节点的唯一ID，现在的实现和MAC地址的低32位相同。
  int mode;    //灯 or 开关
  int status;   //节点状态
  char config_status; //IP地址，ID是否已经配置:0xEE已经配置，其它值未配置
  char ipaddr[4];
  char gateway[4];
  char netmask[4];
  char name_length;   //名称的长度
  char name[NODE_NAME_MAX_LENGTH];//节点的名称

  Group_Table *group_table;  //所属的场景列表
}NodeAttr,*pNodeAttr;

extern NodeAttr node_info;

void Init_NodeAttr(void);

static void NodeAttr_Save(void);

void NodeAttr_SetID(int id);

void NodeAttr_Set_Config_Status(void);

char NodeAttr_Get_Config_Status(void);

void NodeAttr_Clr_Config_Status(void);

char NodeAttr_Is_Configed(void);

void NodeAttr_SetIP(char *ip,char *gwip,char *netmask);

void NodeAttr_SetName(char *name,int length);

void NodeAttr_SetMode(int mode);

void NodeAttr_SetStatus(int status);

int NodeAttr_GetID(void);

int NodeAttr_GetName(char *name);

char NodeAttr_GetMode(void);

int NodeAttr_GetStatus(void);

#endif//_NODEATTR_H_
