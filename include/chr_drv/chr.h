/**
 * 2020��09��05�� 14/00/18
 * �ַ��豸
 */
#ifndef _OS_CHR_H_
#define	_OS_CHR_H_

PUBLIC	void	initTty();
PUBLIC	void	keyboardHandler(int irq);
PUBLIC	void	conWrite(TTY_STRUCT* tty);
PUBLIC	void	rsWrite(TTY_STRUCT* tty);

#endif // !_OS_CHR_H_
