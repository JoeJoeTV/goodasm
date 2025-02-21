	;
	; Music demo with drums #9
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/07/2022.
	;

	.lang 6502
	.include vcs.asm

MUSIC1:	EQU $80

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
	; Joystick 1 button launches music
	;
	lda INPT4	; Read joystick 1 button.
	BMI L1		; Jump if not pressed.
	lda MUSIC1	; Already playing music?
	bne L1		; Yes, jump.
	lda #16*16-1	; 16 notes at 16 frames each one.
	sta MUSIC1
L1:

	;
	; This code plays the music.
	;
	lda MUSIC1	; Read music counter.
	BEQ L2		; Jump if zero.
	tay
	EOR #$FF
	@lsr
	@lsr
	@lsr
	@lsr
	tax
	lda music_table,X
	sta AUDF1	; Counter directly to frequency.
	TYA
	@lsr
	and #7
	sta AUDV1
	lda #12		; Pure sound.
	sta AUDC1

	lda MUSIC1	; Get music counter
	and #$3F	; Modulo 64 (each 4 notes)
	cmp #$30	; First note?
	BCC L4		; No, jump.
	and #$0F	; Modulo 16 (first note duration)
	@lsr		; Divide by two.
	sta AUDV0	; Slowly decreasing volume.
	lda #7		; Frequency similar to drum.
	sta AUDF0
	lda #8		; White noise.
	sta AUDC0
	jmp L5

L4:	lda #0
	sta AUDV0
L5:

	dec MUSIC1
	jmp L3

L2:	lda #0
	sta AUDV0	; Turn off sound channel 0.
	sta AUDV1	; Turn off sound channel 1.
L3:

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	jmp show_frame

music_table:
	.byte $11	; D4
	.byte $11	; D4
	.byte $08	; D5
	.byte $08	; D5
	.byte $0E	; F4
	.byte $06	; F5#0x
	.byte $11	; D4
	.byte $08	; D5
	.byte $15	; A3#0x
	.byte $15	; A3#0x
	.byte $0a	; A4#0x
	.byte $08	; D5
	.byte $17	; A3
	.byte $17	; A3
	.byte $0b	; A4
	.byte $08	; D5

	.org $FFFC
	.word start
	.word start
