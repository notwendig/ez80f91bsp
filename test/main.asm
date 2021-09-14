	.include "bsp.inc"
	segment CODE
	.assume adl=1
	xdef	_main
_main:
			call	init_bsp
			
			ret 
	
	END