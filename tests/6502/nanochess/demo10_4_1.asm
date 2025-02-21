	;
	; 8K ROM demo (bank 0)
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

	.org $D000

start:
	sta BANK1
	nop
	nop
	nop
	nop
	nop

ALT_CODE:
	sta BANK0
	jmp KERNEL

EXT_BANK:
	sta BANK1
	nop
	nop
	nop

start2:
KERNEL:
	;
	; Joystick 1 button launches sound effect
	;
	lda INPT4	; Read joystick 1 button.
	BMI L1		; Jump if not pressed.
	lda #10		; Counter for effect 1.
	sta EFFECT1
L1:

	;
	; This code plays the sound effect.
	;
	lda EFFECT1	; Read effect 1 counter.
	BEQ L2		; Jump if zero.
	dec EFFECT1	; Count towards zero.
	lda #12		; Volume.
	sta AUDV0
	lda #4		; Pure tone.
	sta AUDC0
	lda #$11	; 880 hz frequency (NTSC).
	sta AUDF0
	jmp L3

L2:	lda #0
	sta AUDV0	; Turn off sound.
L3:

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

	jmp EXT_BANK

	.org $DFFC
	.word start
	.word start
