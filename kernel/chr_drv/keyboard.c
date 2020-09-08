/**
 * 2020��09��05�� 14/00/18
 * ���̴��������غ���
 */
#include	<type.h>
#include	<chr_drv/keyboard.h>
#include	<fs.h>
#include	<mm.h>
#include	<signal.h>
#include	<protect.h>
#include	<process.h>
#include	<chr_drv/termios.h>
#include	<chr_drv/tty.h>

t_bool		shift_l = FALSE;		//l shift state
t_bool		shift_r = FALSE;		//r shift state
t_bool		alt_l = FALSE;			//l alt state
t_bool		alt_r = FALSE;			//r alt state
t_bool		ctrl_l = FALSE;			//l ctrl state
t_bool		ctrl_r = FALSE;			//r ctrl state
t_bool		caps_lock = FALSE;		//caps_lock��״̬
t_bool		num_lock = TRUE;		//num_lock��״̬
t_bool		scroll_lock = FALSE;	//scroll_lock��״̬
t_bool		code_with_E0 = FALSE;

PRIVATE		u_32	pos = 0x460;
PUBLIC	void	keyboardHandler(int irq)
{
	t_bool	make;
	t_32	key = 0;						//��һ����������ʾһ����
	u_32*	keyrow;							//keymap��ĳһ��
	u_32	column = 0;
	t_bool	pad = FALSE;
	u_8		scan_code = inByte(KB_DATA);

	//û�д���printScreen��pause�����������е�ʱ����ܻ����δ֪�Ĵ���
	if (scan_code == 0xE1)
	{
		return;
	}
	else if (scan_code == 0xE0)
	{
		code_with_E0 = TRUE;
		return;
	} 

	keyrow = keymap[(scan_code & 0x7F)];			//�ȶ�λ��keymap����
	make = (scan_code & FLAG_BREAK ? FALSE : TRUE);	//��make����break
	keyrow = keymap[(scan_code & 0x7F)];			//�ȶ�λ��keymap����
	t_bool caps = shift_l || shift_r;

	//�������capslock��
	if (caps_lock)
	{
		//ͬʱ������ĸ�ַ���Ҫ��ת��Сд
		if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z') || (keyrow[1] >= 'A') && (keyrow[1] <= 'Z'))
		{
			caps = !caps;
		}
	}									
	//�����shift��capslock���£����ǵڶ���
	if (caps)
	{
		column = 1;
	}
	//������
	if (code_with_E0)
	{
		column = 2;
		code_with_E0 = FALSE;
	}
	//ȡ����ת���ַ�
	key = keyrow[column];
	//����
	switch (key)
	{
		case SHIFT_L:
			shift_l = make;
			break;
		case SHIFT_R:
			shift_r = make;
			break;
		case CTRL_L:
			ctrl_l = make;
			break;
		case CTRL_R:
			ctrl_r = make;
			break;
		case ALT_L:
			alt_l = make;
			break;
		case ALT_R:
			alt_r = make;
			break;
		case CAPS_LOCK:
			if (make)
			{
				caps_lock = !caps_lock;
				setLeds();
			}
			break;
		case NUM_LOCK:
			if (make)
			{
				num_lock = !num_lock;
				setLeds();
			}
			break;
		case SCROLL_LOCK:
			if (make)
			{
				scroll_lock = !scroll_lock;
				setLeds();
			}
			break;
		default:
			break;
	}

	//����break code
	if (make)
	{
		if (key >= PAD_SLASH && key <= PAD_9)
		{
			pad = FALSE;
			switch (key)
			{
				case PAD_SLASH:
					key = '/';
					break;
				case PAD_STAR:
					key = '*';
					break;
				case PAD_MINUS:
					key = '-';
					break;
				case PAD_PLUS:
					key = '+';
					break;
				case PAD_ENTER:
					key = ENTER;
					break;
				default:			/* keys whose value depends on the NumLock */
					if (num_lock) 	/* '0' ~ '9' and '.' in num pad */
					{
						if ((key >= PAD_0) && (key <= PAD_9))
						{
							key = key - PAD_0 + '0';
						}
						else if (key == PAD_DOT)
						{
							key = '.';
						}
					}
					else
					{
						switch (key)
						{
						case PAD_HOME:
							key = HOME;
							break;
						case PAD_END:
							key = END;
							break;
						case PAD_PAGEUP:
							key = PAGEUP;
							break;
						case PAD_PAGEDOWN:
							key = PAGEDOWN;
							break;
						case PAD_INS:
							key = INSERT;
							break;
						case PAD_UP:
							key = UP;
							break;
						case PAD_DOWN:
							key = DOWN;
							break;
						case PAD_LEFT:
							key = LEFT;
							break;
						case PAD_RIGHT:
							key = RIGHT;
							break;
						case PAD_DOT:
							key = DELETE;
							break;
						default:
							break;
						}
					}
					break;
			}
		}
		key |= (shift_l ? FLAG_SHIFT_L : 0);
		key |= (shift_r ? FLAG_SHIFT_R : 0);
		key |= (ctrl_l ? FLAG_CTRL_L : 0);
		key |= (ctrl_r ? FLAG_CTRL_R : 0);
		key |= (alt_l ? FLAG_ALT_L : 0);
		key |= (alt_r ? FLAG_ALT_R : 0);
		key |= pad ? FLAG_PAD : 0;

		//��key�������̨������
		TTY_TABLE[0].read_q.buf[TTY_TABLE[0].read_q.head] = key;
		TTY_TABLE[0].read_q.head = (TTY_TABLE[0].read_q.head + 1) % TTY_BUF_SIZE;
	}
}

//���ü��̵�
PRIVATE	void	setLeds()
{
	t_8 leds = (caps_lock << 2) | (num_lock << 1) | (scroll_lock);
	kbWait();
	outByte(KB_DATA, LED_CODE);
	kbAck();
	kbWait();
	outByte(KB_DATA, leds);
	kbAck();
}

//�ȴ�8042�����뻺������
PRIVATE	void	kbWait()
{
	t_8 kb_stat;
	do
	{
		kb_stat = inByte(KB_CMD);
	} while (kb_stat & 0x02);
}

//��ȡ״̬
PRIVATE	void	kbAck()
{
	t_8 kb_read;
	do
	{
		kb_read = inByte(KB_DATA);
	} while (kb_read = !KB_ACK);
}