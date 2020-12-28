/*
 * STMFlash.h
 *
 *  Created on: 2020��6��19��
 *      Author: loyer
 */

#ifndef FLASH_STMFLASH_H_
#define FLASH_STMFLASH_H_

#include "stm32f10x.h"
#include <string.h>

#define STM32_FLASH_SIZE 256 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0����ʹ��;1��ʹ��)

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 BootFLASH����ʼ��ַ

//ģ��EEPROM����ʼ��ַ
#define EEPROM_ADDR   0x0803F804

#define VERSION_ADDR  0x0803F820

#define APP_SERVER_ADDR  0x0803F840
//8�ֽ��豸���  1�ֽ��Ƿ�д����  1�ֽ��Ƿ����߸��³���

#define IGNORE_LOCK_ADDR  0x0803FF00
extern unsigned char IgnoreLock[6];

#define WIFI_FLAG_ADDR  0x0803FF10
#define WIFI_SSID_ADDR  0x0803FF12
#define WIFI_PWD_ADDR   0x0803FF80

__IO extern char *WDeviceID;
__IO extern char RDeviceID[9];

extern char *WVersion;
extern char RVersion[21];

extern char APPServer[101];

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������
void STMFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr, u16 Len);				//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead); //��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr, u16 WriteData);
void WriteDeviceID(void);
void ReadDeviceID(void);
void writeVersion(void);
void ReadVersion(void);
void WriteAPPServer(char *server);
void ReadAPPServer(void);

void WriteIgnoreLock(u8 inx, u8 sta);
u8 ReadIgnoreLock(u8 inx);

#endif /* FLASH_STMFLASH_H_ */
