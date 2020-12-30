/*
 * STMFlash.c
 *
 *  Created on: 2020年6月19日
 *      Author: loyer
 */
#include "STMFlash.h"

__IO char *WDeviceID = NULL;
__IO char RDeviceID[9] =
{ 0 };

char *WVersion = NULL;
char RVersion[21] =
{ 0 };

char APPServer[101] =
{ 0 };

unsigned char IgnoreLock[6] =
{ 0 };

//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*) faddr;
}
#if STM32_FLASH_WREN	//如果使能了写
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u16 i;
	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2; //地址增加2.
	}
}
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else
#define STM_SECTOR_SIZE	2048
#endif

u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2]; //最多是2K字节

void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)
	u16 i;
	u32 offaddr;   //去掉0X08000000后的地址

	if (WriteAddr < STM32_FLASH_BASE
			|| (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
		return;   //非法地址
	FLASH_Unlock();						//解锁
	offaddr = WriteAddr - STM32_FLASH_BASE;		//实际偏移地址.
	secpos = offaddr / STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff = (offaddr % STM_SECTOR_SIZE) / 2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE / 2 - secoff;		//扇区剩余空间大小
	if (NumToWrite <= secremain)
		secremain = NumToWrite;		//不大于该扇区范围
	while (1)
	{
		STMFLASH_Read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF,
		STM_SECTOR_SIZE / 2);		//读出整个扇区的内容
		for (i = 0; i < secremain; i++)		//校验数据
		{
			if (STMFLASH_BUF[secoff + i] != 0XFFFF)
				break;		//需要擦除
		}
		if (i < secremain)		//需要擦除
		{
			FLASH_ErasePage(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE);//擦除这个扇区
			for (i = 0; i < secremain; i++)		//复制
			{
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE,
					STMFLASH_BUF, STM_SECTOR_SIZE / 2);		//写入整个扇区
		}
		else
			STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//写已经擦除了的,直接写入扇区剩余区间.
		if (NumToWrite == secremain)
			break;		//写入结束了
		else		//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff = 0;				//偏移位置为0
			pBuffer += secremain;  	//指针偏移
			WriteAddr += secremain;	//写地址偏移
			NumToWrite -= secremain;	//字节(16位)数递减
			if (NumToWrite > (STM_SECTOR_SIZE / 2))
				secremain = STM_SECTOR_SIZE / 2;	//下一个扇区还是写不完
			else
				secremain = NumToWrite;	//下一个扇区可以写完了
		}
	};
	FLASH_Lock();	//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
	u16 i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);	//读取2个字节.
		ReadAddr += 2;	//偏移2个字节.
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr, u16 WriteData)
{
	STMFLASH_Write(WriteAddr, &WriteData, 1);	//写入一个字
}

void WriteDeviceID(void)
{
	STMFLASH_Write(EEPROM_ADDR, (u16 *) WDeviceID, 4);
}
void ReadDeviceID(void)
{
	STMFLASH_Read(EEPROM_ADDR, (u16 *) RDeviceID, 4);
	RDeviceID[8] = '\0';
}

void writeVersion(void)
{
	STMFLASH_Write(VERSION_ADDR, (u16 *) WVersion, 10);
}

void ReadVersion(void)
{
	STMFLASH_Read(VERSION_ADDR, (u16 *) RVersion, 10);
	RVersion[20] = '\0';
}

void WriteAPPServer(char *server)
{
	STMFLASH_Write(APP_SERVER_ADDR, (u16 *) server, 50);
}

void ReadAPPServer(void)
{
	STMFLASH_Read(APP_SERVER_ADDR, (u16 *) APPServer, 50);
	APPServer[100] = '\0';
}

void WriteIgnoreLock(u8 inx, u8 sta)
{
	IgnoreLock[inx] = sta;
	STMFLASH_Write(IGNORE_LOCK_ADDR, (u16 *) IgnoreLock, 3);
}

u8 ReadIgnoreLock(u8 inx)
{
	STMFLASH_Read(IGNORE_LOCK_ADDR, (u16 *) IgnoreLock, 3);
	return IgnoreLock[inx];
}

void WriteWifiSsid(void)
{
	STMFLASH_Write(WIFI_SSID_ADDR, (u16 *) ParamsOfWifiJoinAPInit.ssid, 50);
}
void ReadWifiSsid(void)
{
	STMFLASH_Read(WIFI_SSID_ADDR, (u16 *) ParamsOfWifiJoinAPInit.ssid, 50);
	ParamsOfWifiJoinAPInit.ssid[99] = '\0'; //添加结束符
}

void WriteWifiPwd(void)
{
	STMFLASH_Write(WIFI_PWD_ADDR, (u16 *) ParamsOfWifiJoinAPInit.pwd, 50);
}
void ReadWifiPwd(void)
{
	STMFLASH_Read(WIFI_PWD_ADDR, (u16 *) ParamsOfWifiJoinAPInit.pwd, 50);
	ParamsOfWifiJoinAPInit.pwd[99] = '\0'; //添加结束符
}

void WriteWifiFlag(void)
{
	u16 flag = 0x5746;
	STMFLASH_Write(WIFI_FLAG_ADDR, &flag, 1);
}

u16 ReadWifiFlag(void)
{
	u16 flag = 0;
	STMFLASH_Read(WIFI_FLAG_ADDR, &flag, 1);
	return flag;
}
