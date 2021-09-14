MGTBUSY			EQU	80h
PHY_ADDRESS		EQU 001Fh

PHY_ID1			EQU	0022h
PHY_ID2			EQU	561Bh
PHY_LINK_EST	EQU	04h

;
; Common Phy registers.
;
PHY_CREG	.equ	0
PHY_SREG	.equ	1
PHY_ID1_REG	.equ	2
PHY_ID2_REG	.equ	3
PHY_ANEG_ADV_REG	.equ	4
PHY_DIAG_REG	EQU	18
;
; MII Management Control Register (Register 0) bit definitions
;
PHY_RST	.equ	8000h
PHY_LOOPBACK	.equ	4000h
PHY_100BT	.equ	2000h
PHY_AUTO_NEG_ENABLE	.equ	1000h
PHY_POWER_DOWN	.equ	0800h
PHY_ISOLATE	.equ	0400h
PHY_RESTART_AUTO_NEG	.equ	0200h
PHY_FULLD	.equ	0100h
PHY_COLLISION_TEST	.equ	0080h
PHY_10BT	.equ	0
PHY_HALFD	.equ	0

;
; MII Management Status Register (Register 1) bit definitions
;
PHY_AUTO_NEG_COMPLETE	.equ	0020h
PHY_CAN_AUTO_NEG	.equ	0008h
PHY_LINK_ESTABLISHED	.equ	0004h

;
; Aut-Negotiation Advertisement Register (Register 4) bit definitions
;
PHY_ANEG_100_FD	.equ	0100h
PHY_ANEG_100_HD	.equ	0080h
PHY_ANEG_10_FD	.equ	0040h
PHY_ANEG_10_HD	.equ	0020h
PHY_ANEG_802_3	.equ	0001h

;
; Function prototypes
;
