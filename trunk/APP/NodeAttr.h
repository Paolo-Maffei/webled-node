
#ifndef _NODEATTR_H_
#define _NODEATTR_H_

#define NODEATTR_FLASH_ADDR   (WIFI_PROFILE_ADDR+KBytes(1))

#define NODE_NAME_MAX_LENGTH  50

typedef struct{
  int id;            //��ʶ�ڵ��ΨһID�����ڵ�ʵ�ֺ�IP��ַ������ID�ֶ���ͬ��
  int name_length;   //���Ƶĳ���
  char name[NODE_NAME_MAX_LENGTH];//�ڵ������
  char mode;    //�� or ����
  int status;   //����״̬
  Group_Table *group_table;  //�����ĳ����б�
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
