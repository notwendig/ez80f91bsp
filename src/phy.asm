	nolist
	.include "emac.inc"
	.include "phy.inc"
	.include "console.inc"
	list
	
	xdef WtPhyReg
	xdef RdPhyReg
	xdef init_phy
	xdef IsPhyConnected
	xdef PhyStatus

	segment	DATA
	xdef mgdonesem
mgdonesem:		DB		0
	
	
	segment CODE
	.assume adl=1
					
$waitphy:		ld		a,(mgdonesem)
				or		a,a
				jr		z,$waitphy
				xor		a,a
				ld		(mgdonesem),a
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
				
IsPhyConnected:	push	hl
				ld		a,PHY_SREG
				call	RdPhyReg
				ld		a,LOW(PHY_LINK_ESTABLISHED)
				and		a,l
				pop		hl
				ret

PhyStatus:		push	hl
				ld		hl,msg_nolink
				call	IsPhyConnected
				jr		z,$last
				ld		hl,msg_link
				call	puts
				ld		a,PHY_DIAG_REG
				call	RdPhyReg
				ld		a,HIGH(PHY_100_MBPS)
				and		a,h
				push	hl
				ld		hl,msg_100
				jr		nz,$F
				ld		hl,msg_10
$$:				call	puts
				pop		hl
				ld		a,HIGH(PHY_FULL_DUPLEX)
				ld		hl,msg_full
				jr		nz,$last
				ld		hl,msg_half
$last:			call	puts
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
					
$$:				call	IsPhyConnected
				jr		nz,$exit
				dec		e
				jr		nz,$B
				dec		d
				jr		nz,$B
				jr		$exit
				
		segment TEXT

msg_nolink:	ascii		"Phy: No link.",0
msg_link:	ascii		"Phy: Link established.",0
msg_100:	ascii		"Phy: Link speed 100MB/s",0
msg_10:		ascii		"Phy: Link speed 10MB/s",0
msg_full:	ascii		"Phy: Full duplex",0
msg_half:	ascii		"Phy: Half duplex",0	