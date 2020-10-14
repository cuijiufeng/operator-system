/**
 * 2020年07月30日 19/23/07
 * 内存相关
 */
#ifndef _OS_MM_H_
#define	_OS_MM_H_
//页表地址
#define	PAGE_DIR	0x0
//页大小
#define	PAGE_SIZE	4096
//内核使用的内存
#define	KERNEL_MEM	0x100000
//内存大小
#define	MEM_SIZE	(32*1024*1024)
//二级分页项数
#define	PAGES_SIZE	(MEM_SIZE>>12)
//表项格式
#define	PG_P		1   //页存在标志位
#define	PG_RWW		2	//R/W 属性位值, 读/写/执行
#define	PG_USU		4	//U/S 属性位值, 用户级

//---------------------------------------------------------------------------------------
EXTERN	t_8	MEM_MAP[PAGES_SIZE];					//-1表示内存不可用，0表示内存可用，>0表示内存页已被映射数
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
PUBLIC	void	initMemory(t_32 mem_size);
PUBLIC	u_32	getFreePage();
PUBLIC	void	freePage(u_32 addr);
PUBLIC	t_32	copyPageTables(u_32 dest_addr, u_32 src_addr, u_32 size);
PUBLIC	t_32	freePageTables(u_32 addr, u_32 size);
PUBLIC	void	writeProtectPage(u_32 err_code, u_32 addr);
PUBLIC	void	unWriteProtectPage(u_32* pg_tb_item);
PUBLIC	void	missingPage(u_32 error_code, u_32 address);
//---------------------------------------------------------------------------------------

#endif // !_OS_MM_H_
