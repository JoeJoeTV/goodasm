;;; GameBoy Hello World example for GoodASM's SM83 Target
;;; Based on https://github.com/NotImplementedLife/Gameboy-ASM-Examples/

;;; Cartridge entry point is 0x100 bytes in, for boot ROM reasons.
	.lang sm83

	.include "gameboy.asm"	; I/O register definitions and flags.

;;; Fill the first 256 bytes.
	.org 0
	.db 0xff
	.org 256

entrypoint:
	di			; Disable Interrupts
	jp start		; Jump to entry point.

;;; Nintendo Logo
	.org 0x104
	.db 0xCE 0xED 0x66 0x66 0xCC 0x0D 0x00 0x0B 0x03 0x73 0x00 0x83 0x00 0x0C 0x00 0x0D
	.db 0x00 0x08 0x11 0x1F 0x88 0x89 0x00 0x0E 0xDC 0xCC 0x6E 0xE6 0xDD 0xDD 0xD9 0x99
	.db 0xBB 0xBB 0x67 0x63 0x6E 0x0E 0xEC 0xCC 0xDD 0xDC 0x99 0x9F 0xBB 0xB9 0x33 0x3E

;;; Title
	.org 0x134
	;; 16-character title, uppercase ASCII with null termination and padding.
cartname:	
	.db "HELLO WORLD", 0	; Title.
	
	.org 0x144
	;; Licensee code.
	.db 0x00		; Unlicensed.
	.org 0x147
	.db 0x00		; ROM Only
	.db 0x00		; No cartridge RAM.
	.db 0x00		; Japan (and possibly overseas)
	.db 0x00		; Old Licensee Code of None
	.db 0x00		; Mask Rom Revision Number
	.db 0xFF		; FIXME: This should be the header checksum at 0x14D.
	;; Header checksum comes next, but it's not verified.

	.org 0x150
start:	
	;;  Turn off the LCD.
waitvblank:
	ld a, @rLY
	cp #144			; Check if LCD is past VBlank
	jr c, waitvblank

	xor a 			; Clears A.
	ld @rLCDC, a

	;; Copy font to VRAM
	ld hl, #0x9000
	ld de, #FontTiles
	ld bc, #2048		; 2048 bytes in the file.
copyFont:
	ld a, @de		; Grab one byte from the source.
	ld (hl+), a		; Place it at destination, incrementing HL.
	inc de			; Move to next byte.
	dec bc			; Decrement length.
	ld a, b			; Check if count is 0, since dec bc doesn't update flags.
	or c
	jr nz, copyFont

	ld hl, #0x9800		; Print the string at the 0, 0 position of the screen.
	ld de, #helloWorldStr	; Load the string pointer to de.
copyString:
	ld a, @de
	ld (hl+), a
	inc de
	and a			; Check if byte we copied is zero.
	jr nz, copyString	; Continue if it's not.

	;; Init display registers.
	ld a, #0b11100100
	ld @rBGP, a
	xor a			; Clear A.
	ld @rSCX, a
	ld @rSCY, a

	;;  Shut down sound.
	ld @rNR52, a

	;;  Turn screen on, display background.
	ld a, #0b10000001
	ld @rLCDC, a

	;; Lock up.
lockup:
	jr lockup		; while(1);
	


FontTiles:
	.incbin "font.chr"
	
FontTilesEnd:
	

helloWorldStr:
	.db "Hello World.", 0x00
	.org 4096
	

	
