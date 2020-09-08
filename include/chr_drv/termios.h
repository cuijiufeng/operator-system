/**
 * 2020年09月05日 19/35/25
 * 终端I/O接口定义
 */
#ifndef _OS_TERMIOS_H_
#define	_OS_TERMIOS_H_

//c_iflag标志位
#define IGNBRK	0000001	   //输入时忽略break条件
#define BRKINT	0000002	   //在break时产生sigint信号
#define IGNPAR	0000004	   //忽略奇偶校验出错的字符
#define PARMRK	0000010	   //标志奇偶校验错
#define INPCK	0000020	   //允许输入奇偶校验
#define ISTRIP	0000040	   //屏蔽字符第8位
#define INLCR	0000100	   //输入时将换行符NL映射成回车符CR
#define IGNCR	0000200	   //忽略回车符CR
#define ICRNL	0000400	   //在输入时将回车符CR映射成换行符NL
#define IUCLC	0001000	   //在输入时将大写字符转换成小写字符
#define IXON	0002000	   //允许在开始/停止(XON/XOFF)输出控制
#define IXANY	0004000	   //允许任何字符重启输出
#define IXOFF	0010000	   //允许开始/停止(XON/XOFF)输入控制
#define IMAXBEL	0020000	   //输入队列满时响铃

//c_oflag标志位
#define OPOST	0000001	   //执行输出处理
#define OLCUC	0000002	   //在输出时将小写字符转换成大写字符
#define ONLCR	0000004	   //在输出时将换行符NL映射成回车-换行符CR-NL
#define OCRNL	0000010	   //在输出时将回车符CR映射成换行符NL
#define ONOCR	0000020	   //在0列不输出回车符CR
#define ONLRET	0000040	   //换行符NL执行回车符的功能
#define OFILL	0000100	   //延迟时使用填充字符而不使用时间延迟
#define OFDEL	0000200	   //填充字符是ASCII码DEL。如果未设置，则使用ASCII NULL
#define NLDLY	0000400	   //选择换行延迟
#define   NL0	0000000	   //换行延迟类型0
#define   NL1	0000400	   //换行延迟类型1
#define CRDLY	0003000	   //选择回车延迟
#define   CR0	0000000	   //回车延迟类型0
#define   CR1	0001000	   //回车延迟类型1
#define   CR2	0002000	   //回车延迟类型2
#define   CR3	0003000	   //回车延迟类型3
#define TABDLY	0014000	   //选择水平制表延迟
#define   TAB0	0000000	   //水平制表延迟类型0
#define   TAB1	0004000	   //水平制表延迟类型1
#define   TAB2	0010000	   //水平制表延迟类型2
#define   TAB3	0014000	   //水平制表延迟类型3
#define   XTABS	0014000	   //将制表符TAB换成空格，该值表示空格数
#define BSDLY	0020000	   //选择退格延迟
#define   BS0	0000000	   //退格延迟类型0
#define   BS1	0020000	   //退格延迟类型1
#define VTDLY	0040000	   //纵向制表延迟
#define   VT0	0000000	   //纵向制表延迟类型0
#define   VT1	0040000	   //纵向制表延迟类型1
#define FFDLY	0040000	   //选择换页延迟
#define   FF0	0000000	   //换页延迟类型0
#define   FF1	0040000	   //换页延迟类型1


//c_cflag标志位
#define CBAUD	0000017	   //传输速率位屏蔽码
#define  B0		0000000    //挂断线路
#define  B50	0000001	   //波特率50
#define  B75	0000002	   //波特率75
#define  B110	0000003	   //波特率110
#define  B134	0000004	   //波特率134
#define  B150	0000005	   //波特率150
#define  B200	0000006	   //波特率200
#define  B300	0000007	   //波特率300
#define  B600	0000010	   //波特率600
#define  B1200	0000011	   //波特率1200
#define  B1800	0000012	   //波特率1800
#define  B2400	0000013	   //波特率2400
#define  B4800	0000014	   //波特率4800
#define  B9600	0000015	   //波特率9600
#define  B19200	0000016	   //波特率19200
#define  B38400	0000017	   //波特率38400
#define EXTA B19200		   //扩展波特率A
#define EXTB B38400		   //扩展波特率B
#define CSIZE	0000060	   //字符位宽度屏蔽码
#define   CS5	0000000	   //每字符5比特位
#define   CS6	0000020	   //每字符6比特位
#define   CS7	0000040	   //每字符7比特位
#define   CS8	0000060	   //每字符8比特位
#define CSTOPB	0000100	   //设置两个停止位，而不是1个
#define CREAD	0000200	   //允许接收
#define CPARENB	0000400	   //开启输出时产生奇偶位，输入时进行奇偶校验
#define CPARODD	0001000	   //输入/输出校验是奇校验
#define HUPCL	0002000	   //最后进程关闭挂断
#define CLOCAL	0004000	   //忽略调制解调器控制线路
#define CIBAUD	03600000	//输入波特率(未使用)
#define CRTSCTS	020000000000//流控制

#define PARENB CPARENB		//开启输出时产生奇偶位，输入时进行奇偶校验
#define PARODD CPARODD		//输入/输出校验是奇校验


//c_lflag标志位
#define ISIG	0000001		//当收到字符INTR\QUIT\SUSP\DSUSP，产生相应的信号
#define ICANON	0000002		//开启规范模式
#define XCASE	0000004		//若设置了ICANON，则终端是大写字符的
#define ECHO	0000010		//回显示输入字符
#define ECHOE	0000020		//若设置了ICANON，则ERASE/WERASE将探险前一字符/单词
#define ECHOK	0000040		//若设置了ICANON，则KILL字符将探险当前行
#define ECHONL	0000100		//若设置了ICANON，则即使ECHO没有开启也回显NL字符。
#define NOFLSH	0000200		//当生成SIGINT和SIGQUIT信号时不刷新输入输出队列，当生成SIGSUSP信号时，刷新输入队列
#define TOSTOP	0000400		//发送SIGTTOU信号到后台进程组，该后台进程试图写自己的控制终端
#define ECHOCTL	0001000		//若设置了ECHO，则除TAB\NL\START和STOP以外撤销ASCII控制信号将被回显成将^x式样，X值是控制符+0x40
#define ECHOPRT	0002000		//若设置了ICANON和ICHO，则字符在擦除时将显示
#define ECHOKE	0004000		//若设置了ICANON，则KILL通过擦除行上的所有字符被回显
#define FLUSHO	0010000		//输出被刷新，通过键入DISCARD字符，该标志被翻转
#define PENDIN	0040000		//当下一个字符是读时，输入队列中的所有字符将被重显
#define IEXTEN	0100000		//开启实现时定义的输入处理

//mode线路信号符号常数
#define TIOCM_LE	0x001	//线路允许
#define TIOCM_DTR	0x002	//数据终端就绪
#define TIOCM_RTS	0x004	//请求发送
#define TIOCM_ST	0x008	//串行数据发送
#define TIOCM_SR	0x010	//串行数据接收
#define TIOCM_CTS	0x020	//清除发送
#define TIOCM_CAR	0x040	//载波监测
#define TIOCM_RNG	0x080	//响铃指示
#define TIOCM_DSR	0x100	//数据设备就绪
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG

//c_cc数组中的字符
#define VINTR 0				//c_cc[VINTR]	= INTER		(^C) \003 中断字符
#define VQUIT 1				//c_cc[VQUIT]	= QUE		(^\) \003 退出字符
#define VERASE 2			//c_cc[VERASE]	= ERASE		(^H) \003 擦出字符
#define VKILL 3				//c_cc[VKILL]	= KILL		(^U) \003 终止字符
#define VEOF 4				//c_cc[VEOF]	= EOF		(^D) \003 文件结束字符
#define VTIME 5				//c_cc[VTIME]	= TIME		(\0) \003 定时器值字符
#define VMIN 6				//c_cc[VMIN]	= MIN		(\1) \003 定时值
#define VSWTC 7				//c_cc[VSWTC]	= SWTC		(\0) \003 交换字符
#define VSTART 8			//c_cc[VSTART]	= START		(^Q) \003 开始字符
#define VSTOP 9				//c_cc[VSTOP]	= STOP		(^S) \003 停止字符
#define VSUSP 10			//c_cc[VSUSP]	= SUSP		(^Z) \003 挂起字符
#define VEOL 11				//c_cc[VEOL]	= EOL		(\0) \003 行结束字符
#define VREPRINT 12			//c_cc[VREPRINT]	= REPRINT	(^R) \003 重显示字符
#define VDISCARD 13			//c_cc[VDISCARD]	= DISCARD	(^O) \003 丢弃字符
#define VWERASE 14			//c_cc[VWERASE]	= WERASE	(^W) \003 单词擦除字符
#define VLNEXT 15			//c_cc[VLNEXT]	= LNEXT		(^V) \003 下一行字符
#define VEOL2 16			//c_cc[VEOL2]	= EOL2		(\0) \003 行结束字符2


//POSIX的termios结构
#define	NCCS	17				//termios结构中控制字符数组的长度
typedef struct termios {
	u_32 c_iflag;		//输入模式标志
	u_32 c_oflag;		//输出模式标志
	u_32 c_cflag;		//控制模式标志
	u_32 c_lflag;		//本地模式标志
	u_8 c_line;			//线程规程(速率)
	u_8 c_cc[NCCS];		//控制字符数组
} TERMIOS;

#endif // !_OS_TERMIOS_H_
