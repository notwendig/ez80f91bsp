
ifndef _PHY_INC_
_PHY_INC_	.equ	1

; PHY AMD79C874

MGTBUSY				.equ	80h
PHY_ADDRESS			.equ 	001Fh

PHY_ID1				.equ	0022h
PHY_ID2				.equ	561Bh
PHY_LINK_EST		.equ	04h

;
; Common Phy registers.
;
PHY_CREG			.equ	0		; MII Management Control Register
PHY_SREG			.equ	1       ; MII Management Status Register
PHY_ID1_REG			.equ	2       ; PHY Identifier 1 Register
PHY_ID2_REG			.equ	3       ; PHY Identifier 2 Register
PHY_ANEG_ADV_REG	.equ	4       ; Auto-Negotiation Advertisement Register
PHY_ANEG_LPA_REG	.equ	5		; Auto-Negotiation Link Partner Ability Register
PHY_ANEG_EXP_REG	.equ	6		; Auto-Negotiation Expansion Register
PHY_NPADV_REG		.equ	7		; Next Page Advertisement Register
						;	8-15	  Reserved
PHY_MISCFEAT_REG	.equ	16		; Miscellaneous Features Register
PHY_ICTL_ISTAT_REG	.equ	17		; Interrupt Control/Status Register
PHY_DIAG_REG		.equ	18      ; Diagnostic Register
PHY_MP_LB_REG		.equ	19		; Power Management & Loopback Register
						;	20		; Reserved
PHY_MODE_CTRL_REG	.equ	21      ; Mode Control Register
						;	22		  Reserved
PHY_DISCON_CNT_REG	.equ	23		; Disconnect Counter
PHY_RXERR_CNT		.equ	24		; Receive Error Counter
						;	25-31 	  Reserved

;
; MII Management Control Register (Register 0) bit definitions
;
PHY_RST				.equ	8000h
PHY_LOOPBACK		.equ	4000h
PHY_100BT			.equ	2000h
PHY_AUTO_NEG_ENABLE	.equ	1000h
PHY_POWER_DOWN		.equ	0800h
PHY_ISOLATE			.equ	0400h
PHY_RESTART_AUTO_NEG .equ	0200h
PHY_FULLD			.equ	0100h
PHY_COLLISION_TEST	.equ	0080h
PHY_10BT			.equ	0
PHY_HALFD			.equ	0

;
; Diagnostic Register (Register 18) bit definitions
;
PHY_FULL_DUPLEX		.equ	0800h
PHY_100_MBPS		.equ	0400h
PHY_RX_PASS			.equ	0200h
PHY_RX_LOCK			.equ	0100h

;
; MII Management Status Register (Register 1) bit definitions
;
PHY_AUTO_NEG_COMPLETE .equ	0020h
PHY_CAN_AUTO_NEG	.equ	0008h
PHY_LINK_ESTABLISHED .equ	0004h

;
; Aut-Negotiation Advertisement Register (Register 4) bit definitions
;
PHY_ANEG_100_FD		.equ	0100h
PHY_ANEG_100_HD		.equ	0080h
PHY_ANEG_10_FD		.equ	0040h
PHY_ANEG_10_HD		.equ	0020h
PHY_ANEG_802_3		.equ	0001h

;
; Mode Control Register (Register 21) bit definitions
;
PHY_GPSI_EN			.equ	0800h

	xref mgdonesem

;
; Function prototypes
;
	xref init_phy
;
; write phy register in A with the value in BC
;
	xref WtPhyReg

;
; read phy register in A store the value in BC (UMB is undifined)
;
	xref RdPhyReg
	
	xref PhyStatus		; print Phy status
	
	xref IsPhyConnected	; a != 0 connected
	
endif ; _PHY_INC_	