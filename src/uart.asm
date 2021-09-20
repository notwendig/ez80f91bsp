	nolist
	.include "uart.inc"
	.include "console.inc"
	list
	
	xref	_set_vector
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
i_tbe:		ld		bc,0101h				
			jr		nc,$F				; FIFO not uset
			ld		bc,0F0Fh			; FIFO takes 16 bytes
			ld		hl,(ix+rtx)
			ld		a,(ix+wtx)
$$:			cp		a,l					;w-r			
			jr		z,$F				;w<r; a=used
			ld		e,(hl)
			out0	(UART0_THR),e
			inc		l
			djnz	$B
$$:			ld		(ix+rtx),l
			ld		a,c
			cp		a,b
			ret		nz
			in0		a,(UART0_IER)
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
	
	xdef init_uart0			; iy = UARTCFG
init_uart0:	
	; Configure UART0 for 115200,8,1,n. Tx flow DSR, Rx flow DTR, RTS
			SAVEIMASK
			xor		a,a
			out0	(UART0_IER), a		; disable uart0 interrupts
			out0	(UART0_FCTL), a		; FIFO reset
			ld		ix,uart0
			lea		hl,ix+0
			lea		de,ix+1
			ld		bc,UARTSTATSZ-1 
			ld		(hl),a
			ldir
			lea		hl,iy+0
			lea		de,ix+cfg
			ld		bc,UARTCFGSZ
			ldir
			
			ld		a, UART_LCTL_DLAB	; 80h
			out0	(UART0_LCTL), a		; Enable access to BRG.
			ld		a, (ix+cfg.divisor)
			out0	(UART0_BRG_L), a	; Load low byte of BRG.
			ld		a, (ix+cfg.divisor+1)
			out0	(UART0_BRG_H), a	; Load high byte of BRG.
			ld		a,(ix+cfg.lctl)		; Load line settings
			out0	(UART0_LCTL), a
			
			ld		hl,Uart0IRQ
			push	hl
			ld		hl,UART0_IVECT
			push	hl
			call	_set_vector
			pop		hl
			pop		hl
			
			ld		hl,uart0rx
			ld		(ix+rrx),hl
			ld		(ix+wrx),hl
			ld		hl,uart0tx
			ld		(ix+rtx),hl
			ld		(ix+wtx),hl
			
			ld		a,FCTL_TRIG_FFTL14|UART_FCTL_FIFOEN|UART_FCTL_CLRTXF|UART_FCTL_CLRRXF
			out0	(UART0_FCTL), a		; Enable receiver and transmitter
			ld		a, UART_IER_RIE|UART_IER_LSIE|UART_IER_MIIE;|UART_IER_TCIE;|UART_IER_TIE
			out0	(UART0_IER), a		; enable all interrupts
			ld		a, UART_MCTL_RTS|UART_MCTL_DTR
			out0	(UART0_MCTL), a		; Select activate RTS, DTR
			RESTOREIMASK
			xor		a,a
			ret
		
			scope
	xdef uart0_putc
uart0_putc:	push	ix
			push	hl
			push	bc
			ld		c,a
			SAVEIMASK
			ld		ix,uart0
			ld		hl,(ix+wtx)
			ld		a,(ix+rtx)
			dec		a
			sub		a,l
			jr		z,$F
			ld		(hl),c
			inc		(ix+wtx)
			ld		a,1
$$:			ld		c,a
			in0		a,(UART0_MSR)
			tst		a,UART_MSR_CTS
			jr		z,$F
			in0		a,(UART0_IER)
			tst		a,UART_IER_TIE
			jr		nz,$F
			or		a,UART_IER_TIE
			out0	(UART0_IER),a
$$:			RESTOREIMASK
			ld		a,c
			or		a,a
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
	xdef uart0_puts						; hl => c-str; ret hl = chars printed
uart0_puts:	push	de
			push	hl
$$:			ld		a,(hl)
			or		a,a
			jr		z,$F
			call	uart0_putc
			jr		z,$ex
			inc		hl
			jr		$B
$$:			ld		a,0ah
			call	uart0_putc
			jr		z,$B
$ex:		pop		de
			sbc		hl,de
			pop		de
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
