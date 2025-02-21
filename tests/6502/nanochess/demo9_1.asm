	;
	; Diamond Craze (chapter 9 of Programming Games for Atari 2600)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jul/01/2022.
	; Revision date: Jul/03/2022. Completed.
	; Revision date: Nov/13/2022. Added NTSC definition to choose NTSC/PAL.
	;

	.lang 6502
	.include vcs.asm

NTSC	= 1		; Define to 1 for NTSC, 0 for PAL

@frame	= $80		; Displayed frames count.
NEXTSPR	= $81		; Next sprite to show.
MODE	= $82		; Game mode.
LIVES	= $83		; Total lives remaining.
TEMP1	= $84		; Temporary variable 1.
TEMP2	= $85		; Temporary variable 2.
TEMP3	= $86		; Temporary variable 3.
Rand	= $87		; Pseudorandom number.
SPRITE0	= $88		; Pointer to bitmap for sprite 0.
SPRITE1	= $90		; Pointer to bitmap for sprite 1.
YPOS0	= $98		; Y-position for sprite 0.
YPOS1	= $99		; Y-position for sprite 1.
XPOS	= $9a		; X-position of things.
YPOS	= $9f		; Y-position of things.
SPRITE	= $a4		; Sprite number of things.
DIR	= $a9		; Direction/state of things.
SCORE1	= $ae		; First digit of score.
SCORE2	= $af		; Second digit of score.
ROW	= $b0		; Maze row.
SOUND	= $b1		; Sound effect duration.

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

	lda #$00	; Configure SWCHA as input
	sta @swacnt
	sta @swbcnt	; Also SWCHB

	lda #0		; Game stopped.
	sta MODE	; Set mode.

	jsr restart_game	; Prepare variables.

show_frame:
	lda #$00	; Black.
	sta @colubk	; Background color.
	lda #$88	; Blue.
	sta @colupf	; Playfield color.
	lda #$01	; Mirror right side.
	sta @ctrlpf

	sta HMCLR	; Clear horizontal motion registers

	sta CXCLR	; Clear collision registers

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
    IF NTSC
	lda #42		; Time for NTSC top border
    ELSE
	lda #71		; Time for PAL top border
    ENDIF
	sta @tim64t
	lda #0		; End of vertical retrace.
	sta @vsync

	ldx NEXTSPR	; Get current sprite.
	INX		; Increment.
	CPX #5		; Is it 5?
	bne M1		; No, jump.
	ldx #0		; Make it zero.
M1:	stx NEXTSPR	; Save new current sprite.
	
	lda YPOS,X	; Get Y-position of sprite.
	clc
	adc #8		; Adjust for faster drawing.
	sta YPOS0	; Save Y-position for player 0.

	lda SPRITE,X	; Get frame of sprite.
	tay
	lda sprites_color,Y	; Get color.
	sta @colup0	; Set color for player 0.

	TYA		; Index frame to bitmaps.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	tay
 	lda sprites_bitmaps,Y	; Copy in RAM for faster drawing.
	sta SPRITE0
 	lda sprites_bitmaps+1,Y
	sta SPRITE0+1
 	lda sprites_bitmaps+2,Y
	sta SPRITE0+2
 	lda sprites_bitmaps+3,Y
	sta SPRITE0+3
 	lda sprites_bitmaps+4,Y
	sta SPRITE0+4
 	lda sprites_bitmaps+5,Y
	sta SPRITE0+5
 	lda sprites_bitmaps+6,Y
	sta SPRITE0+6
 	lda sprites_bitmaps+7,Y
	sta SPRITE0+7

	lda XPOS,X	; Desired X position
	ldx #0		; Player 0
	jsr x_position	; Set position.

	ldx NEXTSPR	; Get current sprite.
	INX		; Increment.
	CPX #5		; Is it 5?
	bne M2		; No, jump.
	ldx #0		; Make it zero.
M2:	stx NEXTSPR	; Save new current sprite.

	lda YPOS,X	; Get Y-position of sprite.
	clc
	adc #8		; Adjust for faster drawing.
	sta YPOS1	; Save Y-position for player 1.

	lda SPRITE,X	; Get frame of sprite.
	tay
	lda sprites_color,Y	; Get color.
	sta @colup1	; Set color for player 1.

	TYA		; Index frame to bitmaps.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	tay
 	lda sprites_bitmaps,Y	; Copy in RAM for faster drawing.
	sta SPRITE1
 	lda sprites_bitmaps+1,Y
	sta SPRITE1+1
 	lda sprites_bitmaps+2,Y
	sta SPRITE1+2
 	lda sprites_bitmaps+3,Y
	sta SPRITE1+3
 	lda sprites_bitmaps+4,Y
	sta SPRITE1+4
 	lda sprites_bitmaps+5,Y
	sta SPRITE1+5
 	lda sprites_bitmaps+6,Y
	sta SPRITE1+6
 	lda sprites_bitmaps+7,Y
	sta SPRITE1+7

	lda XPOS,X	; Desired X position.
	ldx #1		; Player 1.
	jsr x_position	; Set position.

	sta @wsync	; Wait for scanline start.
	sta @hmove	; Write @hmove, only can be done
			; just after sta @wsync.

	lda #0
	sta ROW		; Index into maze data.

	;
	; Macros for sprite handler.
	; This only defines the macros.
	;
	; No code is generated until the
	; macros are invoked.
	;

	MAC sprite_handler_prev

	dec YPOS0	; 5: Decrement Y-coordinate for player 0.
	dec YPOS1	; 10: Decrement Y-coordinate for player 1.

	lda #$00	; 12: No graphic for player 1.
	ldx YPOS1	; 15: Get Y-coordinate for player 1.
	CPX #$08	; 17: Is it visible?
	BCS .+4		; 19: No, jump.
	lda SPRITE1,X	; 23: Load graphic from bitmap.
	tay		; 25: Save into Y register.

	lda #$00	; 27: No graphic for player 0.
	ldx YPOS0	; 30: Get Y-coordinate for player 0.
	CPX #$08	; 32: Is it visible?
	BCS .+4		; 34: No, jump.
	lda SPRITE0,X	; 38: Load graphic from bitmap.
	tax		; 40: Save into X register.

	ENDM

	MAC sprite_handler_post

	stx @grp0	; 3: Setup graphic for player 0.
	sty @grp1	; 6: Setup graphic for player 1.

	ENDM

	; Move player
	lda MODE	; Read game state.
	cmp #1		; Game playing?
	bne M27		; No, jump.
	lda SWCHA	; Read the joystick.
	and #$F0	; Separate joystick 1 bits.
	cmp #$F0	; Any movement?
	BEQ M27		; No, jump.
	ldy #$83	; Right + bitmask.
	ROL		; Move to right? (rotate bit into carry)
	BCC M5		; Yes, jump. (carry = 0)
	ldy #$42	; Left + bitmask.
	ROL		; Move to left? (rotate bit into carry)
	BCC M5		; Yes, jump. (carry = 0)
	ldy #$21	; Down + bitmask.
	ROL		; Move to down? (rotate bit into carry)
	BCC M5		; Yes, jump. (carry = 0)
	ldy #$10	; Up + bitmask (it must be)
M5:	sty TEMP2	; Desired direction.
	ldx #0		; X = 0 (player index)
	jsr aligned	; Player is grid-aligned?
	BCC M6		; No, jump.
	jsr can_move	; Get possible directions
	lda TEMP2	; Get desired direction.
	and #$F0	; Separate bitmask.
	BIT TEMP1	; Can it move?
	bne M28		; No, jump.
	lda TEMP2	; Get desired direction.
	and #$03	; Separate number.
	sta DIR		; Put as new direction.
M28:	ldy DIR		; Get current direction.
	lda bit_mapping+4,Y	; Get bitmask.
	BIT TEMP1	; Can it move?
	bne M27		; No, jump.
M6:	jsr move_sprite	; Move player.
M27:

	sprite_handler_prev

wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	lda #$88	; Color of playfield
	sta @colupf
	lda #1
	sta @ctrlpf	; Mirrored playfield

	sta @wsync
	lda #0		; Disable blanking
	sta @vblank
	sta HMCLR

M3:
	sta @wsync	; 3:
	sprite_handler_post	; 9:
	ldx ROW		; 12:
	lda Maze_@pf0,X	; 16: Read maze pixels PF0.
	sta @pf0		; 19: Set TIA PF0
	lda Maze_@pf1,X	; 23: Read maze pixels PF1.
	sta @pf1		; 27: Set TIA PF1
	lda Maze_@pf2,X	; 30: Read maze pixels PF2.
	sta @pf2		; 34: Set TIA PF2

	sprite_handler_prev	; 74: Just in time.

	sta @wsync	; 3:
	sprite_handler_post	; 9:
	inc ROW		; 14:
	sprite_handler_prev	; 54:

	sta @wsync	; 3:
	sprite_handler_post	; 9:
	sprite_handler_prev	; 49:

	sta @wsync	; 3:
	sprite_handler_post	; 9:
	sprite_handler_prev	; 49:

	lda ROW		; 52:
	cmp #46		; 54: Has it displayed all rows?
	BEQ M4		; 56: Yes, exit loop.
	jmp M3		; 59: No, jump back to display.
M4:

	sta @wsync	
	lda #$C8	; Green color.
	sta @colup0	; For score digit 0.
	sta @colup1	; For score digit 1.
	ldx LIVES	; Get current number of lifes.
	lda bitmap_lives,X	; Index into table.
	ldy #0		; Zero for other playfield registers.
	sty @pf0		; Zero for PF0.
	sta @pf1		; Lifes in PF1.
	sty @pf2		; Zero for PF2.
	lda SCORE1	; First score digit.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	sta TEMP1	; Store offset.
	lda SCORE2	; Second score digit.
	ASL		; x2
	sta @resp0	; Position first digit.
	sta @resp1	; Position second digit.
	ASL		; x4
	ASL		; x8
	sta TEMP2	; Store offset.

	ldy #7		; 7 scanlines for score
M38:	sta @wsync	; Synchronize with scanline.
	ldx TEMP1	; Row on score 1.
	lda numbers_bitmaps,X	; Read bitmap.
	sta @grp0	; Write as graphic for player 0.
	ldx TEMP2	; Row on score 2.
	lda numbers_bitmaps,X	; Read bitmap.
	sta @grp1	; Write as graphic for player 1.
	inc TEMP1	; Increase row of score 1.
	inc TEMP2	; Increase row of score 2.
	dey		; Decrease scanlines to display.
	bne M38		; Jump if still there are some.

	lda #2		; Enable blanking
	sta @wsync
	sta @vblank

    IF NTSC
	lda #35		; Time for NTSC bottom border
    ELSE
	lda #64		; Time for PAL bottom border
    ENDIF
	sta @tim64t

	lda #0		; Disable ALL graphics.
	sta @pf0		; Playfield.
	sta @pf1
	sta @pf2
	sta @grp0	; Player 0.
	sta @grp1	; Player 1.
	sta @enam0	; Missile 0.
	sta @enam1	; Missile 1.
	sta @enabl	; Ball.

	;
	; Detect reset pressed (stops the game)
	;
	lda SWCHB	; Read console switches.
	and #1		; Reset pressed?
	bne M33		; No, jump.
	lda #0		; Disable game.
	sta MODE	; Set mode.
M33:
	;
	; Detect select pressed (starts the game)
	;
	lda SWCHB	; Read console switches.
	and #2		; Select pressed?
	bne M32		; No, jump.
	lda #1		; Start game.
	sta MODE	; Set mode.
	lda #3		; 3 lifes to start.
	sta LIVES	; Set variable.
	lda #0		; Reset score.
	sta SCORE1
	sta SCORE2
	jsr restart_game	; Reset enemies/diamond.
M32:
	;
	; Detect gameplay mode
	;
	lda MODE
	cmp #1		; Gameplay enabled?
	BEQ M34		; Yes, jump.
	ldx #0		; Turn off background "music"
	stx AUDV1
	cmp #2		; Dead player?
	bne M36		; No, jump.
	dec TEMP3	; Countdown.
	bne M36		; Completed? No, jump.
	dec LIVES	; Decrease one life.
	BEQ M37		; Zero? Yes, jump.
	jsr restart_game	; Reset enemies/diamond.
	lda #1		; Restart game.
	sta MODE
	jmp M35

M37:	lda #0		; Disable game.
	sta MODE
M36:
	jmp M35
M34:
	; Background siren (arcade-like)
	lda @frame	; Get frame number.
	and #$10	; In alternate 16 frames?
	BEQ M41		; No, jump.
	lda @frame	; Read frame number.
	and #$0F	; Modulo 16.
	EOR #$0F	; Exclusive OR gets value 15-0
	jmp M42

M41:	lda @frame	; Read frame number.
	and #$0F	; Modulo 16 (0-15)
M42:	sta AUDF1	; Set frequency.
	lda #12		; Set volume.
	sta AUDC1
	lda #6		; Set shape.
	sta AUDV1

	; Catch diamond
	lda XPOS	; X-position of player.
	cmp XPOS+4	; Is same as X-position of diamond?
	bne M7		; No, jump.
	lda YPOS	; Y-position of player.
	cmp YPOS+4	; Is same as Y-position of player?
	bne M7		; No, jump.
	lda #6		; Start sound effect.
	sta AUDF0
	lda #6
	sta AUDC0
	lda #12
	sta AUDV0
	lda #15		; Duration: 15 frames.
	sta SOUND
	jsr restart_diamond	; Put another diamond.
	inc SCORE2	; Increase low-digit of score.
	lda SCORE2
	cmp #10
	bne M7
	lda #0
	sta SCORE2
	inc SCORE1	; Increase high-digit of score.
	lda SCORE1
	cmp #10
	bne M7
	lda #9		; Limit to 99.
	sta SCORE2
	sta SCORE1
M7:

	; Enemy catches player
	ldx #1		; Enemy 1
M8:	lda XPOS	; X-position of player.
	SEC
	SBC XPOS,X	; Minus X-position of enemy.
	BCS M29		; Jump if result is positive (no borrow).
	EOR #$FF	; Negate.
	adc #1
M29:	cmp #4		; Near to less than 4 pixels?
	BCS M30		; No, jump.
	lda YPOS	; Y-position of player.
	SEC
	SBC YPOS,X	; Minus Y-position of enemy.
	BCS M31		; Jump if result is positive (no borrow).
	EOR #$FF	; Negate.
	adc #1
M31:	cmp #4		; Near to less than 4 pixels?
	BCS M30		; No, jump.
	lda #2		; Player dead.
	sta MODE	; Set mode.
	lda #60		; 60 frames.
	sta TEMP3	; Set counter.
	lda #30		; Start sound effect.
	sta AUDF0
	lda #6
	sta AUDC0
	lda #12
	sta AUDV0
	lda #30		; Duration: 30 frames.
	sta SOUND
M30:
	INX		; Go to next enemy.
	CPX #4		; All enemies checked?
	bne M8		; No, continue.

	;
	; Move enemies.
	;
	lda SWCHB	; Read console switches.
	and #$40	; Difficulty 1 is (A)dvanced?
	bne M39		; Yes, jump.
	lda @frame	; Get current frame number.
	and #1		; Only move each 2 frames.
	bne M39		; Jump if enemies can move.
	jmp M26		; Or avoid code.
M39:
	lda #1		; Enemy 1.
M16:	pha		; Save counter.
	tax		; Put into X to access enemy.
	jsr aligned	; Enemy is grid-aligned?
	BCC M17		; No, jump to move.
	jsr can_move	; Get possible directions.
	lda DIR,X	; Current direction.
	cmp #2		; Is it up or down?
	BCS M21		; No, jump.
	lda XPOS,X	; Get enemy X-coordinate.
	cmp XPOS	; Compare with player X-coordinate.
	BEQ M25		; Same? Try to move in same direction.
	BCC M22		; If enemy is to the left, jump.
	ldy #2
	lda #$40	; Left direction.
	BIT TEMP1	; Can it go?
	BEQ M23		; Yes, jump.
M22:	ldy #3		
	lda #$80	; Right direction.
	BIT TEMP1	; Can it go?
	BEQ M23		; Yes, jump.
	ldy #2
	lda #$80	; Left direction.
	BIT TEMP1	; Can it go?
	BEQ M23		; Yes, jump.
M25:	ldy DIR,X	; Get current direction.
	lda bit_mapping+4,Y	; Get bitmask.
	BIT TEMP1	; Can keep going?
	BEQ M23		; Yes, jump.
	INY		; Try other direction.
	TYA
	and #$03	; Limit to four.
	sta DIR,X	; Update direction.
	jmp M25		; Verify if can move.

M21:	lda YPOS,X	; Get enemy Y-coordinate.
	cmp YPOS	; Compare with player Y-coordinate.
	BEQ M25		; Same? Try to move in same direction.
	BCC M24		; If the enemy is above player, jump.
	ldy #0
	lda #$10	; Up direction.
	BIT TEMP1	; Can it go?
	BEQ M23		; Yes, jump.
M24:	ldy #1
	lda #$20	; Down direction.
	BIT TEMP1	; Can it go?
	BEQ M23		; Yes, jump.
	ldy #0
	lda #$10	; Up direction.
	BIT TEMP1	; Can it go?
	bne M25		; No, try same direction or another.

M23:	sty DIR,X	; Write new movement direction.

M17:	jsr move_sprite	; Move enemy
	lda @frame	; Get current frame number.
	and #4		; Each four frames change animation.
	@lsr
	@lsr
	adc #1
	sta SPRITE,X	; Update animation frame.
	PLA		; Restore counter.
	clc		; Increment by one.
	adc #1
	cmp #4		; Processed all three enemies?
	BEQ M26		; Yes, jump.
	jmp M16		; No, continue.
M26:

M35:
	;
	; Turn off sound effect when playing finished.
	;
	dec SOUND	; Decrement duration of sound effect.
	bne M40		; Jump if not zero.
	lda #0		; Turn off volume.
	sta AUDV0
M40:

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	inc @frame	; Count frames

	jmp show_frame	; Repeat the game loop.

	;
	; Restart the game.
	;
restart_game:
	ldx #19		; 5 XPOS + 5 YPOS + 5 SPRITE + 5 DIR - 1
M9:
	lda start_positions,X	; Load initialization table.
	sta XPOS,X	; Update RAM.
	dex		; Decrement counter.
	bpl M9		; Jump if still positive.
restart_diamond:
	jsr random	; Get a pseudorandom number.
	and #$07	; Modulo 8.
	tax		; Copy to X for index.
	lda x_diamond,X	; Get X position for diamond.
	sta XPOS+4	; Set X of diamond.
	lda y_diamond,X	; Get Y position for diamond.
	sta YPOS+4	; Set Y of diamond.
	RTS

	;
	; Start positions for all sprites.
	;
start_positions:
	.byte 77,61,69,85,85	; Values for XPOS.
	.byte 100,76,76,76,76	; Values for YPOS.
	.byte 0,1,1,1,3		; Sprite frame number.
	.byte 0,2,0,0,0		; Starting movement direction.

	; Coordinates where diamonds can appear
	; X and Y coordinates are paired.
x_diamond:
	.byte 5,149,81,81,81,89,149,5
y_diamond:
	.byte 4,4,28,76,132,156,172,172

	; Playfield bitmap for available lives.
bitmap_lives:
	.byte $00,$80,$a0,$a8

	;
	; Generates a pseudo-random number.
	;
random:
	lda Rand
	SEC
	ROR
	EOR @frame
	ROR
	EOR Rand
	ROR
	EOR #9
	sta Rand
	RTS

	;
	; Detect if a sprite is grid-aligned
	;
	; X = Sprite number (0-4)
	;
	; Returns: Carry flag set if it is grid-aligned.
	;
aligned:
	lda XPOS,X	; Get the X-position of sprite.
	SEC
	SBC #5		; Minus 5.
	and #7		; Modulo 8.
	bne M15		; If not zero, jump.
	lda YPOS,X	; Get the Y-position of sprite.
	SEC
	SBC #4		; Minus 4.
	and #7		; Modulo 8.
	bne M15		; If not zero, jump.
	SEC		; Set carry flag (aligned).
	RTS

M15:	clc		; Clear carry flag (unaligned).
	RTS

	;
	; Detect possible directions for a sprite.
	; The sprite should be grid-aligned.
	;
	; X = Sprite number (0-4)
	;
can_move:
	TXA
	pha
	; Test for up direction.
	lda YPOS,X	; Y-coordinate of sprite.
	tay		; Copy into Y.
	dey		; One pixel upwards
	lda XPOS,X	; X-coordinate of sprite.
	tax		; Copy into X.
	jsr hit_wall	; Hit wall?
	ROR TEMP1	; Insert carry into bit 7.

	PLA
	tax
	pha
	; Test for down direction.
	lda YPOS,X	; Y-coordinate of sprite.
	clc
	adc #8		; 8 pixels downward.
	tay		; Put into Y.
	lda XPOS,X	; X-coordinate of sprite.
	tax		; Copy into X.
	jsr hit_wall	; Hit wall?
	ROR TEMP1	; Insert carry into bit 7.

	PLA
	tax
	pha
	; Test for left direction.
	lda YPOS,X	; Y-coordinate of sprite.
	tay		; Put into Y.
	lda XPOS,X	; X-coordinate of sprite.
	tax		; Put into X.
	dex		; One pixel to left.
	jsr hit_wall	; Hit wall?
	ROR TEMP1	; Insert carry into bit 7.

	PLA
	tax
	pha
	; Test for right direction.
	lda YPOS,X	; Y-coordinate of sprite.
	tay		; Put into Y.
	lda XPOS,X	; X-coordinate of sprite.
	clc
	adc #8		; 8 pixels to the right.
	tax		; Put into X.
	jsr hit_wall	; Hit wall?
	ROR TEMP1	; Insert carry into bit 7.
	PLA
	tax
	RTS

	; Detect wall hit
	; X = X-coordinate
	; Y = Y-coordinate
hit_wall:
	TYA		; Copy Y into A
	@lsr		; /2
	@lsr		; /4
	sta TEMP3	; Maze row to test.
	dex		
	TXA		; Copy X into A.
	@lsr		; Divide by 4 as each playfield...
	@lsr		; ...pixel is 4 pixels.
	tax
	lda wall_mapping,X
	and #$F8	; Playfield register to test (@pf0/@pf1/@pf2)
	clc
	adc TEMP3	; Add to maze row to create byte offset.
	tay		; Y = Playfield byte offset
	lda wall_mapping,X
	and #$07	; Extract bit number.
	tax
	lda bit_mapping,X	; Convert to bit mask.
	and Maze_@pf0,Y	; Check against maze data
	BEQ no_hit	; Jump if zero (no hit).

	SEC		; Wall hit.
	RTS

no_hit:	clc		; No wall hit.
	RTS

	; Mapping of horizontal pixel to maze byte.
wall_mapping:
	.byte $04,$05,$06,$07
	.byte $37,$36,$35,$34,$33,$32,$31,$30
	.byte $60,$61,$62,$63,$64,$65,$66,$67
	.byte $67,$66,$65,$64,$63,$62,$61,$60
	.byte $30,$31,$32,$33,$34,$35,$36,$37
	.byte $07,$06,$05,$04

	; Conversion of bit to bitmask.
bit_mapping:
	.byte $01,$02,$04,$08,$10,$20,$40,$80

	;
	; Move a sprite in the current direction.
	;
move_sprite:
	lda DIR,X	; Get the current direction of sprite.
	cmp #3		; Right?
	BEQ M20		; Yes, jump.
	cmp #2		; Left?
	BEQ M19		; Yes, jump.
	cmp #1		; Down?
	BEQ M18		; Yes, jump.
	dec YPOS,X	; Must be up. Decrease Y-coordinate.
	RTS

M18:	inc YPOS,X	; Increase Y-coordinate.
	RTS

M19:	dec XPOS,X	; Decrease X-coordinate.
	RTS

M20:	inc XPOS,X	; Increase X-coordinate.
	RTS

	org $fe00

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

; mode: symmetric mirrored line-height 4

Maze_@pf0:
	.byte $F0,$10,$10,$90,$90,$90,$90,$10
	.byte $10,$90,$90,$90,$90,$10,$10,$F0
	.byte $00,$00,$00,$00,$00,$00,$00,$00
	.byte $00,$00,$00,$00,$F0,$10,$10,$90
	.byte $90,$10,$10,$70,$40,$40,$70,$10
	.byte $10,$90,$90,$10,$10,$F0,$00,$00
Maze_@pf1:
	.byte $FF,$00,$00,$E7,$24,$24,$E7,$00
	.byte $00,$E6,$26,$26,$E6,$06,$06,$E7
	.byte $24,$24,$27,$26,$26,$20,$20,$26
	.byte $26,$26,$26,$26,$E6,$00,$00,$E7
	.byte $E7,$60,$60,$66,$66,$66,$66,$06
	.byte $06,$FF,$FF,$00,$00,$FF,$00,$00
Maze_@pf2:
	.byte $3F,$20,$20,$27,$24,$24,$E7,$00
	.byte $00,$FE,$02,$02,$FE,$80,$80,$9F
	.byte $90,$90,$9F,$00,$00,$FE,$02,$02
	.byte $FE,$00,$00,$FE,$FE,$80,$80,$9F
	.byte $9F,$00,$00,$FE,$02,$02,$FE,$80
	.byte $80,$9F,$9F,$00,$00,$FF,$00,$00

	; Color for each sprite frame
sprites_color:
	.byte $2e,$5e,$5e,$0e

	; Bitmaps for each sprite frame.
sprites_bitmaps:
	.byte %00111100	; 0: Happy face.
	.byte %01111110
	.byte %11000011
	.byte %10111101
	.byte %11111111
	.byte %10011001
	.byte %01111110
	.byte %00111100

	.byte %11000000	; 1: Monster 1.
	.byte %01111100
	.byte %01000011
	.byte %11100110
	.byte %11111111
	.byte %11011011
	.byte %10111101
	.byte %01111110

	.byte %00000011	; 2: Monster 2.
	.byte %00111110
	.byte %11000010
	.byte %01100111
	.byte %11111111
	.byte %11011011
	.byte %10111101
	.byte %01111110

	.byte %00000000	; 3: Diamond.
	.byte %00011000
	.byte %00110100
	.byte %01111010
	.byte %11111101
	.byte %10000011
	.byte %01111110
	.byte %00000000

	.org $FFFC
	.word start
	.word start
