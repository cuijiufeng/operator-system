/**
 * 2020年10月09日 20/50/51
 * //todo
 */
#include	<type.h>
#include	<fs.h>
#include	<lib.h>

SUPER_BLOCK SUPER_BLK[NR_SUPER];

PRIVATE void waitOnSuper(SUPER_BLOCK* sb)
{
	//关中断
	disableInt();
	while (sb->s_lock) 
	{
		sleepOn(&(sb->s_wait));
	}
	//开中断
	enableInt();
}

SUPER_BLOCK* getSuper(int dev)
{
	SUPER_BLOCK* s;

	if (!dev)
		return NULL;
	s = 0 + SUPER_BLK;
	while (s < NR_SUPER + SUPER_BLK)
	{
		if (s->s_dev == dev) 
		{
			waitOnSuper(s);
			if (s->s_dev == dev)
				return s;
			s = 0 + SUPER_BLK;
		}
		else 
		{
			s++;
		}
	}
	return NULL;
}