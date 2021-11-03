; Save Interrupt State
SAVEIMASK MACRO
    ld a, i                    ; sets parity bit to value of IEF2
    push af
    di                         ; disable interrupts while loading table 
    ENDMACRO

; Restore Interrupt State
RESTOREIMASK MACRO
    pop af
    jp po, $F                  ; parity bit is IEF2
    ei
$$:	
	ENDMACRO

PUSHALL	MACRO
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy
	ex		af,af'
	exx
	push	af
	push	bc
	push	de
	push	hl
	ENDMACRO

POPALL	MACRO
	pop		hl
	pop		de
	pop		bc
	pop		af
	exx
	ex		af,af'
	pop		iy
	pop		ix
	pop		hl
	pop		de
	pop		bc
	pop		af
	ENDMACRO

SAVECONTEXT	MACRO
	push	af
	ld		a,i
	PUSHALL
	ENDMACRO

RESTORCONTEXT	MACRO
	POPALL
	jr		po,$F
	ei
$$:	pop		af
	ENDMACRO

