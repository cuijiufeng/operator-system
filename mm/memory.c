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

//寻找空闲的分页，找到返回分页的真实地址，失败返回0
PUBLIC	u_32	getFreePage()
{
	int i, j;
	for (i = 0; i < PAGES_SIZE; i++)
	{
		//寻找空闲的分页
		if (MEM_MAP[i] == 0)
		{
			MEM_MAP[i] = 1;		//设置内存已经被映射
			u_32 addr = i << 12;//内存面的地址
			//清空内存中的数据
			for (j = addr; j < (4096+addr); j+=4)
			{
				*((u_32*)j) = 0;
			}
			return addr;
		}
	}
	//如果已经没有空闲的分页返回0
	return 0;
}