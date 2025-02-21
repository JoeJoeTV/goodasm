	;
	; 48 pixel kernel demo
	;
	; by Óscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/16/2022.
	;

	.lang 6502
	.include vcs.asm

digits:	EQU $80		; 12 bytes for each digit address.
score:	EQU $8C		; Current score in BCD (3 bytes).
temp1:	EQU $91
temp2:	EQU $92

scorecolor	= $38

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
	lda #$88
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

	; Convert each digit of the score to the...
	; ...addresses of each digit graphics.
	ldx #digits
	lda score
	jsr two_digits
	lda score+1
	jsr two_digits
	lda score+2
	jsr two_digits


wait_for_top:
	lda @intim	; Read timer
	bne wait_for_top	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

	sta @wsync
	lda #0		; Disable blanking
	sta @vblank

	ldx #183
visible:
	sta @wsync
	dex
	bne visible

        lda #$01            
        ldx #0              
        sta @wsync           ; 3
        sta @hmove           ; 6
        sta @ctrlpf          ; 9
        stx @enabl           ; 12
        stx @grp0            ; 15
        stx @grp1            ; 18
        stx @enam0           ; 21
        stx @enam1           ; 24
        stx @colubk          ; 27
        lda #scorecolor    ; 29
        sta @colup0          ; 32
        sta @colup1          ; 35
        lda #$03            ; 37 Three copies together
        ldx #$f0            ; 39
        stx @resp0           ; 42
        stx @resp1           ; 45
        stx @hmp0            ; 48
        sta @nusiz0          ; 51
        sta @nusiz1          ; 54
        @lsr                 ; 56
        sta @vdelp0          ; 59
        sta @vdelp1          ; 61
        sta @wsync           ; 3
        sta @hmove           ; 6
        lda #6              ; 8
        sta temp2           ; 11
mp1:    ldy temp2           ; 14/67
        lda (digits),y      ; 19/72
        sta @grp0            ; 22/75
        sta @wsync           ; 3
        lda (digits+2),y    ; 8
        sta @grp1            ; 11
        lda (digits+4),y    ; 16
        sta @grp0            ; 19
        lda (digits+6),y    ; 24
        sta temp1           ; 27
        lda (digits+8),y    ; 32
        tax                 ; 34
        lda (digits+10),y   ; 39
        tay                 ; 41
        lda temp1           ; 44
        sta @grp1            ; 47
        stx @grp0            ; 50
        sty @grp1            ; 53
        sta @grp0            ; 56
        dec temp2           ; 61
        bpl mp1             ; 63
mp3:
        ; Detect code isn't going across pages
        IF (mp1&$ff00)!=(mp3&$ff00)
		ECHO "Error: Page crossing"
		ERR
        ENDIF

	sta @wsync
	lda #2
	sta @vblank

	lda #35		; Time for NTSC bottom border
	sta @tim64t

        ldy #0              
        sty @vdelp0          
        sty @vdelp1          
	sty @grp0
	sty @grp1

	sed		; Set decimal mode.
	lda score+2	; Get lower two digits.
	clc		; Clear carry.
	adc #1		; Add one.
	sta score+2	; Store lower two digits.
	lda score+1	; Get middle two digits.
	adc #0		; Add carry.
	sta score+1	; Store middle two digits.
	lda score	; Get upper two digits.
	adc #0		; Add carry.
	sta score	; Store upper two digits.
	cld		; Clear decimal mode.

wait_for_bottom:
	lda @intim	; Read timer
	bne wait_for_bottom	; Branch if not zero.
	sta @wsync	; Resynchronize on last border row

	jmp show_frame

	; Separate two BCD digits
two_digits:
	pha
	@lsr		; Shift right 4 bits.
	@lsr
	@lsr
	@lsr
	jsr one_digit	; Process upper digit.
	PLA
one_digit:
	and #$0F	; Process lower digit.
	ASL		; x2
	ASL		; x4
	ASL		; x8
	sta 0,X		; Store lower byte of address.
	INX		; Advance one byte.
	lda #numbers_bitmaps>>8	; High byte of address.
	sta 0,X		; Store byte.
	INX		; Advance one byte.
	RTS

	.org $fe00

numbers_bitmaps:
	.byte $fe,$86,$86,$86,$82,$82,$fe,$00	; 0
	.byte $30,$30,$30,$30,$10,$10,$10,$00	; 1
	.byte $fe,$c0,$c0,$fe,$02,$02,$fe,$00	; 2
	.byte $fe,$06,$06,$fe,$02,$02,$fe,$00	; 3
	.byte $06,$06,$06,$fe,$82,$82,$82,$00	; 4
	.byte $fe,$06,$06,$fe,$80,$80,$fe,$00	; 5
	.byte $fe,$c6,$c6,$fe,$80,$80,$fe,$00	; 6
	.byte $06,$06,$06,$02,$02,$02,$fe,$00	; 7
	.byte $fe,$c6,$c6,$fe,$82,$82,$fe,$00	; 8
	.byte $fe,$06,$06,$fe,$82,$82,$fe,$00	; 9

	.org $FFFC
	.word start
	.word start
