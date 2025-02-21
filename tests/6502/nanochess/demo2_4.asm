	;
	; Players @nusiz demo with missiles
	;
	; by Oscar Toledo G.
	; https://nanochess.org/
	;
	; Creation date: Jun/02/2022.
	;

	.lang 6502
	.include vcs.asm

;;; Variable addresses in register memory.
	.equ frame 0x80		; Frame number saved in this address.
	.equ seconds 0x81	; Seconds value saved in this address.
	.equ vpos 0x82		; Vertical position of missile.
	
	

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

show_frame:
	lda #0x88	; Blue.
	sta @colubk	; Background color.
	lda #0x0F	; White.
	sta @colup0	; Player 0 color.
	lda #0x00	; Black.
	sta @colup1	; Player 1 color.
	lda #0x03	; 3 copies shown of player.
	sta @nusiz0
	sta @nusiz1

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0		; End of vertical retrace.
	sta @vsync

	; Player 0 and 1 horizontal position
	sta @wsync	; Cycle 3
	nop		; 5
	nop		; 7
	nop		; 9
	nop		; 11
	nop		; 13
	nop		; 15
	nop		; 17
	nop		; 19
	nop		; 21
	nop		; 23
	nop		; 25
	sta @resp0	; 28
	nop		; 30
	nop		; 32
	nop		; 34
	nop		; 36
	nop		; 38
	nop		; 40
	nop		; 42
	nop		; 44
	nop		; 46
	sta @resp1	; 49

	sta @wsync
	lda #2		; Reset missile to player position
	sta @resmp0	; Reset missile 0	
	sta @resmp1	; Reset missile 1

	sta @wsync
	lda #0		; Allow missile to be displayed
	sta @resmp0	; 
	sta @resmp1	; 

	ldx #33		; Remains 33 scanlines of top border
TOP:
	sta @wsync
	dex
	bne TOP
	lda #0		; Disable blanking
	sta @vblank

	ldx #92		; 92 scanlines in blue
visible:
	sta @wsync
	dex
	bne visible

	sta @wsync	; One scanline
	lda #0x42	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x24	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x3C	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x5A	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xFF	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xBD	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0xA5	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x24	; 
	sta @grp0
	sta @grp1

	sta @wsync	; One scanline
	lda #0x00	; 
	sta @grp0
	sta @grp1

	ldx #91		; 91 scanlines in deep blue
	ldy @vpos	; Load @vpos into Y register.
visible2:
	sta @wsync
	lda #0		; A = $00 Disable missiles
	cpy #0		; Y is zero?
	bne L2		; Branch if Not Equal.
	lda #2		; A = $02 Enable missiles
L2:	sta @enam0	; Enable or disable missile 0
	sta @enam1	; Enable or disable missile 1
	dey		; Decrease Y (temporary copy of @vpos)
	dex
	bne visible2

	lda #2		; Enable blanking
	sta @vblank
	ldx #30		; 30 scanlines of bottom border
bottom:
	sta @wsync
	lda #0		; Disable missile
	sta @enam0
	sta @enam1

	dex
	bne bottom

	inc @vpos	; Increase vertical position
			; Doesn't mind limiting it as
			; it will cycle 0-255 and repeat.

	inc @frame	; Increase frame number
	lda @frame	; Read frame number
	cmp #60		; Is it 60?
	bne L1		; Branch if not equal.
	lda #0		; Reset frame number to zero.
	sta @frame
	inc @seconds	; Increase number of seconds.
L1:
	jmp show_frame

	.org 0xFFFC
	.word start		; Entry point.
	.word start		; Entry point.
	
