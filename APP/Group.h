/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#ifndef _GROUP_H_
#define _GROUP_H_

#define  GROUP_TABLE_ADDR  	(NODEATTR_FLASH_ADDR+KBytes(1))
//#define  MAX_GROUP_NUM	20

typedef struct{
	int size;
	int *table;
}Group_Table,*pGroup_Table;


extern Group_Table Group_Match_Table;

static void GroupTable_Save(void);
static int GroupTable_Find(int GroupID);

void GroupTable_Init(void);

void GroupTable_Add(int GroupID);
void GroupTable_Del(int GroupID);
int GroupTable_Exist(int GroupID);


#endif