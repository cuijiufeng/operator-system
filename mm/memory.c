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
		if (i <= (KERNEL_MEM >> 12))
		{
			MEM_MAP[i] = -1;
		}
		else if (i <= (buf_mem_end >> 12))
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

//释放内存页
void freePage(u_32 addr)
{
	//如果是内核内存，不准释放
	if (addr < KERNEL_MEM)
	{
		return;
	}
	//如果超出最大内存，释放失败
	if (addr >= MEM_SIZE)
	{
		while (1);
	}
	addr >>= 12;
	//设置内存引用数减一
	if (MEM_MAP[addr]--)
	{
		return;
	}
	MEM_MAP[addr] = 0;
}

//复制分页数据，以达到让两个线性地址映射到同一段物理地址上去
PUBLIC	t_32	copyPageTables(u_32 dest_addr, u_32 src_addr, u_32 size)
{
	u_32 t_page;
	u_32* from_dir, *to_dir;
	u_32* from_page_tables, *to_page_tables;
	//因为一个一级页表项可以管理4MB内存，所以地址要4MB内存对齐，不然有些麻烦
	if((dest_addr&0x3fffff) || (src_addr& 0x3fffff))
	{
		return -1;
	}
	from_dir = (t_32*)((src_addr>>22)<<2);	//dest_addr/4MB为地址在一级页中的项索引，再*4为地址的一级页页项内存地址
	to_dir = (t_32*)((dest_addr>>22)<<2);
	size = ((size+ 0x3fffff)>>22);		//size+0x3ffffff是如果内存大小字节数不足4MB时也拷贝一个一级页页项
	//遍历一级页表项并复制
	for(; size-- > 0; from_dir++, to_dir++)
	{
		//如果to_dir页项已经存在的则失败
		if(PG_P & (*to_dir))
		{
			return -1;
		}
		//如果from_dir页项不存在,继续下一项
		if(!(PG_P & (*from_dir)))
		{
			continue;
		}
		from_page_tables = (t_32*)(0xfffff000 & *from_dir);	//一级页表项对应的二级页表的地址
		if (!(to_page_tables = (t_32*)getFreePage()))		//新分配一页内存用来拷贝二级页的内容
		{
			//如果没有可用的内存则不能拷贝二级页，所以失败
			return -1;
		}
		*to_dir = ((t_32)to_page_tables) | PG_P | PG_RWW | PG_USU;	//设置一级页表项值为新内存页的地址
		int cnt = (src_addr == 0) ? 0xA0 : 1024;			//如果被拷贝进程是0号，则只拷贝0xA0000以下的内存即可,(0xA0项*4KB=0xA0000,1024项*4KB=4MB)
		for (; cnt-- > 0; from_page_tables++, to_page_tables++)
		{
			t_page = *from_page_tables;						//二级页表项对应的内存的地址
			//如果二级页表项PG_P位为0则继续下一项
			if (!(t_page & PG_P))
			{
				continue;
			}
			t_page &= ~2;	//(~2=0xFD=1111 1101B),即设置页属性位PG_RWW为只读,在页写内存的时候会触发中断，然后真正的分配自己的内存(写时复制)
			*to_page_tables = t_page;						//设置二级页表项值为内存页的地址
			//如果此页不是内核内存
			if (t_page > KERNEL_MEM)
			{
				*from_page_tables = t_page;					//设置被拷贝页属性位PG_RWW只读
				t_page >>= 12;								//mem_map索引下标
				MEM_MAP[t_page]++;							//内存引用数加一
			}
		}
	}
	//重新设置cr3
	loadCR3(PAGE_DIR);
	//成功则返回0
	return 0;
}

//释放页表映射
PUBLIC	t_32	freePageTables(u_32 addr, u_32 size)
{
	u_32* dir;
	u_32* page_tables;
	//因为一个一级页表项可以管理4MB内存，所以地址要4MB内存对齐，不然有些麻烦
	if (addr & 0x3fffff)
	{
		return -1;
	}
	//0地址是内核空间，不准释放
	if (!addr)
	{
		return -1;
	}
	size = ((size + 0x3fffff) >> 22);		//size+0x3ffffff是如果内存大小字节数不足4MB时也拷贝一个一级页页项
	dir = (t_32*)((addr >> 22) << 2);
	//遍历一级页表项
	for (; size-- > 0; dir++)
	{
		//如果页已经是不存在的
		if (!(PG_P & *dir))
		{
			continue;
		}
		page_tables = (t_32*)(0xfffff000 & *dir);	//一级页表项对应的二级页表的地址
		int i = 1024;
		//遍历二级页表项
		for (; i-- > 0; page_tables++)
		{
			//如果页存在，则释放
			if (*page_tables & PG_P)
			{
				freePage(0xfffff000 & *page_tables);//释放页内存
			}
			*page_tables = 0;				//设置二级页表项为0
		}
		freePage(0xfffff000 & *dir);				//释放二级页内存
		*dir = 0;							//设置一级页表项为0
	}
	//重新设置cr3
	loadCR3(PAGE_DIR);
	//成功则返回0
	return 0;
}