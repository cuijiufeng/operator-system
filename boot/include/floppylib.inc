;2020年07月05日 17/02/07
;读取软盘用的函数抽取
;========================================================================================================

;读取一个扇区(把从ax号开始的cx个扇区读入到es:bx中去)
;接收的参数（es:bx	-->缓冲区	ax	-->扇区号	cx	-->读取扇区个数）
;--------------------------------------------------------------------------------------------------
readSector:				
	push ax
	push cx
	mov bp,sp

	mov dl,[ds:BPB_SecPerTrk]	;每磁道扇区数
	div dl
	push ax						;暂存除法结果的商与余数
	shr al,1					;商右移一位为柱面号
	mov ch,al
	pop ax						;取出除法的商与余数
	and al,1					;商并1为磁头号
	mov dh,al
	inc ah						
	mov cl,ah					;余数加1为起始扇区号
	mov dl,[ds:BS_DrvNum]
	
	mov ax,[ss:bp]
	LOOP_READ_SECTOR_FAIL:
		mov ah,02h					;2号功能
		int 13H
	jc LOOP_READ_SECTOR_FAIL		;如果读取失败，则cf被置1
	
	pop cx
	pop ax
	ret
;-----------------------------------------------------------------------------------------------------

;给定一个文件内容的扇区号，查找是否还有后继扇区
;接收的参数(ax	-->	扇区号		ax	-->	后继扇区号)
;--------------------------------------------------------------------------------------------------
readFATValue:
		push es
		push bx
		
		mov dx,BUFFER_FAT_ADDRESS
		mov es,dx
		mov byte [ds:EvenOrOdd],0				;开始认为它是偶数
		
		mov bx,3
		mul bx
		mov bx,2
		div bx									;扇区号*3/2=FAT项在FAT表中的偏移
		cmp dx,0								;如果余数为0，则它是偶数项
	je EVEN_NUMBER
		mov byte [ds:EvenOrOdd],1				;奇数
	EVEN_NUMBER:
		xor dx,dx
		mov bx,[ds:BPB_BytsPerSec]				;偏移/512=所在扇区(余数为相对于所在扇区中的偏移)
		div bx
		push dx
		add ax,FAT_SECTOR_START					;扇区号
		mov cx,2								;要读的扇区个数,FAT项有可能是跨扇区的
		mov bx,BUFFER_FAT_OFFSET
		call readSector							;读FAT扇区
		pop dx
		add bx,dx								;加上相对于所占扇区中的偏移就是我们所要找的数据的地址
		mov ax,[es:bx]
		pop bx
		pop es									;还原es,bx值
		cmp byte [ds:EvenOrOdd],0
	jne ODD
	EVEN:
		and ax,0FFFH							;扔掉高4位
		ret
	ODD:
		shr ax,4								;扔掉低4位
		ret
;--------------------------------------------------------------------------------------------------

;关闭软驱马达，不然软驱灯会一直亮着	
;-----------------------------------------------------------------------------------------------------
killMotor:
		push dx
		mov dx,03F2H
		mov al,0
		out dx,al			;往03F2H端口写入al中的值
		pop dx
		ret
;-----------------------------------------------------------------------------------------------------