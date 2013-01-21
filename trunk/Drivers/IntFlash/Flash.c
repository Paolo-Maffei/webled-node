/******************************
微嵌电子工作室 版权所有
STM32 WIFI 开发板
适用于微嵌电子“灵通壹号WIFI开发板”
详情请登录 http://yuikin.taobao.com
源代码使用协议：本代码仅提供给客户做学习、或者参考WIFI原理之用，你不能在你发布的产品里面
包含本源代码的完整文件或者部分片段），不能出售或者使用本工作室提供的源代码制作相同类型产品，
例如：开发板、学习板，营利性教学讲座等等，否则视为侵权，本工作室保留追究相关法律责任的权利。
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


