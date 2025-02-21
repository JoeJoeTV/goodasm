#include "gamnemonic.h"

#include "galangpic16c5x.h"

/* This implements the 12-bit PIC16C5x series, which is
 * NOT the samea s the rest of the 14-bit PIC16 series.
 * It's probably similar to PIC12.
 *
 * This implementation works on 16-bit words.  On disk,
 * the less significant bit comes first, but in memory,
 * we swap things to make mimic the in-memory order of
 * the chip.
 *
 * Docs begin on page 50 of DS30453E.
 */

//Just to keep things shorter.
#define mnem new GAMnemonic

GALangPIC16C5x::GALangPIC16C5x(){
    //Support for weird addressing.
    wordsize=2;   // 12 bits to a word, but here we mean bytes.
    epsilon=2;    // Address is the unpacked word, not the byte.
    align=2;      // Alignment is one word.  (12 bits, 2 bytes)
    maxbytes=2;   // Two bytes maximum per instruction.
    minbytes=2;   // Two byte minimum per word.
    swapwordonload=true;  //File is little endian, but we work in big endian.
    name="pic16c5x";
    endian=GALanguage::BIG;

    insert(mnem("addwf",2,"\x01\xC0","\xff\xC0"))
        ->help("Add W and f.")
        ->example("addwf @0x1f, #1")
        ->abs("\x00\x1f") // f -- Register.
        ->imm("\x00\x20"); // d -- Store to W (0) or to reg (1)
    insert(mnem("andwf",2,"\x01\x40","\xff\xC0"))
        ->help("AND W with f.")
        ->example("andwf @0x1f, #1")
        ->abs("\x00\x1f") // f -- Register.
        ->imm("\x00\x20"); // d
    insert(mnem("clrf",2,"\x00\x60","\xff\xe0"))
        ->help("Clears F.")
        ->example("clrf @0x1f")
        ->abs("\x00\x1f");
    insert(mnem("clrw",2,"\x00\x40","\xff\xff"))
        ->help("Clears W.")
        ->example("clrw");
    insert(mnem("comf",2,"\x02\x40","\xff\xc0"))
        ->help("Complement f.")
        ->example("comf @0x1f, #0")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("decf",2,"\x00\xc0","\xff\xc0"))
        ->help("Decrement f.")
        ->example("decf @0x1f, #1")
        ->abs("\x00\x1f") // f
        ->imm("\x00\x20"); // d
    insert(mnem("decfsz",2,"\x02\xc0","\xff\xc0"))
        ->help("Decrement f, skip if zero.")
        ->example("decfsz @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("incf",2,"\x02\x80","\xff\xc0"))
        ->help("Increment f.")
        ->example("incf @0x1f, #1")
        ->abs("\x00\x1f") // f
        ->imm("\x00\x20"); // d
    insert(mnem("incfsz",2,"\x03\xc0","\xff\xc0"))
        ->help("Increment f, skip if zero.")
        ->example("incfsz @0x1f, #1")
        ->abs("\x00\x1f") // f
        ->imm("\x00\x20"); // d
    insert(mnem("iorwf",2,"\x01\x00","\xff\xc0"))
        ->help("Inclusive OR W with f.")
        ->example("iorwf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("movf",2,"\x02\x00","\xff\xc0"))
        ->help("Move f.")
        ->example("movf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("movwf",2,"\x00\x20","\xff\xe0"))
        ->help("Move W to f.")
        ->example("movwf @0x1f")
        ->abs("\x00\x1f"); // f
    insert(mnem("nop",2,"\x00\x00","\xff\xff"))
        ->help("No operation.")
        ->prioritize()
        ->example("nop");
    insert(mnem("rlf",2,"\x03\x40","\xff\xc0"))
        ->help("Rotate left f through carry.")
        ->example("rlf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("rrf",2,"\x03\x00","\xff\xc0"))
        ->help("Rotate right f through carry.")
        ->example("rrf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d
    insert(mnem("subwf",2,"\x00\x80","\xff\xc0"))
        ->help("Subtract W from f.")
        ->example("subwf @0x1f, #1")
        ->abs("\x00\x1f") // f
        ->imm("\x00\x20"); // d
    insert(mnem("swapf",2,"\x03\x80","\xff\xc0"))
        ->help("Swap f")
        ->example("swapf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d    What does this do?
    insert(mnem("xorwf",2,"\x01\x80","\xff\xc0"))
        ->help("XOR W with f.")
        ->example("xorwf @0x1f, #1")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\x20"); // d;

    //  Bit-oriented file register operations.
    //BCF f,b
    insert(mnem("bcf",2,"\x04\x00","\xff\x00"))
        ->help("Bit Clear f.")
        ->example("bcf @0x1f, #7")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\xe0"); // b
    //BSF f,b
    insert(mnem("bsf",2,"\x05\x00","\xff\x00"))
        ->help("Bit Set f.")
        ->example("bsf @0x1f, #7")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\xe0"); // b
    //BTFSC f,b
    insert(mnem("btfsc",2,"\x06\x00","\xff\x00"))
        ->help("Bit Test f, Skip if Clear.")
        ->example("btfsc @0x1f, #7")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\xe0"); // b
    //BTFSS f,b
    insert(mnem("btfss",2,"\x07\x00","\xff\x00"))
        ->help("Bit Test f, Skip if Set.")
        ->example("btfss @0x1f, #7")
        ->abs("\x00\x1f")  // f
        ->imm("\x00\xe0"); // b

    //  Literal and control operations.
    insert(mnem("andlw",2,"\x0E\x00","\xff\x00"))
        ->help("AND literal with W.")
        ->example("andlw #0xff")
        ->imm("\x00\xff");
    insert(mnem("call",2,"\x09\x00","\xff\x00"))
        ->help("Call a subroutine.")
        ->example("call 0xff")
        ->adr("\x00\xff");        //FIXME -- How to handle PIC16 code addresses?
    insert(mnem("clrwdt",2,"\x00\x04","\xff\xff"))
        ->help("Clear Watchdog Timer.")
        ->example("clrwdt");
    insert(mnem("goto",2,"\x0a\x00","\xfe\x00"))
        ->help("Unconditional branch.")
        ->example("goto 0x1ff")    //FIXME: Make this match symbols.
        ->adr("\x01\xff");
    insert(mnem("iorlw",2,"\x0d\x00","\xff\x00"))
        ->help("Inclusive OR literal with W.")
        ->example("iorlw #0xff")
        ->imm("\x00\xff");
    insert(mnem("movlw",2,"\x0c\x00","\xff\x00"))
        ->help("Move literal to W.")
        ->example("movlw #0xff")
        ->imm("\x00\xff");
    insert(mnem("option",2,"\x00\x02","\xff\xff"))
        ->help("Load the OPTION register.")
        ->example("option");
    insert(mnem("retlw",2,"\x08\x00","\xff\x00"))
        ->help("Return, place literal in W.")
        ->example("retlw #0xff")
        ->imm("\x00\xff");

    insert(mnem("sleep",2,"\x00\x03","\xff\xff")) //Overlaps with TRIS.
        ->help("Go into standby mode.")
        ->prioritize() // Against Tris.
        ->example("sleep");


    // Not sure why, but TRIS seems to collide with everything, even those that look like
    // they don't overlap.
    // TRIS #0 -- NOP
    // TRIS #1 --
    // TRIS #2 -- OPTION
    // TRIS #3 -- SLEEP
    // TRIS #4 -- CLRWDT
    // TRIS #5 --
    // TRIS #6 --
    // TRIS #7 --
    /*
    insert(mnem("tris",2,"\x00\x00","\xff\xf8"))  //Overlaps with SLEEP and many others.
        ->help("Load TRIS register.")
        ->prioritize(2)
        ->example("tris #7")
        ->imm("\x00\x07");
    */

    //XORLW k
    insert(mnem("xorlw",2,"\x0f\x00","\xff\x00"))
        ->help("XOR literal with W.")
        ->example("xorlw #0xff")
        ->imm("\x00\xff");

}
