	include "macros.inc"
	include "queue.inc"
	
	segment code
	.assume  adl=1
	
	with QUEUE
	
		xdef	qpush	; append on ix = queue, hl = element
qpush:			SAVEIMASK
				call	qpush_save
				RESTOREIMASK
				ret
		xdef	qpush_save		
qpush_save:		push	de
				push	hl
				ld		hl,ix
				ld		de,0
				xor		a,a
				sbc		hl,de
				pop		hl
				pop		de
				jr		nz,$F
				ld		ix,hl
				ld		(ix+succsessor),hl
				ld		(ix+predecessor),hl
				ret
$$:				push	iy
				ld		iy,(ix+predecessor)
				ld		(iy+succsessor),hl
				ld		iy,hl
				ld		(iy+succsessor),ix
				ld		(ix+predecessor),hl
				pop		iy
				ret
				
		xdef	qpushh	; insert at top of in/out ix = queue, hl = element
qpushh:			SAVEIMASK
				call	qpush_save
				ld		ix,(ix+predecessor)
				RESTOREIMASK
				ret
				
		xdef	qpop	; remove top in/out ix = queue or NULL if empty
qpop:			SAVEIMASK
				call	qpop_save
				RESTOREIMASK
				ret
				
		xdef	qpop_save
qpop_save:		push	de
				push	hl
				push	iy 
				ld		iy,(ix+succsessor)
				ld		hl,(ix+predecessor)
				ld		(iy+predecessor),hl
				ld		iy,hl
				ld		de,(ix+succsessor)
				ld		(iy+succsessor),de
				or		a,a
				sbc		hl,de
				jr		nz,$F
				ex		de,hl
$$:				ld		ix,de
				pop		iy
				pop		hl
				pop		de
				ret
				