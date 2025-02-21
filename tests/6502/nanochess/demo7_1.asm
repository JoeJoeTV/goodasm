	;
	; Invaders (chapter 7)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/11/2022.
	; Revision date: Jun/12/2022. Stabilized scanline count.
	; Revision date: Jun/13/2022. Added missing things.
	;

	.lang 6502
	.include vcs.asm

XPOS	= $0080		; Player X-position
HIT	= $0081		; Player hit
SHOTX	= $0082		; Shot X-position
SHOTY	= $0083		; Shot Y-position
INVS	= $0084		; Invaders state
INVW	= $0085		; Invaders width
INVX	= $0086		; Invaders X-position
INVY	= $0087		; Invaders Y-position
BOMBX	= $0088		; Bomb X-position
BOMBY	= $0089		; Bomb Y-position
SPEED	= $008a		; Invaders speed
CURRENT	= $008b		; Current speed counter
SCORE1	= $008c		; Score digit 1
SCORE2	= $008d		; Score digit 2
TEMP1	= $008e		; Temporary variable 1
TEMP2	= $008f		; Temporary variable 2
TEMP3	= $0090		; Temporary variable 3
TEMP4	= $0091		; Temporary variable 4
TEMP5	= $0092		; Temporary variable 5
TEMP6	= $0093		; Temporary variable 6
@frame	= $0094		; Frame counter
SOUND0	= $0095		; Sound 0 duration
SOUND1	= $0096		; Sound 1 duration
ALT	= $0097		; Alternate invader frames
ARMY	= $0098		; Army of invaders (5 bytes)

INITIAL_SPEED = 2	; Initial speed of invaders

INVH	= 13	; Height in pixels of invader row.
INVROWS	= 5	; Total of invader rows

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
	sta XPOS	; Spaceship.

	lda #$00	; Configure SWCHA as input
	sta @swacnt
	sta @swbcnt	; Also SWCHB

	jsr reset_invaders

show_frame:
	lda #$00	; Black.
	sta @colubk	; Background color.
	lda #$38	; Orange.
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
	INX		; Player 1
	jsr x_position

	lda BOMBX	; Desired X position
	INX		; Missile 0
	jsr x_position

	lda SHOTX	; Desired X position
	ldx #4		; Ball
	jsr x_position

	sta @wsync	; Wait for scanline start
	sta @hmove	; Write @hmove, only can be done
			; just after sta @wsync.

	lda #$05	; Missile 1px width.
	sta @nusiz0	; Player x2 width.
	sta @nusiz1

	lda #$00	; Ball 1px width.
	sta @ctrlpf

	ldx SHOTY	; Y-position of shot.
	INX		; Increment by one.
	stx TEMP5	; Start temporary counter.
	ldx BOMBY	; Y-position of bomb.
	INX		; Increment by one.
	stx TEMP6	; Start temporary counter.

wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	sta @wsync
	lda #0		; Disable blanking
	sta @vblank
	sta HMCLR

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

	ldx INVY	; Y-position of invaders = scanlines of space.
INVADE1:
	sta @wsync	; Synchronize with scanline.

	dec TEMP5	; Counter for shot.
	PHP		; Copy Z...
	PLA		; ...into accumulator.
	sta @enabl	; Enable/disable ball.

	dec TEMP6	; Counter for bomb.
	PHP		; Copy Z...
	PLA		; ...into accumulator.
	sta @enam0	; Enable/disable missile 0.

	dex		; Decrease X.
	bne INVADE1	; Repeat until zero.

	lda #$C8	; Green color for invaders.
	sta @colup0
	sta @colup1

	; Show invaders army.
	ldy ARMY
	jsr show_invaders_row
	ldy ARMY+1
	jsr show_invaders_row
	ldy ARMY+2
	jsr show_invaders_row
	ldy ARMY+3
	jsr show_invaders_row
	ldy ARMY+4
	jsr show_invaders_row

	lda #177-INVH*5	; 177 scanlines - 5 rows of aliens
	SEC
	SBC INVY	; Subtract current origin Y of aliens.
	tax		; Copy into X to use as counter.
INVADE2:
	sta @wsync	; Synchronize with scanline.

	dec TEMP5	; Counter for shot.
	PHP		; Copy Z...
	PLA		; ...into accumulator.
	sta @enabl	; Enable/disable ball.

	dec TEMP6	; Counter for bomb.
	PHP		; Copy Z...
	PLA		; ...into accumulator.
	sta @enam0	; Enable/disable missile 0.

	dex		; Decrease X.
	bne INVADE2	; Repeat until zero.

	;
	; Start positioning code for spaceship
	;
	sta @wsync	; 3: Start scanline synchro.
	sec		; 5: Set carry (so SBC doesn't subtract extra)
	lda XPOS	; 8: X-position for spaceship.
x4_p1:
	sbc #15		; 10: Divide X by 15
	bcs x4_p1	; 12: If the loop goes on, add +5 each time
x4_p2:
	tay		; 14:
	lda fine_adjust-$f1,y	; 18:
	sta HMBL	; 21: Fine position
	nop		; 23:
	sta RESBL	; 26: Time of setup for coarse position.

	sta @wsync	; Synchronize to next scanline.
	sta @hmove
	lda #$10	; 2px width ball.
	sta @ctrlpf
	lda #$02	; Enable ball.
	sta @enabl
	ldx #$a8	; Turquoise color.
	lda HIT		; Spaceship has been hit?
	BEQ M38		; No, jump.
	ldx #$48	; Red color.
M38:	stx @colupf	; Update playfield/ball color.
	
	sta @wsync	; Synchronize to next scanline.
	sta HMCLR	; Clear fine movement counters.
	lda #$10	; Move ball 1 pixel to left.
	sta HMBL

	sta @wsync	; Synchronize to next scanline.
	sta @hmove	; Do fine movement.
	lda #$20	; 4px width ball
	sta @ctrlpf	; Update.
	
	sta @wsync	; Synchronize to next scanline.
	sta HMCLR	; Clear fine movement counters.
	lda #$20	; Move ball 2 pixels to left.
	sta HMBL

	sta @wsync	; Synchronize to next scanline.
	sta @hmove	; Do fine movement.
	lda #$30	; 8px width ball.
	sta @ctrlpf
	
	sta @wsync	; Synchronize to next scanline.
	sta HMCLR	; Do fine movement.

	lda #2		; Enable blanking
	sta @wsync
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	; Move the spaceship

	lda SWCHA	; Read joystick.
	and #$40	; Left? (player 1)
	bne M2		; No, jump.
	lda XPOS	; Read current position.
	cmp #14		; At left?
	BEQ M2		; Yes, jump.
	dec XPOS	; Move to left 2 pixels.
	dec XPOS
M2:

	lda SWCHA	; Read joystick.
	and #$80	; Right? (player 1)
	bne M3		; No, jump.
	lda XPOS	; Read current position.
	cmp #136	; At right?
	BEQ M3		; Yes, jump.
	inc XPOS	; Move to right 2 pixels.
	inc XPOS
M3:

	; Move the invaders
	lda CURRENT	; Speed counter.
	clc
	adc SPEED	; Add current speed.
	sta CURRENT	; Save again.

	lda CURRENT	; Read speed counter.
	SEC
	SBC #$40	; Completed one frame?
	BCS M6		; Yes, jump.
	jmp M16		; No, exit loop.
M6:
	sta CURRENT	; Update speed counter.
	
	lda ALT
	EOR #$08	; Alternate animation frame.
	sta ALT
	bne M10		; Happened twice? No, jump.

	lda SOUND0	; Playing another sound?
	bpl M10		; Yes, jump.
	
	lda #$10	; Marching sound effect.
	sta AUDF0
	lda #$06
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #2		; Two frame duration.
	sta SOUND0
M10:
	lda INVS	; Invaders state.
	cmp #0		; Is it zero?
	bne M11		; No, jump.
	dec INVX	; Move invaders to left.
	dec INVX	; Move invaders to left.
	lda INVX
	cmp #10		; Reached left limit?
	bne M12		; No, jump.
	lda #3		; State = Down + Right.
	sta INVS
	jmp M12

M11:	cmp #1		; Is it one?
	bne M14		; No, jump.
	inc INVX	; Move invaders to right.
	inc INVX	; Move invaders to right.
	lda INVW	; Get current width of rectangle.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	ASL		; x16
	EOR #$FF	; Negate.
	adc #151	; Add to right limit.
	cmp INVX	; Reached right limit?
	bne M12		; No, jump.
	lda #2		; State = Down + Left
	sta INVS
	jmp M12

M14:
	inc INVY	; Other state: Go downwards.
	lda INVS
	clc
	adc #2		; Advance state
	sta INVS
	SEC
	SBC #2+INVH/2*2	; Advanced a full row?
	BCC M12		; No, jump.
	sta INVS	; Update state to 0 or 1.

M12:

M16:
	; Start the game
	lda SWCHB
	and #$02	; Select button pressed?
	bne M19		; No, jump.
	lda SPEED	; Already started?
	bne M19		; Yes, jump.
	jsr reset_invaders	; Reset invaders.
	lda #INITIAL_SPEED	; Set initial speed.
	sta SPEED
M19:

	; Spaceship shot
	lda SHOTY	; Read Y-coordinate of shot.
	cmp #$FF	; Active?
	BEQ M18		; No, jump.
	SEC
	SBC #2		; Move two pixels upward.
	BCS M21		; Crossed zero? No, jump.
	lda #$FF	; Set to inactive.
M21:
	sta SHOTY	; Update Y-coordinate of shot.

	lda SHOTX	; Get X-coordinate of shot.
	SEC
	SBC INVX	; Subtract invaders X origin.
	BCC M22		; Exit if lesser than X origin.
	cmp #96		; Exceeds maximum width?
	BCS M22		; Yes, jump.
	@lsr		; /2
	@lsr		; /4
	@lsr		; /8
	@lsr		; /16
	BCS M22		; Jump if it is space between invaders.
	tay		; Number of invader column in Y.

	lda SHOTY	; Get Y-coordinate of shot.
	SEC
	SBC INVY	; Subtract invaders Y origin.
	cmp #INVROWS*INVH	; Not inside height of invaders?
	BCS M22		; No, jump.
			; Succesive subtraction for division.
	ldx #$FF	; X = Final result.
M23:	INX		; X = X + 1
	SEC
	SBC #INVH	; Divide by invader row height.
	BCS M23		; Completed? No, repeat.
	cmp #$FD	; Shot in space between invaders?
	BCS M22		; Yes, jump.
	
	lda ARMY,X	; Get row of invaders.
	and invader_bit,Y	; Logical and with specific invader.
	BEQ M22		; There is one? No, jump.

	; Remove bullet
	lda #$FF
	sta SHOTY

	; Destroy invader
	lda invader_bit,Y	; Invader bit.
	EOR #$FF	; Complement.
	and ARMY,X	; and-out the invader.
	sta ARMY,X	; Update memory.
	
	; Explosion sound effect.
	lda #$06
	sta AUDF0
	lda #$08
	sta AUDC0
	lda #$0C
	sta AUDV0
	lda #16		; Duration: 16 frames.
	sta SOUND0

	; Speed up game
	inc SPEED

	; Score a point
	inc SCORE2	; Increase second digit.
	lda SCORE2
	cmp #10		; Is it 10?
	bne M22		; No, jump.
	lda #0		; Put back to zero.
	sta SCORE2
	inc SCORE1	; Increase first digit.
	lda SCORE1	
	cmp #10		; Is it 10?
	bne M22		; No, jump.
	lda #9		; Set score to maximum 99.
	sta SCORE1
	sta SCORE2
M22:
	jmp M15
M18:
	lda INPT4	; Joystick 1 button pressed?
	BMI M15		; No, jump.
	lda SPEED	; Game started?
	BEQ M15		; No, jump.

	lda XPOS	; Shot X = Spaceship X.
	sta SHOTX
	lda #175	; Shot Y = 175.
	sta SHOTY

	; Shoot sound effect.
	lda #$06
	sta AUDF1
	lda #$03
	sta AUDC1
	lda #$0C
	sta AUDV1
	lda #5		; Duration: 5 frames.
	sta SOUND1

M15:
	; Turn off sound
	dec SOUND0	; Decrease sound counter.
	bne M20		; Is it zero? No, jump.
	lda #0		; Turn off sound effect.
	sta AUDV0
M20:
	dec SOUND1	; Decrease sound counter.
	bne M39		; Is it zero? No, jump.
	lda #0		; Turn off sound effect.
	sta AUDV1
M39:

	; Move invaders bomb
	lda SPEED	; Game active?
	BEQ M32		; No, jump.
	ldx BOMBY	; Read bomb Y-coordinate.
	CPX #$FF	; Active?
	BEQ M34		; No, jump.
	INX		; Make it to go down.
	CPX #177	; Reached Y-limit?
	bne M33		; No, jump?
	lda CXM0FB	; Collision of M0 against...
	and #$40	; ...Ball?
	BEQ M37		; No, jump.
	jmp M36		; Spaceship hit.
M37:
	ldx #$FF	; Y-coordinate to inactive bomb.
M33:	stx BOMBY	; Update bomb.
	jmp M32

M34:
	lda ARMY+INVROWS-1	; Read bottommost invader row.
	and #$07	; Are there invaders?
	BEQ M32		; No, jump.
	tax
	lda invaders_offset0,X	; Get offset of leftmost one.
	adc INVX	; Add origin X of invaders rectangle.
	adc #4		; Center under invader.
	sta BOMBX	; Set X-coordinate of bomb.
	lda INVY	; Get origin Y of invaders rectangle.
	adc #5*INVH-3	; Add offset to bottommost row.
	sta BOMBY	; Set Y-coordinate of bomb.

	; Invaders shoot sound effect.
	lda #$03
	sta AUDF1
	lda #$0E
	sta AUDC1
	lda #$0C
	sta AUDV1
	lda #3		; Duration: 3 frames.
	sta SOUND1

M32:

	; Verify if it should realign invaders vertically.
	lda ARMY+INVROWS-1	; Bottommost row destroyed?
	bne M26		; No, jump.
	lda INVY	; Get current origin Y of rectangle.
	SEC
	SBC #INVH	; Minus height of invader row.
	BCC M26		; If negative, exit this code.
	cmp #16		; If too low, exit this code.
	BCC M26
	sta INVY	; Invaders relocated on Y.
	ldx #INVROWS-2	; Displace all bytes of invaders.
M27:	lda ARMY,X	; Upper row...
	sta ARMY+1,X	; ...going down.
	dex
	bpl M27
	lda #0		; Uppermost row set to nothing.
	sta ARMY
M26:

	; Verify if the invaders are destroyed.
M17:	lda #0		; Load accumulator with zero.
	ldx #INVROWS-1	; Bottommost invaders row.
M24:	ORA ARMY,X	; Logical OR of accumulator with data.
	dex		; Go one row up.
	bpl M24		; Jump if still positive row.

	cmp #0		; Is it zero?
	bne M25		; No, jump.
	jsr reset_invaders	; Yes, reset invaders.
	lda #2		; Restart march.
	sta SPEED
	lda #2		; Make them go down.
	sta INVS
	jmp M28

	; Realign invaders horizontally?
M25:	tax		; Save in X (it will be used again)
	and #$01	; Left column open?
	bne M30		; No, jump.
	lda INVX	; Current X-origin for invaders.
	clc
	adc #16		; Add 16 pixels.
	sta INVX	; Update X-origin.
	ldx #INVROWS-1	; Bottommost invaders row.
M29:	@lsr ARMY,X	; Displace row one bit to right.
	dex		; Go one row up.
	bpl M29		; Jump if still positive row.
M35:	dec INVW	; Decrease rectangle width.
	jmp M28

M30:	ldy INVW	; Get current rectangle width.
	dey		; Minus 1.
	TXA		; Get current OR'ed column.
	and invader_bit,Y	; Right column open?
	BEQ M35		; Yes, jump backwards.

M28:
	; Verify if invaders won.
	lda INVY	; Y origin of invaders?
	cmp #177-INVH*INVROWS-2	; 177 scanlines - rows of aliens
	BCC M31		; Reached limit? No, jump.
M36:
	lda #0		; Stop the game.
	sta SPEED
	lda #$FF	
	sta BOMBY	; Remove any bomb.
	sta SHOTY	; Remove any shot.
	lda #1		; Spaceship is hit.
	sta HIT

	; Big explosion sound effect.
	lda #$0F
	sta AUDF0
	lda #$08
	sta AUDC0
	lda #$0F
	sta AUDV0
	lda #32		; Duration: 32 frames.
	sta SOUND0

M31:
	lda #0		
	sta @enabl	; Remove ball. 
	sta @enam0	; Remove missile 0.

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	inc @frame	; Count frames

	jmp show_frame	; Repeat the game loop.

	;
	; Reset invaders
	;
reset_invaders:
	ldx #INVROWS-1	; Bottommost row of invaders.
	lda #$3f	; 6 invaders present on row.
INVADER:
	sta ARMY,X	; Set invader row.
	dex
	bpl INVADER
	lda #36		; Invaders X start
	sta INVX
	lda #16		; Invaders Y start
	sta INVY
	lda #0		; Invaders state = 0
	sta INVS
	lda #6		; Width of invaders rectangle.
	sta INVW
	lda #0		; Static
	sta SPEED	; Speed of invaders.
	sta HIT		; Not hit.
	lda #$FF
	sta BOMBY	; No active bomb.
	sta SHOTY	; No active shot.
	RTS

	;
	; Show a row of invaders.
	;
show_invaders_row:
	sta @wsync	; 3:
	dec TEMP5	; 8: Decrease position for shot.
	PHP		; 11:
	PLA		; 15:
	sta @enabl	; 18: Enable/disable shot.
	dec TEMP6	; 23: Decrease position for bomb.
	PHP		; 26:
	PLA		; 30:
	sta @enam0	; 33: Enable/disable bomb.
	lda invaders_bits0,Y	; 37: Read repeat table...
	sta @nusiz0	; 40: ...for left-side invaders.
	lda invaders_frame0,Y	; 44: Graphic bitmap...
	nop		; 46:
	EOR ALT		; 49: ...with alternate animation...
	sta TEMP3	; 52: ...to be used.
	dec TEMP6	; 57: Decrease position for bomb.
	PHP		; 60: Save for later use.
	dec TEMP5	; 65: Decrease position for shot.
	PHP		; 68: Save for later use.
	PLA		; 72: Time is now.
	sta.W @enabl	; 76: Enable/disable shot.
			;     sta.W instruction is one cycle
			;     slower than direct sta.

	lda invaders_offset0,Y	; 4: Start row synchro
	adc INVX	; 7: Eat 3 cycles
	sec		; 9: Set carry (so SBC doesn't subtract extra)
x2_p1:
	sbc #15		; 11: Divide X by 15
	bcs x2_p1	; 13: If the loop goes on, add +5 each time
x2_p2:
	tax		; 15:
	lda fine_adjust-$f1,x	; 19:
	sta.W @hmp0	; 23: Fine position
	sta @resp0	; 26: Time of setup for coarse position.

	PLA		; 
	sta @enam0	; Enable/disable bomb.

	sta @wsync	; 3:
	dec TEMP5	; 8: Decrease position for shot.
	PHP		; 11:
	PLA		; 15:
	sta @enabl	; 18: Enable/disable shot.
	dec TEMP6	; 23: Decrease position for bomb.
	PHP		; 26:
	PLA		; 30:
	sta @enam0	; 33: Enable/disable bomb.
	lda invaders_bits1,Y	; 37: Read repeat table...
	sta @nusiz1	; 40: ...for right-side invaders.
	lda invaders_frame1,Y	; 44: Graphic bitmap...
	nop		; 46:
	EOR ALT		; 49: ...with alternate animation...
	sta TEMP4	; 52: ...to be used.
	dec TEMP6	; 57: Decrease position for bomb...
	PHP		; 60: ...to be used later.
	dec TEMP5	; 65: Decrease position for shot...
	PHP		; 68: ...to be used later...
	PLA		; 72: ...time is now.
	sta.W @enabl	; 76: Enable/disable shot.

	lda invaders_offset1,Y	; 4: Start row synchro
	adc INVX	; 7:
	sec		; 9: Set carry (so SBC doesn't subtract extra)
x3_p1:
	sbc #15		; 11: Divide X by 15
	bcs x3_p1	; 13: If the loop goes on, add +5 each time
x3_p2:
	tax		; 15:
	lda fine_adjust-$f1,x	; 19:
	sta.W @hmp1	; 23: Fine position
	sta @resp1	; 26: Time of setup for coarse position.
	PLA		;
	sta @enam0	; Enable/disable bomb.

	; Draw the proper invaders.
	ldy #8		; Height of 8 pixels.
sv1:	sta @wsync	; 3: Wait for next screen row.
	sta @hmove	; 6: Fine movement.
	ldx TEMP3	; 9: Offset for left-side invaders.
	lda invaders_bitmaps,X	; 13: Read bitmap.
	sta @grp0	; 16: Update bitmap.
	ldx TEMP4	; 19: Offset for right-side invaders.
	lda invaders_bitmaps,X	; 23: Read bitmap.
	sta @grp1	; 26: Update bitmap.
	dec TEMP5	; 31: Decrease position for shot.
	PHP		; 34:
	PLA		; 38:
	sta @enabl	; 41: Enable/disable shot.
	dec TEMP6	; 46: Decrease position for bomb.
	PHP		; 49:
	PLA		; 53:
	sta @enam0	; 56: Enable/disable bomb.
	inc TEMP3	; 61: Next bitmap offset left-side.
	inc TEMP4	; 66: Next bitmap offset right-side.
	sta HMCLR	; 69: Clear fine movement.

	dey		; 71: Count height.
	bne sv1		; 73: Loop if not finished.

	; Cannot make previous loop to be ldy #9...
	; ...as we need time to execute:...
	; ...RTS + ldy # + jsr...
	sta @wsync	; 3: Wait for next row.
	lda #0		; 5: Turn off invaders graphics.
	sta @grp0	; 8:
	sta @grp1	; 11:
	dec TEMP5	; 16: We cannot miss update of
	PHP		; 19: bomb/shot in each row.
	PLA		; 23:
	sta @enabl	; 26:
	dec TEMP6	; 31:
	PHP		; 34:
	PLA		; 38:
	sta @enam0	; 41:
	RTS		; 47: Return.
			; 49: ldy (in caller code)
			; 55: jsr (in caller code)
			;     Again in this subroutine.

	.org $fd00

	; Repeat bits for @nusizx
	; bit 0 - Leftmost invader.
	; bit 1 - Center invader.
	; bit 2 - Right invader.
	;
invaders_bits0:
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03
	.byte $00,$00,$00,$01,$00,$02,$01,$03

	; Graphics frames for invader.
	; $00 when there are no invaders to show.
invaders_frame0:
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10
	.byte $00,$10,$10,$10,$10,$10,$10,$10

	; @nusizx cannot offset a player
	; So we need the offset for the leftmost invader.
	;
invaders_offset0:
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
	.byte $00,$00,$10,$00,$20,$00,$10,$00
invaders_bits1:
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $01,$01,$01,$01,$01,$01,$01,$01
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $02,$02,$02,$02,$02,$02,$02,$02
	.byte $01,$01,$01,$01,$01,$01,$01,$01
	.byte $03,$03,$03,$03,$03,$03,$03,$03
invaders_frame1:
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
invaders_offset1:
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $40,$40,$40,$40,$40,$40,$40,$40
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $50,$50,$50,$50,$50,$50,$50,$50
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $40,$40,$40,$40,$40,$40,$40,$40
	.byte $30,$30,$30,$30,$30,$30,$30,$30

	; Invader bit on row (leftmost to rightmost)
invader_bit:
	.byte $01,$02,$04,$08,$10,$20


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
		echo "Error: Page crossing 1"
		err	; Force assembler error
        endif
	if (x2_p1 & $ff00) != (x2_p2 & $ff00)
		echo "Error: Page crossing 2"
		err	; Force assembler error
        endif
	if (x3_p1 & $ff00) != (x3_p2 & $ff00)
		echo "Error: Page crossing 3"
		err	; Force assembler error
        endif
	if (x4_p1 & $ff00) != (x4_p2 & $ff00)
		echo "Error: Page crossing 4"
		err	; Force assembler error
        endif
	
	org $fef1	; Table at last page of ROM
			; Shouldn't cross page
fine_adjust:
	.byte $70,$60,$50,$40,$30,$20,$10,$00
	.byte $f0,$e0,$d0,$c0,$b0,$a0,$90


invaders_bitmaps:
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000

	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000

	.byte %01100110
	.byte %00111100
	.byte %01011010
	.byte %11111111
	.byte %10111101
	.byte %10100101
	.byte %00100100
	.byte %00000000

	.byte %00100100
	.byte %00100100
	.byte %00111100
	.byte %01011010
	.byte %11111111
	.byte %10111101
	.byte %01000010
	.byte %10000001

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
