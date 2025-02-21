	;
	; Road demo
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/16/2022.
	;

	.lang 6502
	.include vcs.asm

frame:		EQU $80
road_move:	EQU $81
car_frame:	EQU $82

	.org 0xf000
start:
	sei
	cld
	ldx #$FF
	txs
	lda #$00
clear:
	sta 0,X
	dex
	bne clear

	lda #$00	; Allow to read console switches
	sta @swacnt
	sta @swbcnt

show_frame:
	lda #$98
	sta @colubk

	sta @wsync
	lda #2
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #42		; Time for NTSC top border
	sta @tim64t
	lda #0
	sta @vsync

	lda #50
	ldx #1		; Player 1
	jsr x_position

	lda #88
	ldx #2		; Missile 0 (left side of road)
	jsr x_position

	lda #147
	ldx #4		; Ball (right side of road)
	jsr x_position

	lda #$30	; Missiles 8px width
	sta @nusiz0
	sta @ctrlpf
	lda #$07	; Player 1 width x4
	sta @nusiz1

wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

	sta @wsync	
	sta @hmove	
	lda #0		; Disable blanking
	sta @vblank

	ldx #96
SKY1:
	sta @wsync
	dex
	bne SKY1

	sta HMCLR
	lda #$C4
	sta @colubk
	lda #$00
	sta @colup1
	lda #$02
	sta @enam0
	sta @enabl

	ldx #(car_sprite-240)&255
	lda frame
	and #4
	BEQ GRASS0
	ldx #(car_sprite+16-240)&255
GRASS0:
	stx car_frame
	ldx #(car_sprite-240)>>8
	stx car_frame+1
	ldx #0
	ldy #$48
GRASS1:
	sta @wsync
	sta @hmove
	lda road_color,X
	EOR road_move
	sta @colup0
	sta @colupf
	lda #0
	dey
	cpy #$F0
	BCC GRASS2
	lda (car_frame),Y
GRASS2:	sta @grp1
	lda road_adjust0,X
	sta HMM0
	lda road_adjust1,X
	sta HMBL

	INX
	CPX #96
	bne GRASS1

	sta @wsync
	lda #2
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

	lda #0
	sta @enam0
	sta @enam1
	sta @enabl
	sta @grp1

	lda #12		; Volume.
	sta AUDV0
	lda #6		; Noise.
	sta AUDC0
	lda #$18	
	sta AUDF0

	inc frame
	lda frame
	and #$07
	bne M1
	lda road_move
	EOR #$4E
	sta road_move
M1:

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
	
	org $fdf1	; Table at last page of ROM
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

road_adjust0:
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $20,$20,$20,$20,$20,$20,$20,$20
	.byte $10,$10,$10,$10,$10,$10,$10,$10
	.byte $10,$00,$10,$00,$10,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00
	.byte $10,$00,$00,$10,$00,$00,$10,$00

road_adjust1:
	.byte $30,$30,$30,$30,$30,$30,$30,$30
	.byte $20,$20,$20,$20,$20,$20,$20,$20
	.byte $10,$00,$10,$00,$10,$00,$10,$00
	.byte $F0,$00,$F0,$00,$F0,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00
	.byte $F0,$00,$00,$F0,$00,$00,$F0,$00

road_color:
	.byte $0a,$44,$0a,$44,$0a,$44,$0a,$44
	.byte $0a,$0a,$44,$44,$0a,$0a,$44,$44
	.byte $0a,$0a,$44,$44,$0a,$0a,$44,$44
	.byte $0a,$0a,$44,$44,$0a,$0a,$44,$44
	.byte $0a,$0a,$44,$44,$0a,$0a,$44,$44
	.byte $0a,$0a,$0a,$44,$44,$44,$0a,$0a
	.byte $44,$44,$44,$0a,$0a,$0a,$44,$44
	.byte $0a,$0a,$0a,$44,$44,$44,$0a,$0a
	.byte $44,$44,$44,$0a,$0a,$0a,$44,$44
	.byte $0a,$0a,$0a,$0a,$44,$44,$44,$44
	.byte $0a,$0a,$0a,$0a,$44,$44,$44,$44
	.byte $0a,$0a,$0a,$0a,$44,$44,$44,$44

car_sprite:
	.byte %10000010
	.byte %01011001
	.byte %10111110
	.byte %01111101
	.byte %10111110
	.byte %01011001
	.byte %10100110
	.byte %01111101
	.byte %00011000
	.byte %00000000
	.byte %00011000
	.byte %00011000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000

	.byte %01000001
	.byte %10011010
	.byte %01111101
	.byte %10111110
	.byte %01111101
	.byte %10011010
	.byte %01100101
	.byte %10111110
	.byte %00011000
	.byte %00000000
	.byte %00011000
	.byte %00011000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000

	.org $FFFC
	.word start
	.word start
