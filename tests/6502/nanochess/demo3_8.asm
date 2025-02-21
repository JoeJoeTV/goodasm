	;
	; Horizontal/vertical positioning demo (Chapter 3 section 8)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/04/2022.
	;

	.lang 6502
	.include vcs.asm

XPOS	= $0080		; Current X position
XDIR    = $0081         ; Current X direction 
XPOS2   = $0082		; Current X position (fly)
YPOS2	= $0083		; Current Y position (fly)
XPOS3   = $0084		; Current X position (bullet)
YPOS3	= $0085		; Current Y position (bullet)
TEMP	= $0086		; Temporary variable

	.org 0xf000
start:
	sei		; Disable interrupts.
	cld		; Clear decimal mode.
	ldx #$FF	; X = $ff
	txs		; S = $ff
	lda #$00	; A = $00
clear:
	sta 0,X		; Clear memory.
	dex		; Decrement X.
	bne clear	; Branch if not zero.

	lda #76		; Center of screen
	sta XPOS
	lda #1		; Go to right
	sta XDIR
	lda #1
	sta XPOS2
	lda #20
	sta YPOS2
	lda #$ff	; Bullet hidden
	sta YPOS3

	lda #$00	; Configure SWCHA as input
	sta @swacnt
	sta @swbcnt	; Also SWCHB

show_frame:
	lda #$88	; Blue.
	sta @colubk	; Background color.
	lda #$0F	; White.
	sta @colup0	; Player 0 color.
	lda #$cF	; Green.
	sta @colup1	; Player 1 color.

	sta HMCLR	; Clear horizontal motion registers

	sta CXCLR	; Clear collision registers

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #42		; Time for NTSC top border
	sta @tim64t
	lda #0		; End of vertical retrace.
	sta @vsync

	lda XPOS	; Desired X position
	ldx #0		; Player 0
	jsr x_position

	lda XPOS2	; Desired X position
	ldx #1		; Player 1
	jsr x_position

	lda XPOS3	; Desired X position
	ldx #2		; Missile 0
	jsr x_position

	sta @wsync	; Wait for scanline start
	sta @hmove	; Write @hmove, only can be done
			; just after sta @wsync.

wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	sta @wsync
	lda #0		; Disable blanking
	sta @vblank

	lda YPOS3
	sta TEMP

	ldx #183	; 183 scanlines in blue.
	ldy YPOS2	; Y position of fly.
SPRITE1:
	sta @wsync	; Synchronize with scanline.
	lda #0		; A = $00 no graphic.
	dey		; Decrement Y.
	cpy #$F8	; Y >= $f8? (carry = 1)
	BCC L3		; No, jump if carry clear.
	lda FLY_BITMAP-$F8,Y	; Load byte of graphic.
L3:	sta @grp1	; Update @grp1.

	dec TEMP	; Decrement Y for missile.
	PHP		; Save processor status
	PLA		; Restore in accumulator
	sta @enam0	; Enable/disable missile 0

	dex		; Decrease X.
	bne SPRITE1	; Repeat until zero.

	ldx #9
	ldy #0
SPRITE0:
	sta @wsync	; Synchronize with scanline.
	lda #0		; A = $00 no graphic.
	dey		; Decrement Y.
	cpy #$F8	; Y >= $f8? (carry = 1)
	BCC L4		; No, jump if carry clear.
	lda SHIP_BITMAP-$F8,Y	; Load byte of graphic.
L4:	sta @grp0	; Update @grp0.
	dex		; Decrease X.
	bne SPRITE0	; Repeat until zero.

	lda #2		; Enable blanking
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	; Detect pause
	lda SWCHB
	and #$02	; Select button?
	bne L14
	jmp show_frame	; Return to show video frame
L14:

	; Detect fly killed
	lda CXM0P	; Bit 7: M0-P1. Bit 6: M0-P0.
	bpl L12		; Jump if collision M0-P1 didn't happen
	lda #0		; Restart fly at the left
	sta XPOS2
	lda #$FF	; Remove bullet
	sta YPOS3
L12:

	; Move the ship
	ldx XPOS	; Reg. X = XPOS

	lda SWCHA	; Read joystick.
	and #$40	; Test left movement.
	bne L6		; Jump if not moved.
	dex		; If moved, decrement X.

L6:	lda SWCHA	; Read joystick.
	and #$80	; Test right movement.
	bne L7		; Jump if not moved.
	INX		; If moved, increment X.
L7:
	CPX #1		; X < 1?
	BCS L8		; No, jump.
	ldx #1		; X = 1
L8:
	CPX #153	; X >= 153?
	BCC L9		; No, jump.
	ldx #152	; X = 152
L9:
	stx XPOS	; XPOS = Reg. X

	; Move and activate bullet
	lda YPOS3	; Read current bullet Y
	cmp #$FF	; Is it active?
	bne L10		; Yes, jump.
	lda INPT4	; Read joystick 1 button
	BMI L11		; Jump if not pressed
	lda #181	; Setup bullet Y
	sta YPOS3
	lda XPOS	; Get X of spaceship
	clc
	adc #3		; Center bullet
	sta XPOS3	; Setup bullet X
	jmp L11

L10:	dec YPOS3	; Decrease bullet Y
	dec YPOS3	; Decrease bullet Y
L11:

	; Move the fly
	lda XPOS2	; A = XPOS2
	clc		; Clear carry (becomes zero)
	adc #1		; A = A + 1 + Carry
	cmp #153	; Reached X-position 153?
	bne L5		; Branch if Not Equal
	lda #0		; If equal, reset to zero
L5:	sta XPOS2	; XPOS2 = A

	and #3		; Get modulo 4 of XPOS2
	adc #20		; Add base Y-coordinate
	sta YPOS2	; YPOS2 = A

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

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
	ldy $80		; 7: Eat 3 cycles
x_p1:
	sbc #15		; 10: Divide X by 15
	bcs x_p1	; 12: If the loop goes on, add 5 cycles each time
x_p2:
	tay		; 14:
	lda fine_adjust-$f1,y	; 18:
	sta @hmp0,x	; 22: Fine position
	sta @resp0,x	; 26: Time of setup for coarse position.
	rts

x_position_end:

	; Detect code divided between two pages
	; Cannot afford it because it takes one cycle more
	if (x_p1 & $ff00) != (x_p2 & $ff00)
		echo "Error: Page crossing"
		err	; Force assembler error
        endif
	
	org $fef1	; Table at last page of ROM
			; Shouldn't cross page
fine_adjust:
	.byte $70	; 7px to left.
	.byte $60	; 6px to left.
	.byte $50	; 5px to left.
	.byte $40	; 4px to left.
	.byte $30	; 3px to left.
	.byte $20	; 2px to left.
	.byte $10	; 1px to left.
	.byte $00	; No adjustment.
	.byte $f0	; 1px to right.
	.byte $e0	; 2px to right.
	.byte $d0	; 3px to right.
	.byte $c0	; 4px to right.
	.byte $b0	; 5px to right.
	.byte $a0	; 6px to right.
	.byte $90	; 7px to right.

FLY_BITMAP:
	.byte %00000000
	.byte %01100000
	.byte %11100100
	.byte %01101000
	.byte %00010000
	.byte %00101100
	.byte %00101110
	.byte %00000100

SHIP_BITMAP:
	.byte %00100100
	.byte %11111111
	.byte %01100110
	.byte %00100100
	.byte %00111100
	.byte %00011000
	.byte %00011000
	.byte %00011000

	.org $FFFC
	.word start
	.word start
