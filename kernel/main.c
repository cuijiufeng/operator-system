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
}
inline __attribute__((always_inline)) t_32 pause()
{
	t_32 rs;
	__asm__("int $0x80":"=a"(rs) : "a"(NR_PAUSE));
}
//==================================================================================================

PUBLIC	void	cmain()
{
	if(!fork())
	{
		while (1);
	}
	//0号进程一直pause在这里，并且不停的的执行pause中schedule,
	while (1);
	//{
	//	pause();
	//}
}