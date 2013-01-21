/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

//
// Created by csword 2011/01/13
// All rights reserved
//
#include "Project.h"

#include "helper_spi_8385.h"
#include "gspi8385.h"

BYTE ucBuffer[FLASH_PAGE_SIZE];

void main(void)
{
	UINT16 usID;

	// Bsp Init
	Set_System();

	TraceInit();
	Board_Init ();
	Flash_Init();

	if (1) {
		UINT32 ReadAddrOffset, AddrBase, DataSize, i;
		PBYTE pOriginalData;
		UINT32 usLength;

		if (1) {
			// Write data to flash
			AddrBase = 0;
			Flash_Write (WIFI_FW_HELPER_ADDR, (UINT8*)helperimage, sizeof (helperimage));
			// Read back data for checking...
			ReadAddrOffset = 0;
			pOriginalData = (PBYTE)helperimage;
			DataSize = sizeof (helperimage);
			while (ReadAddrOffset < DataSize) {
				if (ReadAddrOffset + FLASH_PAGE_SIZE < DataSize)
					usLength = FLASH_PAGE_SIZE;
				else
					usLength = DataSize - ReadAddrOffset;
				Flash_Read(AddrBase + ReadAddrOffset, ucBuffer, usLength);
				for (i = 0; i < usLength; i ++) {
					if (ucBuffer[i] != pOriginalData[ReadAddrOffset + i])
						break;
				}
				ReadAddrOffset += usLength;
				if ((i != usLength) && (ReadAddrOffset != DataSize))
					TRACE("Data Verify failed1!\r\n");
			}
		}

		if (1) {
			// Write data to flash
			AddrBase = WIFI_FW_FW_ADDR;
			Flash_Write (AddrBase, (UINT8*)fmimage, sizeof (fmimage));
			// Read back data for checking...
			ReadAddrOffset = 0;
			pOriginalData = (PBYTE)fmimage;
			DataSize = sizeof (fmimage);
			while (ReadAddrOffset < DataSize) {
				if (ReadAddrOffset + FLASH_PAGE_SIZE < DataSize)
					usLength = FLASH_PAGE_SIZE;
				else
					usLength = DataSize - ReadAddrOffset;
				Flash_Read(AddrBase + ReadAddrOffset, ucBuffer, usLength);
				for (i = 0; i < usLength; i ++) {
					if (ucBuffer[i] != pOriginalData[ReadAddrOffset + i])
						break;
				}
				ReadAddrOffset += usLength;
				if ((i != usLength) && (ReadAddrOffset != DataSize))
					TRACE("Data Verify failed1!2\r\n");
			}
		}
	}

	while (1);
}
