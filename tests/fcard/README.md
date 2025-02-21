Howdy y'all,

These are test cases for the DSS P1 smart card, also known as the
DirecTV F Card.  It's a Motorola 6805SC21 chip whose instruction
decode latch has been scrambled; this changes the opcodes but
not the parameter bytes.

`exploit.asm` is an early exploit for the card.

`sample.cor` is a memory dump that was first distributred with Pierre
G. Martineau's MC68HC05SC21 emulator in 1996.

--Travis


```
0x0000 Registers:
0: Port A
1: Port B
4: DDR A
5: DDR B
8: Misc: bit4=DCTST, bit6=INTFF, bit7=ROMPG
9: Prog: bit0=PLE, bit1=PGE, bit2=VPON, bit7=W/E

0x0080: SRAM, stack growing down from 0x00ff.

0x0520: EEPROM, with security bytes from 10f0 to 10ff.

0x1100 to 0x1ff7: ROM Page 0
0x1100 to 0x19ff: ROM Page 1
0x1ff8 to 0x1fff: ROM User Vectors, with IRQ at 1ffa, SWA at 1ffc, and RESET at 1ffe.
```