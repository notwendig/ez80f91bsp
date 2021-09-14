	nolist
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

	XDEF	send_emac	; de => eth-msg, bc = msg-len
send_emac:		push	ix
				push	iy
				push	de
				ld		ix,emacstat
				ld		iy,syscfg.emac
				xor		a,a
				sbc		hl,hl
				ex		de,hl
				ld		hl,(ix+txwp)
				add		hl,bc
				ld		e,(iy+EMACCFG.bufalign)
				adc		hl,de
				ld		a,e
				ld		de,MACDESCRIPTORSZ
				adc		hl,de
				cpl	
				and		a,l
				ld		l,a							
				push	hl				;hl => next
				ld		de,(iy+EMACCFG.bp)
				sbc		hl,de
				jr		c,$F
				ld		de,#(__RAM_ADDR_U_INIT_PARAM << 16) + C000h
				add		hl,de
				ex		(sp),hl
$$:				ex		(sp),iy			; next
				xor		a,a
				ld		(iy+MACDESCRIPTOR.STATUS+1),a	; host owns next buffer
				ex		(sp),iy
				pop		hl				; next
				ex		(sp),iy
				push	iy				; eth-msg
				ld		iy,(ix+txwp)
				ld		(iy+MACDESCRIPTOR.NP),hl
				ld		(iy+MACDESCRIPTOR.PKTSIZE),c
				ld		(iy+MACDESCRIPTOR.PKTSIZE+1),b
				lea		de,iy+MACDESCRIPTORSZ			; dest
				ld		hl,(iy+EMACCFG.bp)
				xor		a,a
				sbc		hl,de							; max unwrap
				push	hl
				sbc		hl,bc
				pop		hl
				jr		c,$wrap
				pop		hl				; srcs
				ldir
				jr		$toemac
$wrap:				
$toemac:		pop		iy
				ld		hl,(iy+MACDESCRIPTOR.NP)
				ld		(ix+txwp),hl
				ld		a,TxOwner >> 8
				ld		(iy+MACDESCRIPTOR.STATUS+1),a	; emac owns this buffer
				pop		iy
				pop		ix
				ret
				
				
				
	XDEF	init_emac
init_emac:		xor		a,a
				out0	(EMAC_IEN),a
				ld		a,SRST|HRTFN|HRRFN|HRTMC|HRRMC|HRMGT
				out0	(EMAC_RST),a
				ld		ix,emacstat
				ld		iy,syscfg.emac
				lea		hl,ix+0
				lea		de,ix+1
				ld		bc,EMACSTATSZ-1
				xor		a,a
				ld		(hl),a
				ldir
				out0	(EMAC_RST),a
				out0	(EMAC_TEST),a
				lea		hl,iy+EMACCFG.macaddr
				ld		c,EMAC_STAD_0
				ld		b,6
				otimr
				lea		hl,ix+srcmac
				ld		c,EMAC_STAD_0
				ld		b,6
				inimr
				lea		hl,iy+EMACCFG.macaddr
				lea		de,ix+srcmac
				ld		bc,6
				cpir
				jr		z,$F
				ld		a,EMACERR_SETMAC
				ret
$$:				ld		a,ffh
				out0	(EMAC_ISTAT),a
				ld		a,(iy+EMACCFG.bufsz)
				out0	(EMAC_BUFSZ),a
				ld		hl,14h
				out0	(EMAC_TPTV_L),l
				out0	(EMAC_TPTV_H),h
				push	iy
				ld		iy,#(__RAM_ADDR_U_INIT_PARAM << 16) + C000h
				xor		a,a
				ld		(iy+MACDESCRIPTOR.STATUS+1),a
				ld		hl,iy
				pop		iy
				out0	(EMAC_TLBP_H),h
				out0	(EMAC_TLBP_L),l
				ld		(ix+txwp),hl
				ld		(ix+txrp),hl
				ld		hl,(ix+EMACCFG.bp)
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
	
;ifdef MULTICAST
					     
;endif
				out0	(EMAC_AFR),a
				xor		a,a
				out0	(EMAC_MAXF_L),a
				ld		a,6
				out0	(EMAC_MAXF_H),a		; MTU 1536
				ld		a,CLKS_20
				out0	(EMAC_MIIMGT),a
				ld		a,HRTFN|HRRFN|HRTMC|HRRMC
				out0	(EMAC_RST),a
				call	init_phy
				ld		a,0
				out0	(EMAC_RST),a
				ret		z
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

