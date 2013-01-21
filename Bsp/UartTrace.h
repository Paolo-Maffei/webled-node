#ifndef __UART_TRACE_H__
#define __UART_TRACE_H__
//
// Macros definitions
//
#define TR_CLR_FORE_BLACK		30
#define TR_CLR_FORE_RED			31
#define TR_CLR_FORE_GREEN		32
#define TR_CLR_FORE_YELLOW		33
#define TR_CLR_FORE_BLUE		34
#define TR_CLR_FORE_PURPLE		35
#define TR_CLR_FORE_CYAN		36
#define TR_CLR_FORE_WHITE		37

#define TR_CLR_BACK_BLACK		40
#define TR_CLR_BACK_RED			41
#define TR_CLR_BACK_GREEN		42
#define TR_CLR_BACK_YELLOW		43
#define TR_CLR_BACK_BLUE		44
#define TR_CLR_BACK_PURPLE		45
#define TR_CLR_BACK_CYAN		46
#define TR_CLR_BACK_WHITE		47

//
// Exported functions declare
//
#ifndef NO_TRACE
BOOL TraceInit(void);
void TRACE(const char *fmt, ...);
void TRACEDATA(const char* pszPrompt,PVOID pData,int nSize);
void CTRACE(UCHAR ucFore, UCHAR ucBack, const char *fmt, ...);
#else // NO_TRACE
#define TraceInit(void)
static void TRACE(const char *fmt, ...){}
static void TRACEDATA(const char* pszPrompt,PVOID pData,int nSize){}
static void CTRACE(UCHAR ucFore, UCHAR ucBack, const char *fmt, ...){}
#endif // NO_TRACE

#ifndef NO_DEBUG
#undef ASSERT
#define ASSERT(x)						\
	do {								\
		if (!(x)) {						\
			TRACE("ASSERT error!\r\n");	\
			while(1);					\
		}								\
	} while(0)
#else // NO_DEBUG
#define ASSERT(x)
#endif

#endif
