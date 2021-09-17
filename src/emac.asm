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
				xor		a,a
				sbc		hl,hl
				ex		de,hl
				ld		hl,(ix+txwp)
				add		hl,bc				; hl = txbuf + msglen
				ld		e,(iy+EMACCFG.bufalign)
				adc		hl,de
				ld		a,e
				ld		de,MACDESCRIPTORSZ
				adc		hl,de
				cpl	
				and		a,l
				ld		l,a					; hl = align(txtbuf + msglen + disciptor)						
				push	hl					; hl => next
				ld		de,(ix+cfg.bp)
				sbc		hl,de				; hl = next - end of txram
				jr		c,$F				
				ld		de,(ix+cfg.tlbp)
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
$wrap:			push	bc
				ex		de,hl
				xor		a,a
				sbc		hl,de			; size - wrap
				
				ld		de,bc		
				ex		de,hl
				sbc		hl,de
$toemac:		pop		iy
				ld		hl,(iy+MACDESCRIPTOR.NP)
				ld		(ix+txwp),hl
				ld		a,TxOwner >> 8
				ld		(iy+MACDESCRIPTOR.STATUS+1),a	; emac owns this buffer
				pop		iy
				pop		ix
				ret
				
				
				
	XDEF	init_emac	; iy = EMACCFG
	
init_emac:		xor		a,a
				out0	(EMAC_IEN),a
				ld		a,SRST|HRTFN|HRRFN|HRTMC|HRRMC|HRMGT
				out0	(EMAC_RST),a
				ld		ix,emacstat
				lea		de,ix+cfg
				lea		hl,iy+0
				ld		bc,EMACCFGSZ
				ldir							; copy globale emac config to local
				; reset emac
				xor		a,a
				out0	(EMAC_RST),a
				out0	(EMAC_TEST),a
				; config mac addr
				lea		hl,ix+cfg.macaddr
				ld		c,EMAC_STAD_0
				ld		b,6
				otimr
				xor		a,a
				sbc		hl,hl
				ld		iy,hl
				add		iy,sp
				ld		hl,-6
				add		hl,sp
				ld		sp,hl
				ld		c,EMAC_STAD_0
				ld		b,6
				inimr
				dec		hl
				lea		de,ix+cfg.macaddr+6
				ld		bc,6
				cpdr
				ld		sp,iy
				jr		z,$F					; mac Ok
				ld		a,EMACERR_SETMAC		; couldn't set mac-addr
				ret
				
$$:				ld		a,ffh
				out0	(EMAC_ISTAT),a
				ld		a,(ix+cfg.bufsz)
				out0	(EMAC_BUFSZ),a
				ld		a,14h
				out0	(EMAC_TPTV_L),a
				xor		a,a
				out0	(EMAC_TPTV_H),a
				ld		iy,(ix+cfg.tlbp)
				ld		(iy+MACDESCRIPTOR.STATUS+1),a	; host owns current tx-buffer
				ld		a,iyh
				out0	(EMAC_TLBP_H),a
				ld		a,iyl
				out0	(EMAC_TLBP_L),a
				ld		(ix+txwp),iy
				ld		(ix+txrp),iy
				ld		hl,(ix+cfg.bp)
				ld		a,(ix+cfg.bp+2)
				out0	(EMAC_BP_U),a
				out0	(EMAC_BP_H),h
				out0	(EMAC_BP_L),l
				out0	(EMAC_RRP_H),h
				out0	(EMAC_RRP_L),l
				ld		(ix+rxwp),hl
				ld		(ix+rxrp),hl
				ld		hl,(ix+cfg.rhbp)
				out0	(EMAC_RHBP_H),h
				out0	(EMAC_RHBP_L),l
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
				ld		a,BCM|PROM;|QCM
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
				out0	(EMAC_IEN),a
				or		a,a
				ret


