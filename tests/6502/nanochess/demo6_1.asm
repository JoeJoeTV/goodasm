	;
	; Wall Breaker (chapter 6)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/10/2022.
	;

	.lang 6502
	.include vcs.asm

XPOS	= $0080		; Player X-position
BALLX	= $0081		; Ball X-position
BALLY	= $0082		; Ball Y-position
DIRX	= $0083		; Ball X-direction
DIRY	= $0084		; Ball Y-direction
SPEED	= $0085		; Ball speed
CURRENT	= $0086		; Current speed counter
SCORE1	= $0087		; Score digit 1
SCORE2	= $0088		; Score digit 2
TEMP1	= $008b		; Temporary variable 1
TEMP2	= $008c		; Temporary variable 2
TEMP3	= $008d		; Temporary variable 3
@frame	= $008e		; Frame counter
SOUND	= $008f		; Sound duration
TOTAL	= $0090		; Total bricks
BRICKS	= $0091		; Bricks (5 * 5 bytes)

INITIAL_SPEED = 32	; Initial speed of ball

Band1COL	= $48	; Band 1 color
Band2COL	= $28	; Band 2 color
Band3COL	= $18	; Band 3 color
Band4COL	= $C8	; Band 4 color
Band5COL	= $0C	; Band 5 color

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
	sta XPOS	; Paddle.

	lda #$00	; Configure SWCHA as input
	sta @swacnt
	sta @swbcnt	; Also SWCHB

	lda #$10	; Ball 2px width.
	sta @ctrlpf
	lda #$35	; Missile 8px width.
	sta @nusiz0	; Player x2 width.
	sta @nusiz1

	jsr reset_wall

show_frame:
	lda #$88	; Blue.
	sta @colubk	; Background color.
	lda #$cF	; Green.
	sta @colup0	; Player 0 color.
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

	lda #64		; Desired X position
	ldx #0		; Player 0
	jsr x_position

	lda #80		; Desired X position
	ldx #1		; Player 1
	jsr x_position

	lda XPOS	; Desired X position
	ldx #2		; Missile 0
	jsr x_position

	lda XPOS	; Desired X position
	clc
	adc #8
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

	lda SCORE1	; Score digit 1.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	sta TEMP1	; Use as offset to read bitmap.

	lda SCORE2	; Score digit 2.
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

	ldy BALLY	; Y position of ball.
	dey

	ldx #16		; 16 scanlines of space.
BRICK1:
	sta @wsync	; Synchronize with scanline.
	cpy #$FC	; Inside visual area?
	ROL
	ROL		; Move carry flag to position.
	sta @enabl	; Enable/disable ball.
	dey		; Decrease ball scanline
	dex		; Decrease X.
	bne BRICK1	; Repeat until zero.

	lda #Band1COL

	ldx #8		; 8 scanlines of bricks.
BRICK2:
	sta @wsync	; 3: Synchronize with scanline.
	sta @colupf	; 6: Set color of playfield.
	lda BRICKS+0	; 9: 
	sta @pf0		; 12: 4 pixels.
	lda BRICKS+1	; 15:
	sta @pf1		; 18: 8 pixels.
	cpy #$FC	; 20: Inside visual area?
	ROL		; 22:
	ROL		; 24: Move Carry flag to position.
	sta @enabl	; 27: Enable/disable ball.
	lda BRICKS+2	; 30:
	sta @pf2		; 33: 8 pixels.
	lda BRICKS+0	; 36:
	ASL		; 38:
	ASL		; 40:
	ASL		; 42:
	ASL		; 44:
	sta @pf0		; 47: 4 pixels.
	lda BRICKS+3	; 50:	
	sta @pf1		; 53: 8 pixels.
	lda BRICKS+4	; 56:
	sta @pf2		; 59: 8 pixels.
	lda #Band1COL	; 61: Color of playfield.
	dey		; 63: Decrease ball row.
	dex		; 65: Decrease X.
	bne BRICK2	; 67: Repeat until zero.

	lda #Band2COL	; 69:

	ldx #8		; 71: scanlines of bricks.

BRICK3:
	sta @wsync	; 3: Synchronize with scanline.
	sta @colupf	; 6: Set color of playfield.
	lda BRICKS+5	; 9:
	sta @pf0		; 12: 4 pixels.
	lda BRICKS+6	; 15:
	sta @pf1		; 18: 8 pixels.
	cpy #$FC	; 20: Inside visual area?
	ROL		; 22:
	ROL		; 24: Move Carry flag to position.
	sta @enabl	; 27: Enable/disable ball.
	lda BRICKS+7	; 30:
	sta @pf2		; 33: 8 pixels.
	lda BRICKS+5	; 36:
	ASL		; 38:
	ASL		; 40:
	ASL		; 42:
	ASL		; 44:
	sta @pf0		; 47: 4 pixels.
	lda BRICKS+8	; 50:	
	sta @pf1		; 53: 8 pixels.
	lda BRICKS+9	; 56:
	sta @pf2		; 59: 8 pixels.
	lda #Band2COL	; 61: Color of playfield.
	dey		; 63: Decrease ball scanline.
	dex		; 65: Decrease X.
	bne BRICK3	; 67: Repeat until zero.

	ldx #8		; 69:
	lda #Band3COL	; 71:

BRICK4:
	sta @wsync	; 3: Synchronize with scanline.
	sta @colupf	; 6: Set color of playfield.
	lda BRICKS+10	; 9:
	sta @pf0		; 12: 4 pixels.
	lda BRICKS+11	; 15:
	sta @pf1		; 18: 8 pixels.
	cpy #$FC	; 20: Inside visual area?
	ROL		; 22:
	ROL		; 24: Move Carry flag to position.
	sta @enabl	; 27: Enable/disable ball.
	lda BRICKS+12	; 30:
	sta @pf2		; 33: 8 pixels.
	lda BRICKS+10	; 36:
	ASL		; 38:
	ASL		; 40:
	ASL		; 42:
	ASL		; 44:
	sta @pf0		; 47: 4 pixels.
	lda BRICKS+13	; 50:	
	sta @pf1		; 53: 8 pixels.
	lda BRICKS+14	; 56:
	sta @pf2		; 59: 8 pixels.
	lda #Band3COL	; 61: Color of playfield.
	dey		; 63: Decrease ball scanline.
	dex		; 65: Decrease X.
	bne BRICK4	; 67: Repeat until zero.

	lda #Band4COL	; 69:
	ldx #8		; 71:

BRICK5:
	sta @wsync	; 3: Synchronize with scanline.
	sta @colupf	; 6: Set color of playfield.
	lda BRICKS+15	; 9:
	sta @pf0		; 12: 4 pixels.
	lda BRICKS+16	; 15:
	sta @pf1		; 18: 8 pixels.
	cpy #$FC	; 20: Inside visual area?
	ROL		; 22:
	ROL		; 24: Move Carry flag to position.
	sta @enabl	; 27: Enable/disable ball.
	lda BRICKS+17	; 30:
	sta @pf2		; 33: 8 pixels.
	lda BRICKS+15	; 36:
	ASL		; 38:
	ASL		; 40:
	ASL		; 42:
	ASL		; 44:
	sta @pf0		; 47: 4 pixels.
	lda BRICKS+18	; 50:	
	sta @pf1		; 53: 8 pixels.
	lda BRICKS+19	; 56:
	sta @pf2		; 59: 8 pixels.
	lda #Band4COL	; 61: Color of playfield.
	dey		; 63: Decrease ball scanline.
	dex		; 65: Decrease X.
	bne BRICK5	; 67: Repeat until zero.

	lda #120	; 69: Y-position.
	sta TEMP1	; 72: For paddle.
	lda #Band5COL	; 74:
	ldx #8		; 76:

BRICK6:
	sta @wsync	; 3: Synchronize with scanline.
	sta @colupf	; 6: Set color of playfield.
	lda BRICKS+20	; 9:
	sta @pf0		; 12: 4 pixels.
	lda BRICKS+21	; 15:
	sta @pf1		; 18: 8 pixels.
	cpy #$FC	; 20: Inside visual area?
	ROL		; 22:
	ROL		; 24: Move Carry flag to position.
	sta @enabl	; 27: Enable/disable ball.
	lda BRICKS+22	; 30:
	sta @pf2		; 33: 8 pixels.
	lda BRICKS+20	; 36:
	ASL		; 38:
	ASL		; 40:
	ASL		; 42:
	ASL		; 44:
	sta @pf0		; 47: 4 pixels.
	lda BRICKS+23	; 50:	
	sta @pf1		; 53: 8 pixels.
	lda BRICKS+24	; 56:
	sta @pf2		; 59: 8 pixels.
	lda #Band5COL	; 61: Color of playfield.
	dey		; 63: Decrease ball scanline.
	dex		; 65: Decrease X.
	bne BRICK6	; 67: Repeat until zero.

	dec TEMP1
	lda #$00
	ldx #128	; 128 scanlines of space.
BRICK7:
	sta @wsync	; Synchronize with scanline.
	sta @pf0		; Clean playfield
	sta @pf1		; 
	sta @pf2		; 
	lda TEMP1	; Paddle scanline.
	cmp #$FC	; Inside visual area?
	ROL
	ROL		; Move Carry flag to position.
	sta @enam0	; Enable/disable missile 0.
	sta @enam1	; Enable/disable missile 1.
	cpy #$FC	; Inside visual area?
	ROL		;  
	ROL		; Move Carry flag to position.
	sta @enabl	; Enable/disable ball.
	lda #$00
	dec TEMP1
	dey		; Decrease ball scanline.
	dex		; Decrease X.
	bne BRICK7	; Repeat until zero.

	lda #2		; Enable blanking
	sta @wsync
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	; Move the paddle

	lda SWCHA	; Read joystick.
	and #$40	; Left? (player 1)
	bne M2		; No, jump.
	lda XPOS	; Read current position.
	cmp #2		; At left?
	BEQ M2		; Yes, jump.
	dec XPOS	; Move to left 2 pixels.
	dec XPOS
M2:

	lda SWCHA	; Read joystick.
	and #$80	; Right? (player 1)
	bne M3		; No, jump.
	lda XPOS	; Read current position.
	cmp #144	; At right?
	BEQ M3		; Yes, jump.
	inc XPOS	; Move to right 2 pixels.
	inc XPOS
M3:

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

	; Check hit against paddle
	lda XPOS	; X-coordinate.
	SEC
	SBC #1
	cmp TEMP1	; If X < X_POS-1 then EXIT
	BCS M8
	clc
	adc #16
	cmp TEMP1	; If X >= X_POS+15 then EXIT
	BCC M8
	lda #176-4	; If YPOS-4 >= Y then EXIT
	cmp TEMP2
	BCS M8
	lda #176+3	; If YPOS+3 < Y then EXIT
	cmp TEMP2
	BCC M8
	lda TEMP1
	SEC
	SBC XPOS	; A = X - XPOS
	jsr paddle_hit	; Calculate new ball direction.
	jmp M10
M8:
	; Detect wall hit (top, left and right)
	lda TEMP2	; Y-coordinate
	cmp #2		; If Y < 2 then wall hit
	BCS M14
	lda #0
	SEC
	SBC DIRY	; Just reverse DIRY (diry = -diry)
	sta DIRY
	lda TEMP1	; X-coordinate.
	cmp #2		; If X < 2 then wall hit
	BCC M11
	cmp #158	; If X >= 158 then wall hit
	BCS M11
	jmp M19
M14:
	lda TEMP1	; X-coordinate.
	cmp #2		; If X < 2 then wall hit
	BCC M11
	cmp #158	; If X >= 158 then wall hit
	BCC M12
M11:	lda #0
	SEC
	SBC DIRX	; Just reverse DIRX (dirx = -dirx)
	sta DIRX

	; Sound effect for wall hit.
M19:	lda #$17
	sta AUDF0
	lda #$0C
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #10		; Effect duration: 10 frames.
	sta SOUND
	jmp M10
M12:
	; Detect if the ball hits a brick
	ldx TEMP1
	ldy TEMP2
	jsr hit_brick
	BCS M10		; Jump if brick hit.
	ldx TEMP1
	INX
	ldy TEMP2
	jsr hit_brick
	BCS M10		; Jump if brick hit.
	ldx TEMP1
	ldy TEMP2
	INY
	INY
	INY
	jsr hit_brick
	BCS M10		; Jump if brick hit.
	ldx TEMP1
	INX
	ldy TEMP2
	INY
	INY
	INY
	jsr hit_brick
	BCS M10		; Jump if brick hit.
 
M21:
	; Detect if the ball exits the courtyard
	lda TEMP2	; X-coordinate.
	cmp #180	; If X >= 180 then ball out
	BCC M15
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
	BMI M18		; No, jump.
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
	; Turn off sound
	dec SOUND	; Decrease sound counter.
	bne M20		; Is it zero? No, jump.
	lda #0		; Turn off sound effect.
	sta AUDV0
M20:

	; Verify if the wall is destroyed
M17:	lda TOTAL
	bne M23
	jsr reset_wall
M23:
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

	;
	; Reset wall
	;
reset_wall:
	ldx #BRICKS	; Get address into X
	ldy #25		; 5 * 5 bytes.
	lda #$ff	; Set to brick present.
BRICK:	sta 0,X		; Set bricks.
	INX
	dey
	bne BRICK
	lda #100	; Total of bricks
	sta TOTAL
	lda #80		; Center of screen.
	sta BALLX	; Ball X-coordinate.
	sta BALLY	; Ball Y-coordinate.
	lda #INITIAL_SPEED
	sta SPEED	; Speed of ball.
	RTS

	; Detect brick hit
	; X = X-coordinate
	; Y = Y-coordinate
hit_brick:
	TYA		; Copy Y into A
	SEC
	SBC #16		; Y-coordinate too upwards?
	BCC no_hit	; Yes, jump.
	cmp #40		; Y-coordinate too downwards?
	BCS no_hit	; Yes, jump.
	@lsr		; /2
	@lsr		; /4
	@lsr		; /8
	sta TEMP3
	ASL		; x2
	ASL		; x4
	adc TEMP3	; x5 as each row is 5 bytes in RAM
	sta TEMP3
	TXA		; Copy X into A.
	@lsr		; Divide by 4 as each playfield...
	@lsr		; ...pixel is 4 pixels.
	tax
	lda brick_mapping,X
	pha
	@lsr
	@lsr
	@lsr
	clc
	adc TEMP3
	tay		; Y = brick byte offset
	PLA
	and #$07
	tax
	lda bit_mapping,X
	and BRICKS,Y
	BEQ no_hit

	lda bit_mapping,X
	EOR #$FF
	and BRICKS,Y
	sta BRICKS,Y

	dec TOTAL	; One brick less on the wall

	lda #$00
	SEC
	SBC DIRY
	sta DIRY

	inc SCORE2
	lda SCORE2
	cmp #10
	bne h1
	lda #0
	sta SCORE2
	inc SCORE1
	lda SCORE1
	cmp #10
	bne h1
	lda #9
	sta SCORE1
	sta SCORE2
h1:
	; Start sound effect for brick hit.
	lda #$05
	sta AUDF0
	lda #$06
	sta AUDC0
	lda #$0c
	sta AUDV0
	lda #10		; Effect duration: 10 frames.
	sta SOUND

	SEC
	RTS

no_hit:	clc
	RTS

brick_mapping:
	.byte 4,5,6,7
	.byte 15,14,13,12,11,10,9,8
	.byte 16,17,18,19,20,21,22,23
	.byte 0,1,2,3
	.byte 31,30,29,28,27,26,25,24
	.byte 32,33,34,35,36,37,38,39
bit_mapping:
	.byte $03,$03,$0c,$0c,$30,$30,$c0,$c0

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

	inc SPEED	; Increase ball speed
	PLA

	cmp #2	
	bpl p1
	lda #-2
	sta DIRX
	lda #-1
	sta DIRY
	RTS

p1:	cmp #4
	BCS p2
	lda #-2
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p2:	cmp #7
	BCS p3
	lda #-1
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p3:	cmp #9
	BCS p4
	lda #0
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p4:	cmp #12
	BCS p5
	lda #1
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p5:	cmp #14
	BCS p6
	lda #2
	sta DIRX
	lda #-2
	sta DIRY
	RTS

p6:	lda #2
	sta DIRX
	lda #-1
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
	.byte $70,$60,$50,$40,$30,$20,$10,$00
	.byte $f0,$e0,$d0,$c0,$b0,$a0,$90

numbers_bitmaps:
	.byte $fe,$82,$82,$86,$86,$86,$fe,$00	; 0
	.byte $10,$10,$10,$30,$30,$30,$30,$00	; 1
	.byte $fe,$02,$02,$fe,$c0,$c0,$fe,$00	; 2
	.byte $fe,$02,$02,$fe,$06,$06,$fe,$00	; 3
	.byte $82,$82,$82,$fe,$06,$06,$06,$00	; 4
	.byte $fe,$80,$80,$fe,$06,$06,$fe,$00	; 5
	.byte $fe,$80,$80,$fe,$c6,$c6,$fe,$00	; 6
	.byte $fe,$02,$02,$02,$06,$06,$06,$00	; 7
	.byte $fe,$82,$82,$fe,$c6,$c6,$fe,$00	; 8
	.byte $fe,$82,$82,$fe,$06,$06,$fe,$00	; 9

	.org $FFFC
	.word start
	.word start
