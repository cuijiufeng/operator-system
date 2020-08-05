/**
 * 2020年07月30日 19/23/07
 * 内存相关
 */
#include	<type.h>
#include	<mm.h>

PUBLIC	void	initMemory(t_32 mem_size)
{
	if (mem_size > 0x3C00000)
	{
		mem_size = 0x3C00000;					//暂时不能超过60M内存，因为页表在0x90000
	}
	mem_size &= 0xFFFFF000;						//小于4k的内存忽略
	if (mem_size < 0x400000)
	{
		while (1);								//TODO 内存不能小于4MB，小于4MB暂时死循环吧，没想到好方法
	}
	t_32	main_mem_start = mem_size / 4;
	t_32	buf_mem_end = mem_size / 4-1;		//四分之一的内存用于缓冲内存
	t_32	main_mem_end = mem_size - 1;
	int i;
	for (i = 0; i < PAGES_SIZE; i++)			//初始化内存管理map
	{
		MEM_MAP[i] = -1;
		if (i <= (buf_mem_end >> 12))
		{
			MEM_MAP[i] = 100;
		} 
		else if (i <= (main_mem_end >> 12))
		{
			MEM_MAP[i] = 0;
		}
	}
}