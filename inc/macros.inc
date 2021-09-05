; Save Interrupt State
SAVEIMASK MACRO
    ld a, i                    ; sets parity bit to value of IEF2
    push af
    di                         ; disable interrupts while loading table 
    ENDMACRO

; Restore Interrupt State
RESTOREIMASK MACRO
    pop af
    jp po, $+5                 ; parity bit is IEF2
    ei
    ENDMACRO