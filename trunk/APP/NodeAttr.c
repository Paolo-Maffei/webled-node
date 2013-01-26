/*
  Wifi节点的属性定义，相关数据操作
*/

#include "Project.h"

NodeAttr node_info;

void Init_NodeAttr(void)  //call GroupTable_Init() before
{
  Flash_Read(NODEATTR_FLASH_ADDR,(PBYTE)&node_info,sizeof(NodeAttr));
  node_info.group_table = &Group_Match_Table;
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

void NodeAttr_SetMode(int mode)
{
  node_info.mode = mode;
  NodeAttr_Save();
}

char NodeAttr_GetMode(void)
{
  return node_info.mode;
}

void NodeAttr_SetStatus(int status)
{
  node_info.status = status;
  NodeAttr_Save();
}

int NodeAttr_GetStatus(void)
{
  return node_info.status;
}