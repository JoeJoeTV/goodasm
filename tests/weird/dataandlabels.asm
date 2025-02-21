	.lang 6805
	jmp loop
	nop
loop:	jmp loop
	nop
data:	.db 0xbc 0x00 		; data comment
	.word 0xdead		; data word
	nop
loop2:	jmp loop2
	nop			; Ending NOP
	nop                     ; space nop
	
	
	
