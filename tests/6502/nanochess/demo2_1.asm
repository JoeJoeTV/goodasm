;;; Ball Demo
;;; by oscar Toledo G
;;; https://nanochess.org/

;;; Translated from the original to GoodASM format.

;;; Set processor and base address.
	.lang 6502
	.org 0xf000


;;; Symbols should come from another source, but this will do.
	.equ vsync  0x00	; Vertical Sync Set-Clear
	.equ vblank 0x01	; Vertical Blank Set-Clear
	.equ wsync  0x02	; Wait for Horizontal Blank
	.equ rsync  0x03	; Reset Horizontal Sync Counter
	.equ nusiz0 0x04	; Number-Size player/missile 0
	.equ nusiz1 0x05	; Number-Size player/missile 1
	.equ colup0 0x06	; Color-Luminance Player 0
	.equ colup1 0x07	; Color-Luminance Player 1
	.equ colupf 0x08	; Color-Luminance Playfield
	.equ colubk 0x09	; Color-Luminance Background
	.equ ctrlpf 0x0a	; Control Platfield, Ball, Collisions
	.equ refp0  0x0b	; Reflection Player 0
	.equ refp1  0x0c	; Reflection Player 1
	.equ pf0    0x0d	; Playfield Register Byte 0
	.equ pf1    0x0e	; Playfield Register Byte 1
	.equ pf2    0x0f	; Playfield Register Byte 2
	.equ resp0  0x10	; Reset Player 0
	.equ resp1  0x11	; Reset Player 1
	.equ resm0  0x12	; Reset Missile 0
	.equ resm1  0x13	; Reset Missile 1
	.equ resbl  0x14	; Reset Ball
	.equ audc0  0x15	; Audio Control 0
	.equ audc1  0x16	; Audio Control 1
	.equ audf0  0x17	; Audio Frequency 0
	.equ audf1  0x18	; Audio Frequency 1
	.equ audv0  0x19	; Audio Volume 0
	.equ audv1  0x1a	; Audio Volume 1
	.equ grp0   0x1b	; Graphics Register Player 0
	.equ grp1   0x1c	; Graphics Register Player 1
	.equ enam0  0x1d	; Graphics Enable Missile 0
	.equ enam1  0x1e	; Graphics Enable Missile 1
	.equ enabl  0x1f	; Graphics Enable Ball
	.equ hmp0   0x20	; Horizontal Motion Player 0
	.equ hmp1   0x21	; Horizontal Motion Player 1
	.equ hmm0   0x22	; Horizontal Motion Missile 0
	.equ hmm1   0x23	; Horizontal Motion Missile 0
	.equ hmbl   0x24	; Horizontal Motion Ball
	.equ vdelp0 0x25	; Vertical Delay Player 0
	.equ vdelp1 0x26	; Vertical Delay Player 1
	.equ vdelbl 0x27	; Vertical Delay Ball
	.equ resmp0 0x28	; Reset Millile 0 to Player 0
	.equ resmp1 0x29	; Reset Millile 1 to Player 1
	.equ hmove  0x2a	; Apply Horizontal Motion
	.equ hmclr  0x2b	; Clear Horizontal Move Registers
	.equ cxclr  0x2c	; Clear Collision Latches
	
	
	

	
	
	
	

	
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
	sta @colupf	; Playfield color.

	sta @wsync
	lda #2		; Start of vertical retrace.
	sta @vsync
	sta @wsync
	sta @wsync
	sta @wsync
	lda #0		; End of vertical retrace.
	sta @vsync

	; Ball horizontal position (23 nops for center)
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
	nop		; 27
	nop		; 29
	nop		; 31
	nop		; 33
	nop		; 35
	nop		; 37
	nop		; 39
	nop		; 41
	nop		; 43
	nop		; 45
	nop		; 47
	nop		; 49
	sta @resbl	; 52

	ldx #35		; Remains 35 scanlines of top border
top:
	sta @wsync
	dex
	bne top
	lda #0		; Disable blanking
	sta @vblank

	ldx #95		; 95 scanlines in blue
visible:
	sta @wsync
	dex
	bne visible

	sta @wsync	; One scanline
	lda #0x02	; Ball enable
	sta @enabl

	sta @wsync	; One scanline
	lda #0x00
	sta @enabl

	lda #0xF8	; Sand color
	sta @colubk

	ldx #95		; 95 scanlines
visible2:
	sta @wsync
	dex
	bne visible2

	lda #2		; Enable blanking
	sta @vblank
	ldx #30		; 30 scanlines of bottom border
bottom:
	sta @wsync
	dex
	bne bottom

	jmp show_frame

	.org 0xFFFC
	.word start
	.word start
