/**
 * 2020Äê09ÔÂ13ÈÕ 13/26/46
 * 
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<stat.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>

PUBLIC	t_32	sysWrite(u_32 fd, const char* buf, u_32 count)
{
	FILE* file;
	MINODE* inode;

	if (fd >= NR_OPEN || count < 0 || !(file = CURRENT->filp[fd]))
	{
		return -1;
	}
	if (!count)
	{
		return 0;
	}
	inode = file->f_inode;
	//todo
	//if (inode->i_pipe)
	//	return (file->f_mode & 2) ? write_pipe(inode, buf, count) : -1;
	if (S_ISCHR(inode->i_mode))
		return rwChar(WRITE, inode->i_zone[0], buf, count, &file->f_pos);
	//if (S_ISBLK(inode->i_mode))
	//	return block_write(inode->i_zone[0], &file->f_pos, buf, count);
	//if (S_ISREG(inode->i_mode))
	//	return file_write(inode, file, buf, count);
	printk("(Write)inode->i_mode=%x\n\r", inode->i_mode);
	return -1;
}