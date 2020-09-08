/**
 * 2020年09月05日 14/00/18
 * 字符设备
 */
#ifndef _OS_CHR_H_
#define	_OS_CHR_H_

PUBLIC	void	initTty();
PUBLIC	void	keyboardHandler(int irq);
PUBLIC	void	conWrite(TTY_STRUCT* tty);
PUBLIC	void	rsWrite(TTY_STRUCT* tty);

#endif // !_OS_CHR_H_
