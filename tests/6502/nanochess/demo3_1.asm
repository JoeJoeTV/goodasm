	;
	; Horizontal positioning demo (Chapter 3 section 1)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/03/2022.
	;

	.lang 6502
	.include vcs.asm

	.equ xpos 0x80		; Current X position
	.equ xdir 0x81		; Current X direction

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

	lda #76		; Center of screen
	sta @xpos
	lda #1		; Go to right
	sta @xdir

show_frame:
	lda #0x88	; Blue.
	sta @colubk	; Background color.
	lda #0x0F	; White.
	sta @colup0	; Player 0 color.

	sta @hmclr	; Clear horizontal motion registers

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0		; End of vertical retrace.
	sta @vsync

	lda @xpos	; Desired X position
	ldx #0		; Player 0
	jsr x_position

	sta @wsync	; Wait for scanline start
	sta @hmove	; Write @hmove, only can be done
			; just after sta @wsync.

	ldx #34		; Remains 34 scanline of top border
TOP:
	sta @wsync
	dex
	bne TOP
	lda #0		; Disable blanking
	sta @vblank

	ldx #183	; 183 scanlines in blue
visible:
	sta @wsync
	dex
	bne visible

	sta @wsync	; One scanline
	lda #0x18	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x18	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x18	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x3c	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x24	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x66	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0xff	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x24	; 
	sta @grp0

	sta @wsync	; One scanline
	lda #0x00	; 
	sta @grp0

	lda #2		; Enable blanking
	sta @vblank
	ldx #30		; 30 scanlines of bottom border
bottom:
	sta @wsync
	dex
	bne bottom

	lda @xpos	; A = @xpos
	clc		; Clear carry (C flag in P register becomes zero)
	adc @xdir	; A = A + @xdir + Carry
	sta @xpos	; @xpos = A
	cmp #1		; Reached minimum X-position 1?
	beq L1		; Branch if EQual
	cmp #153	; Reached maximum X-position 153?
	bne L2		; Branch if Not Equal
L1:
	lda #0		; A = 0
	sec		; Set carry (it means no borrow for subtraction)
	sbc @xdir	; A = 0 - @xdir (reverses direction)
	sta @xdir	; @xdir = A
L2:
	jmp show_frame

	;
	; Position an item in X
	; Input:
	;   A = X position (1-159)
	;   X = Object to position (0=P0, 1=P1, 2=M0, 3=M1, 4=BALL)
	;
	; The internal loop should fit a 256-byte page.
	;
x_position:		; Start cycle
	sta @wsync	; 3: Start scanline synchro
	sec		; 5: Set carry (so SBC doesn't subtract extra)
	ldy @0x80	; 7: Eat 3 cycles
x_p1:
	sbc #15		; 10: Divide X by 15
	bcs x_p1	; 12: If the loop goes on, add 5 cycles each time
x_p2:
	tay		; 14:
	;;lda fine_adjust-$f1,y	; 18:
	lda @0xfe00, y	      ; Manual calculation of the above.
	sta @hmp0,x	; 22: Fine position
	sta @resp0,x	; 26: Time of setup for coarse position.
	rts

x_position_end:

	; Detect code divided between two pages
	; Cannot afford it because it takes one cycle more
	;; if (x_p1 & $ff00) != (x_p2 & $ff00)
	;; 	echo "Error: Page crossing"
	;; 	err	; Force assembler error
        ;; endif
	
	.org 0xfef1	; Table at last page of ROM
			; Shouldn't cross page
fine_adjust:
	.db 0x70	; 7px to left.
	.db 0x60	; 6px to left.
	.db 0x50	; 5px to left.
	.db 0x40	; 4px to left.
	.db 0x30	; 3px to left.
	.db 0x20	; 2px to left.
	.db 0x10	; 1px to left.
	.db 0x00	; No adjustment.
	.db 0xf0	; 1px to right.
	.db 0xe0	; 2px to right.
	.db 0xd0	; 3px to right.
	.db 0xc0	; 4px to right.
	.db 0xb0	; 5px to right.
	.db 0xa0	; 6px to right.
	.db 0x90	; 7px to right.

	.org 0xFFFC
	.word start
	.word start
