	;
	; Rainbow sky demo
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/16/2022.
	;

	.lang 6502
	.include vcs.asm

@frame:	EQU $80
TEMP0:	EQU $81

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

	lda #$00	; Allow to read console switches.
	sta @swacnt
	sta @swbcnt

show_frame:
	lda #$88
	sta @colubk

	sta @wsync
	lda #2
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #42		; Time for NTSC top border.
	sta @tim64t
	lda #0
	sta @vsync

	lda #0		; Clear playfield.
	sta @pf0
	sta @pf1
	sta @pf2

wait_for_top:
	lda @intim	; Read timer.
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row.

	sta @wsync
	lda #0		; Disable blanking.
	sta @vblank

	lda @frame	; Current frame counter.
	@lsr		; Divide by 2.
	@lsr		; Divide by 4.
	@lsr		; Divide by 8.
	sta TEMP0	; Use as base offset for sky color.

	ldx #12		; 12 rows.
SKY1:
	sta @wsync
	lda TEMP0	; Load offset for sky color.
	and #$1F	; Modulo 32.
	tay		; Put in register Y.
	lda color_bgnd,Y	; Read sky color.
	sta @colubk	; Setup as background color.
	inc TEMP0	; Advance sky color offset.
	dex
	bne SKY1
	
	lda @frame	; Current frame counter.
	@lsr		; Divide by 2.
	@lsr		; Divide by 4.
	@lsr		; Divide by 8.
	@lsr		; Divide by 16.
	and #$0F	; Modulo 16.
	tax		; Put in register X.
	lda mountains_color,X	; Read mountains color.
	sta @colupf

	ldx #0		; Offset into playfield graphics.
MOUNTAIN1:
	sta @wsync
	lda mountains_pf,X	; Read mountain byte.
	sta @pf0		; Setup PF0.
	INX
	lda mountains_pf,X	; Read mountain byte.
	sta @pf1		; Setup PF1.
	INX
	lda mountains_pf,X	; Read mountain byte.
	sta @pf2		; Setup PF2.
	lda TEMP0	; Load offset for sky color.
	and #$1F	; Modulo 32.
	tay		; Put in register Y.
	lda color_bgnd,Y	; Read sky color.
	sta @colubk	; Setup as background color.
	inc TEMP0	; Advance sky color offset.
	INX		; Increase playfield offset.
	CPX #48		; Reached 16*3 bytes?
	bne MOUNTAIN1	; No, branch back.

	ldx #164
EMPTY1:
	sta @wsync
	dex
	bne EMPTY1

	sta @wsync
	lda #2
	sta @vblank

	lda #35		; Time for NTSC bottom border.
	sta @tim64t

	inc @frame

wait_for_bottom:
	lda @intim	; Read timer.
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row.

	jmp show_frame

	; Colors for background.
color_bgnd:
	.byte $68,$66,$58,$56,$48,$46,$38,$36,$28,$26,$f8,$f6
	.byte $b8,$b6,$ae,$ac,$aa,$a8,$a6,$9e,$9c,$9a,$98,$96
	.byte $94,$8c,$8a,$88,$86,$84,$78,$76

	; Colors for mountains.
mountains_color:
	.byte $c8,$c6,$c4,$c2
	.byte $c0,$c0,$c0,$c0
	.byte $c2,$c4,$c6,$c8
	.byte $ca,$cc,$ce,$ce

mountains_pf:
; mode: symmetric repeat line-height 1
	.byte $80,$00,$00 ;|   X                | (  0)
	.byte $C0,$80,$00 ;|  XXX               | (  1)
	.byte $E0,$C0,$00 ;| XXXXX              | (  2)
	.byte $F0,$E0,$00 ;|XXXXXXX             | (  3)
	.byte $F0,$F0,$80 ;|XXXXXXXX           X| (  4)
	.byte $F0,$F8,$C1 ;|XXXXXXXXX   X     XX| (  5)
	.byte $F0,$FD,$E3 ;|XXXXXXXXXX XXX   XXX| (  6)
	.byte $F0,$FF,$F7 ;|XXXXXXXXXXXXXXX XXXX| (  7)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| (  8)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| (  9)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 10)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 11)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 12)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 13)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 14)
	.byte $F0,$FF,$FF ;|XXXXXXXXXXXXXXXXXXXX| ( 15)

	.org $FFFC
	.word start
	.word start
