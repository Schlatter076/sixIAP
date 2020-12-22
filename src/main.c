/* Includes */
#include <stddef.h>
#include "stm32f10x.h"
#include "SysTick.h"
#include "stdio.h"
#include "usart.h"
#include "usart_4G.h"
#include "STMFlash.h"
#include "common.h"
#include "iap.h"
#include "tim4.h"

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// �� RCC �����ʼ���ɸ�λ״̬������Ǳ����
	RCC_DeInit();

	//ʹ�� HSI
	RCC_HSICmd(ENABLE);

	// �ȴ� HSI ����
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

	// ֻ�� HSI ����֮�����������ִ��
	if (HSIStartUpStatus == RCC_CR_HSIRDY)
	{
		//-------------------------------------------------------------//

		// ʹ�� FLASH Ԥ��ȡ������
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		// SYSCLK �������������ʱ��ı������ã�����ͳһ���ó� 2
		// ���ó� 2 ��ʱ��SYSCLK ���� 48M Ҳ���Թ�����������ó� 0 ���� 1 ��ʱ��
		// ������õ� SYSCLK �����˷�Χ�Ļ���������Ӳ�����󣬳��������
		// 0��0 < SYSCLK <= 24M
		// 1��24< SYSCLK <= 48M
		// 2��48< SYSCLK <= 72M
		FLASH_SetLatency(FLASH_Latency_2);
		//------------------------------------------------------------//

		// AHB Ԥ��Ƶ��������Ϊ 1 ��Ƶ��HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// APB2 Ԥ��Ƶ��������Ϊ 1 ��Ƶ��PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// APB1 Ԥ��Ƶ��������Ϊ 1 ��Ƶ��PCLK1 = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//-----------���ø���Ƶ����Ҫ��������������-------------------//
		// ���� PLL ʱ����ԴΪ HSE������ PLL ��Ƶ����
		// PLLCLK = 4MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
		//-- -----------------------------------------------------//

		// ���� PLL
		RCC_PLLCmd(ENABLE);

		// �ȴ� PLL �ȶ�
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		// �� PLL �ȶ�֮�󣬰� PLL ʱ���л�Ϊϵͳʱ�� SYSCLK
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// ��ȡʱ���л�״̬λ��ȷ�� PLLCLK ��ѡΪϵͳʱ��
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
	else
	{
		// ��� HSI ����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
		// �� HSE ����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ��� HSI ����Ϊϵͳʱ�ӣ�
		// HSI ���ڲ��ĸ���ʱ�ӣ�8MHZ
		while (1)
		{
		}
	}
}

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void)
{
//	SystemInit();
	HSI_SetSysClock(RCC_PLLMul_9);
	SysTick_Init(36);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��S
	my_mem_init(); //�ڴ�����ʼ��
	IAP_WriteFlag(INIT_FLAG_DATA);
	IAP_Init();
	printf("Now In IAP\r\n");
	TIM4_Init(10, 36000 - 1); //10ms

	//����Ƿ���Ҫ���ظ���
	delay_ms(1000);
	printf("{localUpdate?}\r\n");
	delay_ms(1000);
	delay_ms(1000);

	F4G_Init(115200);

	/* Infinite loop */
	while (1)
	{
		switch (IAP_ReadFlag())
		{
		case APPRUN_FLAG_DATA:  //jump to app
			if (IAP_RunApp())
				IAP_WriteFlag(UPDATE_FLAG_DATA);
			break;
		case INIT_FLAG_DATA:  //initialze state (blank mcu)
			IAP_Main_Menu();
			break;
		case UPDATE_FLAG_DATA:  // download app state
			if (IAP_Update())
			{
				//IAP_WriteFlag(APPRUN_FLAG_DATA);
				NVIC_SystemReset();
			}
			else
				IAP_WriteFlag(INIT_FLAG_DATA);
			break;
		case UPLOAD_FLAG_DATA:  // upload app state
			IAP_Upload();
			IAP_WriteFlag(INIT_FLAG_DATA);
			break;
		case ERASE_FLAG_DATA:		// erase app state
			IAP_Erase();
			IAP_WriteFlag(INIT_FLAG_DATA);
			break;
		case SET_DeviceID_FLAG_DATA:		// set DeviceID
			Set_DeviceID();
			break;
		case GET_DeviceID_FLAG_DATA:		// read DeviceID
			Get_DeviceID();
			IAP_WriteFlag(INIT_FLAG_DATA);
			break;
		default:
			break;
		}
	}
}
