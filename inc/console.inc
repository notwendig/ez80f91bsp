	xref putc		; a = char
	xref getc		; char => a
	xref puts		; hl => c-str; ret hl = chars printed
	xref gets		; de => c-str, bc = max
	xref kbhit		; a!=0 => key pressed 

	xref prnt_u24_int	; IN: hl=int, d=size, e=fill
	xref prnt_u8_hex	; IN: a=int
	xref prnt_u16_hex	; IN: hl=int
	xref prnt_u24_hex	; IN: hl=int
	xref prnt_mac		; hl => mac	
	
	xref strlen		;hl=>c-str, ret len in bc; zf=1 strlen 0