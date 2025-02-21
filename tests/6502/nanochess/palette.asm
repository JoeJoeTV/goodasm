	;
	; Atari palette viewer
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Nov/04/2022.
	; Revision date: Nov/13/2022. Added NTSC definition to choose NTSC/PAL.
	;

	.lang 6502
	.include vcs.asm

NTSC		= 1	; Define to 1 for NTSC, 0 for PAL

@frame_LOW	= $0080
@frame_HIGH	= $0081
COLUMN		= $0082

	.org 0xf000
start:
	sei
	cld
	ldx #$FF
	txs
	lda #$00
clear:
	sta 0,X
	dex
	bne clear

show_frame:
	lda #$88
	sta @colubk

	sta @wsync
	lda #2
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0
	sta @vsync

    IF NTSC
	ldx #36
    ELSE
	ldx #61
    ENDIF
TOP:
	sta @wsync
	dex
	bne TOP
	lda #0
	sta @vblank

	lda @frame_HIGH
	ASL
	and #$0E
	sta COLUMN

	ldx #0
visible:
	sta @wsync
	lda #$0F
	sta @pf1
	TXA
	clc
	adc COLUMN
	sta @colupf

	sta @wsync
	sta @wsync
	sta @wsync

	sta @wsync
	sta @wsync
	sta @wsync
	sta @wsync

	sta @wsync
	sta @wsync
	lda #$00
	sta @pf1
	sta @wsync
	sta @wsync

	TXA
	clc
	adc #$10
	tax
	BCC visible

	lda #2
	sta @vblank
    IF NTSC
	ldx #30
    ELSE
	ldx #55
    ENDIF
bottom:
	sta @wsync
	dex
	bne bottom

	inc @frame_LOW
	bne L1
	inc @frame_HIGH
L1:
	jmp show_frame

	.org $FFFC
	.word start
	.word start
