/**
 * 2020��09��28�� 21/18/07
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
	//��������inode�ڵ�
	for (i = 0; i < NR_INODE; i++, inode++) 
	{
		//�ȴ�i�ڵ����
		waitOnInode(inode);
		//���i�ڵ��޸ı�־���ҹܵ���־��
		if (inode->i_dirt && !inode->i_pipe)
		{
			//дi�ڵ�
			writeInode(inode);
		}
	}
}

//�������
PRIVATE void lockInode(MINODE* inode)
{
	//���ж�
	disableInt();
	while (inode->i_lock)
	{
		sleepOn(&inode->i_wait);
	}
	inode->i_lock = 1;
	//���ж�
	enableInt();
}

//�������
PRIVATE void unlockInode(MINODE* inode)
{
	inode->i_lock = 0;
	wakeUp(&inode->i_wait);
}

//д���
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

//�ȴ�i�ڵ����
PRIVATE void waitOnInode(MINODE* inode)
{
	disableInt();
	//ѭ���ȴ�i�ڵ������˯�ߵȴ�����
	while (inode->i_lock)
	{
		sleepOn(&inode->i_wait);
	}
	enableInt();
}