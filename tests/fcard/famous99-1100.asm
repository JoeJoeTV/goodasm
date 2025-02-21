
	.lang fcard
	.org 0x1100
EEPROM:	
	bset bit6, @0x008e	; Set 8e bit 6, In Post flag.
        lda @0x10fe		; 55h>cc (They can't write this byte.)
        beq famous99		; If acc=0, go to the death loop.
        lda @0x0528		; loc 528 -> acc.
        sta @0x008b		; acc -> loc 8b.
        eor @0x0529		; xor loc 529 with acc (529 sb = FA)
        inc a			; acc+1 (acc sb=ff after the next xor)
        bne famous99		; if acc is !0, card is 99'ed.
        brset bit1, @0x8b, 0x1117
        bclr bit6, @0x008e	; clear 8e bit 6.
        rts			; Return to caller, card is OK.

famous99:	
        lda #0x0099		; Famous 99 code.
        jsr 0x1e7f		; Transmit the byte.
        bra famous99		; Keep transmitting forever.
