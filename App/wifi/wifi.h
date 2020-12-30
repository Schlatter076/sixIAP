/*
 * wifi.h
 *
 *  Created on: 2020Äê10ÔÂ26ÈÕ
 *      Author: loyer
 */

#ifndef _WIFI_H_
#define _WIFI_H_

#include "tcp_public.h"
#include <stdlib.h>

#define WIFI_CH_PD_Pin     GPIO_Pin_9
#define WIFI_CH_PD_Pin_Port     GPIOC
#define WIFI_CH_PD_Pin_Periph_Clock  RCC_APB2Periph_GPIOC

#define WIFI_RST_Pin     GPIO_Pin_2
#define WIFI_RST_Pin_Port     GPIOC
#define WIFI_RST_Pin_Periph_Clock  RCC_APB2Periph_GPIOC

#define WIFI_CH_PD_Pin_SetH     GPIO_SetBits(WIFI_CH_PD_Pin_Port,WIFI_CH_PD_Pin)
#define WIFI_CH_PD_Pin_SetL     GPIO_ResetBits(WIFI_CH_PD_Pin_Port,WIFI_CH_PD_Pin)
#define WIFI_RST_Pin_SetH     GPIO_SetBits(WIFI_RST_Pin_Port,WIFI_RST_Pin)
#define WIFI_RST_Pin_SetL     GPIO_ResetBits(WIFI_RST_Pin_Port,WIFI_RST_Pin)

#define WIFI_USART(fmt, ...)	 _USART_printf (UART4, fmt, ##__VA_ARGS__)

void UART4_Init(u32 bound);
void WIFI_Init(u32 bound);
bool WIFI_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode);
bool WIFI_JoinAP(char * pSSID, char * pPassWord);
bool WIFI_Enable_MultipleId(FunctionalState enumEnUnvarnishTx);
bool WIFI_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char* ComNum, ENUM_ID_NO_TypeDef id);
bool WIFI_UnvarnishSend(void);
bool WIFI_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId);
void WIFI_ExitUnvarnishSend(void);
u8 WIFI_Get_LinkStatus(void);
bool ConnectToServerByWIFI(char* addr, char* port);

#endif /* _WIFI_H_ */
