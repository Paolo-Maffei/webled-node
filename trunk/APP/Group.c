/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"

Group_Table Group_Match_Table = {0,};

void GroupTable_Init(void)
{
	Flash_Read(GROUP_TABLE_ADDR,(PBYTE)&Group_Match_Table.size,sizeof(Group_Match_Table.size));
	if(-1 == Group_Match_Table.size)
	{
		Group_Match_Table.size = 0;
		GroupTable_Save();
	}
	else if(0 != Group_Match_Table.size)
	{
		FreeMemory(Group_Match_Table.table);
		Group_Match_Table.table = AllocMemory(sizeof(*Group_Match_Table.table)*Group_Match_Table.size);
		Flash_Read((GROUP_TABLE_ADDR+sizeof(Group_Match_Table.size)),(PBYTE)Group_Match_Table.table,
		sizeof(*Group_Match_Table.table)*Group_Match_Table.size);
	}
	else
	{
	}
	//Console_Print("GroupTable Initialized,table size:%d\n",Group_Match_Table.size);
}

static void GroupTable_Save(void)
{
	Flash_Write(GROUP_TABLE_ADDR,(PBYTE)&Group_Match_Table.size,sizeof(Group_Match_Table.size));
	Flash_Write((GROUP_TABLE_ADDR+sizeof(Group_Match_Table.size)),(PBYTE)Group_Match_Table.table,
	sizeof(*Group_Match_Table.table)*Group_Match_Table.size);
	//Console_Print("GroupTable Saved,table size:%d\n",Group_Match_Table.size);
}

void GroupTable_Add(int GroupID,char *status)
{
	if(0 != GroupTable_Exist(GroupID))
		return;
	Flash_Write((GROUP_TABLE_ADDR+sizeof(Group_Match_Table.size)+sizeof(*Group_Match_Table.table)*Group_Match_Table.size),
	(PBYTE)&GroupID,sizeof(GroupID));
	Group_Match_Table.size++;
	Flash_Write(GROUP_TABLE_ADDR,(PBYTE)&Group_Match_Table.size,sizeof(Group_Match_Table.size));
	GroupTable_Init();
	//Console_Print("GroupTable Added,table size:%d\n",Group_Match_Table.size);
}
void GroupTable_Del(int GroupID)
{
	int index = GroupTable_Find(GroupID);
	if(0 < index) //founded!
	{
		int i;
		index--;  //recovery real index
		for(i=index;i<Group_Match_Table.size;i++)
		{
			Group_Match_Table.table[i] = Group_Match_Table.table[i+1];
		}
		Group_Match_Table.size--;
		GroupTable_Save();
		GroupTable_Init();
		//Console_Print("GroupTable_Del,table size:%d\n",Group_Match_Table.size);
	}
	//Console_Print("GroupTable_Del,GroupID not found!:%d\n",Group_Match_Table.size);
}

void GroupTable_DelAll(void)
{
  Group_Match_Table.size = 0;
  GroupTable_Save();
}

static int GroupTable_Find(int GroupID)
{
	int i;
	for(i=0;i<Group_Match_Table.size;i++)
	{
		if(GroupID == Group_Match_Table.table[i])
			return (i+1);   //return index+1;
	}
	return 0;  //not found 
}

int GroupTable_Exist(int GroupID)
{
	return (!!GroupTable_Find(GroupID));  //0:not found;
}

int GroupTable_IDasm(char *g) //组装计算网络传来的ID
{
  int ret = (int)g[0] + ((int)g[1]<<8) + ((int)g[2]<<16) + ((int)g[3]<<24);
  return ret;
}
