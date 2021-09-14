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
	
.end	