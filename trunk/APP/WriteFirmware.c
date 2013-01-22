
#include "Project.h"

volatile int32_t uIP_RunTime = 0;//__IO == volatile
void main(void)
{
		// Bsp Init
	Set_System();

	TraceInit();
	Board_Init ();
	Flash_Init();
  
	GroupTable_Init();
	GroupTable_Add(0x1234);
	GroupTable_Add(0x4321);
	GroupTable_Init();
	if(0 != GroupTable_Exist(0x1234))
		Console_Print("0x1234 existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	GroupTable_Del(0x1234);
  if(0 != GroupTable_Exist(0x1234))
		Console_Print("0x1234 Existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	else
		Console_Print("0x1234 not existed in GroupTable ,table size:%d\n",Group_Match_Table.size);
	while (1);
}
