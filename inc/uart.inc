
.include "macros.inc"

	xref _SYS_CLK_FREQ
	xref init_uart0						; iy = SYSCFG
	xref uart0_putc						; a = char
	xref uart0_kbhit					; a!=0 => key pressed 
	xref uart0_getc						; char => a
	xref uart0_puts						; hl => c-str; ret hl = chars printed
	xref uart0_gets						; de => c-str, bc = max
	xref uart0_flush
	
UARTCFG			struct
divisor			DS		2
lctl			DB		1
UARTCFGSZ		endstruct UARTCFG

UARTSTAT		struct	
cfg				.tag	UARTCFG
rrx:			DS		3
wrx:			DS		3	
rtx:			DS		3
wtx:			DS		3
dropedrx:		DS		3
dropedtx:		DS		3
lsr_err:		DS		3
lsr_bl:			DS		3
lsr_fe:			DS		3
lsr_pe:			DS		3
lsr_oe:			DS		3
lsr_dr:			DS		3
cto:			DS		3
UARTSTATSZ		endstruct UARTSTAT


UART0RXLOW		.equ	20

; UART Registers

;UARTs
UART0_IVECT		.equ	70h
UART1_IVECT 	.equ	74h

UART0_BRG_L		.equ	C0h
UART1_BRG_L		.equ	D0h				; 00h-FFh These bits represent the Low byte of the 16-bit BRG divider value. The complete
										;	BRG divisor value is returned by {UART_BRG_H,UART_BRG_L}
UART0_BRG_H		.equ	C1h
UART1_BRG_H		.equ	D1h				; 00h-FFh TThese bits represent the High byte of the 16-bit BRG divider value. The complete
										;	BRG divisor value is returned by {UART_BRG_H,UART_BRG_L}.
UART0_THR		.equ	C0h
UART1_THR		.equ	D0h				; 00h-FFh Transmit data byte.

UART0_RBR		.equ	C0h
UART1_RBR		.equ	D0h				; 00h-FFh Receive data byte.

UART0_IER		.equ	C1h
UART1_IER		.equ	D1h				; The UARTx_IER register is used to enable and disable the UART interrupts. The UARTx_IER
										;	registers share the same I/O addresses as the UARTx_BRG_H registers.
UART_IER_RIE	.equ	00000001b		; 1	Receive interrupt and receiver time-out interrupt are enabled. Interrupt is generated
										;	if the FIFO/buffer contains data ready to be read or if the receiver times out.
										; 0	Receive interrupt is disabled.
UART_IER_TIE	.equ	00000010b		; 1	Transmit interrupt is enabled. Interrupt is generated when the transmit FIFO/buffer is
										;	empty indicating no more bytes available for transmission.
										; 0	Transmit interrupt is disabled
UART_IER_LSIE	.equ	00000100b		; 1	Line status interrupt is enabled for receive data errors: incorrect parity bit received,
										;	framing error, overrun error, or break detection.
										; 0	Line status interrupt is disabled.
UART_IER_MIIE	.equ	00001000b		; 1	Modem interrupt on edge detect of status inputs is enabled.
										; 0 Modem interrupt on edge detect of status inputs is disabled.
UART_IER_TCIE	.equ	00010000b		; 1	Transmission complete interrupt is generated when both the transmit hold register and
										;	the transmit shift register are empty.
										; 0 Transmission complete interrupt is disabled.

UART0_IIR		.equ	C2h
UART1_IIR		.equ	D2h				; The Read Only UARTx_IIR register allows you to check whether the FIFO is enabled and the
										;	status of interrupts. These registers share the same I/O addresses as the UARTx_FCTL
										;	registers.
UART_IIR_FSTS	.equ	10000000b		; 0	FIFO is disabled.
										; 1	IFO is enabled.
UART_IIR_INSTS	.equ	00001110b		; Interrupt Status Code mask.
IIR_INSTS_RLS	.equ	00000110b		;	011 3 Highest Receiver Line Status
IIR_INSTS_RDRTL	.equ	00000100b		;	010 2 Second Receive Data Ready or Trigger Level
IIR_INSTS_CTOUT	.equ	00001100b		;	110 6 Third Character Time-out
IIR_INSTS_TXCPL	.equ	00001010b		;	101 5 Fourth Transmission Complete
IIR_INSTS_TXBEM	.equ	00000010b		;	001 1 Fifth Transmit Buffer Empty
IIR_INSTS_MODST	.equ	00000000b		;	000 0 Lowest Modem Status
UART_IIR_INTBIT	.equ	00000001b		; 0	There is an active interrupt source within the UART.
										; 1 There is not an active interrupt source within the UART.

UART0_FCTL		.equ	C2h
UART1_FCTL		.equ	D2h				; This register is used to monitor trigger levels, clear FIFO pointers, and enable or disable
										; the FIFO. The UARTx_FCTL registers share the same I/O addresses as the UARTx_IIR registers.
UART_FCTL_TRIG	.equ	11000000b		;
FCTL_TRIG_FFTL1	.equ	00000000b		; 00 Receive FIFO trigger level set to 1. Receive data interrupt is generated when there is
										;	 1 byte in the FIFO. Valid only if FIFO is enabled.
FCTL_TRIG_FFTL4	.equ	01000000b		; 01 Receive FIFO trigger level set to 4. Receive data interrupt is generated when there are
										;	 4bytes in the FIFO. Valid only if FIFO is enabled.
FCTL_TRIG_FFTL8	.equ	10000000b		; 10 Receive FIFO trigger level set to 8. Receive data interrupt is generated when there are
										;	 8 bytes in the FIFO. Valid only if FIFO is enabled.
FCTL_TRIG_FFTL14 .equ	11000000b		; 11 Receive FIFO trigger level set to 14. Receive data interrupt is generated when there are
										;	 14 bytes in the FIFO. Valid only if FIFO is enabled.
UART_FCTL_CLRTXF .equ	00000100b		; 0 Transmit Disable. This register bit works differently than the standard 16550 UART.
										;	This bit must be set to transmit data. When it is reset the transmit FIFO logic is reset
										;	along with the associated transmit logic to keep them in sync. This bit is now
										;	persistent-it does not self clear and it must remain at 1 to transmit data.
										; 1 Transmit Enable.
UART_FCTL_CLRRXF .equ	00000010b		; 0 Receive Disable. This register bit works differently than the standard 16550 UART. This
										;	bit must be set to receive data. When it is reset the receive FIFO logic is reset along
										;	with the associated receive logic to keep them in sync and avoid the previous version’s
										;	lookup problem. This bit is now persistent-it does not self clear and it must remain at
										;	1 to receive data.
										; 1 Receive Enable.
UART_FCTL_FIFOEN .equ	00000001b		; 0 FIFOs are not used.
										; 1 Receive and transmit FIFOs are used-You must clear the FIFO logic using bits 1 and 2.
										;	First enable the FIFOs by setting bit 0 to 1 then enable the receiver and transmitter by
										;	setting bits 1 and 2.

UART0_LCTL		.equ	C3h
UART1_LCTL		.equ	D3h				; This register is used to control the communication control parameters.
UART_LCTL_DLAB 	.equ	10000000b		; 0 Access to the UART registers at I/O addresses C0h, C1h, D0h and D1h is enabled.
										; 1 Access to the Baud Rate Generator registers at I/O addresses C0h, C1h, D0h, D1h is enabled.
UART_LCTL_SB 	.equ	01000000b		; 0 Do not send a BREAK signal.
										; 1 Send Break. UART sends continuous zeroes on the transmit output from the next bit boundary.
										;	The transmit data in the transmit shift register is ignored. After forcing this bit High,
										;	the TxD output is 0 only after the bit boundary is reached. Just before forcing TxD to
										;	0, the transmit FIFO is cleared. Any new data written to the transmit FIFO during a
										;	break must be written only after the THRE bit of UARTx_LSR register goes High.
										;	This new data is transmitted after the UART recovers from the break. After the break
										;	is removed, the UART recovers from the break for the next BRG edge.
UART_LCTL_FPE 	.equ	00100000b		; 0 Do not force a parity error.
										; 1 Force a parity error. When this bit and the parity enable bit (pen) are both 1, an
										;	incorrect parity bit is transmitted with the data byte.
UART_LCTL_EPS 	.equ	00010000b		; 0 Even Parity Select. Use odd parity for transmit and receive. The total number of 1 bits
										;	in the transmit data plus parity bit is odd. Used as SPACE bit in Multidrop Mode.
										;	Note: Receive Parity is set to SPACE in multidrop mode.
										; 1 Use even parity for transmit and receive. The total number of 1 bits in the transmit
										;	data plus parity bit is even. Used as MARK bit in Multidrop Mode.
UART_LCTL_PEN 	.equ	00001000b		; 0 Parity bit transmit and receive is disabled.
										; 1 Parity bit transmit and receive is enabled. For transmit, a parity bit is generated and
										;	transmitted with every data character. For receive, the parity is checked for every
										;	incoming data character. In Multidrop Mode, receive parity is checked for space parity.
UART_LCTL_CHAR 	.equ	00000111b		; 000-111 UART Character Parameter Selection.
LCTL_CHAR_5_1 	.equ	00000000b		;	5 Data-bits 1 Stop-bit
LCTL_CHAR_6_1 	.equ	00000001b		;	6 Data-bits 1 Stop-bit
LCTL_CHAR_7_1 	.equ	00000010b		; 	7 Data-bits 1 Stop-bit
LCTL_CHAR_8_1 	.equ	00000011b		; 	8 Data-bits 1 Stop-bit
LCTL_CHAR_5_2 	.equ	00000100b		; 	5 Data-bits 2 Stop-bit
LCTL_CHAR_6_2 	.equ	00000101b		; 	6 Data-bits 2 Stop-bit
LCTL_CHAR_7_2 	.equ	00000110b		; 	7 Data-bits 2 Stop-bit
LCTL_CHAR_8_2 	.equ	00000111b		;	8 Data-bits 2 Stop-bit

UART0_MCTL		.equ	C4h				; UART Modem Control Register
UART1_MCTL		.equ	D4h				;	This register is used to control and check the modem status.
UART_MCTL_POLARITY .equ	01000000b		; 0 TxD and RxD signals—Normal Polarity.
										; 1 Invert Polarity of TxD and RxD signals.
UART_MCTL_MDM	.equ	00100000b		; 0 Multidrop Mode disabled.
										; 1 Multidrop Mode enabled.
UART_MCTL_LOOP	.equ	00010000b		; 0 LOOP BACK mode is not enabled.
										; 1 LOOP BACK mode is enabled.
										;	The UART operates in internal LOOP BACK mode. The transmit data output port is
										;	disconnected from the internal transmit data output and set to 1. The receive data input
										;	port is disconnected and internal receive data is connected to internal transmit data.
										;	The modem status input ports are disconnected and the four bits of the modem control
										;	register are connected as modem status inputs. The two modem control output ports
										;	(OUT1&2) are set to their inactive state.
UART_MCTL_OUT2	.equ	00001000b		; 0-1 No function in normal operation. In LOOP BACK mode, this bit is connected to the DCD
										;	bit in the UART Status
UART_MCTL_OUT1	.equ	00000100b		; 0-1 No function in normal operation. In LOOP BACK mode, this bit is connected to the RI
										;	bit in the UART Status
UART_MCTL_RTS	.equ	00000010b		; 0-1 Request to Send. In normal operation, the RTS output port is the inverse of this bit.
										;	In LOOP BACK mode, this bit is connected to the CTS bit in the UART Status Register.
UART_MCTL_DTR	.equ	00000001b		; 0-1 Data Terminal Ready. In normal operation, the DTR output port is the inverse of this
										;	bit. In LOOP BACK mode, this bit is connected to the DSR bit in the UART Status Register.

UART0_LSR		.equ	C5h				; UART Line Status Register
UART1_LSR		.equ	D5h				;	This register is used to show the status of UART interrupts and registers.
UART_LSR_ERR	.equ	10000000b		; 0 Always 0 when operating in with the FIFO disabled. With the FIFO enabled, this bit is
										;	reset when the UARTx_LSR register is read and there are no more bytes with error status
										;	in the FIFO.
										; 1 Error detected in the FIFO. There is at least 1 parity, framing or break indication
										;	error in the FIFO.
UART_LSR_TEMT	.equ	01000000b		; 0 Transmit holding register/FIFO is not empty or transmit shift register is not empty or
										;	transmitter is not idle.
										; 1 Transmit holding register/FIFO and transmit shift register are empty and the transmitter
										;	is idle. This bit cannot be set to 1 during the BREAK condition. This bit only becomes 1
										;	after the BREAK command is removed.
UART_LSR_THRE	.equ	00100000b		; 0 Transmit holding register/FIFO is not empty.
										; 1 Transmit holding register/FIFO are empty. This bit cannot be set to 1 during the BREAK
										;	condition. This bit only becomes 1 after the BREAK command is removed.
UART_LSR_BL		.equ	00010000b		; 0 Receiver does not detect a BREAK condition. This bit is reset to 0 when the UARTx_LSR
										;	register is read.
										; 1 Receiver detects a BREAK condition on the receive input line. This bit is 1 if the
										;	duration of BREAK condition on the receive data is longer than one character transmission
										;	time, the time depends on the programming of the UARTx_LSR register. In case of FIFO only
										;	one null character is loaded into the receiver FIFO with the framing error. The framing
										;	error is revealed to the eZ80 whenever that particular data is read from the receiver FIFO.
UART_LSR_FE		.equ	00001000b		; 0 No framing error detected for character at the top of the FIFO. This bit is reset to 0
										;	when the UARTx_LSR register is read.
										; 1 Framing error detected for the character at the top of the FIFO. This bit is set to 1
										;	when the stop bit following the data/parity bit is logic 0.
UART_LSR_PE		.equ	00000100b		; 0 The received character at the top of the FIFO does not contain a parity error. In
										;	multidrop mode, this indicates that the received character is a data byte. This bit is
										;	reset to 0 when the UARTx_LSR register is read.
										; 1 The received character at the top of the FIFO contains a parity error. In multidrop mode,
										;	this indicates that the received character is an address byte.
UART_LSR_OE		.equ	00000010b		; 0 The received character at the top of the FIFO does not contain an overrun error.
										;	This bit is reset to 0 when the UARTx_LSR register is read.
										; 1 Overrun error is detected. If the FIFO is not enabled, this indicates that the data in
										;	the receive buffer register was not read before the next character was transferred into
										;	the receiver buffer register. If the FIFO is enabled, this indicates the FIFO was already
										;	full when an additional character was received by the receiver shift register. The
										;	character in the receiver shift register is not put into the receiver FIFO.
UART_LSR_DR		.equ	00000001b		; 0 This bit is reset to 0 when the UARTx_RBR register is read or all bytes are read from the
										;	receiver FIFO.
										; 1 Data ready. If the FIFO is not enabled, this bit is set to 1 when a complete incoming
										;	character is transferred into the receiver buffer register from the receiver shift
										;	register. If the FIFO is enabled, this bit is set to 1 when a character is received and
										;	transferred to the receiver FIFO.

UART0_MSR		.equ	C6h				; UART Modem Status Register
UART1_MSR		.equ	D6h				;	This register is used to show the status of the UART signals.
UART_MSR_DCD	.equ	10000000b		; 0-1 Data Carrier Detect. In NORMAL mode, this bit reflects the inverted state of the DCDx
										;	input pin. In LOOP BACK mode, this bit reflects the value of the UARTx_MCTL[3] = out2.
UART_MSR_RI		.equ	01000000b		; 0-1 Ring Indicator. In NORMAL mode, this bit reflects the inverted state of the RIx input
										;	pin. In LOOP BACK mode, this bit reflects the value of the UARTx_MCTL[2] = out1.
UART_MSR_DSR	.equ	00100000b		; 0-1 Data Set Ready. In NORMAL mode, this bit reflects the inverted state of the DSRx input
										;	pin. In LOOP BACK mode, this bit reflects the value of the UARTx_MCTL[0] = DTR.
UART_MSR_CTS	.equ	00010000b		; 0-1 Clear to Send. In NORMAL mode, this bit reflects the inverted state of the CTSx
										;	input pin. In LOOP BACK mode, this bit reflects the value of the UARTx_MCTL[1] = RTS.
UART_MSR_DDCD	.equ	00001000b		; 0-1 Delta Status Change of DCD. This bit is set to 1 whenever the DCDx pin changes
										;	state. This bit is reset to 0 when the UARTx_MSR register is read.
UART_MSR_TERI	.equ	00000100b		; 0-1 Trailing Edge Change on RI. This bit is set to 1 whenever a falling edge is detected
										;	on the RIx pin. This bit is reset to 0 when the UARTx_MSR register is read.
UART_MSR_DDSR	.equ	00000010b		; 0-1 Delta Status Change of DSR. This bit is set to 1 whenever the DSRx pin changes state.
										;	This bit is reset to 0 when the UARTx_MSR register is read.
UART_MSR_DCTS	.equ	00000001b		; 0-1 Delta Status Change of CTS. This bit is set to 1 whenever the CTSx pin changes state.
										;	This bit is reset to 0 when the UARTx_MSRs register is read.

UART0_SPR		.equ	C7h				; UART Scratch Pad Register
UART1_SPR		.equ	D7h				;	The UARTx_SPR register is used by the system as a general-purpose Read/Write register.
