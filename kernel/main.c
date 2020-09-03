/**
 * 2020年08月22日 20/40/26
 * 进入用户态之后
 */
#include	<type.h>
#include	<syscall.h>

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
//==================================================================================================

PUBLIC	void	cmain()
{
	//fork复制一个进程，父进程返回子进程的pid会执行下面的while，子进程返回0会执行if。程序分开执行了
	if(!fork())
	{
		while (1);
	}
	//0号进程一直pause在这里，并且不停的的执行pause中schedule,
	while (1)
	{
		pause();
	}
}