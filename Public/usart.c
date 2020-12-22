#include "usart.h"
#include "stdio.h"
#include "usart_4G.h"
#include "string.h"
#include "base64.h"

struct USART_Fram USART1_Record_Struct =
{ 0 };

volatile u8 localUpdate = 0;

//*/
/*******************************************************************************
 * �� �� ��         : USART1_Init
 * ��������		   : USART1��ʼ������
 * ��    ��         : bound:������
 * ��    ��         : ��
 *******************************************************************************/
void USART1_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	    //RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA, &GPIO_InitStructure); /* ��ʼ��GPIO */

	//USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;		  //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		 //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		  //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;		  //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;		  //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1

	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1

	USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

}
/*******************************************************************************
 * �� �� ��         : USART1_IRQHandler
 * ��������		   : USART1�жϺ���
 * ��    ��         : ��
 * ��    ��         : ��
 *******************************************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 r;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r = USART_ReceiveData(USART1);  //(USART1->DR);	//��ȡ���յ�������
		if (USART1_Record_Struct.InfBit.FramLength >= RX_BUF_MAX_LEN)
		{
			printf("Cmd size over.\r\n");
			memset(USART1_Record_Struct.RX_BUF, 0, RX_BUF_MAX_LEN);
			USART1_Record_Struct.InfBit.FramLength = 0;
		}
		if (USART1_Record_Struct.InfBit.FramFinishFlag == 0) //δ������
		{
			USART1_Record_Struct.RX_BUF[USART1_Record_Struct.InfBit.FramLength++] =
					r;
		}
		if (r == '$')
		{
			localUpdate = 1;
			USART1_Record_Struct.InfBit.FramLength = 0;
		}
		if (r == ']' && (bool) strchr(USART1_Record_Struct.RX_BUF, '['))
		{
			char *res = USART1_Record_Struct.RX_BUF;
			while (*res != '[')
			{
				res++;
			}
			res++;

			memcpy(USART1_Record_Struct.DATA, strtok(res, "]"), 512);

			USART1_Record_Struct.InfBit.FramLength = 0;
			USART1_Record_Struct.InfBit.FramFinishFlag = 1;
		}
	}
	USART_ClearFlag(USART1, USART_FLAG_TC);
}
/**
 * ��strͨ��delims���зָ�,���õ��ַ��������res��
 * @str ��ת���������ַ���
 * @delims �ָ���
 */
void UT_split(char str[], char *delims)
{
	char *result = str;
	u8 inx = 0;
	while (inx < 2)
	{
		result++;
		if (*result == ',')
		{
			++inx;
		}
	}
	result++;
	memcpy(USART1_Record_Struct.ServerData, result, 512);
	//printf("comd2=%s\r\n", F4G_Fram_Record_Struct.ServerData);
	result = strtok(str, delims);
	USART1_Record_Struct.Server_Command[0] = result;
	result = strtok( NULL, delims);
	USART1_Record_Struct.Server_Command[1] = result;
}
