/**
 * 2020��09��09�� 20/53/17
 * printͷ�ļ�
 */
#ifndef _OS_PRINT_H_
#define	_OS_PRINT_H_

#define ZEROPAD	1		/* pad with zero */
#define LEFT	2		/* left justified */
#define SMALL	4		/* use 'abcdef' instead of 'ABCDEF' */
#define SIGN	8		/* unsigned/signed long */

PUBLIC	int	vsprintf(char buf[], const char * fmt, int* arg_addr);
PUBLIC	int	printf(const char* fmt, ...);
PRIVATE char* number(char* str, int num, int base, int width, int flags);

#endif // !_OS_PRINT_H_
