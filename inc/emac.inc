
EMAC_Rx_IVECT	.equ	40h
EMAC_Tx_IVECT	.equ	44h
EMAC_Sys_IVECT	.equ	48h

;Transmit Descriptor Status
TxOwner		.equ	1 << 15 ; 0 = Host (eZ80 ® ) owns, 1 = EMAC owns.
TxAbort		.equ	1 << 14 ; 1 = Packet aborted (not transmitted).
TxBPA		.equ	1 << 13 ; 1 = Back pressure applied.
TxHuge		.equ	1 << 12 ; 1 = Packet size is very large (Pkt_Size > EmacMaxf).
TxLOOR		.equ	1 << 11 ; 1 = Type/Length field is out of range (larger than 1518 bytes).
TxLCError	.equ	1 << 10 ; 1 = Type/Length field is not a Type field and it does not match the
                            ;     actual data byte length of the Ethernet packet. The data byte length is
                            ;     the number of bytes of data in the Ethernet packet between the Type/
                            ;     Length field and the FCS.
TxCrcError	.equ	1 <<  9 ; 1 = The packet contains an invalid FCS (CRC). This flag is set when
                            ;     CRCEN = 0 and the last 4 bytes of the packet are not the valid FCS.
TxPktDeferred .equ	1 <<  8 ; 1 = Packet is deferred.
TxXsDfr		.equ	1 <<  7 ; 1 = Packet is excessively deferred. (> 6071 nibble times in 100 BaseT
                            ;     or 24,287 bit times in 10 BaseT).
TxFifoUnderRun .equ	1 <<  6 ; 1 = TxFIFO experiences underrun. Check the TxAbort bit to see if the
                            ;      packet is aborted or retried.
TxLateCol	.equ	1 <<  5 ; 1 = A late collision occurs. Collision is detected at a byte count >
                            ;     EmacCfg2[5:0]. Collisions detected before the byte count reaches
                            ;     EmacCfg2[5:0] are early collisions and retried.
TxMaxCol	.equ	1 <<  4 ; 1 = The maximum number of collisions occurs. # Collisions >
                            ;     EmacCfg3[3:0]. These packets are aborted.
TxNumberOfCollisions .equ 111b   ; 0-7 This field contains the number of collisions that occur while transmitting
                            ;     the packet.

; Receive Descriptor Status
RxOK 		.equ	1 << 15 ; 1 = Packet received intact.
RxAlignError .equ	1 << 14 ; 1 = An odd  number of nibbles is received.
RxCrcError	.equ	1 << 13 ; 1 = The CRC (FCS) is in error.
RxLongEvent	.equ	1 << 12 ; 1 = A Long or Dropped Event occurs. A Long Event is when a packet
                            ;     over 50,000 bit times occurs. A Dropped Packet occurs if the minimum
                            ;     interpacket gap is not met, the preamble is not pure, and the
                            ;     EmacCfg3[PUREP] bit is set, or if a preamble over 11 bytes in length is
                            ;     detected and the EmacCfg3[LONGP] bit is set to 1.
RxPCF		.equ	1 << 11 ; 1 = The packet is a pause control frame.
RxCF		.equ	1 << 10 ; 1 = The packet is a control frame.
RxMcPkt		.equ	1 <<  9 ; 1 = The packet contains a multicast address.
RxBcPkt		.equ	1 <<  8 ; 1 = The packet contains a broadcast address.
RxVLAN		.equ	1 <<  7 ; 1 = The packet is a VLAN packet.
RxUOpCode	.equ	1 <<  6 ; 1 = An unsupported opcode is indicated in the opcode field of the
                            ;     Ethernet packet.
RxLOOR		.equ	1 <<  5 ; 1 = The Type/Length field is out of range (larger than 1518 bytes).
RxLCError	.equ	1 <<  4 ; 1 = Type/Length field is not a Type field and it does not match the
                            ;     actual data byte length of the Ethernet packet. The data byte length is
                            ;     the number of bytes of data in the Ethernet packet between the Type/
                            ;     Length field and the FCS.
RxCodeV		.equ	1 <<  3 ; 1 = A code violation is detected. The PHY asserts Rx error (RxER).
RxCEvent	.equ	1 <<  2 ; 1 = A carrier event is previously seen. This event is defined as Rx error
                            ;     RxER = 1, receive data valid (RxDV) = 0 and receive data (RxD) = Eh.
RxDvEvent	.equ	1 <<  1 ; 1 = A receive data (RxDV) event is previously seen. Indicates that the
                            ;     last Receive event is not long enough to be a valid packet.
RxOVR		.equ	1 <<  0 ; 1 = A Receive overrun occurs in this packet. An overrun occurs when
                            ;     all of the EMAC Receive buffers are in use and the Receive FIFO is full.
                            ;     The hardware ignores all incoming packets until the EmacIStat
                            ;     Register [Rx_Ovr] bit is cleared by the software. There is no indication
                            ;     as to how many packets are ignored.

MACDESCRIPTOR	struct
NP:				DS		3
PKTSIZE:		DS		2
STATUS:			DS		2
MACDESCRIPTORSZ endstruct MACDESCRIPTOR

EMACCFG			struct
macaddr:		DS		6
bufsz: 			DS		1			; reg const
bufalign:		DS		1			; buffer size -1
bp:				DS		3			; end of tx buffer, start of rx buffer
EMACCFGSZ		endstruct EMACCFG

ETHHDR			struct
destmac:		DS		6
srcmac:			DS		6
lentype:		DS		3
ETHHDRSZ		endstruct ETHHDR

EMACSTAT		struct
txfsmerr:		DS		3
mgtdone:		DS		3
rxovr:			DS		3
ctlfrm:			DS		3
pctlfrm:		DS		3
rxdone:			DS		3
physem:			DS		1
autoneg:		DS		1	
srcmac:			DS		6
rxhigh:			DS		3
txwp:			DS		3
txrp:			DS		3
rxwp:			DS		3
rxrp:			DS		3
txcf:			DS		3
txdone:			DS		3	
EMACSTATSZ		endstruct EMACSTAT

EMACERR_SETMAC	.equ	-1

; EMAC Registers
; After a system reset, all EMAC registers are set to their default values. Any Writes to
; unused registers or register bits are ignored and reads return a value of 0. For compatibil-
; ity with future revisions, unused bits within a register must always be written with a value
; of 0. Read/Write attributes, reset conditions, and bit descriptions of all of the EMAC reg-
; isters are provided in this section.
    
;                EMAC Test Register
;                The EMAC Test Register allows test functionality of the EMAC block. Available test
;                modes are defined for bits [6:0].

EMAC_TEST	.equ	0020h

;Bit                        7       6       5       4       3         2      1          0
;Reset                      0       0       0       0       0         0      0          0
;CPU Access                 R     R/W      R/W     R/W     R/W      R/W    R/W         R/W
;Bit Position       Value    Description
;7                  0        Reserved.
;6                  0        FIFO test mode disabled Normal operation.
;                   1        FIFO test mode enabled.
TEST_FIFO	.equ	01000000b
;5                  0        Select the Receive FIFO when FIFO test mode is enabled.
;                   1        Select the Transmit FIFO when FIFO test mode is enabled.
TxRx_SEL	.equ	00100000b
;4                  0        Normal operation.
;                   1        Short Cut Slot Timer Counter. Slot time is shortened to
;                            speed up simulation.
SSTC		.equ	00010000b
;3                  0        Normal operation.
;                   1        Simulation Reset.
SIMR		.equ	00001000b
;2                  0        Normal operation.
;                   1        Force Overrun error in Receive FIFO.
FRC_OVR_ERR	.equ	00000100b
;1                  0        Normal operation.
;                   1        Force Underrun error in Transmit FIFO.
FRC_UND_ERR	.equ	00000010b
;0                  0        Normal operation.
;                   1        EMAC Transmit interface is looped back into EMAC Receive
;                            interface.
LPBK		.equ	00000001b



;                EMAC Configuration Register 1
;                The EMAC Configuration Register 1 allows control of the padding, autodetection, cyclic
;                redundancy checking (CRC) control, full-duplex, field length checking, maximum packet
;                ignores, and proprietary header options. See Table 181.

EMAC_CFG1	.equ	0021h

;Bit                        7       6        5       4       3        2       1       0
;Reset                      0       0        0       0       0        0       0       0
;CPU Access                R/W     R/W     R/W      R/W     R/W     R/W     R/W      R/W
;Bit Position    Value     Description
;7               0         No padding. Assume all frames presented to EMAC have
;                          proper length.
;                1         EMAC pads all short frames by adding zeroes to the end of the
;                          data field. This bit is used in conjunction with ADPADN and
;                          VLPAD.
PADEN		.equ	10000000b
;6               0         Disable autodetection.
;                1         Enable frame detection by comparing the two bytes following
;                          the source address with 0x8100 (VLAN Protocol ID) and pad
;                          accordingly. This bit is ignored if PADEN is cleared to 0.
ADPADN		.equ	01000000b
;5               0         Do not pad all short frames.
;                1         EMAC pads all short frames to 64 bytes and append a valid
;                          CRC. This bit is ignored if PADEN is cleared to 0.
VLPAD		.equ	00100000b
;4               0         Do not append CRC.
;                1         Append CRC to every frame regardless of padding options.
CRCEN		.equ	00010000b
;3               0         HALF-DUPLEX mode. CSMA/CD is enabled.
;                1         Enable FULL-DUPLEX mode. CSMA/CD is disabled.
FULLD		.equ	00001000b
;2               0         Ignore the length field within Transmit/Receive frames.
;                1         Both Transmit and Receive frame lengths are compared to the
;                          length/type field. If the length/type field represents a length
;                          then the frame length check is performed.
FLCHK		.equ	00000100b
;1               0         Limit the Receive frame-size to the number of bytes specified
;                          in the MAXF[15:0] field.
;                1         Allow unlimited sized frames to be received. Ignore the
;                          MAXF[15:0] field.
HUGEN		.equ	00000010b
;0               0         No proprietary header. Normal operation.
;                1         Four bytes of proprietary header, ignored by CRC, exists on
;                          the front of IEEE 802.3 frames.
DCRCC		.equ	00000001b

; CRC/PAD Features of EMAC Configuration Register

; ADPADN     VLPADN       PADEN      CRCEN Result
;    0            0           0          0     No pad or CRC appended.
;    0            0           0          1     CRC appended.
;    0            0           1          0     Pad to 60 bytes if necessary; append CRC (min. size = 64).
;    0            0           1          1     Pad to 60 bytes if necessary; append CRC (min. size = 64).
;    0            1           0          0     No pad or CRC appended.
;    0            1           0          1     CRC appended.
;    0            1           1          0     Pad to 64 bytes if necessary, append CRC (min. size = 68).
;    0            1           1          1     Pad to 64 bytes if necessary, append CRC (min. size = 68).
;    1            0           0          0     No pad or CRC appended.
;    1            0           0          1     CRC appended.
;    1            0           1          0     If VLAN not detected, pad to 60, add CRC.
;                                              If VLAN detected, pad to 64, add CRC.
;    1            0           1          1     If VLAN not detected, pad to 60, add CRC.
;                                              If VLAN detected, pad to 64, add CRC.
;    1            1           0          0     No pad or CRC appended.
;    1            1           0          1     CRC appended.
;    1            1           1          0     If VLAN not detected, pad to 60, add CRC.
;                                              If VLAN detected, pad to 64, add CRC.
;    1            1           1          1     If VLAN not detected, pad to 60, add CRC.
;                                              If VLAN detected, pad to 64, add CRC.

;                EMAC Configuration Register 2
;                The EMAC Configuration Register 2 controls the behavior of the back pressure and late
;                collision data from the Descriptor table. See Table 183.

EMAC_CFG2	.equ	0022h

;Bit                        7       6        5       4        3       2        1       0
;Reset                      0       0        1       1        0       1        1       1
;CPU Access                R/W     R/W     R/W      R/W     R/W      R/W     R/W     R/W
;Bit Position    Value     Description
;7               0         Use normal back-off algorithm prior to transmitting packet. No
;                          back pressure applied.
;                1         After incidentally causing a collision during back pressure, the
;                          EMAC immediately (that is, no back-off) retransmits the packet
;                          without back-off, which reduces the chance of further collisions
;                          and ensures that the Transmit packets are sent.
BPNB		.equ	10000000b
;6               0         Enable exponential back-off.
;                1         The EMAC immediately retransmits following a collision rather
;                          than use the binary exponential backfill algorithm, as specified
;                          in the IEEE 802.3 specification.
NOBO		.equ	01000000b
;[5:0]           00h-3Fh   Sets the number of bytes after Start Frame Delimiter (SFD) for
;                          which a late collision occurs. By default, all late collisions are
;                          aborted.
LCOL_MSK	.equ	00111111b

;                EMAC Configuration Register 3
;                The EMAC Configuration Register 3 controls preamble length and value, excessive defer-
;                ment, and the number of retransmission tries. See Table 184.

EMAC_CFG3	.equ	0023h

;Bit                          7       6        5       4        3        2       1        0
;Reset                        0       0        0       0        1        1       1        1
;CPU Access                 R/W      R/W     R/W      R/W     R/W      R/W      R/W     R/W
;Bit Position     Value     Description
;7                0         The EMAC allows any preamble length as per the IEEE 802.3
;                           specification.*
;                 1         The EMAC only allows Receive packets that contain preamble
;                           fields less than 12 bytes in length.*
LONGP		.equ	10000000b
;6                0         No preamble error checking is performed.
;                 1         The EMAC verifies the content of the preamble to ensure that it
;                           contains a value of 55h and that it is error-free. Packets
;                           containing an errored preamble are discarded.
PUREP		.equ	01000000b
;5                0         The EMAC aborts when the excessive deferral limit is reached.
;                 1         The EMAC defers to the carrier indefinitely as per the IEEE
;                           802.3 specification.
XSDFR		.equ	00100000b
;4                0         Disable 10 Mbps ENDEC mode.
;                 1         Enable 10 Mbps ENDEC mode.
BITMD		.equ	00010000b
;[3:0]            0h-Fh     A programmable field specifying the number of retransmission
;                      attempts following a collision before aborting the packet due to
;                           excessive collisions.
RETRY_MSK	.equ	00001111b
;Note: IEEE 802.3 specifies a minimum of 56 bits of preamble. A maximum number of bits is not de-
;      fined. For details, see the IEEE 802.3 Specification, Section 7.2.3.2.

;                EMAC Configuration Register 4
;                The EMAC Configuration Register 4 controls pause control frame behavior, back
;                pressure, and receive frame acceptance. See Table 185.

EMAC_CFG4	.equ	0024h
;Bit                        7       6       5        4       3        2         1      0
;Reset                      0       0       0        0       0        0         0      0
;CPU Access                 R     R/W      R/W     R/W      R/W     R/W     R/W       R/W
;Bit Position    Value    Description
;7               0        Reserved.
;6               0        Do not transmit a pause control frame.
;                1        Transmit pause control frame (FULL-DUPLEX mode). TPCF
;                         continually sends pause control frames until negated.
TPCF		.equ	01000000b
;5               0        Disable back pressure.
;                1        EMAC asserts back pressure on the link. Back pressure
;                         causes preamble to be transmitted, raising carrier sense
;                         (HALF-DUPLEX mode).
THDF		.equ	00100000b
;4               0        Only accept frames that meet preset criteria (that is, address,
;                         CRC, length, etc.).
;                1        All frames are received regardless of address, CRC, length, etc.
PARF		.equ	00010000b
;3               0        EMAC ignores received pause control frames.
;                1        EMAC acts upon pause control frames received.
RxFC		.equ	00001000b
;2               0        PAUSE control frames are not allowed to be transmitted.
;                1        PAUSE control frames are allowed to be transmitted.
TxFC		.equ	00000100b
;1               0        Do not force a pause condition.
;                1        Force a pause condition while this bit is asserted.
TPAUSE		.equ	00000010b
;0               0        EMAC receiver disabled.
;                1        EMAC receiver enabled.
RxEN		.equ	00000001b

;                EMAC Station Address Register
;                The EMAC Station Address register is used for two functions. In the address recogni-
;                tion logic for Receive frames, EMAC_STAD_0-EMAC_STAD_5 are matched against
;                the sixth byte Destination Address (DA) field of the Receive frame. EMAC_STAD_0 is
;                matched against the first byte of the Receive frame, and EMAC_STAD_5 is matched
;                against the sixth byte of the Receive frame. Bit 0 of EMAC_STAD_0 (STAD[40]) is
;                matched against the first bit (Unicast/Multicast bit) of the first byte of the Receive
;                frame. This bit ordering is used to logically map the PE-MACMII station address as
;                illustrated below.
;                EMAC_STAD0[7:0] contains STAD[47:40]
;                ....
;                ....
;                EMAC_STAD5[7:0] contains STAD[7:0]

;                The second function of the EMAC Station Address registers is to provide the Source
;                Address (SA) field of Transmit Pause frames when these frames are transmitted by the
;                EMAC. EMAC_STAD_0 provides the first byte of the 6 byte SA field and
;                EMAC_STAD_5 provides the final byte of the SA field in order of transmission. The LSB
;                is the first byte sent out. The EMAC Station Address register is listed in Table 186.

EMAC_STAD_0	.equ	0025h
EMAC_STAD_1	.equ	0026h
EMAC_STAD_2	.equ	0027h
EMAC_STAD_3	.equ	0028h
EMAC_STAD_4	.equ	0029h
EMAC_STAD_5	.equ	002Ah

;Bit                          7       6       5      4       3       2      1        0
;EMAC_STAD_x Reset            0       0       0      0       0       0      0        0
;CPU Access                  R/W    R/W     R/W     R/W    R/W     R/W     R/W     R/W
;Bit Position      Value    Description
;[7:0]             00h-FFh  This 48-bit station address comprises {EMAC_STAD_5,
;EMAC_STAD_x                EMAC_STAD_4, EMAC_STAD_3, EMAC_STAD_2,
;                           EMAC_STAD_1, EMAC_STAD_0}.


;                EMAC Transmit Pause Timer Value Register Low and High Bytes
;                The Low and High bytes of the EMAC Transmit Pause Timer Value Register are inserted
;                into outgoing pause control frames. See Table 187 and Table 188.

EMAC_TPTV_L	.equ	002Bh
EMAC_TPTV_H	.equ	002Ch
;Bit                          7      6      5       4      3       2       1       0
;Reset                        0      0      0       0      0       0       0       0
;CPU Access                 R/W    R/W     R/W    R/W     R/W     R/W    R/W     R/W
;Bit Position      Value    Description
;[7:0]             00h-FFh  The 16-bit value, {EMAC_TPTV_H, EMAC_TPTV_L}, is
;EMAC_TPTV_x                inserted into outgoing pause control frames as the pause
;                           timer value upon asserting TPCF.

;                EMAC Interpacket Gap Register
;                The EMAC Interpacket Gap (IPG) is a programmable field representing the IPG between
;                back-to-back packets. It is the IPG parameter used in FULL-DUPLEX and HALF-
;                DUPLEX modes between back-to-back packets. Set this field to the appropriate number
;                of IPG bytes. The default setting of 15h represents the minimum IPG of 0.96 µs
;                (at 100 Mbps) or 9.6 μs (at 10 Mbps). See Table 191.

EMAC_IPGT	.equ	002Dh
;        Bit                         7         6          5       4        3       2        1     0
;        Reset                       0         0          0       1        0       1        0     1
;        CPU Access                 R        R/W      R/W        R/W      R/W     R/W      R/W   R/W
;Bit Position    Value     Description
;7               0         Reserved.
;[6:0]           00h-7Fh The number of bytes of IPG.
IPGT_MSK	.equ	01111111b

;                EMAC Non-Back-To-Back IPG Register Part 1
;                Part 1 of the EMAC non-back-to-back IPG Register is a programmable field representing
;                the optional carrier sense window referenced in IEEE 802.3/4.2.3.2.1 Carrier Deference. If
;                a carrier is detected during the timing of IPGR1, the EMAC defers to the carrier. If, how-
;                ever, the carrier becomes active after IPGR1, the EMAC continues timing for IPGR2 and
;                transmits, knowingly causing a collision. This collision acts to ensure fair access to the
;                medium. Its range of values is 00h to IPGR2. See Table 192. The default setting of 0Ch
;                represents the Carrier Sense Window Referencing depicted tin IEEE 802.3, Section
;                4.2.3.2.1.

EMAC_IPGR1	.equ	002Eh
;Bit                        7       6       5       4       3       2       1        0
;Reset                      0       0       0       0       1       1       0        0
;CPU Access               R/W      R/W     R/W     R/W     R/W     R/W     R/W     R/W
;Bit Position    Value    Description
;7               0        Reserved.
;[6:0]           00h-7Fh  This is a programmable field representing the optional carrier
;                         sense window referenced in IEEE 802.3/4.2.3.2.1 Carrier
;                         Deference.
IPGR1_MSK	.equ	01111111b

;                EMAC Non-Back-To-Back IPG Register Part 2
;                Part 2 of the EMAC non-back-to-back IPG Register is a programmable field representing
;                the non-back-to-back IPG. Its default is12h, which represents the minimum IPG of
;                0.96 µs at 100 Mbps or 9.6 µs at 10 Mbps. See Table 193 on page 309.

EMAC_IPGR2	.equ	002Fh
;Bit                        7        6       5       4       3        2       1       0
;Reset                      0        0       0       1       0        0       1       0
;CPU Access                 R      R/W     R/W      R/W     R/W     R/W     R/W      R/W
;Bit Position    Value      Description
;7               0          Reserved.
;[6:0]           00h-7Fh    This bit range is a programmable field representing the non-
;                           back-to-back interpacket gap.
IPGR2_MSK		.equ	01111111b

;                EMAC Maximum Frame Length Register Low and High Bytes
EMAC_MAXF_L	.equ	0030h
EMAC_MAXF_H	.equ	0031h
;Bit                         7        6         5     4     3       2       1        0
;Reset                       0        0         0     0     0       0       0       0
;CPU Access                 R/W      R/W       R/W   R/W   R/W     R/W     R/W     R/W
;Bit Position                Value        Description
;[7:0]              00h-FFh  These bits represent the Low byte of the 2 byte MAXF
;                            value, {EMAC_MAXF_H, EMAC_MAXF_L}. Bit 7 is bit 7 of
;                            the 16-bit value. Bit 0 is bit 0 (lsb) of the 16-bit value.

;                EMAC Address Filter Register
;                The EMAC Address Filter Register functions as a filter to control Promiscuous mode, and
;                multicast and broadcast messaging. See Table 196.

EMAC_AFR	.equ	0032h
;Bit                        7       6        5        4        3        2        1        0
;Reset                      0       0        0        0        0        0        0        0
;CPU Access                 R       R        R        R      R/W      R/W      R/W      R/W
;Bit Position             Value    Description
;[7:4]           0        Reserved.
;3               1        Enable Promiscuous Mode. Receive all incoming packets
;                         regardless of station address. Disables station address
;                         filtering.
;                0        Disable Promiscuous Mode.
PROM		.equ	00001000b
;2               1        Accept any multicast message. A multicast packet is
;                         determined by the first bit in the destination address. If the first
;                         LSB is a 1, it is a group address and is globally or locally
;                         administered depending on the 2nd bit. For more information,
;                         see IEEE 802.3/3.2.3.
;                0        Do not accept multicast messages of any type.
MCM			.equ	00000100b
;1               1        Accept only qualified multicast (QMC) messages as
;                         determined by the hash table.
;                0        Do not accept QMC messages.
QMC			.equ	00000010b
;                1        Accept broadcast messages. Broadcast messages have the
;                         destination address set to FFFFFFFFFFFFh.
;                0        Do not accept broadcast messages.
BCM			.equ	00000001b

;                EMAC Hash Table Register
;                The EMAC Hash Table Register represents the 8x8 hash table matrix. This table is used as
;                an option to select between different multicast addresses. If a multicast address is
;                received, the first 6 bits of the CRC are decoded and added to a table that points to a single
;                bit within the hash table matrix. If the selected bit = 1, the multicast packet is accepted. If
;                the bit = 0, the multicast packet is rejected.

EMAC_HTBL_0	.equ	0033h
EMAC_HTBL_1	.equ	0034h
EMAC_HTBL_2	.equ	0035h
EMAC_HTBL_3	.equ	0036h
EMAC_HTBL_4	.equ	0037h
EMAC_HTBL_5	.equ	0038h
EMAC_HTBL_6	.equ	0039h
EMAC_HTBL_7	.equ	003Ah

;Bit                           7       6        5       4       3       2       1        0
;EMAC_HTBL_x Reset             0       0        0       0       0       0       0        0
;CPU Access                   R/W     R/W     R/W     R/W     R/W      R/W     R/W     R/W
;Bit Position               Value     Description
;[7:0]              00h-FFh This field is the hash table. The 64 bit hash table is
;EMAC_HTBL_x                {EMAC_HTBL_7, EMAC_HTBL_6, EMAC_HTBL_5,
;                           EMAC_HTBL_4, EMAC_HTBL_3, EMAC_HTBL_2,
;                           EMAC_HTBL_1, EMAC_HTBL_0}.

;                EMAC MII Management Register
;                The EMAC MII Management Register is used to control the external PHY attached to the
;                MII.
EMAC_MIIMGT	.equ	003Bh
;Bit                        7        6         5     4        3       2       1        0
;Reset                      0        0         0     0        0       0       0        0
;CPU Access                R/W     R/W     R/W      R/W     R/W     R/W      R/W     R/W
;Bit Position    Value     Description
;7               1         Rising edge causes the CTLD control data to be transmitted to
;                          external PHY if MII is not busy. This bit is self clearing.
;                0         No operation.
LCTLD		.equ	10000000b
;6               1         Rising edge causes status to be read from external PHY via
;                          PRSD[15:0] bus if MII is not busy. This bit is self clearing.
;                0         No operation.
RSTAT		.equ	01000000b
;5               1         Scan PHY address increments upon SCAN cycle. The SCAN
;                          bit must also be set for the PHY address to increment after
;                          each scan. The scanning starts at the EMAC_FIAD and
;                          increments up to 1Fh. It then returns to the EMAC_FIAD
;                          address.
;                0         Normal operation.
SCINC		.equ	00100000b
;4               1         Perform continuous Read cycles via MII management. While in
;                          SCAN mode, the EMAC_ISTAT[MGTDONE] bit is set when the
;                          current PHY Read has completed. At this time, the
;                          EMAC_PRSD register holds the Read data and the
;                          EMAC_MIISTAT[4:0] holds the address of the PHY for which
;                          the EMAC_PRSD data pertains.
;                0         Normal operation.
SCAN		.equ	00010000b
;3               1         Suppress the MDO preamble. MDO is management data
;                          output, an internal signal driven from the MDIO pin.
;                0         Normal preamble.
SPRE		.equ	00001000b
;[2:0]           Programmable divisor that produces MDC from SCLK. MDC is the
;                management data clock pin, which clocks MDIO data to and from the
;                PHY. Its frequency is SCLK divided by the MDC clock divider.
CLKS_2		.equ	000b      ; MDC = SCLK ÷ 4.
CLKS_4		.equ	001b      ; MDC = SCLK ÷ 4.
CLKS_6		.equ	010b      ; MDC = SCLK ÷ 6.
CLKS_7		.equ	011b      ; MDC = SCLK ÷ 8.
CLKS_10		.equ	100b      ; MDC = SCLK ÷ 10.
CLKS_14		.equ	101b      ; MDC = SCLK ÷ 14.
CLKS_20		.equ	110b      ; MDC = SCLK ÷ 20.
CLKS_28		.equ	111b      ; MDC = SCLK ÷ 28.
CLKS_MSK	.equ	00000111b

;                EMAC PHY Configuration Data Register Low and High Byte
;                The Low and High bytes of the EMAC PHY Configuration Data Register represents the
;                configuration data written to the external PHY. The EMAC_CTLD_H and
;                EMAC_CTLD_L registers form a 16-bit register. These registers are loaded with data to
;                be sent via the MDIO pin to the PHY. The PHY is selected by setting the EMAC_FIAD.
;                The register inside the PHY is selected by setting EMAC_RGAD. See Table 199 and
;                Table 200 on page 315.

EMAC_CTLD_L	.equ	003Ch
EMAC_CTLD_H	.equ	003Dh
;Bit                         7      6         5      4       3       2       1       0
;Reset                       0      0         0      0       0       0       0       0
;CPU Access                 R/W    R/W      R/W    R/W     R/W     R/W      R/W     R/W
;Bit Position      Value   Description
;[7:0]             00h-FFh These bits represent the Low byte of the 2 byte PHY
;                          configuration data value, {EMAC_CTLD_H, EMAC_CTLD_L}.
;                          Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit
;                          0 (lsb) of the 16 bit value.

;                EMAC PHY Address Register
;                The EMAC PHY Address Register allows access to the external PHY registers. See
;                Table 201.

EMAC_RGAD	.equ	003Eh
;Bit                        7       6       5     4       3       2       1         0
;Reset                      0       0       0     0       0       0       0         0
;CPU Access                 R       R      R     R/W     R/W     R/W     R/W     R/W
;Bit Position   Value   Description
;[7:5]          000     Reserved.
;[4:0]          00h-1Fh Programmable 5 bit value which selects address within the
;                       selected external PHY.
RGAD_MSK	.equ	00011111b
;                EMAC PHY Unit Select Address Register
;                The EMAC PHY Unit Select Address Register allows the selection of multiple connected
;                external PHY devices. See Table 202.

EMAC_FIAD	.equ	003Fh
;Bit                         7       6        5       4        3        2       1        0
;Reset                       0       0        0       0        0        0       0        0
;CPU Access                 R        R        R      R/W     R/W      R/W     R/W      R/W
;Bit Position    Value      Description
;[7:5]           000        Reserved.
;[4:0]           00h-1Fh    Programmable 5-bit value that selects an external PHY.
FIAD_MSK	.equ	00011111b


;                EMAC Transmit Polling Timer Register
;                This register sets the Transmit Polling Period in increments of TPTMR = SYSCLK ÷ 256.
;                Whenever this register is written, the status of the Transmit Buffer Descriptor is checked
;                to determine if the EMAC owns the Transmit buffer. It then rechecks this status every
;                TPTMR (calculated by TPTMR x EMAC_PTMR[7:0]). The Transmit Polling Timer is
;                disabled if this register is set to 00h (which also disables the transmitting of packets). If a
;                transmission is in progress when EMAC_PTMR is set to 00h, the transmission will com-
;                plete. See Table 203.

EMAC_PTMR	.equ	0040h
;Bit                         7       6        5       4        3        2       1        0
;Reset                       0       0        0       0        0        0       0        0
;CPU Access                R/W      R/W     R/W      R/W     R/W      R/W     R/W      R/W
;Bit Position     Value       Description
;[7:0]            00h-FFh     The Transmit polling period.

;                EMAC Reset Control Register
;                The bit values in the EMAC Reset Control Register are not self-clearing bits. You are
;                responsible for controlling their state. See Table 204.

EMAC_RST	.equ	0041h
;Bit                        7        6       5       4      3       2       1       0
;Reset                      0        0       1       0      0       0       0       0
;CPU Access                 R       R      R/W      R/W    R/W     R/W     R/W     R/W
;Bit Position    Value    Description
;[7:6]           00       Reserved
;5               1        Software Reset Active resets Receive, Transmit, EMAC
;                         Control and EMAC MII_MGT functions
;                0        Normal operation
SRST		.equ	00100000b
;4               1        Reset Transmit function
;                0        Normal operation
HRTFN		.equ	00010000b
;3               1        Reset Receive function
;                0        Normal operation
HRRFN		.equ	00001000b
;2               1        Reset EMAC Transmit Control function
;                0        Normal operation
HRTMC		.equ	00000100b
;1               1        Reset EMAC Receive Control function
;                0        Normal operation
HRRMC		.equ	00000010b
;0               1        Reset EMAC Management function
;                0        Normal operation
HRMGT		.equ	00000001b

;                 EMAC Transmit Lower Boundary Pointer Register Low and High Bytes
;                 The EMAC Transmit Lower Boundary Pointer is set to the start of the Transmit buffer in
;                 EMAC shared memory. See Table 205 and Table 206.

EMAC_TLBP_L	.equ	0042h
;Bit                          7        6        5        4          3    2     1       0
;Reset                        0        0        0        0          0    0     0       0
;CPU Access                   R/W      R/W     R/W       R          R    R     R       R
;Bit
;Position            Value     Description
;[7:0]               00h-E0h   These bits represent the High byte of the 2 byte Transmit
;EMAC_TLBP_L                   Lower Boundary Pointer value, {EMAC_TLBP_H,
;                              EMAC_TLBP_L}. Bit 7 is bit 7 of the 16 bit value. Bit
;                              0 is bit 0 of the 16-bit value.

EMAC_TLBP_H	.equ	0043h
;Bit                          7        6        5        4          3    2     1       0
;Reset                        1        1        0        0          0    0     0       0
;CPU Access                   R        R        R      R/W      R/W     R/W   R/W     R/W
;Bit Position        Value     Description
;[7:0]               00h-1Fh   These bits represent the High byte of the 2 byte Transmit
;EMAC_TLBP_H                   Lower Boundary Pointer value, {EMAC_TLBP_H,
;                              EMAC_TLBP_L}. Bit 7 is bit 15 (msb) of the 16 bit value. Bits
;                              7:5 default to 000 on reset; bit 0 is bit 8 of the 16-bit value.

;                EMAC Boundary Pointer Register Low and High Bytes
;                The Boundary Pointer is set to the start of the Receive buffer (end of Transmit buffer +1)
;                in EMAC shared memory. This pointer is 24 bits and determined by {RAM_ADDR_U,
;                EMAC_BP_H, EMAC_BP_L}. The upper 3 bits of the EMAC_BP_H register are hard-
;                wired inside the eZ80F91 device to locate the base of EMAC shared memory. The last 5
;                bits of the EMAC_BP_L register value are hard-wired to keep the addressing aligned to
;                a 32 byte boundary. See Table 207 and Table 208.

EMAC_BP_L	.equ	0044h
;Bit                        7        6       5        4       3        2       1        0
;Reset                      0        0       0        0       0        0       0        0
;CPU Access                R/W     R/W     R/W       R        R       R        R       R
;Bit Position    Value    Description
;[7:0]           00h-FFh  These bits represent the Low byte of the 3 byte EMAC
;EMAC_BP_L                Boundary Pointer value, {EMAC_BP_U, EMAC_BP_H,
;                         EMAC_BP_L}. Bit 7 is bit 7 of the 24 bit value. Bit 0 is bit 0 of
;                         the 24 bit value.
EMAC_BP_H	.equ	0045h
;Bit                              15:13                              12:8
;Reset                      1        1       0        0       0        0       0        0
;CPU Access                 R       R        R      R/W     R/W      R/W     R/W      R/W
;Bit Position    Value   Description
;[7:0]           00h-FFh These bits represent the High byte of the 3 byte EMAC
;EMAC_BP_H               Boundary Pointer value, {EMAC_BP_U, EMAC_BP_H,
;                        EMAC_BP_L}. Bit 7 is bit 15 of the 24 bit value. Bit 0 is bit 8 of
;                        the 24 bit value.

;                EMAC Boundary Pointer Register Upper Byte
;                The EMAC Boundary Pointer Register maps directly to the RAM_ADDR_U register
;                within the eZ80F91 device. This register value is Read Only. See Table 209 on page 320.
EMAC_BP_U	.equ	0046h
;Bit                         7        6         5    4       3        2       1       0
;Reset                       1        1         1    1       1        1       1       1
;CPU Access                 R         R        R     R       R       R        R       R
;Bit Position    Value   Description
;[7:0]           00h-FFh These bits represent the upper byte of the 3 byte EMAC
;EMAC_BP_U               Boundary Pointer value, {EMAC_BP_U, EMAC_BP_H,
;                        EMAC_BP_L}. Bit 7 is bit 23 of the 24 bit value. Bit 0 is bit 16 of
;                        the 24 bit value.


;                EMAC Receive High Boundary Pointer Register Low and High Bytes
;                The Receive High Boundary Pointer Register must be set to the end of the Receive buffer
;                +1 in EMAC shared memory. This RHBP uses the same RAM_ADDR_U as the
;                EMAC_BP_U pointer above. See Table 210 and Table 211 on page 321.
EMAC_RHBP_L	.equ	0047h
;Bit                         7        6         5    4       3        2       1       0
;Reset                       0        0         0    0       0        0       0       0
;CPU Access                 R/W      R/W       R/W   R       R       R        R       R
;Bit Position       Value   Description
;[7:0]              00h-E0h These bits represent the Low byte of the 2 byte EMAC
;EMAC_RHBP_L                Receive High Boundary Pointer value, {EMAC_RHBP_H,
;                           EMAC_RHBP_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit
;                           0 (lsb) of the 16 bit value.
EMAC_RHBP_H	.equ	0048h
;Bit                          7        6         5       4       3        2     1       0
;Reset                        1        1         0       0       0        0     0       0
;CPU Access                   R        R        R      R/W      R/W      R/W   R/W     R/W
;Bit Position       Value   Description
;[7:0       ]       00h-FFh These bits represent the High byte of the 2 byte EMAC
;EMAC_RHBP_H                Receive High Boundary Pointer value, {EMAC_RHBP_H,
;                           EMAC_RHBP_L}. Bit 7 is bit 15 (msb) of the 16 bit value. Bit
;                           0 is bit 8 of the 16 bit value.

;                 EMAC Receive Read Pointer Register Low and High Bytes
;                 The Receive Read Pointer Register must be initialized to the EMAC_BP value (start of the
;                 Receive buffer). This register points to where the next Receive packet is read from. The
;                 EMAC_BP[12:5] is loaded into this register whenever the EMAC_RST [(HRRFN) is set
;                 to 1. The RxDMA block uses the Emac_Rrp[12:5] to compare to EmacRwp[12:5] for
;                 determining how many buffers remain. The result equates to the EmacBlksLeft register.
;                 See Table 212 and Table 213 on page 322.
EMAC_RRP_L	.equ	0049h
;Bit                          7        6         5       4       3        2     1       0
;Reset                        0        0         0       0       0        0     0       0
;CPU Access                  R/W      R/W       R/W     R        R        R    R        R
;Bit Position        Value   Description
;[7:0]               00h-FFh These bits represent the Low byte of the 2 byte EMAC
;EMAC_RRP_L                  Receive Read Pointer value, {EMAC_RRP_H,
;                            EMAC_RRP_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit 0
;                            (lsb) of the 16 bit value.
EMAC_RRP_H	.equ	004Ah
;Bit                        7        6       5       4       3        2       1        0
;Reset                      0        0       0       0       0        0       0       0
;CPU Access                 R       R        R      R/W     R/W     R/W     R/W      R/W
;Bit Position     Value   Description
;[7:0]            00h-FFh These bits represent the High byte of the 2-byte EMAC
;EMAC_RRP_H               Receive Read Pointer value, {EMAC_RRP_H,
;                         EMAC_RRP_L}. Bit 7 is bit 15 (msb) of the 16-bit value. Bits
;                         7:5 default to 000 on reset; bit 0 is bit 8 of the 16-bit value.


;                EMAC Buffer Size Register
;                The lower six bits of this register set the level at which the EMAC either transmits a pause
;                control frame or jams the Ethernet bus, depending on the mode selected. When each of
;                these bits contain a zero, this feature is disabled.
;                In FULL-DUPLEX mode, a Pause Control Frame is transmitted as a One-shot operation.
;                The software must free up a number of Rx buffers so that the number of buffers remaining,
;                EmacBlksLeft, is greater than TCPF_LEV.
;                In HALF-DUPLEX mode, the EMAC jams the Ethernet by sending a continuous stream
;                of hexadecimal 5s (5fh). When the software frees up the Rx buffers and the number of
;                buffers remaining, EmacBlksLeft, is greater than TCPF_LEV, the EMAC stops jamming.
EMAC_BUFSZ	.equ	004Bh
;Bit                         7       6        5       4        3        2       1        0
;Reset                       0       0        0       0        0        0       0        0
;CPU Access                R/W      R/W     R/W      R/W     R/W      R/W     R/W      R/W
;Bit Position        Value     Description
;[7:6]           00        Set EMAC Rx/Tx buffer size to 256 bytes.
;                01        Set EMAC Rx/Tx buffer size to 128 bytes.
;                10        Set EMAC Rx/Tx buffer size to 64 bytes.
;                11        Set EMAC Rx/Tx buffer size to 32 bytes.
BUFSZ_MSK	.equ	11000000b
BUFSZ256	.equ	00000000b	; 00h
BUFSZ128	.equ	01000000b	; 40h
BUFSZ64		.equ	10000000b	; 80h
BUFSZ32		.equ	11000000b	; D0h
;[5:0]           00h-3Fh Transmit Pause Control Frame level. 00h disables the
;                        hardware generated transmit pause control frame.
TPCF_LEV_MSK .equ 	00111111b

;                EMAC Interrupt Enable Register
;                Enabling the Receive Overrun interrupt allows software to detect an overrun condition
;                as soon as it occurs. If this interrupt is not set, then an overrun cannot be detected until
;                the software processes the Receive packet with the overrun and checks the Receive sta-
;                tus in the Rx descriptor table. Because the receiver is disabled by an overrun error until
;                the Rx_OVR bit is cleared in the EMAC_ISTAT register, this packet is the final packet
;                in the Receive buffer. To re-enable the receiver before all of the Receive packets are
;                processed and the Receive buffer is empty, software enables this interrupt to detect the
;                overrun condition early. As it processes the Receive packets, it re-enables the receiver
;                when the number of free buffers is greater than the number of minimum buffers. See
;                Table 215 on page 324.
EMAC_IEN	.equ	004Ch
;Bit                          7       6       5       4        3       2          1       0
;Reset                        0       0       0       0        0       0          0       0
;CPU Access                 R/W     R/W     R/W      R/W     R/W     R/W      R/W       R/W
;Bit Position    Value     Description
;7               1         Enable Transmit State Machine Error Interrupt (system interrupt).
;                0         Disable Transmit State Machine Error Interrupt (system interrupt).
TxFSMERR	.equ	10000000b
;6               1         Enable MII Management. Done Interrupt (system Interrupt).
;                0         Disable MII Management. Done Interrupt (system Interrupt).
MGTDONE		.equ	01000000b
;5               1         Enable Receive Control Frame Interrupt (Receive interrupt).
;                0         Disable Receive Control Frame Interrupt (Receive interrupt).
Rx_CF		.equ	00100000b
;4               1         Enable Receive Pause Control Frame interrupt (Receive interrupt).
;                0         Disable Receive Pause Control Frame interrupt (Receive interrupt).
Rx_PCF		.equ	00010000b
;3               1         Enable Receive Done interrupt (Receive interrupt).
;                0         Disable Receive Done interrupt (Receive interrupt)
Rx_DONE		.equ	00001000b
;2               1         Enable Receive Overrun interrupt (System interrupt).
;                0         Disable Receive Overrun interrupt (System interrupt).
Rx_OVR		.equ	00000100b
;1               1         Enable Transmit Control Frame Interrupt (Transmit interrupt).
;                0         Disable Transmit Control Frame Interrupt (Transmit interrupt).
Tx_CF		.equ	00000010b
;0               1         Enable Transmit Done interrupt (Transmit interrupt).
;                0         Disable Transmit Done Interrupt (Transmit interrupt).
Tx_DONE		.equ	00000001b

;                EMAC Interrupt Status Register
;                When a Receive overrun occurs, all incoming packets are ignored until the
;                Rx_OVR_STAT status bit is cleared by software. Consequently, software controls when
;                the receiver is re-enabled after an overrun. Enable the Rx_OVR interrupt to detect overrun
;                conditions when they occur. Clear this condition when the Rx buffers are freed to avoid
;                additional overrun errors. See Table 216.
;      Note:     Status bits are not self-clearing. Each status bit is cleared by writing a 1 into the selected
;                bit.
EMAC_ISTAT	.equ	004Dh
;Bit                         7       6         5      4        3       2        1        0
;Reset                       0       0         0      0        0       0        0        0
;CPU Access                R/W      R/W     R/W      R/W     R/W      R/W     R/W      R/W
;Bit Position          Value     Description
;7                     1         An internal error occurs in the EMAC Transmit path. The
;                                Transmit path must be reset to reset this error condition.
;                      0         Normal operation no Transmit state machine errors.
TxFSMERR_STAT	.equ	10000000b
;6                     1         The MII Management interrupt has completed a Read
;                                (RSTAT or SCAN) or a Write (LDCTLD) access to the PHY.
;                      0         The MII Management interrupt does not occur.
MGTDONE_STAT	.equ	01000000b
;5                     1         Receive Control Frame interrupt (Receive Interrupt) occurs.
;                      0         Receive Control Frame interrupt does not occur.
Rx_CF_STAT		.equ	00100000b
;4                     1         Receive Pause Control Frame interrupt (Receive Interrupt) occurs.
;                      0         Disable Receive Pause Control Frame interrupt (Receive
;                                Interrupt) does not occur.
Rx_PCF_STAT		.equ	00010000b
;3                     1         Receive Done interrupt (Receive Interrupt) occurs.
;                      0         Disable Receive Done interrupt (Receive Interrupt) does
;                                not occur.
Rx_DONE_STAT	.equ	00001000b
;2                     1         Receive Overrun interrupt (System Interrupt) occurs.
;                      0         Receive Overrun interrupt (System Interrupt) does not
;                                occur.
Rx_OVR_STAT		.equ	00000100b
;1                      1        Transmit Control Frame Interrupt (Transmit Interrupt) occurs.
;                       0        Transmit Control Frame Interrupt (Transmit Interrupt)
;                                does not occur.
Tx_CF_STAT		.equ	00000010b
;0                      1        Transmit Done interrupt (Transmit Interrupt) occurs.
;                       0        Transmit Done interrupt (Transmit Interrupt) does not
;                                occur.
Tx_DONE_STAT	.equ	00000001b


;                EMAC PHY Read Status Data Register Low and High Bytes
;                The PHY MII Management Data Register is where the data Read from the PHY is stored.
;                See Table 217 and Table 218 on page 327.

EMAC_PRSD_L 	.equ	004Eh
EMAC_PRSD_H 	.equ	004Fh

;Bit                        7        6         5     4        3       2       1          0
;Reset                      0        0         0     0        0       0       0          0
;CPU Access                 R        R        R      R       R        R       R          R
;Bit Position      Value    Description
;[7:0]             00h-FFh  These bits represent the Low byte of the 2 byte EMAC PHY
;EMAC_PRSD_L                Read Status Data value, {EMAC_PRSD_H,
;                           EMAC_PRSD_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit
;                           0 (lsb) of the 16 bit value.
;EMAC_PRSD_H         		Bit 7 is bit 15 (msb) of the 16-bit value.
;                    		Bit 0 is bit 8 of the 16-bit value.


;                EMAC MII Status Register
;                The EMAC MII Status Register is used to determine the current state of the external PHY
;                device. See Table 219.

EMAC_MIISTAT	.equ	0050h

;Bit                          7        6        5      4        3   2       1       0
;Reset                        0        0        0      0        0   0       0       0
;CPU Access                  R        R         R      R        R   R       R      R
;Bit Position    Value       Description
;7               1           MII management operation in progress Busy. This status bit
;                            goes busy whenever the LCTLD (PHY Write) or the RSTAT
;                            (PHY Read) is set in the EMAC_MIIMGT register. It is
;                            negated when the Write or Read operation to the PHY has
;                            completed. In SCAN mode, the BUSY will be asserted until
;                            the SCAN is disabled. Use the EmacIStat[MGTDONE]
;                            interrupt status bit to determine when the data is valid.
;                0           Not Busy.
BUSY  		.equ	10000000b
;6               1           Local copy of PHY Link fail bit.
;                0           PHY Link OK.
MIILF		.equ	01000000b
;5              1           MII Scan result is not valid Emac_PRSD is invalid
;                0           Emac_PRSD is valid.
NVALID		.equ	00100000b
;[4:0]           00h-1Fh 	Denotes PHY addressed in current scan cycle.
RDADR_MSK	.equ	00011111b


;                EMAC Receive Write Pointer Register Low Byte
;                The Read Only Receive-Write-Pointer register reports the current RxDMA Receive Write
;                pointer. This pointer gets initialized to EmacTLBP whenever Emac_RST bits SRST or
;                HRRTN are set. Because the size of the packet is limited to a minimum of 32 bytes, the
;                last five bits are always zero. See Table 220 and Table 221 on page 329.
;                EMAC Receive Write Pointer Register High Byte
;                Because of the size of the EMAC’s 8 KB SRAM, the upper three bits of the EMAC
;                Receive Write Pointer Register are always zero.


EMAC_RWP_L	.equ	0051h
EMAC_RWP_H	.equ	0052h
;Bit                         7        6       5       4       3       2          1      0
;Reset                       0        0       0       0       0       0          0      0
;CPU Access                  R       R       R        R       R       R          R      R
;Bit Position       Value   Description
;[7:0]              00h-E0h These bits represent the Low byte of the 2 byte EMAC
;EMAC_RWP_L                 RxDMA Receive Write Pointer value, {EMAC_RWP_H,
;                           EMAC_RWP_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit 0
;                           (lsb) of the 16 bit value.
;EMAC_RWP_H         00h-1Fh Bit 7 is bit 15 (msb) of the 16 bit value. Bit
;                           0 is bit 8 of the 16 bit value.


;                EMAC Transmit Read Pointer Register Low Byte
;                The Low byte of the Transmit Read Pointer register reports the current TxDMA Transmit
;                Read pointer.This pointer is initialized to EmacTLBP whenever Emac_RST bits SRST or
;                HRRTN are set. Because the size of the packet is limited to a minimum of 32 bytes, the
;                last five bits are always zero. See Table 222.
;                EMAC Transmit Read Pointer Register High Byte
;                Because of the size of the EMAC’s 8 KB SRAM, the upper three bits of the EMAC Trans-
;                mit Read Pointer Register are always zero. See Table 223.

EMAC_TRP_L	.equ	0053h
EMAC_TRP_H	.equ	0054h
;Bit                        7        6        5     4        3       2       1        0
;Reset                      0        0        0     0        0       0       0        0
;CPU Access                 R        R        R     R       R        R       R       R
;Bit Position     Value     Description
;[7:0]            00h-E0h These bits represent the Low byte of the 2 byte EMAC
;EMAC_TRP_L               TxDMA Transmit Read Pointer value, {EMAC_TRP_H,
;                         EMAC_TRP_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is bit 0
;                         (lsb) of the 16 bit value.
;[7:0]            00h-1Fh Bit 7 is bit 15 (msb) of the 16 bit value. Bit
;EMAC_TRP_H               0 is bit 8 of the 16 bit value.


;                EMAC Receive Blocks Left Register Low and High Bytes
;                This register reports the number of buffers left in the Receive EMAC shared memory. The
;                hardware uses this information along with the block-level set in the EMAC_BUFSZ regis-
;                ter to de0termine when to transmit a pause control frame. Software uses this information to
;                determine when it must request that a pause control frame be transmitted (by setting bit 6
;                of the EMAC_CFG4 register). For the BlksLeft logic to operate properly, the Receive buf-
;                fer must contain at least one more packet buffer than the number of packet buffers
;                required for the largest packet. That is, one packet cannot fill the entire Receive buffer.
;                Otherwise, the BlksLeft will be in error. See Table 224 and Table 225 on page 331.

EMAC_BLKSLFT_L	.equ	0055h
EMAC_BLKSLFT_H	.equ	0056h
;Bit                        7       6       5       4      3       2       1       0
;Reset                      0       0       0       0      0       0       0       0
;CPU Access                 R      R       R       R       R       R       R       R
;Bit Position           Value   Description
;[7:0]                  00h-FFh These bits represent the Low byte of the 2 byte EMAC
;EMAC_BLKSLFT_L                 Receive Blocks Left value, {EMAC_BLKSLFT_H,
;                               EMAC_BLKSLFT_L}. Bit 7 is bit 7 of the 16 bit value. Bit
;                               0 is bit 0 (lsb) of the 16 bit value.
;[7:0]                  00h-FFh These bits represent the High byte of the 2 byte EMAC
;EMAC_BLKSLFT_H                 Bit 0 is bit 8 of the 16 bit value.


;                EMAC FIFO Data Register Low and High Bytes
;                The FIFO Read/Write Test Access Data Register allows writing and reading the FIFO
;                selected by the EMAC_TEST TxRx_SEL bit when the EMAC_TEST register
;                TEST_FIFO bit is set. See Table 226 and Table 227.

EMAC_FDATA_L	.equ	0057h
EMAC_FDATA_H	.equ	0058h
;Bit                         7        6        5      4        3       2       1       0
;Reset                       X        X        X      X       X       X        X       X
;CPU Access                R/W       R/W     R/W     R/W     R/W     R/W     R/W     R/W
;Bit Position        Value    Description
;[7:0]               00h-FFh  These bits represent the Low byte of the 10 bit EMAC
;EMAC_FDATA_L                 FIFO data value, {EMAC_FDATA_H[1:0],
;                             EMAC_FDATA_L}. Bit 7 is bit 7 of the 16 bit value. Bit 0 is
;                             bit 0 (lsb) of the 10 bit value.
;CPU Access                  R        R        R      R       R       R      R/W     R/W
;[7:2]               00h      Reserved.
;[1:0]               0h-3h    These bits represent the upper two bits of the 10 bit EMAC
;EMAC_FDATA_H                 Bit 1 is bit 9 (msb) of the 16 bit value.
;                             Bit 0 is bit 8 of the 10 bit value.

;                EMAC FIFO Flags Register
;                The FIFO Flags value is set in the EMAC hardware to half full, or 16 bytes. See Table 228.

EMAC_FFLAGS	.equ	0059h
;Bit                        7        6        5      4      3       2       1       0
;Reset                      0        0        1      1      0       0       1       1
;CPU Access                 R       R         R      R      R       R       R       R
;Bit Position    Value    Description
;7               1        Transmit FIFO full
;                0        Transmit FIFO not full
TFF		.equ	10000000b
;6               0        Reserved
;5               1        Transmit FIFO almost empty
;                0        Transmit FIFO not almost empty
TFAE	.equ	00100000b
;4               1        Transmit FIFO empty
;                0        Transmit FIFO not empty
TFE		.equ	00010000b
;3               1        Receive FIFO full
;                0        Receive FIFO not full
RFF		.equ	00001000b
;2               1        Receive FIFO almost full
;                0        Receive FIFO not almost full
RFAF	.equ	00000100b
;1               1        Receive FIFO almost empty
;                0        Receive FIFO not almost empty
RFAE	.equ	00000010b
;0               1        Receive FIFO empty
;                0        Receive FIFO not empty
RFE		.equ	00000001b

