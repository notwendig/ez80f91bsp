	.include "uart.inc"
	.include "console.inc"

	xdef	kbhit
	xdef	putc
	xdef	puts
	xdef	getc
	xdef	gets
	
	segment CODE
	.assume ADL=1

kbhit:	jp	uart0_kbhit
putc:	jp	uart0_putc
puts:	jp	uart0_puts	
getc:	jp	uart0_getc
gets:	jp	uart0_gets
	

	xdef strlen		;hl=>c-str, ret len in bc; zf=1 strlen 0
strlen:		push	hl
			ld		bc,0
			xor		a,a
$$:			cpi
			jr		nz,$B
			ld		hl,-1
			or		a,a
			sbc		hl,bc
			ex		(sp),hl
			pop		bc
			ret	
	
.end	