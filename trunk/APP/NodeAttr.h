
#ifndef _NODEATTR_H_
#define _NODEATTR_H_

#define NODEATTR_FLASH_ADDR   (WIFI_PROFILE_ADDR+KBytes(4))

#define STATUS_CONFIGURED 0xEE

#define NODE_NAME_MAX_LENGTH  50
#define SSID_MAX_LENGTH       20
#define KEY_MAX_LENGTH        20

typedef struct{
  int id;      //��ʶ�ڵ��ΨһID�����ڵ�ʵ�ֺ�MAC��ַ�ĵ�32λ��ͬ��
  char type;    //�� or ����
  char status[5];   //�ڵ�״̬
  char config_status; //IP��ַ��ID�Ƿ��Ѿ�����:0xEE�Ѿ����ã�����ֵδ����
  char ipaddr[4];
  char gateway[4];
  char netmask[4];
  
  char ssid_length;   //���Ƶĳ���
  char ssid[SSID_MAX_LENGTH];//�ڵ������
  
  char key_length;   //���Ƶĳ���
  char key[KEY_MAX_LENGTH];//�ڵ������
  
  char name_length;   //���Ƶĳ���
  char name[NODE_NAME_MAX_LENGTH];//�ڵ������

  Group_Table *group_table;  //�����ĳ����б�
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

void NodeAttr_SetType(char type);

void NodeAttr_SetStatus(char *status);

int NodeAttr_GetID(void);

int NodeAttr_GetName(char *name);

char NodeAttr_GetType(void);

int NodeAttr_GetStatus(char *status);

void NodeAttr_SetSSID(char *ssid,int length);

int NodeAttr_GetSSID(char *ssid);

void NodeAttr_SetKey(char *key,int length);

int NodeAttr_GetKey(char *key);


void NodeAttr_SetIO(char io);

void NodeAttr_SetPWM1(char pwm1);

void NodeAttr_SetPWM2(char pwm2);

void NodeAttr_SetPWM3(char pwm3);

void NodeAttr_SetPWM4(char pwm4);

char NodeAttr_GetIO(void);

char NodeAttr_GetPWM1(void);

char NodeAttr_GetPWM2(void);

char NodeAttr_GetPWM3(void);

char NodeAttr_GetPWM4(void);

#endif//_NODEATTR_H_
