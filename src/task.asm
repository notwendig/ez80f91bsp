	.include "bsp.inc"
	
		segment DATA
		xdef readyq
readyq:		blkp 	MAXPRIO,0
current:	dw24	3	
toptask:	dw24	3

		segment BSS
ticker:		ds	3
	
		segment CODE
		.assume adl=1
		
		with TASKCFG

		xdef init_scheduler	
init_scheduler:di
			ld		ix,readyq
			ld		b,MAXPRIO
			xor		a,a
			sbc		hl,hl
			ld		(ticker),hl
$$:			ld		de,(ix)
			adc		hl,de
			jr		nz,$F
			lea		ix,ix+3
			djnz	$B
			jp		fatal
$$:			ld		(toptask),hl		
			out0	(TMR0_IER),a
			ld		a,(ix+ticks+1)
			out0	(TMR0_DR_H),a
			out0	(TMR0_RR_H),a
			ld		a,(ix+ticks)
			out0	(TMR0_DR_L),a
			out0	(TMR0_RR_L),a	
			ld		hl,Tmr0IRQ
			push	hl
			ld		hl,TMR0_IVECT
			push	hl
			call	_set_vector
			pop		hl
			pop		hl
			
			ld		a,8fh
			out0	(TMR0_CTL),a
			ld		a,1
			out0	(TMR0_IER),a
			xor		a,a
			jr		$run
		with TASK
			
yield:		push	af
			ld		a,i					; save interrupt state
			di 
			jr		$swap
			
Tmr0IRQ:	push	af
			push	hl
			ld		hl,(ticker)
			inc		hl
			ld		(ticker),hl
			pop		hl
			in0		a,(TMR0_IIR)
			xor		a,a				; interrupt was enabled
$swap:		PUSHALL
			ld		iy,(current)
			xor		a,a
			sbc		hl,hl
			add		hl,sp
			ld		(iy+stack),hl
			res		STATB_RUNS,(iy+status)
$run:		ld		iy,(toptask)
			ld		hl,(iy+QUEUE.succsessor)
			ld		(toptask),hl
			ld		(current),iy
			ld		hl,(iy+stack)
			ld		sp,hl		
			set		STATB_RUNS,(iy+status)
			POPALL	
			jp		po,$F
			pop		af
			ei
			reti
$$:			pop		af
			reti
			
		; iy = task
	xdef	addready
addready:	ld		a,(iy+status)
			and		a,1 << STATB_READY
			ret		nz						; is just on ready-state
			SAVEIMASK
			push	ix
			push	hl
			ld		ix,(iy+priority)		; ptr to priority-queue for the tasks to get ready
			ld		ix,(ix)					; top of priority-queue (may be NULL)
			ld		hl,iy
			call	qpush_save
			ld		hl,(iy+priority)
			ld		(hl),ix
			pop		hl
			pop		ix
			set		STATB_READY,(iy+status)
			RESTOREIMASK
			ret
			
		; iy = task
	xdef	rmvready
rmvready:	ld		a,(iy+status)
			and		a,1 << STATB_READY
			ret		z
			push	ix
			ld		ix,iy
			call	qpop
			pop		ix
			ret

	xdef	tasksetup						; iy = task; a = priority, bc = stacksize, hl = stackbot, de = entry
tasksetup:	push	de
			push	hl						
			push	bc						; stack size
			lea		hl,iy+0
			lea		de,iy+1
			ld		bc,TASKSZ-1
			ld		(hl),0
			ldir
			or		a,a
			sbc		hl,hl
			ld		l,a
			ld		h,3
			mlt		hl
			ld		de,readyq
			add		hl,de
			ld		(iy+priority),hl
			pop		bc	
			ld		(iy+stacksz),bc
			pop		hl
			ld		(iy+stackbot),hl
			add		hl,bc
			ld		ix,hl
			sbc		hl,hl
			pop		de
			ld		(ix-3),de				; entry
			ld		(ix-6),hl				; af
			set		2,l
			ld		(ix-9),hl				; i-status
			lea		hl,ix-36
			ld		(iy+stack),hl
			ret