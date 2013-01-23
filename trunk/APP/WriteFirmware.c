
#include "Project.h"

volatile int32_t uIP_RunTime = 0;//__IO == volatile
void main(void)
{
		// Bsp Init
	Set_System();

	TraceInit();
	Board_Init ();
	Console_Init ();
	Flash_Init();
  
	GroupTable_Init();
	GroupTable_Add(0xFFFFFFFF);
	GroupTable_Add(0x87654321);
	GroupTable_Init();
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
