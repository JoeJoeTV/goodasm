	;
	; Playfield demo.
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/02/2022.
	;

	.lang 6502
	.include vcs.asm

	.org 0xf000
start:
	sei		; Disable interrupts.
	cld		; Clear decimal mode.
	ldx #0xFF	; X = $ff
	txs		; S = $ff
	lda #0x00	; A = $00
clear:
	sta @0, x	; Clear memory.
	dex		; Decrement X.
	bne clear	; Branch if not zero.

show_frame:
	lda #0x88	; Blue.
	sta @colubk	; Background color.
	lda #0x28	; White.
	sta @colupf	; Playfield color.
	lda #0x40	; Red
	sta @colup0	; Player 0 color.
	lda #0xc0	; Green
	sta @colup1	; Player 1 color.
	lda #0x01	; Right side of playfield is reflected.
	sta @ctrlpf	

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0		; End of vertical retrace.
	sta @vsync

	ldx #36		; Remains 36 scanlines of top border
TOP:
	sta @wsync
	dex
	bne TOP
	lda #0		; Disable blanking
	sta @vblank

	ldx #8		; 8 scanlines
PART1:
	sta @wsync
	lda #0xF0
	sta @pf0
	lda #0xF0
	sta @pf1
	lda #0xFF
	sta @pf2

	dex
	bne PART1

	ldx #176	; 176 scanlines
PART2:
	sta @wsync
	lda #0x10
	sta @pf0
	lda #0x00
	sta @pf1
	lda #0x00
	sta @pf2

	dex
	bne PART2

	ldx #8		; 8 scanlines
PART3:
	sta @wsync
	lda #0xF0
	sta @pf0
	lda #0xFF
	sta @pf1
	lda #0x3F
	sta @pf2

	dex
	bne PART3

	lda #2		; Enable blanking
	sta @vblank
	ldx #30		; 30 scanlines of bottom border
bottom:
	sta @wsync
	lda #0		; Disable playfield
	sta @pf0
	sta @pf1
	sta @pf2

	dex
	bne bottom

	jmp show_frame

	.org 0xFFFC
	.word start
	.word start
