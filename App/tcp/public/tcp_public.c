/*
 * tcp_public.c
 *
 *  Created on: 2020年10月24日
 *      Author: loyer
 */
#include "tcp_public.h"

struct STRUCT_USART_Params TCP_Params =
{ 0 };
struct STRUCT_USART_Fram F4G_Fram =
{ 0 };

static char *myitoa(int value, char *string, int radix)
{
	int i, d;
	int flag = 0;
	char *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;

	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char) (d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */

void _USART_printf(USART_TypeDef * USARTx, char * Data, ...)
{
	const char *s;
	int d;
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while (*Data != 0)     // 判断是否到达字符串结束符
	{
		if (*Data == 0x5c)  //'\'
		{
			switch (*++Data)
			{
			case 'r':							          //回车符
				USART_SendData(USARTx, 0x0d);
				Data++;
				break;

			case 'n':							          //换行符
				USART_SendData(USARTx, 0x0a);
				Data++;
				break;

			default:
				Data++;
				break;
			}
		}

		else if (*Data == '%')
		{									  //
			switch (*++Data)
			{
			case 's':										  //字符串
				s = va_arg(ap, const char *);

				for (; *s; s++)
				{
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
						;
				}

				Data++;

				break;

			case 'd':
				//十进制
				d = va_arg(ap, int);

				myitoa(d, buf, 10);

				for (s = buf; *s; s++)
				{
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
						;
				}

				Data++;

				break;

			default:
				Data++;

				break;

			}
		}

		else
			USART_SendData(USARTx, *Data++);

		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;

	}
}

/**
 * 对模块发送AT指令
 * @cmd：待发送的指令
 * @ack1，@ack2：期待的响应，为NULL表不需响应，两者为或逻辑关系
 * @time：等待响应的时间
 * @return 1：发送成功 0：失败
 */
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time)
{
	struct STRUCT_USART_Fram *USART_Fram;
	USART_TypeDef *USARTx;

	if (internet == In4G)
	{
		USART_Fram = &F4G_Fram;
		USARTx = USART2;
	}

	//USART_Fram->IsNotInAT = 0; //AT指令进行中

	USART_Fram->InfBit.Length = 0;	//从新开始接收新的数据包
	_USART_printf(USARTx, "%s\r\n", cmd);
	if (ack1 == 0 && ack2 == 0)	 //不需要接收数据
	{
		return true;
	}
	delay_ms(time);	  //延时time时间

	USART_Fram->Data[USART_Fram->InfBit.Length] = '\0';

	printf("%s", USART_Fram->Data);

	if (ack1 != 0 && ack2 != 0)
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack1)
				|| ( bool ) strstr((const char *) USART_Fram->Data, ack2));
	}
	else if (ack1 != 0)
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack1));
	}
	else
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack2));
	}
}

bool AT_Test(ENUM_Internet_TypeDef internet)
{
	u8 count = 0;
	char *module = mymalloc(10);
	if (internet == In4G)
	{
		sprintf(module, "%s", "4G module");
	}
	else
	{
		sprintf(module, "%s", "WIFI module");
	}
	while (count++ < 8)
	{
		Send_AT_Cmd(internet, "AT", "OK", NULL, 500);
	}
	if (Send_AT_Cmd(internet, "AT", "OK", NULL, 500))
	{
		printf("test %s success!\r\n", module);
		myfree(module);
		return 1;
	}
	printf("test %s fail!\r\n", module);
	myfree(module);
	return 0;
}

/**
 * 将str通过delims进行分割,所得的字符串填充在res中
 * @fram
 * @delims 分隔符
 */
void mySplit(struct STRUCT_USART_Fram *fram, char *delims)
{
	char *result = (char *) fram->DeData;
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
	memcpy(fram->ServerData, result, BASE64_BUF_LEN);
	result = strtok((char *) fram->DeData, delims);
	fram->Server_Command[0] = (unsigned char *) result;
	result = strtok( NULL, delims);
	fram->Server_Command[1] = (unsigned char *) result;
}

/**
 * 透传模式下发送字符串
 * @USARTx 串口
 * @str 待发送的字符串
 */
void TCP_sendStr(USART_TypeDef* USARTx, char *str)
{
	u8 i = 0;
	char *cmd = mymalloc(200);
	char *base64Str = mymalloc(200);
	//printf("%s\r\n", str);
	base64_encode((const unsigned char *) str, base64Str);
	snprintf(cmd, 200, "{(%s}", base64Str);
	while (cmd[i] != '}')
	{
		USART_SendData(USARTx, cmd[i]);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;
		i++;
	}
	USART_SendData(USARTx, cmd[i]);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
		;
	myfree(base64Str);
	myfree(cmd);
}

/**
 * 获取注册字符串
 * @strBuf 待获取的字符串
 * @len 字符串长度
 * @upCMD 上行控制字
 * @params 通信模块的一些参数
 * @modulType 所使用的的模块类型
 * @version 固件版本号
 * @num 几口机型
 * 无返回
 */
void getRegisterStr(char *strBuf, int len, const char* upCMD,
		struct STRUCT_USART_Params *params, char moduleType, char *version,
		char *num)
{
	const char* template = "%s,%s,%s-%c-%c-%s-%s_%s-%s";
	ReadDeviceID();
	snprintf(strBuf, len, template, RDeviceID, upCMD, params->ccid,
			params->cops, moduleType, version, params->locations[0],
			params->locations[1], num);
}

/**
 * 获取不带参数的请求字符串
 * @strBuf
 * @len
 * @upCMD 上行控制字
 * 无返回
 */
void getRequestStrWithoutParam(char *strBuf, int len, const char* upCMD)
{
	const char* template = "%s,%s";
	ReadDeviceID();
	snprintf(strBuf, len, template, RDeviceID, upCMD);
}
/**
 * 请求注册
 */
void request4Register(USART_TypeDef* USARTx)
{
	char *buf = mymalloc(100);
	ReadVersion();
	getRegisterStr(buf, 100, "00", &TCP_Params, '2', RVersion, "06");
	TCP_sendStr(USARTx, buf);
	myfree(buf);
}
