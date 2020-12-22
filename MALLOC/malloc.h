/*
 * malloc.h
 *
 *  Created on: 2020��10��19��
 *      Author: loyer
 */

#ifndef MALLOC_H_
#define MALLOC_H_

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

//����2���ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��

//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			32*1024  						//�������ڴ� 32K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С

//�ڴ���������   ����ָ��
struct malloc_cortol_struct
{
	void (*init)(void);					//��ʼ��
	u8 (*perused)(void);		  	    	//�ڴ�ʹ����
	u8 *membase;				//�ڴ�� ����SRAMBANK��������ڴ�
	u16 *memmap; 				//�ڴ����״̬��
	u8 memrdy; 				//�ڴ�����Ƿ����
};
extern struct malloc_cortol_struct malloc_cortol;	 //��mallco.c���涨��

void my_mem_set(void *s, u8 c, u32 num);	//�����ڴ�
void my_mem_cpy(void *des, void *src, u32 len);	//�����ڴ�
void my_mem_init(void);				//�ڴ�����ʼ������(��/�ڲ�����)
u32 my_mem_malloc(u32 size);	//�ڴ����(�ڲ�����)
u8 my_mem_free(u32 offset);		//�ڴ��ͷ�(�ڲ�����)
u8 my_mem_perused(void);				//����ڴ�ʹ����(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *paddr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(u32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(void *paddr, u32 size);			//���·����ڴ�(�ⲿ����)

#endif /* MALLOC_H_ */
