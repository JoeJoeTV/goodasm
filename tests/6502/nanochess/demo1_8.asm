	.lang 6502
	.org 0xf000

	.include vcs.asm

	
start:
	sei
	cld
	ldx #0xFF
	txs
	lda #0x00
clear:
	sta @0, x
	dex
	bne clear

show_frame:	
	lda #0x88
	sta @colubk

	sta @wsync
	lda #2
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0
	sta @vsync

	ldx #36
top:
	sta @wsync
	dex
	bne top
	lda #0
	sta @vblank

	ldx #96
visible:
	sta @wsync
	dex
	bne visible

	lda #0xF8
	sta @colubk

	ldx #96
visible2:
	sta @wsync
	dex
	bne visible2

	lda #2
	sta @vblank
	ldx #30
bottom:
	sta @wsync
	dex
	bne bottom

	jmp show_frame

	.org 0xFFFC
	.word start
	.word start
