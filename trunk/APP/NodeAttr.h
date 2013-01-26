
#ifndef _NODEATTR_H_
#define _NODEATTR_H_

#define NODEATTR_FLASH_ADDR   (WIFI_PROFILE_ADDR+KBytes(1))

#define NODE_NAME_MAX_LENGTH  50

typedef struct{
  int id;            //标识节点的唯一ID，现在的实现和IP地址的子网ID字段相同。
  int name_length;   //名称的长度
  char name[NODE_NAME_MAX_LENGTH];//节点的名称
  char mode;    //灯 or 开关
  int status;   //工作状态
  Group_Table *group_table;  //所属的场景列表
}NodeAttr,*pNodeAttr;


void Init_NodeAttr(void);

static void NodeAttr_Save(void);

void NodeAttr_SetID(int id);

void NodeAttr_SetName(char *name,int length);

void NodeAttr_SetMode(int mode);

void NodeAttr_SetStatus(int status);

int NodeAttr_GetID(void);

int NodeAttr_GetName(char *name);

char NodeAttr_GetMode(void);

int NodeAttr_GetStatus(void);

#endif//_NODEATTR_H_
