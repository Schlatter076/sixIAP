/*
 * usart_4G.c
 *
 *  Created on: 2020��6��11��
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA3  RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��GPIOA3

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;	//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

	USART_ITConfig(USART2, USART_IT_RXNE | USART_IT_IDLE, ENABLE); //�������ڽ��ܺ����߿����ж�

	USART_Cmd(USART2, ENABLE);
}

void F4G_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	F4G_Fram.init = 1;

	USART2_Init(bound);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PKEY_4G | RST_4G;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	if (!localUpdate)
	{
		//����
		do
		{
			GPIO_SetBits(GPIOC, PKEY_4G);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			GPIO_ResetBits(GPIOC, PKEY_4G);
			//��λ4Gģ��
			GPIO_SetBits(GPIOC, RST_4G);
			delay_ms(1100);
			GPIO_ResetBits(GPIOC, RST_4G);
			delay_ms(500);

			F4G_ExitUnvarnishSend();
			Send_AT_Cmd(In4G, "AT+CIPCLOSE", "OK", NULL, 500);
			Send_AT_Cmd(In4G, "AT+RSTSET", "OK", NULL, 500);
		} while (!AT_Test(In4G));

		//���������Ϣ
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
		//�յ��������˷��ص�����
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
 * ͨ��4G�������ӵ�������
 * @addr IP��ַ������
 * @port �˿�
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
//			; //������ģʽ
//		while (!Send_AT_Cmd(In4G, "AT+CIPQSEND=1", "OK", NULL, 500))
//			; //�촫ģʽ
		//1.����ģʽΪTCP͸��ģʽ
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
 * 4Gģ���˳�͸��ģʽ
 */
void F4G_ExitUnvarnishSend(void)
{
	delay_ms(1000);
	F4G_USART("+++");
	delay_ms(500);
}
/***********************���¿�ʼΪ�������ͨ��ҵ����벿��*************************************/
/**
 * ��ȡģ��������Ϣ
 */
void getModuleMes(void)
{
	unsigned char *result = NULL;
	u8 inx = 0;
	strcpy(TCP_Params.locations[0], "0");
	strcpy(TCP_Params.locations[1], "0");
	//��ȡ����������
	while (!Send_AT_Cmd(In4G, "AT+ICCID", "+ICCID:", NULL, 500))
		;
	result = F4G_Fram.Data;
	inx = 0;
	while (!(*result <= '9' && *result >= '0'))
	{
		result++;
	}
	//��ֵΪ��ĸ������ʱ
	while ((*result <= '9' && *result >= '0')
			|| (*result <= 'Z' && *result >= 'A')
			|| (*result <= 'z' && *result >= 'a'))
	{
		TCP_Params.ccid[inx++] = *result;
		result++;
	}
	printf("CCID=%s\r\n", TCP_Params.ccid);

	//��ȡģ��������Ϣ
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
	//��ȡ�ź�
	while (!Send_AT_Cmd(In4G, "AT+CSQ", "OK", NULL, 500))
		;
	result = F4G_Fram.Data;
	while (*result++ != ':')
		;
	result++;
	TCP_Params.rssi = atoi(strtok((char *) result, ","));
	printf("CSQ is %d\r\n", TCP_Params.rssi);
}
