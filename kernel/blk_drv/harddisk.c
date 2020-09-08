/**
 * 2020年09月03日 21/10/09
 * 1.硬盘操作相关函数
 */
#include	<type.h>
#include	<blk_drv/blk.h>

DRIVE_INFO_TABLE	DRIVE_INFO;
HARD_DISK			HARD_DISK_INFO = { 0,};
HD					HD_INFO[5];

PUBLIC	t_32	sysSetup(DRIVE_INFO_TABLE* drv_info)
{
	//设置磁盘参数及类型信息
	HARD_DISK_INFO.head = drv_info->heads;
	HARD_DISK_INFO.sector = drv_info->sectors;
	HARD_DISK_INFO.cylinder = drv_info->cylinders;
	HARD_DISK_INFO.wpcom = drv_info->compensate;
	HARD_DISK_INFO.lzone = drv_info->stop_cylind;
	HARD_DISK_INFO.ctrl = drv_info->complex;
	
	//设置磁盘分区信息
	HD_INFO[0].start_sector = 0;
	HD_INFO[0].cnt_sectors = HARD_DISK_INFO.head * HARD_DISK_INFO.sector * HARD_DISK_INFO.cylinder;
}