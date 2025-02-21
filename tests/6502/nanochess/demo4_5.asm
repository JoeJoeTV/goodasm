	;
	; Sound effect demo #5
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/06/2022.
	;

	.lang 6502
	.include vcs.asm

EFFECT1:	EQU $80

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
	sta @wsync	; Resynchronize on last border scanline

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

	;
	; Joystick 1 button launches sound effect
	;
	lda INPT4	; Read joystick 1 button.
	BMI L1		; Jump if not pressed.
	lda EFFECT1	; Already playing effect 1?
	bne L1		; Yes, jump.
	lda #60		; Counter for effect 1.
	sta EFFECT1
L1:

	;
	; This code plays the sound effect.
	;
	lda EFFECT1	; Read effect 1 counter.
	BEQ L2		; Jump if zero.
	dec EFFECT1	; Count towards zero.
	and #3
	clc
	adc #10		; Volume based on counter (10-7 and repeating)
	sta AUDV0
	adc #10		; Frequency based on counter (20-17 and repeating)
	sta AUDF0
	lda #7		; Rumble.
	sta AUDC0
	jmp L3

L2:	lda #0
	sta AUDV0	; Turn off sound.
L3:

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	jmp show_frame

	.org $FFFC
	.word start
	.word start
