
.list off
    .INCLUDE "eZ80F91.INC"    ; CPU Equates
	.INCLUDE "intvect.inc"
	.INCLUDE "bsp.inc"
	.list on

	segment DATA

syscfg		.tag	SYSCFG
syscfg:
			;emac		.TAG	EMACCFG 
			db 	00h,90h,23h,00h,01h,01h	; macaddr
$$:	
	DS		SYSCFGSZ
	
$critical:	DB		0
	
	segment CODE
	.assume ADL=1
	
xdef init_bsp
init_bsp:
			di
			in0		a,(PD_ALT2)
			or		a,ffh
			out0	(PD_ALT2),a
			in0		a,(PD_ALT1)
			or		a,0
			out0	(PD_ALT1),a
			in0		a,(PD_DDR)
			or		a,ffh
			out0	(PD_DDR),a
			call	init_uart0
       		call	init_emac
			ei
			ret

; Save Interrupt State
	xdef critical_bgn
critical_bgn:di	
			push	af
			ld		a,($critical)
			inc		a
			ld		($critical),a
			call	z,fatal
			pop		af
			ret

; Restore Interrupt State
	xdef critical_end
critical_end:push	af
			ld		a,($critical)
			or		a,a
			call	z,fatal
			dec		a
			ld		($critical),a
			jr		nz,$F
			ei
$$:			pop		af
			ret

	xdef fatal
fatal:		di
			halt
			slp