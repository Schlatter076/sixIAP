/*
 * malloc.h
 *
 *  Created on: 2020年10月19日
 *      Author: loyer
 */

#ifndef MALLOC_H_
#define MALLOC_H_

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

//定义2个内存池
#define SRAMIN	 0		//内部内存池

//mem1内存参数设定.mem1完全处于内部SRAM里面.
#define MEM1_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM1_MAX_SIZE			32*1024  						//最大管理内存 32K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小

//内存管理控制器   函数指针
struct malloc_cortol_struct
{
	void (*init)(void);					//初始化
	u8 (*perused)(void);		  	    	//内存使用率
	u8 *membase;				//内存池 管理SRAMBANK个区域的内存
	u16 *memmap; 				//内存管理状态表
	u8 memrdy; 				//内存管理是否就绪
};
extern struct malloc_cortol_struct malloc_cortol;	 //在mallco.c里面定义

void my_mem_set(void *s, u8 c, u32 num);	//设置内存
void my_mem_cpy(void *des, void *src, u32 len);	//复制内存
void my_mem_init(void);				//内存管理初始化函数(外/内部调用)
u32 my_mem_malloc(u32 size);	//内存分配(内部调用)
u8 my_mem_free(u32 offset);		//内存释放(内部调用)
u8 my_mem_perused(void);				//获得内存使用率(外/内部调用)
////////////////////////////////////////////////////////////////////////////////
//用户调用函数
void myfree(void *paddr);  			//内存释放(外部调用)
void *mymalloc(u32 size);			//内存分配(外部调用)
void *myrealloc(void *paddr, u32 size);			//重新分配内存(外部调用)

#endif /* MALLOC_H_ */
