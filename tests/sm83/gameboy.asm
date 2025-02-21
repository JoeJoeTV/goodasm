;*
;* Gameboy Hardware definitions
;*
;* Based on Jones' hardware.inc
;* And based on Carsten Sorensen's ideas.
;*
;* Rev 1.1 - 15-Jul-97 : Added define check
;* Rev 1.2 - 18-Jul-97 : Added revision check macro
;* Rev 1.3 - 19-Jul-97 : Modified for RGBASM V1.05
;* Rev 1.4 - 27-Jul-97 : Modified for new subroutine prefixes
;* Rev 1.5 - 15-Aug-97 : Added _HRAM, PAD, CART defines
;*                     :  and Nintendo Logo
;* Rev 1.6 - 30-Nov-97 : Added rDIV, rTIMA, rTMA, & rTAC
;* Rev 1.7 - 31-Jan-98 : Added _SCRN0, _SCRN1
;* Rev 1.8 - 15-Feb-98 : Added rSB, rSC
;* Rev 1.9 - 16-Feb-98 : Converted I/O registers to 0xFFXX format
;* Rev 2.0 -           : Added GBC registers
;* Rev 2.1 -           : Added MBC5 & cart RAM enable/disable defines
;* Rev 2.2 -           : Fixed NR42,NR43, & NR44 equates
;* Rev 2.3 -           : Fixed incorrect _HRAM equate
;* Rev 2.4 - 27-Apr-13 : Added some cart defines (AntonioND)
;* Rev 2.5 - 03-May-15 : Fixed format (AntonioND)
;* Rev 2.6 - 09-Apr-16 : Added GBC OAM and cart defines (AntonioND)
;* Rev 2.7 - 19-Jan-19 : Added rPCMXX (ISSOtm)
;* Rev 2.8 - 03-Feb-19 : Added audio registers flags (Álvaro Cuesta)
;* Rev 2.9 - 28-Feb-20 : Added utility rP1 constants
;* Rev 3.0 - 27-Aug-20 : Register ordering, byte-based sizes, OAM additions, general cleanup (Blitter Object)
;* Rev 4.0 - 03-May-21 : Updated to use RGBDS 0.5.0 syntax, changed IEF_LCDC to IEF_STAT (Eievui)
	;; Ported to  GoodASM on 2024-11-27
	



.equ _VRAM        0x8000 ; 0x8000->0x9FFF
.equ _VRAM8000    0x8000
.equ _VRAM8800    0x8800
.equ _VRAM9000    0x9000
.equ _SCRN0       0x9800 ; 0x9800->0x9BFF
.equ _SCRN1       0x9C00 ; 0x9C00->0x9FFF
.equ _SRAM        0xA000 ; 0xA000->0xBFFF
.equ _RAM         0xC000 ; 0xC000->0xCFFF / 0xC000->0xDFFF
.equ _RAMBANK     0xD000 ; 0xD000->0xDFFF
.equ _OAMRAM      0xFE00 ; 0xFE00->0xFE9F
.equ _IO          0xFF00 ; 0xFF00->0xFF7F,0xFFFF
.equ _AUD3WAVERAM 0xFF30 ; 0xFF30->0xFF3F
.equ _HRAM        0xFF80 ; 0xFF80->0xFFFE

; *** MBC5 Equates ***

.equ rRAMG        0x0000 ; 0x0000->0x1fff
.equ rROMB0       0x2000 ; 0x2000->0x2fff
.equ rROMB1       0x3000 ; 0x3000->0x3fff - If more than 256 ROM banks are present.
.equ rRAMB        0x4000 ; 0x4000->0x5fff - Bit 3 enables rumble (if present)


;***************************************************************************
;*
;* Custom registers
;*
;***************************************************************************

; --
; -- P1 (0xFF00)
; -- Register for reading joy pad info. (R/W)
; --
.equ rP1 0xFF00

.equ P1F_5 0b00100000 ; P15 out port, set to 0 to get buttons
.equ P1F_4 0b00010000 ; P14 out port, set to 0 to get dpad
.equ P1F_3 0b00001000 ; P13 in port
.equ P1F_2 0b00000100 ; P12 in port
.equ P1F_1 0b00000010 ; P11 in port
.equ P1F_0 0b00000001 ; P10 in port

.equ P1F_GET_DPAD 0b00100000
.equ P1F_GET_BTN  0b00010000
.equ P1F_GET_NONE 0b00110000


; --
; -- SB (0xFF01)
; -- Serial Transfer Data (R/W)
; --
.equ rSB 0xFF01


; --
; -- SC (0xFF02)
; -- Serial I/O Control (R/W)
; --
.equ rSC 0xFF02


; --
; -- DIV (0xFF04)
; -- Divider register (R/W)
; --
.equ rDIV 0xFF04


; --
; -- TIMA (0xFF05)
; -- Timer counter (R/W)
; --
.equ rTIMA 0xFF05


; --
; -- TMA (0xFF06)
; -- Timer modulo (R/W)
; --
.equ rTMA 0xFF06


; --
; -- TAC (0xFF07)
; -- Timer control (R/W)
; --
.equ rTAC 0xFF07

.equ TACF_START  0b00000100
.equ TACF_STOP   0b00000000
.equ TACF_4KHZ   0b00000000
.equ TACF_16KHZ  0b00000011
.equ TACF_65KHZ  0b00000010
.equ TACF_262KHZ 0b00000001


; --
; -- IF (0xFF0F)
; -- Interrupt Flag (R/W)
; --
.equ rIF 0xFF0F


; --
; -- AUD1SWEEP/NR10 (0xFF10)
; -- Sweep register (R/W)
; --
; -- Bit 6-4 - Sweep Time
; -- Bit 3   - Sweep Increase/Decrease
; --           0: Addition    (frequency increases???)
; --           1: Subtraction (frequency increases???)
; -- Bit 2-0 - Number of sweep shift (# 0-7)
; -- Sweep Time: (n*7.8ms)
; --
.equ rNR10 0xFF10
.equ rAUD1SWEEP 0xFF10

.equ AUD1SWEEP_UP   0b00000000
.equ AUD1SWEEP_DOWN 0b00001000


; --
; -- AUD1LEN/NR11 (0xFF11)
; -- Sound length/Wave pattern duty (R/W)
; --
; -- Bit 7-6 - Wave Pattern Duty (00:12.50b 01:250b 10:500b 11:750b)
; -- Bit 5-0 - Sound length data (# 0-63)
; --
.equ rNR11 0xFF11
.equ rAUD1LEN 0xFF11


; --
; -- AUD1ENV/NR12 (0xFF12)
; -- Envelope (R/W)
; --
; -- Bit 7-4 - Initial value of envelope
; -- Bit 3   - Envelope UP/DOWN
; --           0: Decrease
; --           1: Range of increase
; -- Bit 2-0 - Number of envelope sweep (# 0-7)
; --
.equ rNR12 0xFF12
.equ rAUD1ENV 0xFF12


; --
; -- AUD1LOW/NR13 (0xFF13)
; -- Frequency low byte (W)
; --
.equ rNR13 0xFF13
.equ rAUD1LOW 0xFF13


; --
; -- AUD1HIGH/NR14 (0xFF14)
; -- Frequency high byte (W)
; --
; -- Bit 7   - Initial (when set, sound restarts)
; -- Bit 6   - Counter/consecutive selection
; -- Bit 2-0 - Frequency's higher 3 bits
; --
.equ rNR14 0xFF14
.equ rAUD1HIGH 0xFF14


; --
; -- AUD2LEN/NR21 (0xFF16)
; -- Sound Length; Wave Pattern Duty (R/W)
; --
; -- see AUD1LEN for info
; --
.equ rNR21 0xFF16
.equ rAUD2LEN 0xFF16


; --
; -- AUD2ENV/NR22 (0xFF17)
; -- Envelope (R/W)
; --
; -- see AUD1ENV for info
; --
.equ rNR22 0xFF17
.equ rAUD2ENV 0xff17


; --
; -- AUD2LOW/NR23 (0xFF18)
; -- Frequency low byte (W)
; --
.equ rNR23 0xFF18
.equ rAUD2LOW 0xff18


; --
; -- AUD2HIGH/NR24 (0xFF19)
; -- Frequency high byte (W)
; --
; -- see AUD1HIGH for info
; --
.equ rNR24 0xFF19
.equ rAUD2HIGH 0xff19


; --
; -- AUD3ENA/NR30 (0xFF1A)
; -- Sound on/off (R/W)
; --
; -- Bit 7   - Sound ON/OFF (1=ON,0=OFF)
; --
.equ rNR30 0xFF1A
.equ rAUD3ENA 0xff1a


; --
; -- AUD3LEN/NR31 (0xFF1B)
; -- Sound length (R/W)
; --
; -- Bit 7-0 - Sound length
; --
.equ rNR31 0xFF1B
.equ rAUD3LEN 0xff1b


; --
; -- AUD3LEVEL/NR32 (0xFF1C)
; -- Select output level
; --
; -- Bit 6-5 - Select output level
; --           00: 0/1 (mute)
; --           01: 1/1
; --           10: 1/2
; --           11: 1/4
; --
.equ rNR32 0xFF1C
.equ rAUD3LEVEL 0xff1c


; --
; -- AUD3LOW/NR33 (0xFF1D)
; -- Frequency low byte (W)
; --
; -- see AUD1LOW for info
; --
.equ rNR33 0xFF1D
.equ rAUD3LOW 0xff1d


; --
; -- AUD3HIGH/NR34 (0xFF1E)
; -- Frequency high byte (W)
; --
; -- see AUD1HIGH for info
; --
.equ rNR34 0xFF1E
.equ rAUD3HIGH 0xff1e


; --
; -- AUD4LEN/NR41 (0xFF20)
; -- Sound length (R/W)
; --
; -- Bit 5-0 - Sound length data (# 0-63)
; --
.equ rNR41 0xFF20
.equ rAUD4LEN 0xff20


; --
; -- AUD4ENV/NR42 (0xFF21)
; -- Envelope (R/W)
; --
; -- see AUD1ENV for info
; --
.equ rNR42 0xFF21
.equ rAUD4ENV 0xff21


; --
; -- AUD4POLY/NR43 (0xFF22)
; -- Polynomial counter (R/W)
; --
; -- Bit 7-4 - Selection of the shift clock frequency of the (scf)
; --           polynomial counter (0000-1101)
; --           freq=drf*1/2^scf (not sure)
; -- Bit 3 -   Selection of the polynomial counter's step
; --           0: 15 steps
; --           1: 7 steps
; -- Bit 2-0 - Selection of the dividing ratio of frequencies (drf)
; --           000: f/4   001: f/8   010: f/16  011: f/24
; --           100: f/32  101: f/40  110: f/48  111: f/56  (f=4.194304 Mhz)
; --
.equ rNR43 0xFF22
.equ rAUD4POLY 0xff22


; --
; -- AUD4GO/NR44 (0xFF23)
; --
; -- Bit 7 -   Inital
; -- Bit 6 -   Counter/consecutive selection
; --
.equ rNR44 0xFF23
.equ rAUD4GO 0xff23


; --
; -- AUDVOL/NR50 (0xFF24)
; -- Channel control / ON-OFF / Volume (R/W)
; --
; -- Bit 7   - Vin->SO2 ON/OFF (Vin??)
; -- Bit 6-4 - SO2 output level (volume) (# 0-7)
; -- Bit 3   - Vin->SO1 ON/OFF (Vin??)
; -- Bit 2-0 - SO1 output level (volume) (# 0-7)
; --
.equ rNR50 0xFF24
.equ rAUDVOL 0xff24

.equ AUDVOL_VIN_LEFT  0b10000000 ; SO2
.equ AUDVOL_VIN_RIGHT 0b00001000 ; SO1


; --
; -- AUDTERM/NR51 (0xFF25)
; -- Selection of Sound output terminal (R/W)
; --
; -- Bit 7   - Output sound 4 to SO2 terminal
; -- Bit 6   - Output sound 3 to SO2 terminal
; -- Bit 5   - Output sound 2 to SO2 terminal
; -- Bit 4   - Output sound 1 to SO2 terminal
; -- Bit 3   - Output sound 4 to SO1 terminal
; -- Bit 2   - Output sound 3 to SO1 terminal
; -- Bit 1   - Output sound 2 to SO1 terminal
; -- Bit 0   - Output sound 0 to SO1 terminal
; --
.equ rNR51 0xFF25
.equ rAUDTERM 0xff25

; SO2
.equ AUDTERM_4_LEFT  0b10000000
.equ AUDTERM_3_LEFT  0b01000000
.equ AUDTERM_2_LEFT  0b00100000
.equ AUDTERM_1_LEFT  0b00010000
; SO1
.equ AUDTERM_4_RIGHT 0b00001000
.equ AUDTERM_3_RIGHT 0b00000100
.equ AUDTERM_2_RIGHT 0b00000010
.equ AUDTERM_1_RIGHT 0b00000001


; --
; -- AUDENA/NR52 (0xFF26)
; -- Sound on/off (R/W)
; --
; -- Bit 7   - All sound on/off (sets all audio regs to 0!)
; -- Bit 3   - Sound 4 ON flag (read only)
; -- Bit 2   - Sound 3 ON flag (read only)
; -- Bit 1   - Sound 2 ON flag (read only)
; -- Bit 0   - Sound 1 ON flag (read only)
; --
.equ rNR52 0xFF26
.equ rAUDENA 0xFF26

.equ AUDENA_ON    0b10000000
.equ AUDENA_OFF   0b00000000  ; sets all audio regs to 0!


; --
; -- LCDC (0xFF40)
; -- LCD Control (R/W)
; --
.equ rLCDC 0xFF40

.equ LCDCF_OFF     0b00000000 ; LCD Control Operation
.equ LCDCF_ON      0b10000000 ; LCD Control Operation
.equ LCDCF_WIN9800 0b00000000 ; Window Tile Map Display Select
.equ LCDCF_WIN9C00 0b01000000 ; Window Tile Map Display Select
.equ LCDCF_WINOFF  0b00000000 ; Window Display
.equ LCDCF_WINON   0b00100000 ; Window Display
.equ LCDCF_BG8800  0b00000000 ; BG & Window Tile Data Select
.equ LCDCF_BG8000  0b00010000 ; BG & Window Tile Data Select
.equ LCDCF_BG9800  0b00000000 ; BG Tile Map Display Select
.equ LCDCF_BG9C00  0b00001000 ; BG Tile Map Display Select
.equ LCDCF_OBJ8    0b00000000 ; OBJ Construction
.equ LCDCF_OBJ16   0b00000100 ; OBJ Construction
.equ LCDCF_OBJOFF  0b00000000 ; OBJ Display
.equ LCDCF_OBJON   0b00000010 ; OBJ Display
.equ LCDCF_BGOFF   0b00000000 ; BG Display
.equ LCDCF_BGON    0b00000001 ; BG Display
; "Window Character Data Select" follows BG


; --
; -- STAT (0xFF41)
; -- LCDC Status   (R/W)
; --
.equ rSTAT 0xFF41

.equ STATF_LYC      0b01000000 ; LYC=LY Coincidence (Selectable)
.equ STATF_MODE10   0b00100000 ; Mode 10
.equ STATF_MODE01   0b00010000 ; Mode 01 (V-Blank)
.equ STATF_MODE00   0b00001000 ; Mode 00 (H-Blank)
.equ STATF_LYCF     0b00000100 ; Coincidence Flag
.equ STATF_HBL      0b00000000 ; H-Blank
.equ STATF_VBL      0b00000001 ; V-Blank
.equ STATF_OAM      0b00000010 ; OAM-RAM is used by system
.equ STATF_LCD      0b00000011 ; Both OAM and VRAM used by system
.equ STATF_BUSY     0b00000010 ; When set, VRAM access is unsafe


; --
; -- SCY (0xFF42)
; -- Scroll Y (R/W)
; --
.equ rSCY 0xFF42


; --
; -- SCX (0xFF43)
; -- Scroll X (R/W)
; --
.equ rSCX 0xFF43


; --
; -- LY (0xFF44)
; -- LCDC Y-Coordinate (R)
; --
; -- Values range from 0->153. 144->153 is the VBlank period.
; --
.equ rLY 0xFF44


; --
; -- LYC (0xFF45)
; -- LY Compare (R/W)
; --
; -- When LY==LYC, STATF_LYCF will be set in STAT
; --
.equ rLYC 0xFF45


; --
; -- DMA (0xFF46)
; -- DMA Transfer and Start Address (W)
; --
.equ rDMA 0xFF46


; --
; -- BGP (0xFF47)
; -- BG Palette Data (W)
; --
; -- Bit 7-6 - Intensity for 0b11
; -- Bit 5-4 - Intensity for 0b10
; -- Bit 3-2 - Intensity for 0b01
; -- Bit 1-0 - Intensity for 0b00
; --
.equ rBGP 0xFF47


; --
; -- OBP0 (0xFF48)
; -- Object Palette 0 Data (W)
; --
; -- See BGP for info
; --
.equ rOBP0 0xFF48


; --
; -- OBP1 (0xFF49)
; -- Object Palette 1 Data (W)
; --
; -- See BGP for info
; --
.equ rOBP1 0xFF49


; --
; -- WY (0xFF4A)
; -- Window Y Position (R/W)
; --
; -- 0 <= WY <= 143
; -- When WY = 0, the window is displayed from the top edge of the LCD screen.
; --
.equ rWY 0xFF4A


; --
; -- WX (0xFF4B)
; -- Window X Position (R/W)
; --
; -- 7 <= WX <= 166
; -- When WX = 7, the window is displayed from the left edge of the LCD screen.
; -- Values of 0-6 and 166 are unreliable due to hardware bugs.
; --
.equ rWX 0xFF4B


; --
; -- SPEED (0xFF4D)
; -- Select CPU Speed (R/W)
; --
.equ rKEY1 0xFF4D
.equ rSPD  0xFF4D

.equ KEY1F_DBLSPEED 0b10000000 ; 0=Normal Speed, 1=Double Speed (R)
.equ KEY1F_PREPARE  0b00000001 ; 0=No, 1=Prepare (R/W)


; --
; -- VBK (0xFF4F)
; -- Select Video RAM Bank (R/W)
; --
; -- Bit 0 - Bank Specification (0: Specify Bank 0; 1: Specify Bank 1)
; --
.equ rVBK 0xFF4F


; --
; -- HDMA1 (0xFF51)
; -- High byte for Horizontal Blanking/General Purpose DMA source address (W)
; -- CGB Mode Only
; --
.equ rHDMA1 0xFF51


; --
; -- HDMA2 (0xFF52)
; -- Low byte for Horizontal Blanking/General Purpose DMA source address (W)
; -- CGB Mode Only
; --
.equ rHDMA2 0xFF52


; --
; -- HDMA3 (0xFF53)
; -- High byte for Horizontal Blanking/General Purpose DMA destination address (W)
; -- CGB Mode Only
; --
.equ rHDMA3 0xFF53


; --
; -- HDMA4 (0xFF54)
; -- Low byte for Horizontal Blanking/General Purpose DMA destination address (W)
; -- CGB Mode Only
; --
.equ rHDMA4 0xFF54


; --
; -- HDMA5 (0xFF55)
; -- Transfer length (in tiles minus 1)/mode/start for Horizontal Blanking, General Purpose DMA (R/W)
; -- CGB Mode Only
; --
.equ rHDMA5 0xFF55

.equ HDMA5F_MODE_GP  0b00000000 ; General Purpose DMA (W)
.equ HDMA5F_MODE_HBL 0b10000000 ; HBlank DMA (W)

; -- Once DMA has started, use HDMA5F_BUSY to check when the transfer is complete
.equ HDMA5F_BUSY 0b10000000 ; 0=Busy (DMA still in progress), 1=Transfer complete (R)


; --
; -- RP (0xFF56)
; -- Infrared Communications Port (R/W)
; -- CGB Mode Only
; --
.equ rRP 0xFF56

.equ RPF_ENREAD   0b11000000
.equ RPF_DATAIN   0b00000010 ; 0=Receiving IR Signal, 1=Normal
.equ RPF_WRITE_HI 0b00000001
.equ RPF_WRITE_LO 0b00000000


; --
; -- BCPS (0xFF68)
; -- Background Color Palette Specification (R/W)
; --
.equ rBCPS 0xFF68

.equ BCPSF_AUTOINC 0b10000000 ; Auto Increment (0=Disabled, 1=Increment after Writing)


; --
; -- BCPD (0xFF69)
; -- Background Color Palette Data (R/W)
; --
.equ rBCPD 0xFF69


; --
; -- OCPS (0xFF6A)
; -- Object Color Palette Specification (R/W)
; --
.equ rOCPS 0xFF6A

.equ OCPSF_AUTOINC 0b10000000 ; Auto Increment (0=Disabled, 1=Increment after Writing)


; --
; -- OCPD (0xFF6B)
; -- Object Color Palette Data (R/W)
; --
.equ rOCPD 0xFF6B


; --
; -- SMBK/SVBK (0xFF70)
; -- Select Main RAM Bank (R/W)
; --
; -- Bit 2-0 - Bank Specification (0,1: Specify Bank 1; 2-7: Specify Banks 2-7)
; --
.equ rSVBK 0xFF70
.equ rSMBK 0xFF70


; --
; -- PCM12 (0xFF76)
; -- Sound channel 1&2 PCM amplitude (R)
; --
; -- Bit 7-4 - Copy of sound channel 2's PCM amplitude
; -- Bit 3-0 - Copy of sound channel 1's PCM amplitude
; --
.equ rPCM12 0xFF76


; --
; -- PCM34 (0xFF77)
; -- Sound channel 3&4 PCM amplitude (R)
; --
; -- Bit 7-4 - Copy of sound channel 4's PCM amplitude
; -- Bit 3-0 - Copy of sound channel 3's PCM amplitude
; --
.equ rPCM34 0xFF77


; --
; -- IE (0xFFFF)
; -- Interrupt Enable (R/W)
; --
.equ rIE 0xFFFF

.equ IEF_HILO   0b00010000 ; Transition from High to Low of Pin number P10-P13
.equ IEF_SERIAL 0b00001000 ; Serial I/O transfer end
.equ IEF_TIMER  0b00000100 ; Timer Overflow
.equ IEF_STAT   0b00000010 ; STAT
.equ IEF_VBLANK 0b00000001 ; V-Blank


;***************************************************************************
;*
;* Flags common to multiple sound channels
;*
;***************************************************************************

; --
; -- Square wave duty cycle
; --
; -- Can be used with AUD1LEN and AUD2LEN
; -- See AUD1LEN for more info
; --
.equ AUDLEN_DUTY_12_5    0b00000000 ; 12.50b
.equ AUDLEN_DUTY_25      0b01000000 ; 250b
.equ AUDLEN_DUTY_50      0b10000000 ; 500b
.equ AUDLEN_DUTY_75      0b11000000 ; 750b


; --
; -- Audio envelope flags
; --
; -- Can be used with AUD1ENV, AUD2ENV, AUD4ENV
; -- See AUD1ENV for more info
; --
.equ AUDENV_UP           0b00001000
.equ AUDENV_DOWN         0b00000000


; --
; -- Audio trigger flags
; --
; -- Can be used with AUD1HIGH, AUD2HIGH, AUD3HIGH
; -- See AUD1HIGH for more info
; --

.equ AUDHIGH_RESTART     0b10000000
.equ AUDHIGH_LENGTH_ON   0b01000000
.equ AUDHIGH_LENGTH_OFF  0b00000000


;***************************************************************************
;*
;* CPU values on bootup (a=type, b=qualifier)
;*
;***************************************************************************

.equ BOOTUP_A_DMG    0x01 ; Dot Matrix Game
.equ BOOTUP_A_CGB    0x11 ; Color GameBoy
.equ BOOTUP_A_MGB    0xFF ; Mini GameBoy (Pocket GameBoy)

; if a=BOOTUP_A_CGB, bit 0 in b can be checked to determine if real CGB or
; other system running in GBC mode
.equ BOOTUP_B_CGB    0b00000000
.equ BOOTUP_B_AGB    0b00000001   ; GBA, GBA SP, Game Boy Player, or New GBA SP


;***************************************************************************
;*
;* Cart related
;*
;***************************************************************************

; 0x0143 Color GameBoy compatibility code
.equ CART_COMPATIBLE_DMG     0x00
.equ CART_COMPATIBLE_DMG_GBC 0x80
.equ CART_COMPATIBLE_GBC     0xC0

; 0x0146 GameBoy/Super GameBoy indicator
.equ CART_INDICATOR_GB       0x00
.equ CART_INDICATOR_SGB      0x03

; 0x0147 Cartridge type
.equ CART_ROM                     0x00
.equ CART_ROM_MBC1                0x01
.equ CART_ROM_MBC1_RAM            0x02
.equ CART_ROM_MBC1_RAM_BAT        0x03
.equ CART_ROM_MBC2                0x05
.equ CART_ROM_MBC2_BAT            0x06
.equ CART_ROM_RAM                 0x08
.equ CART_ROM_RAM_BAT             0x09
.equ CART_ROM_MMM01               0x0B
.equ CART_ROM_MMM01_RAM           0x0C
.equ CART_ROM_MMM01_RAM_BAT       0x0D
.equ CART_ROM_MBC3_BAT_RTC        0x0F
.equ CART_ROM_MBC3_RAM_BAT_RTC    0x10
.equ CART_ROM_MBC3                0x11
.equ CART_ROM_MBC3_RAM            0x12
.equ CART_ROM_MBC3_RAM_BAT        0x13
.equ CART_ROM_MBC5                0x19
.equ CART_ROM_MBC5_BAT            0x1A
.equ CART_ROM_MBC5_RAM_BAT        0x1B
.equ CART_ROM_MBC5_RUMBLE         0x1C
.equ CART_ROM_MBC5_RAM_RUMBLE     0x1D
.equ CART_ROM_MBC5_RAM_BAT_RUMBLE 0x1E
.equ CART_ROM_MBC7_RAM_BAT_GYRO   0x22
.equ CART_ROM_POCKET_CAMERA       0xFC
.equ CART_ROM_BANDAI_TAMA5        0xFD
.equ CART_ROM_HUDSON_HUC3         0xFE
.equ CART_ROM_HUDSON_HUC1         0xFF

; 0x0148 ROM size
; these are kilobytes
.equ CART_ROM_32KB   0x00 ; 2 banks
.equ CART_ROM_64KB   0x01 ; 4 banks
.equ CART_ROM_128KB  0x02 ; 8 banks
.equ CART_ROM_256KB  0x03 ; 16 banks
.equ CART_ROM_512KB  0x04 ; 32 banks
.equ CART_ROM_1024KB 0x05 ; 64 banks
.equ CART_ROM_2048KB 0x06 ; 128 banks
.equ CART_ROM_4096KB 0x07 ; 256 banks
.equ CART_ROM_8192KB 0x08 ; 512 banks
.equ CART_ROM_1152KB 0x52 ; 72 banks
.equ CART_ROM_1280KB 0x53 ; 80 banks
.equ CART_ROM_1536KB 0x54 ; 96 banks

; 0x0149 SRAM size
; these are kilobytes
.equ CART_SRAM_NONE  0
.equ CART_SRAM_2KB   1 ; 1 incomplete bank
.equ CART_SRAM_8KB   2 ; 1 bank
.equ CART_SRAM_32KB  3 ; 4 banks
.equ CART_SRAM_128KB 4 ; 16 banks

.equ CART_SRAM_ENABLE  0x0A
.equ CART_SRAM_DISABLE 0x00

; 0x014A Destination code
.equ CART_DEST_JAPANESE     0x00
.equ CART_DEST_NON_JAPANESE 0x01


;***************************************************************************
;*
;* Keypad related
;*
;***************************************************************************

.equ PADF_DOWN   0x80
.equ PADF_UP     0x40
.equ PADF_LEFT   0x20
.equ PADF_RIGHT  0x10
.equ PADF_START  0x08
.equ PADF_SELECT 0x04
.equ PADF_B      0x02
.equ PADF_A      0x01

.equ PADB_DOWN   0x7
.equ PADB_UP     0x6
.equ PADB_LEFT   0x5
.equ PADB_RIGHT  0x4
.equ PADB_START  0x3
.equ PADB_SELECT 0x2
.equ PADB_B      0x1
.equ PADB_A      0x0


;***************************************************************************
;*
;* Screen related
;*
;***************************************************************************

.equ SCRN_X    160 ; Width of screen in pixels
.equ SCRN_Y    144 ; Height of screen in pixels
.equ SCRN_X_B  20  ; Width of screen in bytes
.equ SCRN_Y_B  18  ; Height of screen in bytes

.equ SCRN_VX   256 ; Virtual width of screen in pixels
.equ SCRN_VY   256 ; Virtual height of screen in pixels
.equ SCRN_VX_B 32  ; Virtual width of screen in bytes
.equ SCRN_VY_B 32  ; Virtual height of screen in bytes


;***************************************************************************
;*
;* OAM related
;*
;***************************************************************************

;; ; OAM attributes
;; ; each entry in OAM RAM is 4 bytes (sizeof_OAM_ATTRS)
;; 	;; BSRESET
;; .equ OAMA_Y              RB  1   ; y pos
;; .equ OAMA_X              RB  1   ; x pos
;; .equ OAMA_TILEID         RB  1   ; tile id
;; .equ OAMA_FLAGS          RB  1   ; flags (see below)
;; .equ sizeof_OAM_ATTRS    RB  0

.equ OAM_COUNT           40  ; number of OAM entries in OAM RAM

; flags
.equ OAMF_PRI        0b10000000 ; Priority
.equ OAMF_YFLIP      0b01000000 ; Y flip
.equ OAMF_XFLIP      0b00100000 ; X flip
.equ OAMF_PAL0       0b00000000 ; Palette number; 0,1 (DMG)
.equ OAMF_PAL1       0b00010000 ; Palette number; 0,1 (DMG)
.equ OAMF_BANK0      0b00000000 ; Bank number; 0,1 (GBC)
.equ OAMF_BANK1      0b00001000 ; Bank number; 0,1 (GBC)

.equ OAMF_PALMASK    0b00000111 ; Palette (GBC)

.equ OAMB_PRI        7 ; Priority
.equ OAMB_YFLIP      6 ; Y flip
.equ OAMB_XFLIP      5 ; X flip
.equ OAMB_PAL1       4 ; Palette number; 0,1 (DMG)
.equ OAMB_BANK1      3 ; Bank number; 0,1 (GBC)


;; ;*
;; ;* Nintendo scrolling logo
;; ;* (Code won't work on a real GameBoy)
;; ;* (if next lines are altered.)
;; MACRO NINTENDO_LOGO
;;     DB  0xCE,0xED,0x66,0x66,0xCC,0x0D,0x00,0x0B,0x03,0x73,0x00,0x83,0x00,0x0C,0x00,0x0D
;;     DB  0x00,0x08,0x11,0x1F,0x88,0x89,0x00,0x0E,0xDC,0xCC,0x6E,0xE6,0xDD,0xDD,0xD9,0x99
;;     DB  0xBB,0xBB,0x67,0x63,0x6E,0x0E,0xEC,0xCC,0xDD,0xDC,0x99,0x9F,0xBB,0xB9,0x33,0x3E
;; ENDM
