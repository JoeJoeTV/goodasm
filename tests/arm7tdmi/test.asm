	.lang arm7tdmi

entry:
	b late
	bx r12
	b entry

;;; Data processing instructions.
	add r1, r2, r3, ror #3
	add r1, r2, r3, lsl #0
	add r1, r2, r3		; Same as LSL #0, but shift is omitted.
	add r1, r2, r3, lsl #7
	add r1, r2, r3, lsr r7
	add r1, r2, r3, lsr #32	; Secretly LSR #32.
	add r1, r2, #0xde
	;; Rotated immediates.
	mov r7, #0x2000000a
	
late:
	bx r12
	
