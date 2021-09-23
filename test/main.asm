 	.include "bsp.inc"
	
		segment TEXT

QUOT	macro	label
	\"label\"
	endmacro QUOT

BUFSZ	macro	sz
		db	BUFSZ&sz	; bufsz register constant
		db	sz-1		; bufalign
	endmacro BUFSZ
	
	xdef emaccfg 
emaccfg		.tag	EMACCFG
emaccfg:
			dw24	#(__RAM_ADDR_U_INIT_PARAM << 16) + C000h	; tlbp Transmit Lower Boundary Pointer
			dw24	#(__RAM_ADDR_U_INIT_PARAM << 16) + D000h	; bp Boundary Pointer
			dw24	#(__RAM_ADDR_U_INIT_PARAM << 16) + E000h	; rhbp Receive High Boundary Pointer
			db 		00h,90h,23h,00h,01h,01h						; macaddr
			BUFSZ 32
	; end emaccfg


BAUDRATE0 	EQU 115200 ;57600 ;38400

	xdef	uart0cfg
uart0cfg	.tag	UARTCFG
uart0cfg:
			dw		((_SYS_CLK_FREQ / BAUDRATE0) / 16)	; divisor baudrate 115200
			db  	LCTL_CHAR_8_1						; lctl line control 8,1,n

init_uart0_ok:ascii "init UART0 115200 ,8,1,n RTS/CTS.",0
	
	segment DATA
iethstack:	dw24	ethstack
oethstack:	dw24	ethstack
	
	segment BSS
	.align 100h
ethstack:	ds	103h
	
	xdef HEAPSIZE
HEAPSIZE	.equ	10000h	
	xdef heapstart
heapstart:	ds	HEAPSIZE

	segment CODE
	.assume adl=1
	xdef	_main
_main:	
;			ld		iy,bspcfg

			call	init_bsp
			
			ld		hl,heapstart
			ld		bc,HEAPSIZE
			call	init_heap
			ld		iy,uart0cfg
			call	init_uart0
			ld		hl,init_uart0_ok
			call	puts
			
			call	heapdump
			ld		a,0ah
			call	putc
			
			ld		bc,1
$m0:		call	malloc
			jr		z,$F
			ex		de,hl
			call	malloc
			jr		z,$m1
			call	heapdump
			ld		a,0ah
			call	putc
			ex		de,hl
			call	free
			ex		de,hl
			call	free
			call	heapdump
			ld		a,0ah
			call	putc
			ld		hl,bc
			add		hl,bc
			ld		bc,hl
			jr		nz,$m0
			jr		$F
$m1:		ex		de,hl
			call	free
			call	heapdump
			ld		a,0ah
			call	putc
$$:	
			
			

			
			ld		iy,emaccfg
			call	init_emac
			call	PhyStatus
			jr		$prnt_descr
			
	
	XDEF	rxeth	;hl frame
rxeth:			push	hl
				ld		de,(iethstack)
				inc		e
				jr		z,$F
				inc		e
				jr		z,$F
				inc		e
$$:				ld		hl,(oethstack)
				xor		a,a
				sbc		hl,de
				jr		nz,$F
				pop		hl
				call	free
				ret		
$$:				ld		hl,(iethstack)
				ld		(iethstack),de
				pop		de
				ld		(hl),de
				ret
			
	with MACDESCRIPTOR

$prnt_descr:	ld		de,(oethstack)
				ld		hl,(iethstack)
				xor		a,a
				sbc		hl,de
				jr		z,$prnt_descr
				ld		hl,de
				inc		e
				jr		z,$F
				inc		e
				jr		z,$F
				inc		e
$$:				cp		a,l
				call	z,heapdump
				ld		iy,(hl)				
				ld		(oethstack),de
				call	prnt_u24_hex
				ld		a,'>'
				call	putc
				ld		hl,(iy+NP)		; next
				call	prnt_u24_hex
				ld		a,','
				call	putc
				ld		hl,(iy+PKTSIZE)
				call	prnt_u16_hex
				ld		a,','
				call	putc
				ld		hl,(iy+STATUS)
				call	prnt_u16_hex
				ld		a,' '
				call	putc
				lea		ix,iy+MACDESCRIPTORSZ
	with ETHHDR			
				lea		hl,ix+destmac
				call	prnt_mac
				ld		a,','
				call	putc
				lea		hl,ix+srcmac
				call	prnt_mac
				ld		a,','
				call	putc
				ld		h,(ix+lentype)
				ld		l,(ix+lentype+1)
				call	prnt_u16_hex
				ld		a,0ah
				call	putc
				ld		hl,iy
				call	free
				jr		$prnt_descr
	endwith

	END 