/*
 * tim4.c
 *
 *  Created on: 2020��7��20��
 *      Author: loyer
 */
#include "tim4.h"

u16 updateCnt = 0;
u16 initCnt = 0;

void Init_led(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(GPIOA, &GPIO_InitStructure); /* ��ʼ��GPIO */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//�ر�JTAG������SWD���ͷ�PB3 PB4 PA15
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}


/*******************************************************************************
 * �� �� ��         : TIM4_Init
 * ��������		   : TIM4��ʼ������
 * ��    ��         : per:��װ��ֵ
 psc:��Ƶϵ��
 * ��    ��         : ��
 *******************************************************************************/
void TIM4_Init(u16 per, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	Init_led();
	TIM_DeInit(TIM4);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ��TIM4ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = per;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4, ENABLE); //ʹ�ܶ�ʱ��
}

/*******************************************************************************
 * �� �� ��         : TIM4_IRQHandler
 * ��������		   : TIM4�жϺ���
 * ��    ��         : ��
 * ��    ��         : ��
 *******************************************************************************/
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update))
	{
		if (updating)
		{
			updateCnt++;
			if (updateCnt == 20)
			{
				updateCnt = 0;
				LED_Status ^= 1;
			}
		}
		if (F4G_Fram.init)
		{
			initCnt++;
			if (initCnt == 50)
			{
				initCnt = 0;
				LED_Status ^= 1;
			}
		}
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

