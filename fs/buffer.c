/**
 * 2020年09月13日 13/26/46
 * 缓冲区操作
 */
#include	<type.h>
#include	<fs.h>

u_32	NR_BUFFERS = 0; 
//缓冲区空闲队列
PRIVATE	BUFFER_HEAD* BUFFER_FREE_LIST;
//缓冲区进程等待队列
PRIVATE	PROCESS*	BUFFER_WAIT;
BUFFER_HEAD* BUFFER_HASH_TABLE[NR_HASH];

PUBLIC	void	initBuffer(t_32 mem_size)
{
	int i;
	if (mem_size > 0x3C00000)
	{
		mem_size = 0x3C00000;					//暂时不能超过60M内存
	}
	mem_size &= 0xFFFFF000;						//小于4k的内存忽略
	if (mem_size < 0x400000)
	{
		while (1);								//TODO 内存不能小于4MB，小于4MB暂时死循环吧，没想到好方法
	}
	t_32	buf_mem_end = mem_size / 4;			//四分之一的内存用于缓冲内存
	void*	b = (void *)buf_mem_end;
	BUFFER_HEAD* h = (BUFFER_HEAD*)START_BUFFER_ADDR;

	//初始化缓冲区管理结构(h+1是为了防止覆盖缓冲区管理结构的空间)
	while ((b -= BLOCK_SIZE) >= (void*)(h+1)) 
	{
		h->b_dev = 0;
		h->b_dirt = 0;
		h->b_count = 0;
		h->b_lock = 0;
		h->b_uptodate = 0;
		h->b_wait = NULL;
		h->b_next = NULL;
		h->b_prev = NULL;
		h->b_data = (char*)b;
		h->b_prev_free = h - 1;
		h->b_next_free = h + 1;
		h++;
		NR_BUFFERS++;		//缓冲区块数加一
	}
	BUFFER_WAIT = NULL;
	h--;					//h指向最后一块
	BUFFER_FREE_LIST = (BUFFER_HEAD*)START_BUFFER_ADDR;
	BUFFER_FREE_LIST->b_prev_free = h;	//第一块的前一块指向最后一块，双向循环链表
	h->b_next_free = BUFFER_FREE_LIST;	//最后一块的后一块指向第一块，双向循环链表
	for (i = 0; i < NR_HASH; i++)		//初始化hash数组
	{
		BUFFER_HASH_TABLE[i] = NULL;
	}
}

//从设备上读取数据块，如果是失败返回NULL
PUBLIC	BUFFER_HEAD*	bread(int dev, int block)
{
	BUFFER_HEAD* bh;

	//取设备号指定的缓冲区，
	if (!(bh = getblk(dev, block)))
	{
		printk("bread: getblk returned NULL\n");
		while (1);
	}
	//如果缓冲块更新标志存在
	if (bh->b_uptodate)
	{
		return bh;
	}
	ll_rw_block(READ, bh);
	waitOnBuffer(bh);
	if (bh->b_uptodate)
	{
		return bh;
	}		
	brelse(bh);
	return NULL;
}

//释放缓冲块
PUBLIC	void brelse(BUFFER_HEAD* buf)
{
	//如果缓冲块为空
	if (!buf)
	{
		return;
	}
	//等待缓冲块
	waitOnBuffer(buf);
	//如果缓冲块没有引用
	if (!(buf->b_count--))
	{
		printk("Trying to free free buffer");
		while (1);
	}
	//唤醒等待此缓冲块的进程
	wakeUp(&BUFFER_WAIT);
}

//取高速缓冲区中指定的缓冲块
PUBLIC	BUFFER_HEAD* getblk(int dev, int block)
{
	BUFFER_HEAD* tmp, *bh;
	
repeat:
	//取到指定的缓冲块
	if ((bh = getHashTable(dev, block)))
	{
		return bh;
	}
	//如果没有取到指定的缓冲块，则从空闲链表中寻找缓冲块
	tmp = BUFFER_FREE_LIST;
	//寻找空闲数据块链表
	do 
	{
		//如果有引用则非空闲，寻找下一块
		if (tmp->b_count)
		{
			continue;
		}
		//b_count引用等于0不一定说明缓冲块没有人引用了
		//当bh==0没有取到指定的缓冲块时执行，当tmp缓冲块的权重值小于bh缓冲块的权重值时执行
		if (!bh || BADNESS(tmp) < BADNESS(bh)) 
		{
			bh = tmp;			//当前最合适的取的缓冲块
			//如果此缓冲块的权重为0,说明此缓冲块没有修改标志和锁定标志，可用
			if (!BADNESS(tmp))
			{
				break;
			}
		}
	} while ((tmp = tmp->b_next_free) != BUFFER_FREE_LIST);	//遍历空闲数据块链表
	//如果空闲链表遍历完还是没有找到合适的缓冲块
	if (!bh) 
	{
		//睡眠当前进程，如果有等待缓冲区的进程则设置可被调度,醒来时继续找缓冲块
		sleepOn(&BUFFER_WAIT);
		goto repeat;
	}
	//如果空闲队列存在可用的等待缓冲块
	waitOnBuffer(bh);
	//如果引用不为0，重新寻找缓冲块
	if (bh->b_count)
	{
		goto repeat;
	}
	//如果缓冲块修改标志
	while (bh->b_dirt) 
	{
		syncDev(bh->b_dev);
		waitOnBuffer(bh);
		//引用不为0
		if (bh->b_count)
		{
			goto repeat;
		}
	}
	//确定此块没有被占用
	if (findBuffer(dev, block))
	{
		goto repeat;
	}
	//找到缓冲块
	bh->b_count = 1;
	bh->b_dirt = 0;
	bh->b_uptodate = 0;
	removeFromQueues(bh);
	bh->b_dev = dev;
	bh->b_block_nr = block;
	insertIntoQueues(bh);
	return bh;
}

//利用hash表在高速缓冲区中寻找指定的缓冲块。若找到则对该缓冲块上锁并返回其地址
PUBLIC	BUFFER_HEAD* getHashTable(int dev, int block)
{
	BUFFER_HEAD* bh;

	for (;;) 
	{
		//如果寻找失败返回NULL
		if (!(bh = findBuffer(dev, block)))
		{
			return NULL;
		}
		bh->b_count++;
		waitOnBuffer(bh);
		//返回缓冲块地址
		if (bh->b_dev == dev && bh->b_block_nr == block)
		{
			return bh;
		}
		bh->b_count--;
	}
}

//利用hash表在高速缓冲区中寻找指定设备号和指定块号的缓冲块
//找到返回其指针，否则返回NULL
PRIVATE BUFFER_HEAD* findBuffer(int dev, int block)
{
	BUFFER_HEAD* tmp;
	//搜索hash表
	for (tmp = HASH(dev, block); tmp != NULL; tmp = tmp->b_next)
	{
		//指定设备号和指定块号的缓冲块
		if (tmp->b_dev == dev && tmp->b_block_nr == block)
		{
			return tmp;
		}
	}
	return NULL;
}

//从缓冲区队列删除缓冲块
PRIVATE void removeFromQueues(BUFFER_HEAD* bh)
{
	if (bh->b_next)
		bh->b_next->b_prev = bh->b_prev;
	if (bh->b_prev)
		bh->b_prev->b_next = bh->b_next;
	if (HASH(bh->b_dev, bh->b_block_nr) == bh)
		HASH(bh->b_dev, bh->b_block_nr) = bh->b_next;
	if (!(bh->b_prev_free) || !(bh->b_next_free))
	{
		printk("Free block list corrupted");
		while (1);
	}
	bh->b_prev_free->b_next_free = bh->b_next_free;
	bh->b_next_free->b_prev_free = bh->b_prev_free;
	if (BUFFER_FREE_LIST == bh)
		BUFFER_FREE_LIST = bh->b_next_free;
}

PRIVATE void insertIntoQueues(BUFFER_HEAD* bh)
{
	bh->b_next_free = BUFFER_FREE_LIST;
	bh->b_prev_free = BUFFER_FREE_LIST->b_prev_free;
	BUFFER_FREE_LIST->b_prev_free->b_next_free = bh;
	BUFFER_FREE_LIST->b_prev_free = bh;
	bh->b_prev = NULL;
	bh->b_next = NULL;
	if (!bh->b_dev)
		return;
	bh->b_next = HASH(bh->b_dev, bh->b_block_nr);
	HASH(bh->b_dev, bh->b_block_nr) = bh;
	bh->b_next->b_prev = bh;
}

//等待缓冲块解锁
PRIVATE void waitOnBuffer(BUFFER_HEAD* bh)
{
	//关中断
	disableInt();
	//循环等待缓冲块解锁
	while (bh->b_lock)
	{
		sleepOn(&bh->b_wait);
	}
	//开中断
	enableInt();
}

//设备与缓冲块数据同步
PUBLIC	int syncDev(int dev)
{
	int i;
	BUFFER_HEAD* bh = (BUFFER_HEAD*)START_BUFFER_ADDR;

	//遍历缓冲块
	for (i = 0; i < NR_BUFFERS; i++, bh++)
	{
		//如果设备号不相同
		if (bh->b_dev != dev)
		{
			continue;
		}
		//等待缓冲块
		waitOnBuffer(bh);
		//再次检查设备号与修改标志
		if (bh->b_dev == dev && bh->b_dirt)
		{
			ll_rw_block(WRITE, bh);
		}
	}
	//将i节点中的数据写入高速缓冲区
	syncInodes();
	bh = (BUFFER_HEAD*)START_BUFFER_ADDR;
	for (i = 0; i < NR_BUFFERS; i++, bh++) 
	{
		//如果设备号不相同
		if (bh->b_dev != dev)
		{
			continue;
		}
		//等待缓冲块
		waitOnBuffer(bh);
		//
		if (bh->b_dev == dev && bh->b_dirt)
		{
			ll_rw_block(WRITE, bh);
		}
	}
	return 0;
}