
#ifndef __WIFI_COMMAND__
#define __WIFI_COMMAND__
//
// Macros definition
//
#define WIFICMD_MAXARGS				3

#define WIFICMD_RESULT_SUCCESS			(0)
#define WIFICMD_RESULT_UNKNOWCMD		(-1)
#define WIFICMD_RESULT_UNKNOWOPTION		(-2)
#define WIFICMD_RESULT_ERRORPARAM		(-3)
#define WIFICMD_RESULT_PARAMNOTSET		(-4)

//
// Structs definition
//

//
// Exported functions declare
//
int WiFiCmd_RunCommand (char *pLine);

#endif // __WIFI_COMMAND__

