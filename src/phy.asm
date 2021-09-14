	nolist
	.include "emac.inc"
	.include "phy.inc"
	list
	
	xdef WtPhyReg
	xdef RdPhyReg
	xdef init_phy
		
	segment CODE
	.assume adl=1
					
$waitphy:		nop
				in0		a, (EMAC_MIISTAT)	;read status
				and		a, MGTBUSY			;see if data transfered
				jr		nz, $waitphy
				ret

;
; write phy register in A with the value in HL
;
WtPhyReg:		out0	(EMAC_CTLD_L), l
				out0	(EMAC_CTLD_H), h
				out0	(EMAC_RGAD), a
				in0		a, (EMAC_MIIMGT);	;read the current settings
				or		a, LCTLD			;indicate a write
				out0	(EMAC_MIIMGT), a	;start a write
				jr		$waitphy
				

;
; read phy register in A store the value in HL (UMB is undifined)
;
RdPhyReg:		out0	(EMAC_RGAD), a		;set the register to read
				in0		a, (EMAC_MIIMGT);	;read the current settings
				or		a, RSTAT			;indicate a read
				out0	(EMAC_MIIMGT), a	;start the read
				call	$waitphy
				or		a,a
				sbc		hl,hl
				in0		h, (EMAC_PRSD_H)	;read high byte of data
				in0		l, (EMAC_PRSD_L)	;read low byte of data
				ret
				
phyconnected:	push	hl
				ld		a,PHY_SREG
				call	RdPhyReg
				ld		a,PHY_LINK_ESTABLISHED
				and		a,l
				pop		hl
				ret
	

init_phy:		push	de
				push	hl
				ld		a,PHY_ADDRESS
				out0	(EMAC_FIAD),a
				ld		a,PHY_ID1_REG
				call	RdPhyReg
				ld		de,PHY_ID1
				xor		a,a
				sbc		hl,de
				jr		nz,$exit
				ld		a,PHY_ID2_REG
				call	RdPhyReg
				ld		de,PHY_ID2
				xor		a,a
				sbc		hl,de
				jr		nz,$exit
				ld		hl,PHY_RST
				ld		a,PHY_CREG
				call	WtPhyReg
				ld		a,PHY_SREG
				call	RdPhyReg
				ld		a,l
				tst		a,PHY_CAN_AUTO_NEG
				jr		nz,$F
				ld		a,PADEN|CRCEN
				out0	(EMAC_CFG1),a
				ld		hl,PHY_100BT|PHY_FULLD
				jr		$j0
$$:				ld		hl,PHY_ANEG_100_FD|PHY_ANEG_100_HD|PHY_ANEG_10_FD|PHY_ANEG_10_HD|PHY_ANEG_802_3
				ld		a,PHY_ANEG_ADV_REG
				call	WtPhyReg
				ld		hl,PHY_AUTO_NEG_ENABLE|PHY_RESTART_AUTO_NEG
$j0:			ld		a,PHY_CREG
				call	WtPhyReg
				xor		a,a
				sbc		hl,hl
				ex		de,hl
$waitlink:		nop
				djnz	$waitlink
				ld		a,PHY_SREG
				call	RdPhyReg
				ld		a,l
				tst		a,PHY_AUTO_NEG_COMPLETE|PHY_LINK_ESTABLISHED
				jr		nz,$F
				dec		e
				jr		nz,$waitlink
				dec		d
				jr		nz,$waitlink
				xor		a,a
$exit:			or		a,a
				pop		hl
				pop		de
         				ret
					
$$:				call	phyconnected
				jr		nz,$exit
				dec		e
				jr		nz,$B
				dec		d
				jr		nz,$B
				jr		$exit
				