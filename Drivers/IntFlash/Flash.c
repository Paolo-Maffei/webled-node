/******************************
΢Ƕ���ӹ����� ��Ȩ����
STM32 WIFI ������
������΢Ƕ���ӡ���ͨҼ��WIFI�����塱
�������¼ http://yuikin.taobao.com
Դ����ʹ��Э�飺��������ṩ���ͻ���ѧϰ�����߲ο�WIFIԭ��֮�ã��㲻�����㷢���Ĳ�Ʒ����
������Դ����������ļ����߲���Ƭ�Σ������ܳ��ۻ���ʹ�ñ��������ṩ��Դ����������ͬ���Ͳ�Ʒ��
���磺�����塢ѧϰ�壬Ӫ���Խ�ѧ�����ȵȣ�������Ϊ��Ȩ���������ұ���׷����ط������ε�Ȩ����
*******************************/

#include "Project.h"

BOOL Flash_Init(void)
{
	Flash_UnLock ();

	return TRUE;
}

BOOL Flash_UnLock(void)
{
	FLASH_Unlock();

	return TRUE;
}

BOOL Flash_Read(UINT32 param_flashbase, PBYTE pRomData, UINT32 NumByteToRead)
{
	UINT8* ptr;

	ptr = (UINT8*)param_flashbase;

	while( NumByteToRead-- >0) {
		*pRomData = *ptr;
		pRomData ++;
		ptr ++;
	}

	return TRUE;
}

BOOL Flash_Write(UINT32 param_flashbase, PBYTE pRomData, UINT32 NumByteToWrite)
{
	UINT32 tempaddress;
	UINT32 startaddress;
	UINT32 FlashAddress;
	UINT32 datasource;
	UINT32 pagenumber = 0x0;
	UINT32 EraseCounter = 0x0;
	UINT32 i = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	PBYTE pBuffer1 = NULL;
	BOOL bResult = TRUE;
	
	pBuffer1 = AllocMemory(FLASH_PAGE_SIZE);
	if (!pBuffer1) {
		TRACE("Flash_Write: Memory alloc failed!1");
		bResult = FALSE;
		goto Done;
	}

	startaddress = tempaddress = param_flashbase;

	if ((tempaddress % FLASH_PAGE_SIZE) != 0) {
		if (((startaddress % FLASH_PAGE_SIZE) + NumByteToWrite) >  FLASH_PAGE_SIZE) {
			Flash_Read((tempaddress - (tempaddress % FLASH_PAGE_SIZE)), pBuffer1, FLASH_PAGE_SIZE);
			CopyMemory(pBuffer1 + (tempaddress % FLASH_PAGE_SIZE), pRomData, FLASH_PAGE_SIZE - (tempaddress % FLASH_PAGE_SIZE));

			if (FLASH_COMPLETE == FLASH_ErasePage(tempaddress)) {
				i = FLASH_PAGE_SIZE / 4;
				datasource = (UINT32)pBuffer1;
				FlashAddress = tempaddress - (tempaddress % FLASH_PAGE_SIZE);
				while (i-- > 0) {
					FLASH_ProgramWord(FlashAddress, *(UINT32*)datasource);
					if (*(UINT32*)FlashAddress != *(UINT32*)datasource) {
						bResult = FALSE;
						goto Done;
					}
					datasource += 4;
					FlashAddress += 4;
				}
			}

			NumByteToWrite -= FLASH_PAGE_SIZE - (startaddress % FLASH_PAGE_SIZE);
			tempaddress +=  FLASH_PAGE_SIZE - (startaddress % FLASH_PAGE_SIZE);
			pRomData += FLASH_PAGE_SIZE - (startaddress % FLASH_PAGE_SIZE);
				
			if ((NumByteToWrite % FLASH_PAGE_SIZE) != 0) {
				Flash_Read(tempaddress + NumByteToWrite - (NumByteToWrite % FLASH_PAGE_SIZE), pBuffer1, FLASH_PAGE_SIZE);
				CopyMemory(pBuffer1, pRomData + NumByteToWrite - (NumByteToWrite % FLASH_PAGE_SIZE), NumByteToWrite % FLASH_PAGE_SIZE);

				if (FLASHStatus == FLASH_ErasePage(tempaddress + NumByteToWrite)) {
					i = FLASH_PAGE_SIZE / 4;
					datasource = (UINT32)pBuffer1;
					FlashAddress = tempaddress + NumByteToWrite - (NumByteToWrite % FLASH_PAGE_SIZE);
					while (i-- > 0) {
						FLASH_ProgramWord(FlashAddress, *(UINT32*)datasource);
						if (*(UINT32*)FlashAddress != *(UINT32*)datasource) {
							bResult = FALSE;
							goto Done;
						}
						datasource += 4;
						FlashAddress += 4;
					}
				}
			}

			NumByteToWrite -= NumByteToWrite % FLASH_PAGE_SIZE;
			pagenumber =  NumByteToWrite / FLASH_PAGE_SIZE;

			for (EraseCounter = 0; (EraseCounter < pagenumber) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
				FLASHStatus = FLASH_ErasePage(tempaddress + FLASH_PAGE_SIZE * EraseCounter);

			datasource = (UINT32)pRomData;
			FlashAddress = tempaddress;

			while (pagenumber-- > 0) {
				i = FLASH_PAGE_SIZE / 4;
				while (i -- > 0) {
					FLASH_ProgramWord(FlashAddress, *(UINT32*)datasource);
					if (*(UINT32*)FlashAddress != *(UINT32*)datasource) {
						bResult = FALSE;
						goto Done;
					}
					datasource += 4;
					FlashAddress += 4;
				}
			}
		}
		else {
			Flash_Read((startaddress - (startaddress % FLASH_PAGE_SIZE)), pBuffer1, FLASH_PAGE_SIZE);
			CopyMemory((pBuffer1 + (tempaddress % FLASH_PAGE_SIZE)), pRomData, NumByteToWrite);
			while (FLASHStatus == FLASH_ErasePage(tempaddress)) {
				i = FLASH_PAGE_SIZE / 4;
				datasource = (UINT32)pBuffer1;
				FlashAddress = tempaddress - (tempaddress % FLASH_PAGE_SIZE);
				while (i-- > 0) {
					FLASH_ProgramWord(FlashAddress, *(UINT32*)datasource);
					if (*(UINT32 *)FlashAddress != *(UINT32 *)datasource) {
						bResult = FALSE;
						goto Done;
					}
					datasource += 4;
					FlashAddress += 4;
				}
				break;
			}
		}
	}
	else {
		if ((NumByteToWrite % FLASH_PAGE_SIZE) != 0) {
			Flash_Read((UINT16)(tempaddress + NumByteToWrite - (NumByteToWrite % FLASH_PAGE_SIZE)), pBuffer1, FLASH_PAGE_SIZE);
			CopyMemory(pBuffer1, pRomData + NumByteToWrite - (NumByteToWrite % FLASH_PAGE_SIZE), (NumByteToWrite % FLASH_PAGE_SIZE));
		}
		if ((NumByteToWrite % FLASH_PAGE_SIZE) == 0)
			pagenumber = NumByteToWrite / FLASH_PAGE_SIZE;
		else
			pagenumber = NumByteToWrite / FLASH_PAGE_SIZE + 1;

		for (EraseCounter = 0; (EraseCounter < pagenumber) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
			FLASHStatus = FLASH_ErasePage(startaddress + (FLASH_PAGE_SIZE * EraseCounter));

		if (pagenumber == 1) {
			i = FLASH_PAGE_SIZE / 4;
			datasource = (UINT32)pBuffer1;
			FlashAddress = startaddress;
			while (i-- > 0) {
				FLASH_ProgramWord(FlashAddress, *(UINT32 *)datasource);
				if (*(UINT32 *)FlashAddress != *(UINT32 *)datasource) {
					bResult = FALSE;
					goto Done;
				}
				datasource += 4;
				FlashAddress += 4;
			}
		}
		else {
			int number = pagenumber;
			
			while (number-- > 1) {
				datasource = (UINT32)pRomData;
				FlashAddress = startaddress;
				i = FLASH_PAGE_SIZE / 4;
				while (i -- > 0) {
					FLASH_ProgramWord(FlashAddress, *(UINT32 *)datasource);
					if (*(UINT32 *)FlashAddress != *(UINT32 *)datasource) {
						TRACE("Verify error!1 0x%08X\r\n", FlashAddress);
						bResult = FALSE;
						goto Done;
					}
					datasource += 4;
					FlashAddress += 4;
				}
			}
			datasource = (UINT32)pBuffer1;
			FlashAddress = startaddress + (pagenumber - 1) * FLASH_PAGE_SIZE;
			i = FLASH_PAGE_SIZE / 4;
			while (i -- > 0) {
				FLASH_ProgramWord(FlashAddress, *(UINT32 *)datasource);
				if (*(UINT32 *)FlashAddress != *(UINT32 *)datasource) {
					TRACE("Verify error!2 0x%08X\r\n", FlashAddress);
					bResult = FALSE;
					goto Done;
				}
				datasource += 4;
				FlashAddress += 4;
			}
		}
	}

Done:
	if (pBuffer1)
		FreeMemory (pBuffer1);

	return bResult;
}


