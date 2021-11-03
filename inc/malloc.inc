		;USES: hl,ix
		xref init_heap

		xref	malloc
		; IN:BC requested size
		;OUT:HL ptr or NULL
		;	 a==0, z-flg if no memory
		
		xref 	malloc0		; fill by zero
		; IN:BC requested size
		;OUT:HL ptr or NULL
		;	 a==0, z-flg if no memory
		
		xref	free
		; IN:HL ptr
		;USES:HL
		
		xref	heapdump