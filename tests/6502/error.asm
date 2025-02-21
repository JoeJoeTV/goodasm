;;; Port of demo1_4.asm from Programming Games for Atari 2600 by Oscar
;;; Toledo G.  He wrote for DASM, but I like it as a nice little example
;;; of a 6502 program, or if we're being pedantic, the 6507.

	.lang 6502		; Really 6507.
	.org 0xf000		; ROM is at the end of the address space.

;;; Symbols should come from another source, but this will do.
	.equ vsync  0x00	; Vertical Sync Set-Clear
	.equ vblank 0x01	; Vertical Blank Set-Clear
	.equ wsync  0x02	; Wait for Horizontal Blank
	.equ rsync  0x03	; Reset Horizontal Sync Counter
	.equ nusiz0 0x04	; Number-Size player/missile 0
	.equ nusiz1 0x05	; Number-Size player/missile 1
	.equ colup0 0x06	; Color-Luminance Player 0
	.equ colup1 0x07	; Color-Luminance Player 1
	.equ colupf 0x08	; Color-Luminance Playfield
	.equ colubk 0x09	; Color-Luminance Background

	.include error2.asm
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
	ldx #0xffffff		; ERROR line.
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
