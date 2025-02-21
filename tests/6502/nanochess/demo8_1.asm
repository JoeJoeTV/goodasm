	;
	; The Lost Kingdom (chapter 8)
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	; 
	; Creation date: Jun/14/2022.
	; Revision date: Jun/19/2022. Completed.
	;

	.lang 6502
	.include vcs.asm

@frame	= $0080		; Frame number.
ROOM	= $0081		; Current room (0-9).
XPOS	= $0082		; X position of player.
YPOS	= $0083		; Y position of player.
FPLAYER	= $0084		; Frame for player.
SPRITE0	= $0085		; Player sprite.
XPREV	= $0087		; Previous X position of player.
YPREV	= $0088		; Previous Y position of player.
XOBJ	= $0089		; X position of object.
YOBJ	= $008a		; Y position of object.
SPRITE1	= $008b		; Object sprite.
TEMP1	= $008d		; Temporary 1.
TEMP2	= $008e		; Temporary 2.
TEMP3	= $008f		; Temporary 3.
SEQ	= $0090		; Animated sequence.
VOBJ	= $0091		; Current object on screen.
SOUND	= $0092		; Counter to turn off sound.
OBJ	= $0093		; List of objects (room+x,y)

ROOM_DATA	= $009f	; Pointers to room data (6 bytes)

SPRCOPY	= $00A5		; Sprite copy

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

	jsr restart_game

    if 0		; Enable to 1 to test sword.
	lda #160
	sta YPOS
	lda #4		; Current room
	sta ROOM
	lda #$FF	; Sword carried
	sta OBJ+0
    endif

show_frame:
	lda #$88	; Blue.
	sta @colubk	; Background color.
	lda #$cF	; Green.
	sta @colup1	; Player 1 color.
	lda #$21	; Mirror right side. Ball 4px.
	sta @ctrlpf

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

	lda ROOM	; Get current room
	ASL		; x2
	ASL		; x4
	adc #map&255	; Add to map data (low byte)
	sta TEMP1	; Setup low byte of address.
	lda #map>>8	; High-byte.
	adc #0		; Carry add.
	sta TEMP2	; Setup high byte of address.
	ldy #0		; Offset into map data.
	lda (TEMP1),Y	; Read first byte.
	sta ROOM_DATA	; Store low byte of address.
	INY
	lda (TEMP1),Y	; Read second byte.
	sta ROOM_DATA+1	; Store high byte of address.
	INY
	lda (TEMP1),Y	; Get room color.
	sta @colupf	; Set color for playfield.
	INY
	lda (TEMP1),Y	; Get extra wall.
	BEQ M9		; Jump if no walls.
	ldx #157	; X = 157 for right wall.
	cmp #1		; Right wall?
	BEQ M8		; Yes, jump.
	ldx #2		; X = 2 for left wall.
M8:	TXA		; A = X
	ldx #4		; Ball.
	jsr x_position	; Setup position.
	lda #2		; Enable ball as wall.
	sta @enabl	; Remains enabled.
M9:

	lda ROOM_DATA	; Low byte of room data address.
	clc
	adc #24		; Add 24 bytes (jump @pf0 regs).
	sta ROOM_DATA+2	; Low byte of 2nd room data addr.
	lda ROOM_DATA+1	; Low byte.
	adc #0		; Carry for 2nd room data addr.
	sta ROOM_DATA+3	; High byte.
	lda ROOM_DATA+2	
	clc
	adc #24		; Add 24 bytes (jump @pf1 regs).
	sta ROOM_DATA+4	; Low byte of 3rd room data addr.
	lda ROOM_DATA+3
	adc #0		; Carry for 3rd room data addr.
	sta ROOM_DATA+5	; High byte.

	lda FPLAYER	; Current frame for player.
	ASL		; x2
	tay		; Put in Y to use as index.
	lda ply_sprite,Y	; Read address of graphic data.
	sta SPRITE0	; Low-byte of address.
	lda ply_sprite+1,Y	; Read address of graphic data.
	sta SPRITE0+1	; High-byte of address.
	ldy #$0F	; Point to highest byte of sprite.
M34:	lda (SPRITE0),Y	; Copy from sprite data.
	sta SPRCOPY,Y	; Into zero-page memory.
	dey		; Decrement counter.
	bpl M34		; Jump if it still is positive.

	lda #$E0	; Y-coordinate for non-visible object.
	sta YOBJ	; Save.
	lda #$FF	; No visible object.
	sta VOBJ	; Save.

	ldx #0		; Index into object table.
	ldy #0		; Index into graphics table.
M10:	lda OBJ,X	; Read object room.
	cmp #$FF	; Object carried?
	BEQ M12		; Yes, jump.
	cmp ROOM	; Is it at same room?
	bne M11		; No, jump.
M12:
	sty VOBJ	; Save current object on screen.
	lda SEQ		; Sequency animation.
	cmp #3		; At step 3 or higher?
	BCC M30		; No, jump.
	TYA		
	clc
	adc SEQ		; Add sequency step x2
	adc SEQ		; (for hand coming out of lake)
	adc #2		; Add 2
	tay		; Save as new graphics index.
M30:
	lda obj_sprite,Y	; Graphic low-byte addr.
	sta SPRITE1	; Save.
	lda obj_sprite+1,Y	; Graphic high-byte addr.
	sta SPRITE1+1	; Save.
	lda OBJ,X
	cmp #$FF	; Object being carried?
	bne M14		; No, jump.
	lda XPOS	; Get X-coordinate of player.
	clc
	adc #8		; Put at right of player.
	sta XOBJ	; Save X-coordinate of object.
	lda YPOS	; Get Y-coordinate of player.
	sta YOBJ	; Put object at same coordinate.
	jmp M15
M14:
	lda OBJ+1,X	; Get X-coordinate from table.
	sta XOBJ
	lda OBJ+2,X	; Get Y-coordinate from table.
	sta YOBJ
M11:	INY
	INY
	INX
	INX
	INX
	CPX #12		; All objects revised?
	bne M10		; Branch if not.
M15:
	lda XOBJ	; Desired X position
	ldx #1		; Player 1
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

	;
	; Macro for sprite handler.
	; This only defines the macro.
	;
	; No code is generated until the
	; macro is invoked.
	;
	MAC sprite_handler

	BCS .+9		; 2
	lda player_color,X	; 6
	sta @colup0	; 9
	lda SPRCOPY,X	; 13
	sta @grp0	; 16

	lda #$00	; 18
	cpy #$F0	; 20
	BCC .+4		; 22
	lda (SPRITE1),Y	; 27
	sta @grp1	; 30

	ENDM
	; End of macro

	lda #0		; Index into room data.
	sta TEMP1	; Save for counting.

	lda YPOS	; Y-coordinate of player.
	clc
	adc #$10	; Plus $10 to simplify access.
	tax		; Save into X for counting.

	lda YOBJ	; Y-coordinate of object.
	sta TEMP3	; Save for counting.

M1:
	ldy TEMP1	; Get index of room data.
	lda (ROOM_DATA),Y	; Read pixels @pf0.
	sta @wsync	; 3
	sta @pf0		; 6
	lda (ROOM_DATA+2),Y	; 11 Read pixels @pf1.
	sta @pf1		; 14
	lda (ROOM_DATA+4),Y	; 19 Read pixels @pf2.
	sta @pf2		; 22
	ldy TEMP3	; 25 Y-coordinate of object.
	lda #$00	; 27 For resetting @grp0.
	CPX #$10	; 29 Player is visible?
	sprite_handler	; 59 Invoke macro.
	dex		; 61 Y counter of player.
	dey		; 63 Y counter of object.

	lda #$00	; 65
	CPX #$10	; 67

	sta @wsync	
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	lda #$00	; For resetting @grp0.	
	CPX #$10	; Player is visible?

	sta @wsync
	sprite_handler	; Invoke macro.
	dex		; Y counter of player.
	dey		; Y counter of object.
	sty TEMP3	; Save Y counter of object.

	inc TEMP1	; Increment index into room data.
	lda TEMP1
	cmp #24		; Has it reached 24 rows?
	BEQ M2		; Yes, exit loop.
	jmp M1		; No, jump back to display.
M2:

	lda #2		; Enable blanking
	sta @wsync
	sta @vblank

	lda #35		; Time for NTSC bottom border
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

	; Reset game.
	lda SWCHB
	and #$02	; Select pressed?
	bne M25		; No, jump.
	jsr restart_game	; Restart game.
	jmp M17
M25:

	; Sequence animation.
	lda SEQ		; Sequence animation.
	cmp #1		; Game won?
	bne M26		; No, jump.
	jmp M17
M26:

	lda SEQ		; Sequence animation.
	BEQ M27		; Active? Branch if not.
	lda OBJ+2	; Use Y-coordinate of sword
	@lsr		; as frequency for sound effect.
	@lsr
	sta AUDF0
	lda #12
	sta AUDV0
	lda #6
	sta AUDC0
	lda OBJ+2	; Y-coordinate of sword.
	cmp #$18	; Has it reached center of lake?
	BEQ M28		; Yes, jump.
	dec OBJ+2	; Keep moving sword upwards.
	jmp wait_for_bottom

M28:	lda @frame	; Read frame counter.
	and #7		; Modulo 8.
	bne M31		; Branch if not zero.
	lda SEQ		; Use sequence counter as...
	sta AUDF0	; ...sound frequency.
	lda #12
	sta AUDV0
	lda #14
	sta AUDC0
	inc SEQ		; Increase sequence.
M31:	lda SEQ
	cmp #11		; Hand disappears?
	BEQ M29		; Yes, jump.
	jmp wait_for_bottom

M29:	lda #0		; End animation sequence.
	sta SEQ
	lda #$FF	; Now player carries crown
	sta OBJ+9
	lda #4		; Sound effect.
	sta AUDF0
	lda #6
	sta AUDC0
	lda #12
	sta AUDV0
	lda #30		; Duration: 30 frames.
	sta SOUND
	jmp wait_for_bottom

	; Player catches object.
M27:
	lda CXPPMM	; Player 0 vs Player 1.
	bpl M16		; Jump if no collision.
	lda VOBJ	; Visible object?
	BMI M16		; Jump if no visible object.
	@lsr		; /2
	adc VOBJ	; +VOBJ = x3
	tax		; Move to X to use as index.
	lda #$FF
	sta OBJ,X	; Mark object as being carried.
	lda #12		; Sound effect.
	sta AUDV0
	lda #5
	sta AUDF0
	lda #4
	sta AUDC0
	lda #3		; Duration: 3 frames.
	sta SOUND
M16:
	; Player "rebounds" on wall.
	lda CXP0FB	; Hit of player 0 vs Playfield/Ball
	and #$C0	; Any of the two?
	BEQ M7		; No, jump.
	lda XPREV	; Restore old X coordinate.
	sta XPOS
	lda YPREV	; Restore old Y coordinate.
	sta YPOS
	lda #12		; Sound effect.
	sta AUDV0
	lda #30
	sta AUDF0
	lda #6
	sta AUDC0
	lda #3		; Duration: 3 frames.
	sta SOUND
M7:
	; Move player
	lda XPOS	; Save current X coordinate.
	sta XPREV
	lda YPOS	; Save current Y coordinate.
	sta YPREV

	lda SWCHA	; Read joystick.
	and #$f0	; 
	cmp #$e0	; Going up?
	BEQ M24		; Yes, jump.
	jmp M3		; No, jump.
M24:
	lda @frame	; Get current frame counter.
	@lsr		; /2
	@lsr		; /4
	and #1		; Module 2 = 0 or 1.
	EOR #2		; Plus frame for going up sprite.
	sta FPLAYER	; Set new player frame.
	dec YPOS	; Move player 2 pixels upward.
	dec YPOS
	lda YPOS
	cmp #$68	; Reached door position?
	bne M20		; No, jump.
	lda XPOS	; Current X-coordinate.
	cmp #$45	; At left of door?
	BCC M20		; Yes, jump.
	cmp #$56	; At right of door?
	BCS M20		; Yes, jump.
	lda ROOM	; Get current room.
	cmp #1		; Castle?
	bne M20		; No, jump.
	lda VOBJ	; Current visible object
	cmp #$06	; Crown in room?
	bne M20		; No, jump.
	lda OBJ+9
	cmp #$FF	; Carrying it?
	bne M20		; No, jump.
	lda #0		; Hall room.
	sta ROOM	; Change room.
	sta OBJ+9	; Crown in room.
	lda XPOS	; X-coordinate of player.
	sta OBJ+10	; Setup as X-coordinate of crown.
	lda #156	; Y-coordinate at bottom...
	sta YPOS	; ...for player.
	SEC
	SBC #6		; Minus 6 pixels.
	sta OBJ+11	; Set for crown.
	lda #8		; Triumphant face.
	sta FPLAYER
	lda #1		; End of game.
	sta SEQ
	lda #4		; Sound effect.
	sta AUDF0
	lda #4
	sta AUDC0
	lda #12
	sta AUDV0
	lda #60		; Duration: 60 frames.
	sta SOUND
	jmp M3

M20:	lda YPOS	; Current Y-coordinate.
	cmp #$88	; Border of lake?
	bne M21		; No, jump.
	lda ROOM	; Get current room.
	cmp #4		; Lake?
	bne M21		; No, jump.
	lda VOBJ	; Current visible object
	cmp #$00	; Sword in room?
	bne M21		; No, jump.
	lda OBJ+0
	cmp #$FF	; Carrying it?
	bne M21		; No, jump.
	lda #4		; Sword at room.
	sta OBJ+0
	lda #80		; Sword X-coordinate.
	sta OBJ+1
	lda #$78	; Sword Y-coordinate.
	sta OBJ+2
	lda #2		; Start animated sequence
	sta SEQ
	jmp M3

M21:	lda YPOS	; Get current Y-coordinate?
	cmp #0		; Reached top?
	bne M22		; No, jump.
	lda ROOM	; Get current room?
	cmp #5		; Dead woods (upper room)?
	bne M22		; No, jump.
	lda VOBJ	; Current visible object
	cmp #$04	; Key in room?
	bne M23		; No, jump.
	lda OBJ+6	; Get key room.
	cmp #$FF	; Carrying it?
	bne M23		; No, jump.
	lda #5		; Leave key in room.
	sta OBJ+6
	lda #$10	; Setup X-coordinate for key.
	sta OBJ+7
	lda #$08	; Setup Y-coordinate for key.
	sta OBJ+8
	jmp M22

M23:	inc YPOS	; Player doesn't move upward.
	inc YPOS
M22:
	lda YPOS
	cmp #0		; Player reached top?
	bne M3		; No, jump.
	lda #170	; Player reappears at bottom.
	sta YPOS
	jsr get_connection	; Get connection map for room.
	lda map_connect,X	; Get room connecting by north.
	sta ROOM	; Save as new room.
M3:
	lda SWCHA	; Read joystick.
	and #$f0	; 
	cmp #$d0	; Going down?
	bne M4		; No, jump.
	lda @frame	; Get current frame counter.
	@lsr		; /2
	@lsr		; /4
	and #1		; Module 2 = 0 or 1.
	sta FPLAYER	; Set new player frame.
	inc YPOS	; Move player downward 2 pixels.
	inc YPOS
	lda YPOS
	cmp #174	; Reached bottom?
	bne M4		; No, jump.
	lda #8		; Player reappears at top.
	sta YPOS
	jsr get_connection	; Get connection map for room.
	lda map_connect+2,X	; Get room connecting by south.
	sta ROOM	; Save as new room.
M4:
	lda SWCHA	; Read joystick.
	and #$f0	; 
	cmp #$b0	; Going left?
	bne M5		; No, jump.
	lda @frame	; Get current frame counter.
	@lsr		; /2
	@lsr		; /4
	and #1		; Module 2 = 0 or 1.
	EOR #6		; Plus frame for going left sprite.
	sta FPLAYER	; Set new player frame.
	dec XPOS	; Move player to left one pixel.
	lda XPOS
	cmp #2		; Reached left border?
	bne M5		; No, jump.
	lda #148	; Player reappears at right.
	sta XPOS
	jsr get_connection	; Get connection map for room.
	lda map_connect+3,X	; Get room connecting by west.
	sta ROOM	; Save as new room.
M5:
	lda SWCHA	; Read joystick.
	and #$f0	; 
	cmp #$70	; Going right?
	bne M6		; No, jump.
	lda @frame	; Get current frame counter.
	@lsr		; /2
	@lsr		; /4
	and #1		; Module 2 = 0 or 1.
	EOR #4		; Plus frame for going right sprite.
	sta FPLAYER	; Set new player frame.
	inc XPOS	; Move player to right one pixel.
	lda XPOS
	cmp #154	; Reached right border?
	bne M18		; No, jump.
	lda ROOM	; Get current room.
	cmp #7		; Dead woods (lower room)?
	bne M18		; No, jump.
	lda VOBJ	; Current visible object
	cmp #$02	; Cross in room?
	bne M32		; No, jump.
	lda OBJ+3
	cmp #$FF	; Carrying it?
	BEQ M19		; Yes, jump.
M32:	dec XPOS	; Player cannot move to right.
	jmp M6

M19:	lda ROOM	; Leave cross at room.
	sta OBJ+3
	lda #$10
	sta OBJ+4
M18:	lda XPOS	; Get current X-coordinate.
	cmp #154	; Reached right?
	bne M6		; No, jump.
	lda #8		; Reappear at left.
	sta XPOS
	jsr get_connection	; Get connection map for room.
	lda map_connect+1,X	; Get room connecting by east.
	sta ROOM	; Save as new room.
M6:
	; Drop object
	lda INPT4	; Get joystick button state.
	BMI M17		; Jump if not pressed.
	lda VOBJ	; There is an object on screen?
	BMI M17		; No, jump.
	@lsr		; /2
	adc VOBJ	; +itself = x3
	tax		; Index into object table.
	lda OBJ,X	; Get room of object.
	cmp #$FF	; Is it carried?
	bne M17		; No, jump.
	lda ROOM	; Leave at current room.
	sta OBJ,X
	lda #12		; Sound effect.
	sta AUDV0
	lda #15
	sta AUDF0
	lda #4
	sta AUDC0
	lda #3		; Duration: 3 frames.
	sta SOUND
M17:
	; Counter to turn off sound effects.
	dec SOUND	; Decrement counter.
	bne M33		; Jump if not zero.
	lda #0
	sta AUDV0	; Turn off volume.
M33:

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border scanline

	inc @frame	; Count frames

	jmp show_frame	; Repeat the game loop.

	;
	; Gets the index into the connection table.
	; It is simply x = ROOM * 4
	;
get_connection:
	lda ROOM
	ASL
	ASL
	tax
	RTS

	;
	; Connection map for each room.
	;
map_connect:
	;     U R D L
	.byte 0,0,0,0	; 0 Hall.
	.byte 0,0,2,0	; 1 Castle.
	.byte 1,3,0,5	; 2 Woods.
	.byte 4,0,0,2	; 3 Shack.
	.byte 0,0,3,0	; 4 Lake.
	.byte 6,2,7,0	; 5 Dead woods.
	.byte 0,0,5,0	; 6 Cemetery.
	.byte 5,8,0,0	; 7 Dead woods.
	.byte 8,9,8,7	; 8 Caves.
	.byte 9,9,9,8	; 9 Caves.

	;
	; Restart the game.
	;
restart_game:
	lda #0		; No animated sequence.
	sta SEQ

	lda #1		; Start at room 1 (castle).
	sta ROOM

	lda #80		; X-coordinate for player.
	sta XPOS
	lda #130	; Y-coordinate for player.
	sta YPOS

	ldx #0		; Index into object initial data.
COPY:	lda obj_init,X	; Read byte from ROM.
	sta OBJ,X	; Store byte in RAM.
	INX		; Increment index.
	CPX #12		; All 12 bytes copied?
	bne COPY	; No, jump.

	RTS		; Return.

	;
	; Table with room, X, and Y coordinates of each object.
	;
obj_init:
	.byte 9,$80,$08	; Sword
	.byte 6,$20,$40	; Cross
	.byte 3,$80,$68	; Key
	.byte 0,$50,$50	; Crown (hidden)

	;
	; Table with addresses of graphics for each object.
	;
obj_sprite:
	.word objects_sprites+16-240	; Sword
	.word objects_sprites+32-240	; Cross
	.word objects_sprites+48-240	; Key
	.word objects_sprites+64-240	; Crown
	.word objects_sprites+0-240	; 4
	.word objects_sprites+1-240	; 5
	.word objects_sprites+2-240	; 6
	.word objects_sprites+3-240	; 7
	.word objects_sprites+4-240	; 8
	.word objects_sprites+5-240	; 9
	.word objects_sprites+6-240	; 10
	.word objects_sprites+7-240	; 11

	;
	; Map.
	; Contains the pointer to graphic room data.
	; Also the color for the room, and a flag to
	; indicate if there is a wall to left ($02)
	; or right ($01).
	;
	; The wall (created with the ball) is necessary
	; as the playfield is symmetric, and an opening
	; on the left, would put an opening on the right.
	;
map:
	.word room_pf60	; 0: Hall.
	.byte $7e,$00
	.word room_pf00	; 1: Castle.
	.byte $2e,$00
	.word room_pf10	; 2: Woods.
	.byte $ce,$00
	.word room_pf20	; 3: Shack.
	.byte $3e,$01
	.word room_pf50	; 4: Lake.
	.byte $80,$00
	.word room_pf30	; 5: Dead woods.
	.byte $04,$02
	.word room_pf70	; 6: Cemetery.
	.byte $0e,$00
	.word room_pf80	; 7: Dead woods.
	.byte $04,$02
	.word room_pf40	; 8: Caves.
	.byte $54,$00
	.word room_pf40	; 9: Caves.
	.byte $54,$00

	;
	; Rooms created with Tiny VCS Playfield editor.
	; https://www.masswerk.at/vcs-tools/TinyPlayfieldEditor/
	;
	; You can copy&paste the data for each room
	; to reedit it (set line height as 8px).
	;
; mode: symmetric mirrored line-height 8
room_pf00:
	.byte $F0,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf01:
	.byte $FF,$55,$7F,$7F,$7F,$36,$37,$3E
	.byte $3E,$3E,$37,$37,$3F,$3F,$3F,$3F
	.byte $3F,$3F,$00,$00,$00,$00,$00,$FF
room_pf02:
	.byte $FF,$00,$00,$00,$00,$00,$AA,$00
	.byte $00,$55,$FF,$FF,$FA,$1A,$1F,$1F
	.byte $1F,$1F,$00,$00,$00,$00,$00,$3F

; mode: symmetric mirrored line-height 8
room_pf10:
	.byte $F0,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$00,$00,$00,$00,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf11:
	.byte $FF,$00,$00,$38,$7C,$7C,$38,$10
	.byte $10,$10,$00,$00,$00,$0E,$1F,$1F
	.byte $0E,$04,$04,$00,$00,$00,$00,$FF
room_pf12:
	.byte $3F,$00,$00,$00,$00,$00,$1C,$3E
	.byte $3E,$1C,$08,$08,$08,$00,$00,$00
	.byte $00,$38,$7C,$7C,$38,$10,$00,$FF

; mode: symmetric mirrored line-height 8
room_pf20:
	.byte $F0,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$00,$00,$00,$00,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf21:
	.byte $FF,$00,$00,$00,$00,$00,$00,$20
	.byte $70,$70,$F8,$F8,$70,$20,$20,$00
	.byte $00,$00,$00,$00,$00,$00,$00,$FF
room_pf22:
	.byte $3F,$00,$00,$00,$00,$00,$00,$00
	.byte $F0,$FC,$FE,$AB,$FC,$6C,$7C,$7C
	.byte $00,$00,$00,$00,$00,$00,$00,$FF

; mode: symmetric mirrored line-height 8
room_pf30:
	.byte $F0,$10,$10,$90,$10,$10,$10,$10
	.byte $10,$10,$00,$00,$00,$00,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf31:
	.byte $FF,$00,$00,$20,$A0,$58,$40,$40
	.byte $01,$00,$00,$00,$00,$00,$48,$2B
	.byte $1C,$68,$08,$08,$08,$00,$00,$FF
room_pf32:
	.byte $3F,$00,$00,$00,$00,$00,$00,$04
	.byte $02,$0F,$02,$02,$02,$00,$00,$00
	.byte $00,$28,$10,$1c,$10,$10,$00,$3F

; mode: symmetric mirrored line-height 8
room_pf40:
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $F0,$F0,$00,$00,$00,$00,$F0,$E0
	.byte $20,$20,$20,$20,$20,$20,$20,$20
room_pf41:
	.byte $22,$20,$20,$20,$23,$02,$02,$02
	.byte $FE,$FE,$00,$00,$00,$00,$84,$84
	.byte $04,$04,$04,$3C,$20,$20,$20,$22
room_pf42:
	.byte $C4,$C0,$C0,$C0,$FF,$C0,$C0,$C0
	.byte $C4,$C4,$C0,$C0,$C0,$C0,$FC,$04
	.byte $04,$04,$C4,$C4,$C4,$C4,$C4,$C4

; mode: symmetric mirrored line-height 8
room_pf50:
	.byte $F0,$F0,$F0,$F0,$F0,$F0,$F0,$F0
	.byte $F0,$F0,$F0,$F0,$F0,$D0,$30,$F0
	.byte $F0,$10,$10,$10,$10,$10,$10,$F0
room_pf51:
	.byte $FF,$FF,$F7,$0F,$FF,$FF,$FF,$FF
	.byte $FF,$FD,$C3,$FF,$FF,$FF,$78,$FF
	.byte $FF,$00,$00,$00,$00,$00,$00,$FF
room_pf52:
	.byte $FF,$FF,$FF,$FF,$FF,$DF,$E0,$FF
	.byte $FF,$FF,$DF,$3F,$FF,$FD,$FE,$FF
	.byte $FF,$00,$00,$00,$00,$00,$00,$3F

; mode: symmetric mirrored line-height 8
room_pf60:
	.byte $F0,$90,$90,$90,$90,$90,$90,$90
	.byte $90,$90,$50,$B0,$50,$30,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf61:
	.byte $FF,$24,$24,$24,$2C,$34,$28,$50
	.byte $A1,$40,$80,$00,$00,$00,$01,$00
	.byte $00,$00,$00,$00,$00,$00,$00,$FF
room_pf62:
	.byte $FF,$00,$50,$50,$E0,$C0,$C0,$01
	.byte $C3,$61,$61,$61,$D1,$F1,$63,$E0
	.byte $10,$10,$08,$08,$04,$04,$02,$3F

; mode: symmetric mirrored line-height 8
room_pf70:
	.byte $F0,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf71:
	.byte $FF,$00,$00,$40,$E0,$40,$E0,$E0
	.byte $E0,$00,$00,$08,$1C,$08,$1C,$1C
	.byte $1C,$00,$00,$00,$00,$00,$00,$FF
room_pf72:
	.byte $FF,$00,$00,$00,$04,$0E,$04,$0E
	.byte $0E,$0E,$00,$00,$10,$38,$10,$38
	.byte $38,$38,$00,$00,$00,$00,$00,$3F

; mode: symmetric mirrored line-height 8
room_pf80:
	.byte $F0,$10,$10,$90,$10,$10,$10,$10
	.byte $10,$10,$00,$00,$00,$00,$10,$10
	.byte $10,$10,$10,$10,$10,$10,$10,$F0
room_pf81:
	.byte $FF,$00,$00,$20,$A0,$58,$40,$40
	.byte $01,$00,$00,$00,$00,$00,$48,$2B
	.byte $1C,$68,$08,$08,$08,$00,$00,$FF
room_pf82:
	.byte $3F,$00,$00,$00,$00,$00,$00,$04
	.byte $02,$0F,$02,$02,$02,$00,$00,$00
	.byte $00,$50,$20,$38,$20,$20,$00,$FF

	;
	; List of pointers to player graphics for
	; each frame.
	;
ply_sprite:
	.word player_graphics
	.word player_graphics+16
	.word player_graphics+32
	.word player_graphics+48
	.word player_graphics+64
	.word player_graphics+80
	.word player_graphics+96
	.word player_graphics+112
	.word player_graphics+128

player_graphics:
	.byte %01100000	; Walking down 1
	.byte %01100000
	.byte %00101110
	.byte %00101110
	.byte %00111101
	.byte %00111101
	.byte %10111101
	.byte %01111110
	.byte %00111100
	.byte %01000010
	.byte %01111110
	.byte %01011010
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %00000110	; Walking down 2
	.byte %00000110
	.byte %01110100
	.byte %01110100
	.byte %10111100
	.byte %10111100
	.byte %10111101
	.byte %01111110
	.byte %00111100
	.byte %01000010
	.byte %01111110
	.byte %01011010
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %01100000	; Walking up 1
	.byte %01100000
	.byte %00101110
	.byte %00101110
	.byte %00111101
	.byte %00111101
	.byte %10111101
	.byte %01111110
	.byte %00111100
	.byte %01111110
	.byte %01111110
	.byte %01111110
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %00000110	; Walking up 2
	.byte %00000110
	.byte %01110100
	.byte %01110100
	.byte %10111100
	.byte %10111100
	.byte %10111101
	.byte %01111110
	.byte %00111100
	.byte %01111110
	.byte %01111110
	.byte %01111110
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %00011110	; Walking right 1
	.byte %00011100
	.byte %00011000
	.byte %00011000
	.byte %00111100
	.byte %01011010
	.byte %01011010
	.byte %01011010
	.byte %00111100
	.byte %00111000
	.byte %01111111
	.byte %01111000
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %01110011	; Walking right 2
	.byte %11000111
	.byte %01101101
	.byte %00011000
	.byte %00111100
	.byte %11011101
	.byte %11011110
	.byte %01101100
	.byte %00111100
	.byte %00111000
	.byte %01111111
	.byte %01111000
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %01111000	; Walking left 1
	.byte %00111000
	.byte %00011000
	.byte %00011000
	.byte %00111100
	.byte %01011010
	.byte %01011010
	.byte %01011010
	.byte %00111100
	.byte %00011100
	.byte %11111110
	.byte %00011110
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %11001110	; Walking left 2
	.byte %11100110
	.byte %10110110
	.byte %00011000
	.byte %00111100
	.byte %10111011
	.byte %01111011
	.byte %00110110
	.byte %00111100
	.byte %00011100
	.byte %11111110
	.byte %00011110
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

	.byte %01110110	; Triumph
	.byte %01110110
	.byte %00110100
	.byte %00110100
	.byte %10111101
	.byte %10111101
	.byte %10111101
	.byte %01111110
	.byte %01100110
	.byte %01011010
	.byte %01111110
	.byte %00011000
	.byte %01111110
	.byte %11111111
	.byte %01111110
	.byte %00111100

player_color:
	.byte $24
	.byte $24
	.byte $24
	.byte $24
	.byte $80
	.byte $80
	.byte $80
	.byte $80
	.byte $36
	.byte $36
	.byte $36
	.byte $36
	.byte $02
	.byte $02
	.byte $02
	.byte $02

	;
	; Graphic data for objects.
	;
	; We make the hand to submerge by
	; using one-pixel offsets on the
	; graphic data. 
	;
objects_sprites:
	.byte %00111100	; Hand+Sword
	.byte %00111100
	.byte %01111100
	.byte %01111110
	.byte %01111111
	.byte %01101101
	.byte %01101100
	.byte %01101100
	.byte %00011000
	.byte %00011000
	.byte %01111110
	.byte %00011000
	.byte %00011000
	.byte %00011000
	.byte %00011000
	.byte %00010000

	.byte %00000000	; Sword
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00011000
	.byte %00011000
	.byte %01111110
	.byte %00011000
	.byte %00011000
	.byte %00011000
	.byte %00011000
	.byte %00010000
	.byte %00000000

	.byte %00000000	; Cross
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %01000000
	.byte %11100000
	.byte %01110010
	.byte %00111111
	.byte %00011110
	.byte %00111111
	.byte %01111111
	.byte %00110010
	.byte %00000000

	.byte %00000000	; Key
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %01110000
	.byte %11011000
	.byte %01110000
	.byte %00110100
	.byte %00011101
	.byte %00001110
	.byte %00000100
	.byte %00000000

	.byte %00000000	; Crown
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00111100
	.byte %01000010
	.byte %01111110
	.byte %01000010
	.byte %01111110
	.byte %01011010
	.byte %10011001
	.byte %00100100

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

	.org $FFFC
	.word start
	.word start
