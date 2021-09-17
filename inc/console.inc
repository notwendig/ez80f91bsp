	xref putc		; a = char
	xref getc		; char => a
	xref puts		; hl => c-str; ret hl = chars printed
	xref gets		; de => c-str, bc = max
	xref kbhit		; a!=0 => key pressed 
	
	xref strlen		;hl=>c-str, ret len in bc; zf=1 strlen 0