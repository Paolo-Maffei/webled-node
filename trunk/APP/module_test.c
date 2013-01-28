
#include "Project.h"

char name1[]="hellonode";
char name2[]="zhouqiang";

void main(void)
{
		// Bsp Init
	Set_System();

	TraceInit();
	Console_Init ();
	Flash_Init();
  
	GroupTable_Init();
	GroupTable_Add(0xFFFFFFFF);
	GroupTable_Add(0x87654321);
	GroupTable_Init();
        
//        Init_NodeAttr();
//        NodeAttr_SetID(255);
//        NodeAttr_SetName(name1,sizeof(name1));
//        NodeAttr_SetMode(12);
//        NodeAttr_SetStatus(22);
//         NodeAttr_SetName(name2,sizeof(name2));
//        int tmp = NodeAttr_GetID();
//        
        
        
	if(0 != GroupTable_Exist(0xFFFFFFFF))
		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	
	if(0 != GroupTable_Exist(0x87654321))
		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	
	GroupTable_Del(0x87654321);
  if(0 != GroupTable_Exist(0x87654321))
		Console_Print("0x1234 Existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);

  if(0 != GroupTable_Exist(0xFFFFFFFF))
		Console_Print("0x1234 Existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	
	GroupTable_Add(0x12433448);
	GroupTable_Init();
	if(0 != GroupTable_Exist(0x12433448))
		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	
	while (1);
}
