;;; This is a fragment of the F Card's memory, from a nineties
;;; glitching tutorial.  The 1a10 fragment is from ROM and the
;;; 1100 fragment is from EEPROM.

	
	.lang fcard
	.org 0x1a10
	
RESET:	
        ldx #0x007e		;Reset vector points here.
loop:	
        clr @0x007f, x		; \
        dec x			;  - Clear RAM in 1512 Cycles
        bne loop		; / 
        sei			; Disable Interrupts
        bclr bit7, @0x0008	; Clear bit 7 of Misc Reg
        bset bit0, @0x0000	; Set the I/O pin High
        lda @0x052a		; loc 52a -> acc
        sta @0x008d		; acc -> mem[8d]
        bset bit0, @0x0004	; Set I/O direction to Out.
        lda #0x0004		; 4 -> acc
        sta @0x008a		; acc -> mem[8a]
        rsp			; Reset the Stack Pointer
        jsr 0x1100		; Check for valid card in EEPROM code.
        bcs 0x1a39		; Branch if carry to 1a39.
        jmp 0x0e78		; Kernel entry.
