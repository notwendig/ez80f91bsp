
ifndef _QUEUE_INC_
_QUEUE_INC_	.equ	1


	xref	qpush			; append in ix = queue, in hl = element, use af
	xref	qpush_save
	xref	qpushh			; neu head in ix = queue, in hl = element, use af
	xref	qpop			; in/out ix = queue or NULL if empty, use af
	xref	qpop_save
		
QUEUE		struct
predecessor: ds	3
succsessor:	ds	3
QUEUESZ		endstruct

endif ; _QUEUE_INC_