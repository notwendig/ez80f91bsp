	nolist
	.include "uart.inc"
	list
	
	xref	_set_vector
	xref	DIVISOR0
	xref	critical_bgn
	xref	critical_end
	
	segment BSS
uart0		.TAG UARTSTAT
uart0:		DS	UARTSTATSZ
uart1		.TAG UARTSTAT
uart1:		DS	UARTSTATSZ

	.align  100h
uart0rx		DS 100h
uart0tx		DS 100h
uart1rx		DS 100h
uart1tx		DS 100h

	segment CODE
	.assume ADL=1
	.with UARTSTAT


; Interrupt Status handler.
			; 011 3 Highest Receiver Line Status
			scope
i_rls:		in0		a,(UART0_LSR)
			jr		nc,$F				; FIFO not used
			tst		a,UART_LSR_ERR
			jr		z, $F				; no FIFO error
			ld		hl,(ix+lsr_err)
			inc		hl
			ld		(ix+lsr_err),hl
			; Some error occured.
$$:			tst		a,UART_LSR_BL		; Break detected
			jr		z,$F
			ld		hl,(ix+lsr_bl)
			inc		hl
			ld		(ix+lsr_bl),hl
			jr		$skiprbr
			
$$:			tst		a,UART_LSR_FE
			jr		z,$F
			ld		hl,(ix+lsr_fe)
			inc		hl
			ld		(ix+lsr_fe),hl
			jr		$skiprbr
			
$$:			tst		a,UART_LSR_PE
			jr		z,$F
			ld		hl,(ix+lsr_pe)
			inc		hl
			ld		(ix+lsr_pe),hl
			jr		$skiprbr

$$:			tst		a,UART_LSR_OE
			jr		z,$F
			ld		hl,(ix+lsr_oe)
			inc		hl
			ld		(ix+lsr_oe),hl
			jr		$skiprbr
			
$$:			tst		a,UART_LSR_DR
			jr		z,$F
			ld		hl,(ix+lsr_dr)
			inc		hl
			ld		(ix+lsr_dr),hl
$skiprbr:	in0		a,(UART0_RBR)
$$:			ret
			
			scope
			; 010 2 Second	Receive Data Ready or Trigger Level
i_rdr:		in0		b,(UART0_RBR)		; read byte
			ld		hl,(ix+wrx)
			ld		a,(ix+rrx)
			dec		a
			cp		a,l
			jr		z,$F
			ld		(hl),b
			inc		(ix+wrx)
			ret
			
$$:			in0		a,(UART0_MCTL)
			and		a,~UART_MCTL_RTS
			out0	(UART0_MCTL),a
			ld		hl,(ix+dropedrx)
			inc		hl
			ld		(ix+dropedrx),hl
			ret
	
			scope
			; 110 6 Third	Character Time-out
i_cto:		ld		hl,(ix+cto)
			inc		hl
			ld		(ix+cto),hl
			in0		a,(UART0_LSR) 
			tst		a,UART_LSR_DR
			jr		nz,i_rdr
			ret
			
			scope
			; 101 5 Fourth	Transmission Complete
i_tc:		ret
			
			scope
			; 001 1 Fifth	Transmit Buffer Empty
i_tbe:		ld		b,01h				
			jr		nc,$F				; FIFO not uset
			ld		b,0Fh				; FIFO takes 16 bytes
$$:			ld		hl,(ix+rtx)
			ld		a,(ix+wtx)
			sub		a,l					;w-r			
			jr		nc,$F			;w<r; a=used
			add		a,FFh				;free
$$:			jr		z,$emptytbe
			cp		a,b
			jr		nc,$F
			ld		b,a
$$:			in0		a,(UART0_MSR)
			tst		a,UART_MSR_CTS
			ret		z
$$:			ld		a,(hl)
			inc		l
			out0	(UART0_THR),a
			djnz	$B
			ld		(ix+rtx),l
			ret		
$emptytbe:	in0		a,(UART0_IER)
			and		a,~UART_IER_TIE
			out0	(UART0_IER),a
			ret

			scope
			; 000 0 Lowest	Modem Status
i_ms:		in0		a,(UART0_MSR)
			tst		a,UART_MSR_DCTS
			ret		z
			tst		a,UART_MSR_CTS
			jr		nz,$F			
			in0		a,(UART0_IER)
			tst		a,UART_IER_TIE
			ret		z
			and		a,~UART_IER_TIE
			out0	(UART0_IER),a
			ret
$$:			in0		a,(UART0_IER)
			tst		a,UART_IER_TIE
			ret		nz
			or		a,UART_IER_TIE
			out0	(UART0_IER),a
			ret

			scope
i_nc:									; 100 111 4,7	not sed
			ret
	
			scope		
$jmphl:		jp		(hl)

Uart0IRQ:	push	af
			push	bc
			push	de
			push	hl
			push	ix
			ld		ix,uart0
			ld		de,$uart0itbl
			or		a,a
			sbc		hl,hl
			in0		a,(UART0_IIR)
			tst		a,UART_IIR_INTBIT
			jr		nz, $F				; no interrupt
			add		a,a					; INSTS *4; cy => FIFO enabled
			ld		l,a
			push	af
			add		hl,de
			ld		hl,(hl)
			pop		af
			call	$jmphl
$$:			pop		ix
			pop		hl
			pop		de
			pop		bc
			pop		af
			ei
			reti

			scope
	xdef init_uart0
init_uart0:	
	; Configure UART0 for 115200,8,1,n. Tx flow DSR, Rx flow DTR, RTS
			xor		a,a
			out0	(UART0_IER), a		; disable uart0 interrupts
			out0	(UART0_FCTL), a		; Enable FIFO and clear, int after 14 bytes.

			ld		a, UART_LCTL_DLAB	; 80h
			out0	(UART0_LCTL), a		; Enable access to BRG.
			ld		a, LOW(DIVISOR0)
			out0	(UART0_BRG_L), a	; Load low byte of BRG.
			ld		a, HIGH(DIVISOR0)
			out0	(UART0_BRG_H), a	; Load high byte of BRG.
			ld		a,LCTL_CHAR_8_1		; 03h
			out0	(UART0_LCTL), a		; Select 8 bits, no parity, 1 stop.
			ld		a, UART_MCTL_RTS|UART_MCTL_DTR	; 03h
			out0	(UART0_MCTL), a		; Select activate RTS, DTR
			ld		a,FCTL_TRIG_FFTL14|UART_FCTL_FIFOEN|UART_FCTL_CLRTXF|UART_FCTL_CLRRXF
			out0	(UART0_FCTL), a		; Enable receiver and transmitter
			
			ld		a,11010011b			; D3h. b6 = rx flow, 
										; set RTS=0 and DTR=0 for no rx (b3, b2)
										; set RTS=1 and DTR=1 for rx (b1, b0)
										; b7 = tx flow,
										; DSR = 0, CTS = 1 for tx allowed (b5, b4)
			out0	(UART0_SPR), a		; save the settings in the scratch register		
										; setup for a receive IRQ
			ld		hl,Uart0IRQ
			push	hl
			ld		hl,UART0_IVECT
			push	hl
			call	_set_vector
			pop		hl
			pop		hl
			
			ld		ix,uart0
			lea		hl,ix+0
			lea		de,ix+1
			ld		bc,UARTSTATSZ-1 
			xor		a,a
			ld		(hl),a
			ldir
			ld		hl,uart0rx
			ld		(ix+rrx),hl
			ld		(ix+wrx),hl
			ld		hl,uart0tx
			ld		(ix+rtx),hl
			ld		(ix+wtx),hl
			ld		a, UART_IER_RIE|UART_IER_LSIE|UART_IER_MIIE;|UART_IER_TCIE;|UART_IER_TIE
			out0	(UART0_IER), a		; enable all interrupts
			xor		a,a
			ret

			scope
	xdef uart0_putc
uart0_putc:	push	ix
			push	hl
			push	bc
			ld		c,a
			ld		ix,uart0
			ld		hl,(ix+wtx)
			ld		a,(ix+rtx)
			dec		a
			sub		a,l
			jr		z,$F
			ld		(hl),c
			inc		(ix+wtx)
			ld		a,1
$$:			or		a,a
			push	af
			SAVEIMASK
			in0		a,(UART0_IER)
			tst		a,UART_IER_TIE
			jr		nz,$F
			or		a,UART_IER_TIE
			out0	(UART0_IER),a
$$:			RESTOREIMASK
			pop		af
			pop		bc
			pop		hl
			pop		ix
			ret

			scope
	xdef uart0_kbhit
uart0_kbhit:push	ix
			push	hl
			ld		ix,uart0
$$:			ld		hl,(ix+rrx)
			ld		a,(ix+wrx)
			sub		a,l					;w-r
			pop		hl
			pop		ix
			ret

			scope
	xdef uart0_getc
uart0_getc:	push	ix
			push	hl
			ld		ix,uart0
$$:			ld		hl,(ix+rrx)
			ld		a,(ix+wrx)
			sub		a,l					;w-r
			jr		z,$B
			ld		a,(hl)
			inc		(ix+rrx)
			pop		hl
			pop		ix
			ret
			
			scope
	xdef uart0_puts						; de => c-str
uart0_puts:	push	bc
			ld		bc,0
$$:			ld		a,(de)
			or		a,a
			jr		z,$endstr
			inc		bc
			call	uart0_putc
			jr		nz,$B
			dec		bc
$endstr:	ld		hl,bc
			pop		bc
			ret

	xref uart0_gets						; de => c-str, bc = max
uart0_gets:
	; todo
	ret


	segment TEXT
$uart0itbl:	DW24 i_ms					; 000 0 Lowest Modem Status	
			DB		0
			DW24 i_tbe					; 001 1 Fifth Transmit Buffer Empty
			DB		0
			DW24 i_rdr					; 010 2 Second Receive Data Ready or Trigger Level
			DB		0
			DW24 i_rls					; 011 3 Highest Receiver Line Status		
			DB		0
			DW24 i_nc					; 100 4 not sed
			DB		0
			DW24 i_tc					; 101 5 Fourth Transmission Complete
			DB		0
			DW24 i_cto					; 110 6 Third Character Time-out
			DB		0
			DW24 i_nc					; 111 not sed
			DB		0

	segment DATA
	
putcptr		DW24		


	END
