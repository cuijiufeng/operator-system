/**
 * 2020Äê10ÔÂ09ÈÕ 21/15/20
 * 
 */
#ifndef _OS_STAT_H_
#define _OS_STAT_H_

#define S_IFMT		00170000
#define S_IFBLK		0060000

#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)

#define _S_IFCHR	0x2000 // Character special
#define _S_IFREG	0x8000 // Regular

#define S_IFCHR		_S_IFCHR
#define S_IFREG		_S_IFREG

#endif // !_OS_STAT_H_
