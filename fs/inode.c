/**
 * 2020年09月28日 21/18/07
 *
 */
#include	<type.h>
#include	<fs.h>

MINODE INODE_TABLE[NR_INODE] = { {0,}, };

PUBLIC	void syncInodes()
{
	int i;
	MINODE* inode;

	inode = 0 + INODE_TABLE;
	//遍历所有inode节点
	for (i = 0; i < NR_INODE; i++, inode++) 
	{
		//等待i节点解锁
		waitOnInode(inode);
		//如果i节点修改标志真且管道标志假
		if (inode->i_dirt && !inode->i_pipe)
		{
			//写i节点
			writeInode(inode);
		}
	}
}

//锁定结点
PRIVATE void lockInode(MINODE* inode)
{
	//关中断
	disableInt();
	while (inode->i_lock)
	{
		sleepOn(&inode->i_wait);
	}
	inode->i_lock = 1;
	//开中断
	enableInt();
}

//解锁结点
PRIVATE void unlockInode(MINODE* inode)
{
	inode->i_lock = 0;
	wakeUp(&inode->i_wait);
}

//写结点
PRIVATE void writeInode(MINODE* inode)
{
	SUPER_BLOCK* sb;
	BUFFER_HEAD* bh;
	int block;

	lockInode(inode);
	if (!inode->i_dirt || !inode->i_dev) 
	{
		unlockInode(inode);
		return;
	}
	if (!(sb = getSuper(inode->i_dev)))
	{
		printk("trying to write inode without device");
		while (1);
	}
	block = 2 + sb->s_imap_blocks + sb->s_zmap_blocks + (inode->i_num - 1) / INODES_PER_BLOCK;
	if (!(bh = bread(inode->i_dev, block)))
	{
		printk("unable to read i-node block");
		while (1);
	}
	((DINODE*)bh->b_data)[(inode->i_num - 1) % INODES_PER_BLOCK] = *(DINODE*)inode;
	bh->b_dirt = 1;
	inode->i_dirt = 0;
	brelse(bh);
	unlockInode(inode);
}

//等待i节点解锁
PRIVATE void waitOnInode(MINODE* inode)
{
	disableInt();
	//循环等待i节点解锁，睡眠等待进程
	while (inode->i_lock)
	{
		sleepOn(&inode->i_wait);
	}
	enableInt();
}