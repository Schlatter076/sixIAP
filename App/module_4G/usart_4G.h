/*
 * usart_4G.h
 *
 *  Created on: 2020年6月11日
 *      Author: loyer
 */

#ifndef USART_4G_H_
#define USART_4G_H_

#include "stm32f10x.h"
#include "tcp_public.h"
#include "stdio.h"
#include <string.h>
#include <stdbool.h>
#include "systick.h"
#include "base64.h"
#include "stdlib.h"
#include "STMFlash.h"
#include "iap.h"
#include "iap_config.h"
#include "usart.h"

#define TCPServer_IP    "server.dayitc.com"
//#define TCPServer_IP    "119.23.38.148"
#define TCPServer_PORT  "5599"

#define F4G_USART(fmt, ...)	 _USART_printf (USART2, fmt, ##__VA_ARGS__)
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)

#define UART2_4G_TX GPIO_Pin_2 //GPIOA 4G模块通信
#define UART2_4G_RX GPIO_Pin_3
#define RST_4G      GPIO_Pin_0 //GPIOC
#define PKEY_4G     GPIO_Pin_1

void USART2_Init(uint32_t rate);
void F4G_Init(u32 bound);
bool ConnectToServerBy4G(char* addr, char* port);
void F4G_ExitUnvarnishSend(void);
void getModuleMes(void);

#endif /* USART_4G_H_ */
