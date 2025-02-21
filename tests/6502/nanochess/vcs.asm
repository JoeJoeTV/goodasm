;;; These are symbol definitions for the Atari VCS,
;;; borrowed from our friendly competitors on the DASM Team.
	

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


;;; Manually adding these for GoodASM. --Travis
	.equ intim  0x0284	; INTIM    - PIA Current Timer Value
	.equ instat 0x285	; INSTAT   - PIA Timer Status
	.equ tim1t  0x294	; TIM1T    - PIA Set Timer 838ns/i
	.equ tim8t  0x295	; TIM8T    - PIA Set Timer 6.7us/i
	.equ tim64t 0x296	; TIM64T   - PIA Set Timer 53.6us/i
	.equ t1024t 0x297	; TIM1024T - PIA Set Time 858.2us/i
