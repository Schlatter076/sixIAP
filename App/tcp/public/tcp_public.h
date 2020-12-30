/*
 * tcp_public.h
 *
 *  Created on: 2020年10月24日
 *      Author: loyer
 */

#ifndef _TCP_PUBLIC_H_
#define _TCP_PUBLIC_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "systick.h"
#include "malloc.h"
#include "stdarg.h"
#include "STMFlash.h"
#include "base64.h"

#define TCP_IP "server.dayitc.com"
#define TCP_PORT "5599"


#define TCP_MAX_LEN 1024		  //最大接收缓存字节数
#define BASE64_BUF_LEN 512

struct STRUCT_USART_Fram  //定义一个全局串口数据帧的处理结构体
{
	unsigned char Data[TCP_MAX_LEN];
	unsigned char DeData[BASE64_BUF_LEN];
	unsigned char EnData[BASE64_BUF_LEN];
	char *base64Str;
	unsigned char ServerData[BASE64_BUF_LEN];
	unsigned char *Server_Command[2];
	__IO u8 linkedClosed;
	__IO u8 allowProcessServerData;
	__IO u8 init;
	__IO u8 registerSuccess;
	__IO u8 firstStatuHeartNotSucc;
	__IO u8 serverStatuCnt;
	__IO u8 allowHeart;
	__IO u8 forceHeart_32;
	__IO u8 forceHeart_90;
	__IO u8 AT_test_OK;
	union
	{
		__IO u16 InfAll;
		struct
		{
			__IO u16 Length :15;                               // 14:0
			__IO u16 FinishFlag :1;                                // 15
		} InfBit;
	};
};
extern struct STRUCT_USART_Fram F4G_Fram;
extern struct STRUCT_USART_Fram WIFI_Fram;

extern struct STRUCT_USART_Params
{
	char locations[2][12];
	unsigned char ccid[24];
	unsigned char cops;
	u8 rssi; //信号强度
	int port;
	char dd[20];
	__IO u8 play;
	u8 statuCode[6];
	u8 currentStatuCode[6];
	__IO u8 checkPBst;
	__IO u8 process4G;
	__IO u8 processWIFI;
	__IO u8 wifiParamModified;
} TCP_Params;

typedef enum
{
	STA, AP, STA_AP
} ENUM_Net_ModeTypeDef;

typedef enum
{
	enumTCP, enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum
{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

typedef enum
{
	In4G = 1, InWifi = 2,
} ENUM_Internet_TypeDef;

typedef struct
{
	char ssid[100];
	char pwd[100];
} ParamsOfWifiJoinAP_TypeDef;

extern ParamsOfWifiJoinAP_TypeDef ParamsOfWifiJoinAPInit;

void _USART_printf(USART_TypeDef * USARTx, char * Data, ...);
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time);
bool AT_Test(ENUM_Internet_TypeDef internet);
void mySplit(struct STRUCT_USART_Fram *fram, char *delims);
void TCP_sendStr(USART_TypeDef* USARTx, char *str);
void getRegisterStr(char *strBuf, int len, const char* upCMD,
		struct STRUCT_USART_Params *params, char moduleType, char *version,
		char *num);
void getRequestStrWithoutParam(char *strBuf, int len, const char* upCMD);
void request4Register(USART_TypeDef* USARTx);

#endif /* _TCP_PUBLIC_H_ */
