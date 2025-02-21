	;
	; Game of Ball (chapter 5)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/08/2022.
	;

	.lang 6502
	.include vcs.asm

Y1POS	= $0080		; Player 1 Y-position
Y2POS	= $0081		; Player 2 Y-position
BALLX	= $0082		; Ball X-position
BALLY	= $0083		; Ball Y-position
DIRX	= $0084		; Ball X-direction
DIRY	= $0085		; Ball Y-direction
SPEED	= $0086		; Ball speed
CURRENT	= $0087		; Current speed counter
SCORE1	= $0089		; Score of player 1
SCORE2	= $008a		; Score of player 2
TEMP1	= $008b		; Temporary variable 1
TEMP2	= $008c		; Temporary variable 2
@frame	= $008d		; Frame counter
SOUND	= $008e		; Sound duration

X_LEFT	= 16		; X-coordinate of player 1 paddle
X_RIGHT	= 144		; X-coordinate of player 2 paddle
INITIAL_SPEED = 32	; Initial speed of ball

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

	lda #80		; Center of screen.
	sta Y1POS	; Paddle 1.
	sta Y2POS	; Paddle 2.
	sta BALLX	; Ball X-coordinate.
	sta BALLY	; Ball Y-coordinate.
	lda #INITIAL_SPEED
	sta SPEED	; Speed of ball.

	lda #$00	; Configure SWCHA as input
	sta @swacnt
	sta @swbcnt	; Also SWCHB

	lda #$10	; Ball 2px width.
	sta @ctrlpf
	lda #$25	; Missile 4px width.
	sta @nusiz0	; Player x2 width.
	sta @nusiz1

show_frame:
	lda #$88	; Blue.
	sta @colubk	; Background color.
	lda #$48	; Red.
	sta @colup0	; Player 0 color.
	lda #$cF	; Green.
	sta @colup1	; Player 1 color.
	lda #$0F	; White.
	sta @colupf	; Ball/playfield color.

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

	lda #56		; Desired X position
	ldx #0		; Player 0
	jsr x_position

	lda #88		; Desired X position
	ldx #1		; Player 1
	jsr x_position

	lda #X_LEFT	; Desired X position
	ldx #2		; Missile 0
	jsr x_position

	lda #X_RIGHT	; Desired X position
	ldx #3		; Missile 1
	jsr x_position

	lda BALLX	; Desired X position
	ldx #4		; Ball
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

	lda SCORE1	; Score of player 1.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	sta TEMP1	; Use as offset to read bitmap.

	lda SCORE2	; Score of player 2.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	sta TEMP2	; Use as offset to read bitmap.

	ldy #8		; 8 scanlines for score
M1:	sta @wsync	; Synchronize with scanline.
	ldx TEMP1	; Row on score 1.
	lda numbers_bitmaps,X	; Read bitmap.
	sta @grp0	; Write as graphic for player 0.
	ldx TEMP2	; Row on score 2.
	lda numbers_bitmaps,X	; Read bitmap.
	sta @grp1	; Write as graphic for player 1.
	inc TEMP1	; Increase row of score 1.
	inc TEMP2	; Increase row of score 2.
	dey		; Decrease scanlines to display.
	bne M1		; Jump if still there are some.

	lda Y1POS	; Paddle 1 vertical position.	
	sta TEMP1	; Use as counter.
	lda Y2POS	; Paddle 2 vertical position.
	sta TEMP2	; Use as counter.
	ldx #184	; 184 scanlines in blue.
	ldy BALLY	; Y position of ball.
DISPLAY:
	sta @wsync	; Synchronize with scanline.
	dec TEMP1	; Decrease paddle 1 scanline
	dec TEMP2	; Decrease paddle 2 scanline
	dey		; Decrease ball scanline
	lda TEMP1	; Paddle 1 scanline.
	cmp #$E0	; Inside visual area?
	PHP		; Save processor state.
	PLA		; Restore into accumulator.
	ASL		; Move Carry flag to position.
	sta @enam0	; Enable/disable missile 0.
	lda TEMP2	; Paddle 2 scanline.
	cmp #$E0	; Inside visual area?
	PHP		; Save processor state.
	PLA		; Restore into accumulator.
	ASL		; Move carry flag to position.
	sta @enam1	; Enable/disable missile 1.
	cpy #$FC	; Inside visual area?
	PHP		; Save processor state.
	PLA		; Restore into accumulator.
	ASL		; Move Carry flag to position.
	sta @enabl	; Enable/disable ball.
	dex		; Decrease X.
	bne DISPLAY	; Repeat until zero.

	lda #2		; Enable blanking
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	; Move the paddles

	lda SWCHA	; Read joystick.
	and #$10	; Up? (player 1)
	bne M2		; No, jump.
	lda Y1POS	; Read current position.
	cmp #4		; At top?
	BEQ M2		; Yes, jump.
	dec Y1POS	; Move upwards 2 pixels.
	dec Y1POS
M2:

	lda SWCHA	; Read joystick.
	and #$20	; Down? (player 1)
	bne M3		; No, jump.
	lda Y1POS	; Read current position.
	cmp #148	; At bottom?
	BEQ M3		; Yes, jump.
	inc Y1POS	; Move downwards 2 pixels.
	inc Y1POS
M3:

	lda SWCHA	; Read joystick.
	and #$01	; Up? (player 2)
	bne M4		; No, jump.
	lda Y2POS	; Read current position.
	cmp #4		; At top?
	BEQ M4		; Yes, jump.
	dec Y2POS	; Move upwards 2 pixels.
	dec Y2POS
M4:

	lda SWCHA	; Read joystick.
	and #$02	; Down? (player 2)
	bne M5		; No, jump.
	lda Y2POS	; Read current position.
	cmp #148	; At bottom?
	BEQ M5		; Yes, jump.
	inc Y2POS	; Move downwards 2 pixels.
	inc Y2POS
M5:

	; Move the ball
	lda CURRENT	; Speed counter.
	clc
	adc SPEED	; Add current speed.
	sta CURRENT	; Save again.

M7:
	lda CURRENT	; Read speed counter.
	SEC
	SBC #$40	; Completed one frame?
	BCS M6		; Yes, jump.
	jmp M16		; No, exit loop.
M6:
	sta CURRENT	; Update speed counter.

	lda BALLX	; Ball X-coordinate.
	clc
	adc DIRX	; Add X direction.
	sta TEMP1	; Save in temporary.
	lda BALLY	; Ball Y-coordinate.
	clc
	adc DIRY	; Add Y direction.
	sta TEMP2	; Save in temporary.

	; Check hit against paddle 1
	lda TEMP1	; X-coordinate.
	cmp #X_LEFT-1	; If X < X_LEFT-1 then EXIT
	BCC M8
	cmp #X_LEFT+4	; If X >= X_LEFT+4 then EXIT
	BCS M8
	lda Y1POS	; If Y1POS-4 >= Y then EXIT
	SEC
	SBC #4
	cmp TEMP2
	BCS M8
	clc		; If Y1POS+31 < Y then EXIT
	adc #35
	cmp TEMP2
	BCC M8
	lda TEMP2
	SEC
	SBC Y1POS	; A = Y - Y1POS
	jsr paddle_hit	; Calculate new ball direction.
	jmp M10
M8:
	; Check hit against paddle 2
	lda TEMP1	; X-coordinate.
	cmp #X_RIGHT-1	; If X < X_RIGHT-1 then EXIT
	BCC M9
	cmp #X_RIGHT+4	; If X >= X_RIGHT+4 then EXIT
	BCS M9
	lda Y2POS	; If Y2POS-4 >= Y then EXIT
	SEC
	SBC #4
	cmp TEMP2
	BCS M9
	clc		; If Y2POS+31 < Y then EXIT
	adc #35
	cmp TEMP2
	BCC M9
	lda TEMP2
	SEC
	SBC Y2POS	; A = Y - Y2POS
	jsr paddle_hit	; Calculate new ball direction.
	lda #0
	SEC
	SBC DIRX	; Reverse DIRX (dirx = -dirx)
	sta DIRX
	jmp M10
M9:
	; Detect wall hit (top and bottom)
	lda TEMP2	; Y-coordinate.
	cmp #2		; If Y < 2 then wall hit
	BCC M11
	cmp #181	; If Y >= 181 then wall hit
	BCC M12
M11:	lda #0
	SEC
	SBC DIRY	; Just reverse DIRY (diry = -diry)
	sta DIRY

	; Sound effect for wall hit.
	lda #$17
	sta AUDF0
	lda #$0C
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #10		; Effect duration: 10 frames.
	sta SOUND
	jmp M10
M12:
	; Detect if the ball exits the courtyard
	lda TEMP1	; X-coordinate.
	cmp #2		; If X < 2 then ball out
	BCC M14
	cmp #157	; If X >= 157 then ball out
	BCC M15
	lda SCORE1	; Read score for player 1.
	cmp #9		; Already 9?
	BEQ M19		; Yes, jump.
	inc SCORE1	; No, increase it.
	jmp M19		; Jump.
M14:
	lda SCORE2	; Read score for player 2.
	cmp #9		; Already 9?
	BEQ M19		; Yes, jump.
	inc SCORE2	; No, increase it.
M19:
	lda #80		; Restart X,Y coordinates for ball.
	sta TEMP1
	sta TEMP2
	lda #0		; Make ball static.
	sta DIRX
	sta DIRY
	lda #INITIAL_SPEED	; Restart ball speed.
	sta SPEED

	; Sound effect for ball out.
	lda #$02
	sta AUDF0
	lda #$06
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #15		; Effect duration: 15 frames.
	sta SOUND
M15:
	; Nothing special happened.
	; The ball can move to the new coordinates.
	lda TEMP1
	sta BALLX
	lda TEMP2
	sta BALLY

M10:

	jmp M7

M16:
	; Launch the ball
	lda INPT4	; Joystick 1 button pressed?
	bpl M17		; Yes, jump.
	lda INPT5	; Joystick 2 button pressed?
	BMI M18		; No, jump.
M17:
	lda DIRX	; Ball moving?
	bne M18		; Yes, jump.
	lda @frame	; Get current frame.
	and #$03	; Modulo 4.
	tax
	lda ball_directions,X
	sta DIRX	; Random X direction.
	lda @frame	; Get current frame.
	@lsr		; Divide by 4.
	@lsr
	and #$03	; Modulo 4.
	tax
	lda ball_directions,X
	sta DIRY	; Random Y direction.
	
M18:

	dec SOUND	; Decrease sound counter.
	bne M20		; Is it zero? No, jump.
	lda #0		; Turn off sound effect.
	sta AUDV0
M20:

	lda #0		; Remove remains of ball...
	sta @enabl	; ...as it can touch border.

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	inc @frame	; Count frames

	jmp show_frame	; Repeat the game loop.

	; Ball directions (for random startup)
ball_directions:
	.byte $fe,$ff,$01,$02

	; Paddle hit.
	; A = Relative coordinate where ball hit.
	;
paddle_hit:
	pha
	; Start sound effect for paddle hit.
	lda #$1F
	sta AUDF0
	lda #$0C
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #5		; Effect duration: 5 frames.
	sta SOUND

	inc SPEED	; Increase ball speed by 2.
	inc SPEED
	PLA

	cmp #4		
	bpl p1
	lda #1
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p1:	cmp #9
	BCS p2
	lda #2
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p2:	cmp #14
	BCS p3
	lda #2
	sta DIRX
	lda #-1
	sta DIRY
	RTS

p3:	cmp #19
	BCS p4
	lda #2
	sta DIRX
	lda #0
	sta DIRY
	RTS

p4:	cmp #24
	BCS p5
	lda #2
	sta DIRX
	lda #1
	sta DIRY
	RTS

p5:	cmp #29
	BCS p6
	lda #2
	sta DIRX
	lda #2
	sta DIRY
	RTS

p6:	lda #1
	sta DIRX
	lda #2
	sta DIRY
	RTS

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

numbers_bitmaps:
	.byte %11111110	; 0
	.byte %10000010
	.byte %10000010
	.byte %10000110
	.byte %10000110
	.byte %10000110
	.byte %11111110
	.byte %00000000

	.byte %00010000	; 1
	.byte %00010000
	.byte %00010000
	.byte %00110000
	.byte %00110000
	.byte %00110000
	.byte %00110000
	.byte %00000000

	.byte %11111110	; 2
	.byte %00000010
	.byte %00000010
	.byte %11111110
	.byte %11000000
	.byte %11000000
	.byte %11111110
	.byte %00000000

	.byte %11111110	; 3
	.byte %00000010
	.byte %00000010
	.byte %11111110
	.byte %00000110
	.byte %00000110
	.byte %11111110
	.byte %00000000

	.byte %10000010	; 4
	.byte %10000010
	.byte %10000010
	.byte %11111110
	.byte %00000110
	.byte %00000110
	.byte %00000110
	.byte %00000000

	.byte %11111110	; 5
	.byte %10000000
	.byte %10000000
	.byte %11111110
	.byte %00000110
	.byte %00000110
	.byte %11111110
	.byte %00000000

	.byte %11111110	; 6
	.byte %10000000
	.byte %10000000
	.byte %11111110
	.byte %11000110
	.byte %11000110
	.byte %11111110
	.byte %00000000

	.byte %11111110	; 7
	.byte %00000010
	.byte %00000010
	.byte %00000010
	.byte %00000110
	.byte %00000110
	.byte %00000110
	.byte %00000000
	
	.byte %11111110	; 8
	.byte %10000010
	.byte %10000010
	.byte %11111110
	.byte %11000110
	.byte %11000110
	.byte %11111110
	.byte %00000000

	.byte %11111110	; 9
	.byte %10000010
	.byte %10000010
	.byte %11111110
	.byte %00000110
	.byte %00000110
	.byte %11111110
	.byte %00000000

	.org $FFFC
	.word start
	.word start
