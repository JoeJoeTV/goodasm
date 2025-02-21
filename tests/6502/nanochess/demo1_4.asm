;;; Port of demo1_4.asm from Programming Games for Atari 2600 by Oscar
;;; Toledo G.  He wrote for DASM, but I like it as a nice little example
;;; of a 6502 program, or if we're being pedantic, the 6507.

	.lang 6502		; Really 6507.
	.org 0xf000		; ROM is at the end of the address space.

	.include vcs.asm

start:
	sei
	cld
	ldx #0xff
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

	ldx #192
visible:
	sta @wsync
	dex
	bne visible

	lda #2
	sta @vblank
	ldx #30
bottom:
	sta @wsync
	dex
	bne bottom

	jmp show_frame

	.org 0xfffc
	.word start
	.word start
