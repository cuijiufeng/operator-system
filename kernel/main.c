/**
 * 2020年08月22日 20/40/26
 * 进入用户态之后
 */
#include	<type.h>

PUBLIC	void	main()
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