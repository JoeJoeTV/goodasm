	;
	; Players @nusiz demo
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/02/2022.
	;

	.lang 6502
	.org 0xf000
	.include vcs.asm

	.equ frame 0x0080	; Frame number saved in this address.
	.equ seconds 0x0081	; Seconds value saved in this address.


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

	lda @seconds	; Get number of seconds
;	and #0b00000111	; Let pass only bits 2-0
	and #0x07	; Let pass only bits 2-0
	sta @nusiz0	; Player 0 size/repeat.
	sta @nusiz1	; Player 1 size/repeat.

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
	lda #0x42	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x24	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x3C	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x5A	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xFF	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xBD	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xA5	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x24	; 
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

	inc @frame	; Increase frame number
	lda @frame	; Read frame number
	cmp #60		; Is it 60?
	bne l1		; Branch if not equal.
	lda #0		; Reset frame number to zero.
	sta @frame
	inc @seconds	; Increase number of seconds.
l1:
	jmp show_frame

	.org 0xFFFC
	.word start
	.word start
