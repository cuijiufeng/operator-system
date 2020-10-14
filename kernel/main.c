/**
 * 2020年08月22日 20/40/26
 * 进入用户态之后
 */
#include	<type.h>
#include	<fs.h>
#include	<mm.h>
#include	<protect.h>
#include	<blk_drv/blk.h>
#include	<syscall.h>
#include	<print.h>

//调用系统调用函数
//==================================================================================================
inline __attribute__((always_inline)) t_32 fork()
{
	t_32 rs;
	__asm__("int $0x80":"=a"(rs) : "a"(NR_FORK));
	return rs;
}
inline __attribute__((always_inline)) t_32 pause()
{
	t_32 rs;
	__asm__("int $0x80":"=a"(rs) : "a"(NR_PAUSE));
	return rs;
}
inline __attribute__((always_inline)) t_32 setup(t_8* drv_info)
{
	t_32 rs;
	__asm__("int $0x80":"=a"(rs) : "a"(NR_SETUP), "d"(drv_info));
	return rs;
}
//==================================================================================================

PUBLIC	void	init()
{
	setup(DRIVER_INFOS);
	//printf("%s%d\n", "abde", 100);
	while (1);
}

PUBLIC	void	cmain()
{
	__asm__("add $0x14, %%esp"::);	//把cmain函数生成的栈操作给还原
	//fork复制一个进程，父进程返回子进程的pid会执行下面的while，子进程返回0会执行if。程序分开执行了
	if(!fork())
	{
		//1号init进程
		init();
	}
	//0号进程一直pause在这里，并且不停的的执行pause中schedule,
	while (1)
	{
		pause();
	}
}