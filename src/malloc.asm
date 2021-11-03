	.include "macros.inc"
	.include "malloc.inc"
	.include "console.inc"
	xref fatal
	
HEAPBLK		struct
size:		ds	3
stat:		ds	1	
HEAPBLKSZ  	endstruct HEAPBLK
			
			segment BSS

heapwalk:	ds		3
			
			segment code
			.assume adl=1
			
			with	HEAPBLK
				
	xdef init_heap	; hl=heapstart,bc=heapsize
	;USES: hl,ix
init_heap:	ld		ix,hl
			ld		(heapwalk),ix	; Start of heap chain
			ld		de,HEAPBLKSZ
			ld		(ix+size),de
			ld		(ix+stat),1		; used. First entry to stop downward concatenation for free-function
			add		ix,de
			sla		e
			ld		hl,bc
			sbc		hl,de
			ld		(ix+size),hl
			ld		(ix+stat),0		; unused. Secound entry for the whole free heap
			ex		de,hl
			add		ix,de
			sbc		hl,hl
			ld		(ix+size),hl
			ld		(ix+stat),1		; used. Thirt entry to stop downward concatenation for free-function
			ret
			
		SCOPE
		xdef malloc
		; IN:BC requested size
		;OUT:HL ptr or NULL
		;	 a==0, z-flg if no memory
malloc:		push	bc
			push	de
			push	ix
			push	iy
			SAVEIMASK
			xor		a,a
			sbc		hl,hl
			adc		hl,bc
			jr		z,$retmalloc		; requested size == 0
			ld		de,HEAPBLKSZ
			adc		hl,de
			jr		nc,$F				; requested size + blockchain-haeder overruns addr-range
			ccf
			sbc		hl,hl
			jr		$retmalloc

$$:			ld		bc,hl				; used are wanted size + blockchain haeder
			ld		ix,(heapwalk)		; start of free block chain

$mnext:		add		ix,de			
			xor		a,a
			sbc		hl,hl
			ld		de,(ix+size)
			adc		hl,de
			jr		z,$retmalloc		; end of chain
			or		a,(ix+stat)
			jr		nz,$mnext			; used
			sbc		hl,bc
			jr		c,$mnext			; to small
			ex		de,hl
			ld		hl,-HEAPBLKSZ
			adc		hl,de
			jr		nc,$F				; not enough rest to split the block

			ld		(ix+size),de		; rest of free block
			add		ix,de				; wanted block
			ld		(ix+size),bc		; block size
$$:			ld		a,1
			ld		(ix+stat),a			; flag as used
			lea		hl,ix+HEAPBLKSZ		; user memory starts here

$retmalloc: ld		b,a
			RESTOREIMASK
			ld		a,b
			or		a,a					; set return flag.
			pop		iy
			pop		ix
			pop		de
			pop		bc
			ret

		SCOPE
		
		xdef malloc0
		; IN:BC requested size
		;OUT:HL ptr or NULL
		;OUT:AF 0/z no memory.ffh/nz OK
malloc0:	call	malloc
			ret		z
			push	bc
			push	de
			push	hl
			xor		a,a
			ld		(hl),a
			ld		de,hl
			ldi		
			jp		po,$F
			inc		hl
			ldir
$$:			pop		hl
			pop		de
			pop		bc
			or		a,ffh
			ret
		
		SCOPE
		xdef free
		; IN:HL ptr
		;USES:HL
free:		SAVEIMASK
			push	bc
			push	de
			push	ix
			push	iy
			ld		de,HEAPBLKSZ
			xor		a,a
			sbc		hl,de
			ex		de,hl		; de ptr to block-chain haeder
			jr		c,$retfree	; possible NULL ptr
			ld		iy,(heapwalk)
			jr		$F

$fnext:		ld		iy,ix	
$$:			ld		bc,(iy+size)
			xor		a,a
			sbc		hl,hl
			adc		hl,bc
			call	z,fatal		; end of chain
			ld		ix,iy
			add		ix,bc		; ix is the successor
			cp		a,(ix+stat)
			jr		z,$fnext	; not in use try next
			ld		hl,ix
			xor		a,a
			sbc		hl,de
			jr		nz,$fnext	; not the wanted block

			ld		bc,(ix+size)
			ld		(ix+stat),a	; mark as unused
			cp		a,(iy+stat) ; check if predecessor even unused
			jr		nz,$F		; no
			ld		hl,(iy+size); unite with predecessor
			add		hl,bc
			ld		(iy+size),hl
			ld		ix,iy
			ld		bc,hl
$$:			ld		iy,ix
			add		iy,bc
			cp		a,(iy+stat); check if successor even unused
			jr		nz,$retfree; no
			ld		hl,(iy+size); unite with successor
			add		hl,bc
			ld		(ix+size),hl
$retfree:	pop		iy
			pop		ix
			pop		de
			pop		bc
			RESTOREIMASK
			ret

	xdef	heapdump
heapdump:	push	ix
			push	de
			push	hl
			ld		ix,(heapwalk)
			
$$:			ld		hl,ix
			call	prnt_u24_hex
			ld		a,','
			call	putc
			ld		hl,(ix+size)
			call	prnt_u24_hex
			ld		a,','
			call	putc
			ld		a,'0'
			add		a,(ix+stat)
			call	putc
			ld		a,0ah
			call	putc
			ex		de,hl
			xor		a,a
			sbc		hl,hl
			adc		hl,de
			jr		z,$F
			add		ix,de
			jr		$B
$$:			call	flush
			pop		hl
			pop		de
			pop		ix
			ret
	end
	
	
