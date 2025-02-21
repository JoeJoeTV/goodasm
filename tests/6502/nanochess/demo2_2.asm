;;; Players Demo
;;; by Oscar Toledo G.
;;; https://nanochess.org

	.lang 6502
	.org 0xf000
	
;;; Symbols should come from another source, but this will do.
	.include vcs.asm

	
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
	lda #0x0F	; White.
	sta @colup0	; Player 0 color.
	lda #0x00	; Black.
	sta @colup1	; Player 1 color.

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0		; End of vertical retrace.
	sta @vsync

	; Player 0 and 1 horizontal position
	sta @wsync	; Cycle 3
	nop		; 5
	nop		; 7
	nop		; 9
	nop		; 11
	nop		; 13
	nop		; 15
	nop		; 17
	nop		; 19
	nop		; 21
	nop		; 23
	nop		; 25
	sta @resp0	; 28
	nop		; 30
	nop		; 32
	nop		; 34
	nop		; 36
	nop		; 38
	nop		; 40
	nop		; 42
	nop		; 44
	nop		; 46
	sta @resp1	; 48

	ldx #35		; Remains 35 scanlines of top border
TOP:
	sta @wsync
	dex
	bne TOP
	lda #0		; Disable blanking
	sta @vblank

	ldx #92		; 92 scanlines in blue
visible:
	sta @wsync
	dex
	bne visible

	sta @wsync	; One scanline
	lda #0x3c	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x42	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xa5	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x81	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xa5	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x99	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x42	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x3c	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x00	; 
	sta @grp0
	sta @grp1

	lda #0xF8	; Sand color
	sta @colubk

	ldx #91		; 91 scanlines
visible2:
	sta @wsync
	dex
	bne visible2

	lda #2		; Enable blanking
	sta @vblank
	ldx #30		; 30 scanlines of bottom border
bottom:
	sta @wsync
	dex
	bne bottom

	jmp show_frame

	.org 0xFFFC
	.word start
	.word start
