	nolist
	EMACREGISTER .equ 1
	.include "phy.inc"
	.include "bsp.inc"
	list
	
	
	XREF	_set_vector
	XREF	__FLASH_CTL_INIT_PARAM
	segment DATA

	segment BSS

emacstat	.tag EMACSTAT
emacstat:		DS		EMACSTATSZ
	
	segment CODE
	.assume  ADL=1
	
	with  EMACSTAT

$rxcontrolframe:
$rxpcontrolframe:
$rxdoneframe:
				ret
		
$sysirq:		push	af
				push	bc
				push	de
				push	hl
				push	ix
				ld		ix,emacstat
				in0		e,(EMAC_ISTAT)
				ld		a,e
				tst		a, TxFSMERR_STAT
				jr		z,$F
				ld		a,HRTFN|HRTMC
				out0	(EMAC_RST),a
				xor		a,a
				out0	(EMAC_RST),a
				ld		hl,(ix+txfsmerr)
				inc		hl
				ld		(ix+txfsmerr),hl
				ld		a, TxFSMERR_STAT
				out0	(EMAC_ISTAT),a
				ld		a,e
$$:				tst		a,MGTDONE_STAT
				jr		z,$F
				call	$give_physem
				ld		hl,(ix+mgtdone)
				inc		hl
				ld		(ix+mgtdone),hl
				ld		a, MGTDONE_STAT
				out0	(EMAC_ISTAT),a
				ld		a,e
$$:				tst		a,Rx_OVR_STAT
				jr		z,$F
				ld		hl,(ix+rxovr)
				inc		hl
				ld		(ix+rxovr),hl
				ld		a, Rx_OVR_STAT
				out0	(EMAC_ISTAT),a
$$:				pop		ix
				pop		hl
				pop		de
				pop		bc
				pop		af
				ei
				reti
	
$rxirq:			push	af
				push	bc
				push	de
				push	hl
				push	ix
				ld		ix,emacstat
				in0		e,(EMAC_ISTAT)
				ld		a,e
				tst		a,Rx_CF_STAT
				jr		z,$F
				ld		hl,(ix+ctlfrm)
				inc		hl
				ld		(ix+ctlfrm),hl
				call	$rxcontrolframe
				ld		a, Rx_CF_STAT
				out0	(EMAC_ISTAT),a
				ld		a,e
$$:				tst		a,Rx_PCF_STAT
				jr		z,$F
				ld		hl,(ix+pctlfrm)
				inc		hl
				ld		(ix+pctlfrm),hl
				call	$rxpcontrolframe
				ld		a, Rx_PCF_STAT
				out0	(EMAC_ISTAT),a
				ld		a,e				
 $$:			tst		a,Rx_DONE_STAT
				jr		z,$F
				ld		hl,(ix+rxdone)
				inc		hl
				ld		(ix+rxdone),hl
				call	$rxdoneframe
				ld		a, Rx_DONE_STAT
				out0	(EMAC_ISTAT),a				
$$:				pop		ix
				pop		hl
				pop		de
				pop		bc
				pop		af
				ei				
				reti
				
$txirq:			push	af
				push	hl
				push	ix
				ld		ix,emacstat
				in0		e,(EMAC_ISTAT)
				ld		a,e
				tst		a,Tx_CF
				jr		z,$F
				ld		hl,(ix+txcf)
				inc		hl
				ld		(ix+txcf),hl
				ld		a,Tx_CF
				out0	(EMAC_ISTAT),a
				ld		a,e
$$:				tst		a,Tx_DONE
				jr		z,$F
				ld		hl,(ix+txdone)
				inc		hl
				ld		(ix+txdone),hl
				ld		a,Tx_DONE
				out0	(EMAC_ISTAT),a

$$:				pop		ix
				pop		hl
				pop		af
				ei				
				reti

$$:				inc		(ix+physem)
$take_physem:	dec		(ix+physem)
				jr		nz,$B
				dec		(ix+physem)
				ret
					
$wait_physem:	in0		a, (EMAC_MIISTAT)	;read status
				and		a, MGTBUSY			;see if data transfered
				jr		nz, $wait_physem
				inc		(ix+physem)
				ret

$give_physem:	inc		(ix+physem)
				ret
				

;
; write phy register in A with the value in BC
;
$WtPhyReg:	;	call	$take_physem
				out0	(EMAC_CTLD_L), c
				out0	(EMAC_CTLD_H), b
				out0	(EMAC_RGAD), a
				in0		a, (EMAC_MIIMGT);	;read the current settings
				or		a, 80h				;indicate a write
				out0	(EMAC_MIIMGT), a	;start a write
$$:				call	$wait_physem
			;	call	$give_physem
				ret
				
				in0		a, (EMAC_MIISTAT)	;read status
				and		a, MGTBUSY			;see if data transfered
				jr		nz, $B
				ret

;
; read phy register in A store the value in BC (UMB is undifined)
;
$RdPhyReg:	;	call	$take_physem
				out0	(EMAC_RGAD), a		;set the register to read
				in0		a, (EMAC_MIIMGT);	;read the current settings
				or		a, 40h				;indicate a read
				out0	(EMAC_MIIMGT), a	;start the read
				call	$wait_physem
				ld		bc, 0
				in0		b, (EMAC_PRSD_H)	;read high byte of data
				in0		c, (EMAC_PRSD_L)	;read low byte of data
			;	call	$give_physem
				ret

$init_phy:		ld		a,PHY_ADDRESS
				out0	(EMAC_FIAD),a
				ld		bc,0
				ld		a,PHY_ID1_REG
				call	$RdPhyReg
				ld		hl,PHY_ID1
				xor		a,a
				sbc		hl,bc
				ret		nz
				ld		a,PHY_ID2_REG
				call	$RdPhyReg
				ld		hl,PHY_ID2
				xor		a,a
				sbc		hl,bc
				ret		nz
				ld		bc,PHY_RST
				ld		a,PHY_CREG
				call	$WtPhyReg
				ld		a,PHY_SREG
				call	$RdPhyReg
				ld		a,c
				ld		bc,PHY_100BT|PHY_FULLD
				tst		a,PHY_CAN_AUTO_NEG
				jr		nz,$F
				ld		a,PADEN|CRCEN
				out0	(EMAC_CFG1),a
				jr		$j0
$$:				inc		(ix+autoneg)
				ld		bc,PHY_ANEG_100_FD|PHY_ANEG_100_HD|PHY_ANEG_10_FD|PHY_ANEG_10_HD|PHY_ANEG_802_3
				ld		a,PHY_ANEG_ADV_REG
				call	$WtPhyReg
				ld		bc,PHY_AUTO_NEG_ENABLE|PHY_RESTART_AUTO_NEG
$j0:			ld		a,PHY_CREG
				call	$WtPhyReg
				ld		bc,0
$waitlink:		nop
				djnz	$waitlink
				push	bc
				ld		a,PHY_DIAG_REG
				call	$RdPhyReg
				ld		a,c
				tst		a,PHY_AUTO_NEG_COMPLETE|PHY_LINK_ESTABLISHED
				pop		bc
				jr		nz,$cklink
				dec		c
				jr		nz,$waitlink
$$				or		a,ffh
				ret
$cklink:		tst		a,PHY_LINK_ESTABLISHED 
				jr		z,$B
				xor		a,a
				ret
				
				 
	
	XDEF	init_emac
init_emac:		xor		a,a
				out0	(EMAC_IEN),a
				ld		a,SRST|HRTFN|HRRFN|HRTMC|HRRMC|HRMGT
				out0	(EMAC_RST),a
				ld		ix,emacstat
				ld		iy,syscfg
				lea		hl,ix+0
				lea		de,ix+1
				ld		bc,EMACSTATSZ-1
				xor		a,a
				ld		(hl),a
				dec		de
				ldir
				out0	(EMAC_RST),a
				out0	(EMAC_TEST),a
				lea		hl,iy+SYSCFG.emac.macaddr
				ld		c,EMAC_STAD_0
				ld		b,6
				otimr
				lea		hl,ix+srcmac
				ld		c,EMAC_STAD_0
				ld		b,6
				inimr
				lea		hl,iy+SYSCFG.emac.macaddr
				lea		de,ix+srcmac
				ld		bc,6
				cpir
				jr		z,$F
				ld		a,EMACERR_SETMAC
				ret
$$:				ld		a,ffh
				out0	(EMAC_ISTAT),a
				ld		a,BUFSZ32
				out0	(EMAC_BUFSZ),a
				ld		hl,14h
				out0	(EMAC_TPTV_L),l
				out0	(EMAC_TPTV_H),h
				ld		iy,#(__RAM_ADDR_U_INIT_PARAM << 16)+ C000h
				xor		a,a
				ld		(iy+MACDESCRIPTOR.STATUS),a
				ld		hl,iy
				out0	(EMAC_TLBP_H),h
				out0	(EMAC_TLBP_L),l
				ld		(ix+txwp),hl
				ld		(ix+txrp),hl
				ld		a,10h
				add		a,h
				ld		h,a
				ld		a,__RAM_ADDR_U_INIT_PARAM
				out0	(EMAC_BP_U),a
				out0	(EMAC_BP_H),h
				out0	(EMAC_BP_L),l
				out0	(EMAC_RRP_H),h
				out0	(EMAC_RRP_L),l
				ld		(ix+rxwp),hl
				ld		(ix+rxrp),hl
				ld		a,10h
				add		a,h
				ld		h,a
				out0	(EMAC_RHBP_H),h
				out0	(EMAC_RHBP_L),l
				ld		(ix+rxhigh),hl
				ld		a,1
				out0	(EMAC_PTMR),a
				ld		a,PADEN | CRCEN | FULLD
				out0	(EMAC_CFG1),a
				ld		a,56
				out0	(EMAC_CFG2),a
				ld		a,15
				out0	(EMAC_CFG3),a
				ld		a,RxEN
				out0	(EMAC_CFG4),a
				ld		a,BCM
ifdef MULTICAST
					      |QMC;
endif
				out0	(EMAC_AFR),a
				xor		a,a
				out0	(EMAC_MAXF_L),a
				ld		a,6
				out0	(EMAC_MAXF_H),a		; MTU 1536
				ld		a,CLKS_20
				out0	(EMAC_MIIMGT),a
				ld		a,HRTFN|HRRFN|HRTMC|HRRMC
				out0	(EMAC_RST),a
				call	$init_phy
				ld		a,0
				out0	(EMAC_RST),a
				ret		nz
				dec		a
				out0	(EMAC_ISTAT),a
				ld		hl,$rxirq
				push	hl
				ld		hl,EMAC_Rx_IVECT
				push	hl
				call	_set_vector
				pop		hl
				pop		hl
				ld		hl,$txirq
				push	hl
				ld		hl,EMAC_Tx_IVECT
				push	hl
				call	_set_vector
				pop		hl
				pop		hl
				ld		hl,$sysirq
				push	hl
				ld		hl,EMAC_Sys_IVECT
				push	hl
				call	_set_vector
				pop		hl
				pop		hl
				ld		a,TxFSMERR|MGTDONE|Rx_CF|Rx_PCF|Rx_DONE|Rx_OVR|Tx_CF|Tx_DONE
				out		(EMAC_IEN),a
				or		a,a
				ret

