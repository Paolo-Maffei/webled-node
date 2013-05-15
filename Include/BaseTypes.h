
#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

// char,uchar
typedef char CHAR,INT8,int8,S8,*PCHAR,*pchar,*PINT8,*pint8,*PS8,*ps8,*LPSTR;
typedef const char *LPCSTR;
typedef unsigned char uchar,UCHAR,U8,UINT8,uint8,BYTE,*puchar,*PUCHAR,*PU8,*pu8,*PUINT8,*puint8,*PBYTE,*LPBYTE;
// short,ushort
typedef short SHORT,INT16,int16,*PSHORT,*pshort,*PINT16,*pint16,
*PWCHAR,*S16,*pwchar,*PWORD,*pword;
typedef unsigned short WORD,word,USHORT,ushort,UINT16,uint16,U16,*PUSHORT,*pushort,*PUINT16,*puint16,*PU16,*pu16;
// wchar
typedef unsigned short WCHAR;
// INT,uint
typedef int INT,INT32,int32,S32,DWORD,dword,*PINT,*pint,*PINT32,*pint32,*PS32,*ps32,*PDWORD,*pdword;
typedef unsigned int UINT,uint,UINT32,uint32,U32,*PUINT,*puint,*PUINT32,*puint32,*PU32,*pu32;
// long,ulong
typedef long LONG,*PLONG,*plong;
typedef unsigned long ULONG,ulong,*PULONG,*pulong;
// VOID
typedef void VOID,*PVOID,*LPVOID;
// convenience
typedef int BOOL,*PBOOL;

typedef long HANDLE,HINSTANCE,PHYSICAL_ADDRESS,*PDEVICE_OBJECT;
typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } u;
}ULONGLONG, LARGE_INTEGER;

#endif	// __BASE_TYPES_H__
