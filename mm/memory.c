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
		mem_size = 0x3C00000;					//暂时不能超过60M内存
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

//复制分页数据，以达到让两个线性地址映射到同一段物理地址上去
PUBLIC	t_32	copyPageTables(u_32 dest_addr, u_32 src_addr, u_32 size)
{
	u_32 from_dir, to_dir;
	//因为一个一级页表项可以管理4MB内存，所以地址要4MB内存对齐，不然有些麻烦
	if((dest_addr&0x3ffffff) || (src_addr&0x3ffffff))
	{
		return -1;
	}
	from_dir = ((dest_addr>>22)<<2);	//dest_addr/4MB为地址在一级页中的项索引，再*4为地址的一级页页项内存地址
	to_dir = ((src_addr>>22)<<2);
	size = ((size+0x3ffffff)>>22);		//size+0x3ffffff是如果内存大小字节数不足4MB时也拷贝一个一级页页项
	//遍历一级页表项并复制
	for(; size-- > 0; from_dir+=4, to_dir+=4)
	{
		//如果to_dir页项已经是存在的则失败
		if(PG_P & *((u_32*)to_dir))
		{
			return -1;
		}
		//如果from_dir页项不存在,继续下一项
		if(!(PG_P & *((u_32*)from_dir)))
		{
			continue;
		}
	}
}