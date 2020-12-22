#ifndef __IAP_H__
#define __IAP_H__
#include "stm32f10x.h"
#include "STMFlash.h"
#include "malloc.h"

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

extern volatile u8 updating;

void request(char *cmd, u8 choose);
void IAP_USART_Init(void);
extern void IAP_Init(void);
extern uint16_t IAP_ReadFlag(void);
extern void IAP_WriteFlag(uint16_t flag);
extern u8 IAP_RunApp(void);
extern void IAP_Main_Menu(void);
extern int8_t IAP_Update(void);
extern int8_t IAP_Upload(void);
extern int8_t IAP_Erase(void);
u8 Set_DeviceID(void);
void Get_DeviceID(void);
u8 check_response(char *str);

void catReqStr4UPdate(char buf[], int len, char *cmd, u16 num);
void request4UPdate(char *cmd, u16 num, u8 choose);
void request4UPdataComplete(u8 choose);
#endif
