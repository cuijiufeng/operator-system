/**
 * 2020Äê10ÔÂ09ÈÕ 20/50/51
 * //todo
 */

#include	<type.h>
#include	<fs.h>

#define NRDEVS ((sizeof(CRW_TABLE))/(sizeof(CRW_PTR)))

CRW_PTR CRW_TABLE[] = {
	NULL,		// nodev
	NULL,//rw_memory,	// /dev/mem etc
	NULL,		// /dev/fd
	NULL,		// /dev/hd
	rwTtyx,	// /dev/ttyx
	NULL,//rw_tty,		// /dev/tty
	NULL,		// /dev/lp
	NULL		// unnamed pipes
};

PRIVATE int rwTtyx(int rw, u_32 minor, char * buf, int count, u_32* pos)
{
	return ((rw == READ) ? ttyRead(minor, buf, count) : ttyWrite(minor, buf, count));
}

int rwChar(int rw, int dev, char * buf, int count, u_32* pos)
{
	CRW_PTR call_addr;

	if (MAJOR(dev) >= NRDEVS)
		return -1;
	if (!(call_addr = CRW_TABLE[MAJOR(dev)]))
		return -1;
	return call_addr(rw, MINOR(dev), buf, count, pos);
}
