	;
	; 8K ROM demo (bank 1)
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/17/2022.
	;

	.lang 6502
	.include vcs.asm

EFFECT1	= $0080

	; Bank switching
BANK0	= $FFF8		; Bank 0
BANK1	= $FFF9		; Bank 1

	.org 0xf000
start:
	sta BANK1
	sei
	cld
	jmp start2

ALT_CODE:
	sta BANK0
	nop
	nop
	nop

EXT_BANK:
	sta BANK1
	jmp show_frame

start2:
	ldx #$FF
	txs
	lda #$00
clear:
	sta 0,X
	dex
	bne clear

	lda #$00	; Allow to read console switches
	sta @swacnt
	sta @swbcnt

show_frame:
	lda #$88
	sta @colubk

	sta @wsync
	lda #2
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #42		; Time for NTSC top border
	sta @tim64t
	lda #0
	sta @vsync

wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

	sta @wsync
	lda #0		; Disable blanking
	sta @vblank

	ldx #192
visible:
	sta @wsync
	dex
	bne visible

	sta @wsync
	lda #2
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	jmp ALT_CODE

	.org $FFFC
	.word start
	.word start
