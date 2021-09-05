
	nolist
	.include "ez80f91.inc"
	.include "intvect.inc"
	.include "uart.inc"
	list
	
; System hardware config
	XREF __CS0_LBR_INIT_PARAM 
	XREF __CS0_UBR_INIT_PARAM 
	XREF __CS0_CTL_INIT_PARAM 
	XREF __CS0_BMC_INIT_PARAM 
	XREF __CS1_LBR_INIT_PARAM 
	XREF __CS1_UBR_INIT_PARAM 
	XREF __CS1_CTL_INIT_PARAM 
	XREF __CS1_BMC_INIT_PARAM 
	XREF __CS2_LBR_INIT_PARAM 
	XREF __CS2_UBR_INIT_PARAM 
	XREF __CS2_CTL_INIT_PARAM 
	XREF __CS2_BMC_INIT_PARAM 
	XREF __CS3_LBR_INIT_PARAM 
	XREF __CS3_UBR_INIT_PARAM 
	XREF __CS3_CTL_INIT_PARAM 
	XREF __CS3_BMC_INIT_PARAM 
	XREF __RAM_CTL_INIT_PARAM 
	XREF __RAM_ADDR_U_INIT_PARAM 
	XREF __FLASH_CTL_INIT_PARAM 
	XREF __FLASH_ADDR_U_INIT_PARAM 

	XREF _SYS_CLK_FREQ 

	XREF _OSC_FREQ 
	XREF _SYS_CLK_SRC 
	XREF _OSC_FREQ_MULT 
	XREF __PLL_CTL0_INIT_PARAM 

; System memory config
	XREF __low_romdata 
	XREF __low_data 
	XREF __len_data 
	XREF __low_bss 
	XREF __len_bss 
	XREF __stack 
	XREF __heaptop 
	XREF __heapbot    
	XREF __low_romcode 
	XREF __low_code 
	XREF __len_code 
	XREF __copy_code_to_ram

	; void * _set_vector(unsigned int vector, void(*handler)(void));
	xref _set_vector
	xref init_bsp
	xref critical_bgn
	xref critical_end
	xref fatal