	.include "console.inc"
	segment text
	
d10:	dw24	-10000000
		dw24	-1000000
		dw24	-100000
		dw24	-10000
		dw24	-1000
		dw24	-100
		dw24	-10

	segment	BSS

	segment	code
	.assume adl=1


;Inputs:	
;     HL	Num, ix = tmpbuf > 8
;Outputs:
;	ix	ptr c-str dec-ascii,c = len
Num2Dec:		push	de
				push	hl
				push	ix
				push	iy
				ld		iy,d10
				ld		bc,700h
				call	$loop	
				ld		a,'0'
				add		a,l
				ld		(ix),a
				inc		ix
				ld		(ix),0
				inc		c
				pop		iy
				pop		ix
				pop		hl
				pop		de
				ret
$loop:			ld		de,(iy)				
				lea		iy,iy+3
				ld		a,-1
$$:				inc		a
				add		hl,de
				jr		c,$B
				sbc		hl,de
				or		a,a
				jr		nz,$F
				or		a,c
				jr		z,$next
				xor		a,a
$$:				add		a,'0'				
				ld		(ix),a
				inc		ix
				inc		c
$next:			djnz	$loop
				ret	
				
				
	xdef	prnt_u8_hex		; IN: a=int
prnt_u8_hex:	push 	af
				srl		a
				srl		a
				srl		a
				srl		a
				call	$F
				pop		af
				and		a,0Fh
$$:				add		a,'0'
				cp		a,'9'+1
				jr		c,$F
				add		a,'A' - '9' - 1
$$:				call	putc				
				ret
				
	xdef	prnt_u16_hex	; IN: hl=int
prnt_u16_hex:	ld		a,h
				call	prnt_u8_hex
				ld		a,l
				jr		prnt_u8_hex
	
	xdef	prnt_u24_hex	; IN: hl=int
prnt_u24_hex:	push	hl
				dec		sp
				push	hl
				inc		sp
				pop		hl
				ld		a,h
				pop		hl
				call	prnt_u8_hex
				jr		prnt_u16_hex
	
	xdef	prnt_u24_int	; IN: hl=int, d=size, e=fill
prnt_u24_int:	push	ix
				push	hl
				push	de
				push	bc
				ld		ix,0
				add		ix,sp
				lea		ix,ix-9
				ld		sp,ix
				push	ix
				call	Num2Dec
				ld		a,d				; field size
				sub		a,c				; -chars on buffer
				jr		z,$pdec
				jr		c,$pdec
				ld		d,a				; fill count
				ld		a,e				; fill char
				or		a,a
				jr		nz,$F
				ld		e,' '
$$:				ld		a,e
				call	putc
				dec		d
				jr		nz,$B
$pdec:			ld		a,(ix)
				or		a,a
				jr		z,$F
				call	putc
				inc		ix
				jr		$pdec
$$:				pop		ix
				lea		ix,ix+9
				ld		sp,ix
				pop		bc
				pop		de
				pop		hl
				pop		ix
				ret

	xdef prnt_mac	; hl => mac
prnt_mac:		push	bc
				push	hl
				ld		b,6
				jr		$F
$macloop:		inc		hl				
				ld		a,':'
				call	putc
$$:				ld		a,(hl)
				call	prnt_u8_hex
				djnz	$macloop
				pop		hl
				pop		bc
				ret