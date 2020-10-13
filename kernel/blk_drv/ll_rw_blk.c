/**
 * 2020年09月28日 20/46/06
 *
 */
#include	<type.h>
#include	<fs.h>
#include	<blk_drv/blk.h>

REQUEST REQUEST_INFO[NR_REQUEST];
PROCESS* WAIT_FOR_REQUEST = NULL;
BLK_DEV BLK_DEV_INFO[NR_BLK_DEV];

void initBlkDev()
{
	int i;
	for (i = 0; i < NR_BLK_DEV; i++)
	{
		BLK_DEV_INFO[i].request_fn = NULL;
		BLK_DEV_INFO[i].current_request = NULL;
	}
	for (i = 0; i < NR_REQUEST; i++) 
	{
		REQUEST_INFO[i].dev = -1;
		REQUEST_INFO[i].next = NULL;
	}
}

//锁定缓冲块
PUBLIC void lockBuffer(BUFFER_HEAD* bh)
{
	//关中断
	disableInt();
	//如果在锁定状态则等待
	while (bh->b_lock)
		sleepOn(&bh->b_wait);
	//当等待线程全部执行完锁定缓冲块
	bh->b_lock = 1;
	//开中断
	enableInt();
}

//解锁缓冲块
PUBLIC void unlockBuffer(BUFFER_HEAD* bh)
{
	if (!bh->b_lock)
		printk("ll_rw_block.c: buffer not locked\n\r");
	bh->b_lock = 0;
	wakeUp(&bh->b_wait);
}

//添加request
PRIVATE void addRequest(BLK_DEV* dev, REQUEST* req)
{
	REQUEST* tmp;

	req->next = NULL;
	//关中断
	disableInt();
	if (req->bh)
		req->bh->b_dirt = 0;
	if (!(tmp = dev->current_request)) 
	{
		dev->current_request = req;
		enableInt();
		(dev->request_fn)();
		return;
	}
	//request链表下一结点
	for (; tmp->next; tmp = tmp->next)
		if ((IN_ORDER(tmp, req) || !IN_ORDER(tmp, tmp->next)) && IN_ORDER(req, tmp->next))
			break;
	req->next = tmp->next;
	tmp->next = req;
	//开中断
	enableInt();
}

//结束请求处理
PUBLIC void endRequest(int uptodate)
{
	REQUEST* curr = BLK_DEV_INFO[MAJOR_NR].current_request;
	if (curr->bh) {
		curr->bh->b_uptodate = uptodate;
		unlockBuffer(curr->bh);
	}
	if (!uptodate) 
	{
		printk("harddisk I/O error\n\r");
		printk("dev %04x, block %d\n\r", curr->dev, curr->bh->b_block_nr);
	}
	wakeUp(&curr->waiting);
	wakeUp(&WAIT_FOR_REQUEST);
	curr->dev = -1;
	curr = curr->next;
}

//读写块
PUBLIC	void ll_rw_block(int rw, BUFFER_HEAD* bh)
{
	u_32 major;

	if ((major = MAJOR(bh->b_dev)) >= NR_BLK_DEV || !(BLK_DEV_INFO[major].request_fn))
	{
		printk("Trying to read nonexistent block-device\n\r");
		return;
	}
	makeRequest(major, rw, bh);
}

//生成块请求
PRIVATE void makeRequest(int major, int rw, BUFFER_HEAD* bh)
{
	REQUEST* req;
	int rw_ahead;
	//判断操作
	if ((rw_ahead = (rw == READA || rw == WRITEA))) {
		if (bh->b_lock)
			return;
		if (rw == READA)
			rw = READ;
		else
			rw = WRITE;
	}
	//判断操作
	if (rw != READ && rw != WRITE)
	{
		printk("Bad block dev command, must be R/W/RA/WA");
		while (1);
	}
	lockBuffer(bh);
	//缓冲块已被修改则不能写，缓冲块已更新则脏读
	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodate)) {
		unlockBuffer(bh);
		return;
	}
repeat:
	if (rw == READ)
		req = REQUEST_INFO + NR_REQUEST;
	else
		req = REQUEST_INFO + ((NR_REQUEST * 2) / 3);
	//找空的request
	while (--req >= REQUEST_INFO)
		if (req->dev < 0)
			break;
	//如果没有找到
	if (req < REQUEST_INFO) 
	{
		//如果是不是READ
		if (rw_ahead) 
		{
			unlockBuffer(bh);
			return;
		}
		//睡眠进程
		sleepOn(&WAIT_FOR_REQUEST);
		goto repeat;
	}
	//填充request结构体数据
	req->dev = bh->b_dev;
	req->cmd = rw;
	req->errors = 0;
	req->sector = bh->b_block_nr << 1;
	req->nr_sectors = 2;
	req->buffer = bh->b_data;
	req->waiting = NULL;
	req->bh = bh;
	req->next = NULL;
	//填加request
	addRequest(major + BLK_DEV_INFO, req);
}