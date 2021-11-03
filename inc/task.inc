	include "queue.inc"

	MAXPRIO	.equ	16
	
TASK		struct :QUEUE
priority:	ds	3
status:		ds	1
stacksz:	ds	3
stackbot:	ds	3
stack:		ds	3	
TASKSZ		endstruct

STATB_READY	.equ	0
STATB_RUNS	.equ	1

TASKCFG		struct
ticks:		ds	2		; cpu_clock / 16 / HZ
TASKCFGSZ	endstruct

	xref 	init_scheduler					; iy = TASKCFG
	xref 	tasksetup						; iy = task; a = priority, bc = stacksize, hl = stackbot, de = entry
	xref	addready						; iy = task
	xref	rmvready						; iy = task
