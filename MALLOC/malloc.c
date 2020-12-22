/*
 * malloc.c
 *
 *  Created on: 2020��10��19��
 *      Author: loyer
 */
#include "malloc.h"

//�ڴ��(32�ֽڶ���)
u8 mem1base[MEM1_MAX_SIZE] __attribute__((aligned(32))); //�ڲ�SRAM�ڴ��
//�ڴ�����
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];							//�ڲ�SRAM�ڴ��MAP
//�ڴ�������
const u32 memtblsize = MEM1_ALLOC_TABLE_SIZE;	//�ڴ���С
const u32 memblksize = MEM1_BLOCK_SIZE;			//�ڴ�ֿ��С
const u32 memsize = MEM1_MAX_SIZE;				//�ڴ��ܴ�С
//�ڴ���������
struct malloc_cortol_struct malloc_cortol =
{ my_mem_init,						//�ڴ��ʼ��
		my_mem_perused,						//�ڴ�ʹ����
		mem1base,			//�ڴ��
		mem1mapbase,			//�ڴ����״̬��
		0,  		 					//�ڴ����δ����
		};

//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//len:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void my_mem_cpy(void *des, void *src, u32 len)
{
	u8 *xdes = des;
	u8 *xsrc = src;
	while (len--)
		*xdes++ = *xsrc++;
}

//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//num:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void my_mem_set(void *s, u8 c, u32 num)
{
	u8 *xs = s;
	while (num--)
		*xs++ = c;
}

//�ڴ�����ʼ��
//memx:�����ڴ��
void my_mem_init(void)
{
	my_mem_set(malloc_cortol.memmap, 0, memtblsize * 2); //�ڴ�״̬����������
	my_mem_set(malloc_cortol.membase, 0, memsize);	//�ڴ��������������
	malloc_cortol.memrdy = 1;								//�ڴ�����ʼ��OK
}

//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(0~100)
u8 my_mem_perused(void)
{
	u32 used = 0;
	u32 i;
	for (i = 0; i < memtblsize; i++)
	{
		if (malloc_cortol.memmap[i])
			used++;
	}
	return (used * 100) / (memtblsize);
}

//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
u32 my_mem_malloc(u32 size)
{
	signed long offset = 0;
	u32 xmemb;	//��Ҫ���ڴ����
	u32 kmemb = 0;	//�������ڴ����
	u32 i;
	if (!malloc_cortol.memrdy)
		malloc_cortol.init();	//δ��ʼ��,��ִ�г�ʼ��
	if (size == 0)
		return 0XFFFFFFFF;	//����Ҫ����
	xmemb = size / memblksize;  	//��ȡ��Ҫ����������ڴ����
	if (size % memblksize)
		xmemb++;
	for (offset = memtblsize - 1; offset >= 0; offset--)  //���������ڴ������
	{
		if (!malloc_cortol.memmap[offset])
			kmemb++;  	//�������ڴ��������
		else
			kmemb = 0;								//�����ڴ������
		if (kmemb == xmemb)							//�ҵ�������nmemb�����ڴ��
		{
			for (i = 0; i < xmemb; i++)  					//��ע�ڴ��ǿ�
			{
				malloc_cortol.memmap[offset + i] = xmemb;
			}
			return (offset * memblksize);  					//����ƫ�Ƶ�ַ
		}
	}
	return 0XFFFFFFFF;  					//δ�ҵ����Ϸ����������ڴ��
}

//�ͷ��ڴ�(�ڲ�����)
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;
u8 my_mem_free(u32 offset)
{
	int i;
	if (!malloc_cortol.memrdy)  					//δ��ʼ��,��ִ�г�ʼ��
	{
		malloc_cortol.init();
		return 1;  					//δ��ʼ��
	}
	if (offset < memsize)  					//ƫ�����ڴ����.
	{
		int index = offset / memblksize;			//ƫ�������ڴ�����
		int nmemb = malloc_cortol.memmap[index];	//�ڴ������
		for (i = 0; i < nmemb; i++)  						//�ڴ������
		{
			malloc_cortol.memmap[index + i] = 0;
		}
		return 0;
	}
	else
		return 2;  						//ƫ�Ƴ�����.
}

//�ͷ��ڴ�(�ⲿ����)
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ
void myfree(void *paddr)
{
	u32 offset;
	if (paddr == NULL)
		return;  						//��ַΪ0.
	offset = (u32) paddr - (u32) malloc_cortol.membase;
	my_mem_free(offset);	//�ͷ��ڴ�
}

//�����ڴ�(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(u32 size)
{
	u32 offset;
	offset = my_mem_malloc(size);
	if (offset == 0XFFFFFFFF)
		return NULL;
	else
		return (void*) ((u32) malloc_cortol.membase + offset);
}

//���·����ڴ�(�ⲿ����)
//memx:�����ڴ��
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(void *paddr, u32 size)
{
	u32 offset;
	offset = my_mem_malloc(size);
	if (offset == 0XFFFFFFFF)
		return NULL;
	else
	{
		my_mem_cpy((void*) ((u32) malloc_cortol.membase + offset), paddr, size);//�������ڴ����ݵ����ڴ�
		myfree(paddr);  											 //�ͷž��ڴ�
		return (void*) ((u32) malloc_cortol.membase + offset);  //�������ڴ��׵�ַ
	}
}
