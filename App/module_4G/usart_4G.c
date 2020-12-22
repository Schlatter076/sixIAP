/*
 * usart_4G.c
 *
 *  Created on: 2020年6月11日
 *      Author: loyer
 */
#include "usart_4G.h"

char ip[] = "server.dayitc.com";
char port[] = "5599";

void USART2_Init(uint32_t bound)
{
	USART_DeInit(USART2);

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2  TXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA3  RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIOA3

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2

	USART_ITConfig(USART2, USART_IT_RXNE | USART_IT_IDLE, ENABLE); //开启串口接受和总线空闲中断

	USART_Cmd(USART2, ENABLE);
}

void F4G_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	F4G_Fram.init = 1;

	USART2_Init(bound);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PKEY_4G | RST_4G;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	if (!localUpdate)
	{
		//开机
		do
		{
			GPIO_SetBits(GPIOC, PKEY_4G);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			GPIO_ResetBits(GPIOC, PKEY_4G);
			//复位4G模块
			GPIO_SetBits(GPIOC, RST_4G);
			delay_ms(1100);
			GPIO_ResetBits(GPIOC, RST_4G);
			delay_ms(500);

			F4G_ExitUnvarnishSend();
			Send_AT_Cmd(In4G, "AT+CIPCLOSE", "OK", NULL, 500);
			Send_AT_Cmd(In4G, "AT+RSTSET", "OK", NULL, 500);
		} while (!AT_Test(In4G));

		//填充好相关信息
		getModuleMes();
		ConnectToServerBy4G(ip, port);
	}
}

void USART2_IRQHandler(void)
{
	u8 ucCh;
	if (USART_GetITStatus( USART2, USART_IT_RXNE) != RESET)
	{
		ucCh = USART_ReceiveData( USART2);

		if (F4G_Fram.InfBit.Length < (TCP_MAX_LEN - 1))
		{
			F4G_Fram.Data[F4G_Fram.InfBit.Length++] = ucCh;
		}
		else
		{
			printf("4G Cmd size over.\r\n");
			memset(F4G_Fram.Data, 0, TCP_MAX_LEN);
			F4G_Fram.InfAll = 0;
		}
		//收到服务器端发回的数据
		if (ucCh == ']' && (bool) strchr((const char *) F4G_Fram.Data, '['))
		{
			char *res = (char *) F4G_Fram.Data;
			F4G_Fram.serverStatuCnt = 0;
			while (*res != '[')
			{
				res++;
			}
			while (*res == '[')
				res++;

			F4G_Fram.base64Str = strtok(res, "]");
			base64_decode((const char *) F4G_Fram.base64Str,
					(unsigned char *) F4G_Fram.DeData);
			printf("after Decryption=%s\r\n", F4G_Fram.DeData);
			memset(F4G_Fram.Data, '\0', TCP_MAX_LEN);
			F4G_Fram.InfBit.Length = 0;
			F4G_Fram.InfBit.FinishFlag = 1;
		}
	}
	USART_ClearFlag(USART2, USART_FLAG_TC);
}
/**
 * 通过4G网络连接到服务器
 * @addr IP地址或域名
 * @port 端口
 * @return
 */
bool ConnectToServerBy4G(char* addr, char* port)
{
	char *p = mymalloc(100);
	sprintf(p, "AT+CIPSTART=\"TCP\",\"%s\",%s", addr, port);
	do
	{
		while (!Send_AT_Cmd(In4G, "AT+CIPSHUT", "SHUT OK", NULL, 500))
			;
		while (!Send_AT_Cmd(In4G, "AT+CREG?", "OK", NULL, 500))
			;
		while (!Send_AT_Cmd(In4G, "AT+CGATT?", "OK", NULL, 500))
			;
//		while (!Send_AT_Cmd(In4G, "AT+CIPMUX=0", "OK", NULL, 500))
//			; //单链接模式
//		while (!Send_AT_Cmd(In4G, "AT+CIPQSEND=1", "OK", NULL, 500))
//			; //快传模式
		//1.设置模式为TCP透传模式
		while (!Send_AT_Cmd(In4G, "AT+CIPMODE=1", "OK", NULL, 500))
			;
		if (TCP_Params.cops == '3')
		{
			while (!Send_AT_Cmd(In4G, "AT+CSTT=cmiot", "OK",
			NULL, 1800))
				;
		}
		else if (TCP_Params.cops == '6')
		{
			while (!Send_AT_Cmd(In4G, "AT+CSTT=UNIM2M.NJM2MAPN", "OK", NULL,
					1800))
				;
		}
		else if (TCP_Params.cops == '9')
		{
			while (!Send_AT_Cmd(In4G, "AT+CSTT=CTNET", "OK", NULL, 1800))
				;
		}
		while (!Send_AT_Cmd(In4G, "AT+CIICR", "OK", NULL, 500))
			;
		Send_AT_Cmd(In4G, "AT+CIFSR", "OK", NULL, 500);
	} while (!Send_AT_Cmd(In4G, p, "CONNECT", "ALREADY CONNECT", 1800));
	myfree(p);
	return 1;
}
/**
 * 4G模块退出透传模式
 */
void F4G_ExitUnvarnishSend(void)
{
	delay_ms(1000);
	F4G_USART("+++");
	delay_ms(500);
}
/***********************以下开始为与服务器通信业务代码部分*************************************/
/**
 * 获取模块的相关信息
 */
void getModuleMes(void)
{
	unsigned char *result = NULL;
	u8 inx = 0;
	strcpy(TCP_Params.locations[0], "0");
	strcpy(TCP_Params.locations[1], "0");
	//获取物联网卡号
	while (!Send_AT_Cmd(In4G, "AT+ICCID", "+ICCID:", NULL, 500))
		;
	result = F4G_Fram.Data;
	inx = 0;
	while (!(*result <= '9' && *result >= '0'))
	{
		result++;
	}
	//当值为字母和数字时
	while ((*result <= '9' && *result >= '0')
			|| (*result <= 'Z' && *result >= 'A')
			|| (*result <= 'z' && *result >= 'a'))
	{
		TCP_Params.ccid[inx++] = *result;
		result++;
	}
	printf("CCID=%s\r\n", TCP_Params.ccid);

	//获取模块网络信息
	while (!Send_AT_Cmd(In4G, "AT+COPS=0,1", "OK", NULL, 1000))
		;
	while (!Send_AT_Cmd(In4G, "AT+COPS?", "+COPS", NULL, 500))
		;
	if ((bool) strstr((const char *) F4G_Fram.Data, "CMCC"))
	{
		TCP_Params.cops = '3';
	}
	else if ((bool) strstr((const char *) F4G_Fram.Data, "UNICOM"))
	{
		TCP_Params.cops = '6';
	}
	else
	{
		TCP_Params.cops = '9';
	}
	printf("COPS is \"%c\"\r\n", TCP_Params.cops);
	//获取信号
	while (!Send_AT_Cmd(In4G, "AT+CSQ", "OK", NULL, 500))
		;
	result = F4G_Fram.Data;
	while (*result++ != ':')
		;
	result++;
	TCP_Params.rssi = atoi(strtok((char *) result, ","));
	printf("CSQ is %d\r\n", TCP_Params.rssi);
}
