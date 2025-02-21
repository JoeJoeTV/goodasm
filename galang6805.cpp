#include "gamnemonic.h"
#include "galang6805.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

GALang6805::GALang6805() {
    endian=BIG;
    name="6805";
    maxbytes=3;

    //Register names.
    regnames.clear();
    regnames<<"x"<<"y"<<"a";


    //Sheet 1 from MC68HC05P9

    insert(mnem("adc", 2, "\xa9\x00", "\xff\x00"))
        ->help("Add to A with Carry. (imm)")
        ->example("adc #0xff")
        ->imm("\x00\xff");
    insert(mnem("adc", 2, "\xb9\x00", "\xff\x00"))
        ->help("Add to A with Carry. (dir)")
        ->example("adc @0xff")
        ->abs("\x00\xff");
    insert(mnem("adc", 3, "\xc9\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (ext)")
        ->example("adc @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("adc", 3, "\xd9\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (ix2)")
        ->example("adc @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("adc", 2, "\xe9\x00", "\xff\x00"))
        ->help("Add to A with Carry. (ix1)")
        ->example("adc @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("adc", 1, "\xf9", "\xff"))
        ->help("Add to A with Carry. (ix)")
        ->example("adc @x")
        ->regnameind("x");

    insert(mnem("add", 2, "\xab\x00", "\xff\x00"))
        ->help("Add to A. (imm)")
        ->example("add #0xff")
        ->imm("\x00\xff");
    insert(mnem("add", 2, "\xbb\x00", "\xff\x00"))
        ->help("Add to A. (dir)")
        ->example("add @0xff")
        ->abs("\x00\xff");
    insert(mnem("add", 3, "\xcb\x00\x00", "\xff\x00\x00"))
        ->help("Add to A. (ext)")
        ->example("add @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("add", 3, "\xdb\x00\x00", "\xff\x00\x00"))
        ->help("Add to A. (ix2)")
        ->example("add @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("add", 2, "\xeb\x00", "\xff\x00"))
        ->help("Add to A. (ix1)")
        ->example("add @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("add", 1, "\xfb", "\xff"))
        ->help("Add to A. (ix)")
        ->example("add @x")
        ->regnameind("x");


    insert(mnem("and", 2, "\xa4\x00", "\xff\x00"))
        ->help("AND into A. (imm)")
        ->example("and #0xff")
        ->imm("\x00\xff");
    insert(mnem("and", 2, "\xb4\x00", "\xff\x00"))
        ->help("AND into A. (dir)")
        ->example("and @0xff")
        ->abs("\x00\xff");
    insert(mnem("and", 3, "\xc4\x00\x00", "\xff\x00\x00"))
        ->help("AND into A. (ext)")
        ->example("and @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("and", 3, "\xd4\x00\x00", "\xff\x00\x00"))
        ->help("AND into A. (ix2)")
        ->example("and @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("and", 2, "\xe4\x00", "\xff\x00"))
        ->help("AND into A. (ix1)")
        ->example("and @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("and", 1, "\xf4", "\xff"))
        ->help("AND into A. (ix)")
        ->example("and @x")
        ->regnameind("x");

    insert(mnem("asl", 2, "\x38\x00", "\xff\x00"))
        ->help("Arithmetic Shift Left. (dir)")
        ->example("asl @0xff")
        ->abs("\x00\xff");
    insert(mnem("asl", 1, "\x48", "\xff"))
        ->help("Arithmetic Shift Left. (inh)")
        ->example("asl a")
        ->regname("a");
    insert(mnem("asl", 1, "\x58", "\xff"))
        ->help("Arithmetic Shift Left. (inh)")
        ->example("asl x")
        ->regname("x");
    insert(mnem("asl", 2, "\x68\x00", "\xff\x00"))
        ->help("Arithmetic Shift Left. (ix1)")
        ->example("asl @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("asl", 1, "\x78", "\xff"))
        ->help("Arithmetic Shift Left. (ix)")
        ->example("asl @x")
        ->regnameind("x");

    insert(mnem("asr", 2, "\x37\x00", "\xff\x00"))
        ->help("Arithmetic Shift Right. (dir)")
        ->example("asr @0xff")
        ->abs("\x00\xff");
    insert(mnem("asr", 1, "\x47", "\xff"))
        ->help("Arithmetic Shift Right. (inh)")
        ->example("asr a")
        ->regname("a");
    insert(mnem("asr", 1, "\x57", "\xff\x00"))
        ->help("Arithmetic Shift Right. (inh)")
        ->example("asr x")
        ->regname("x");
    insert(mnem("asr", 2, "\x67\x00", "\xff\x00"))
        ->help("Arithmetic Shift Right. (ix1)")
        ->example("asr @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("asr", 1, "\x77", "\xff"))
        ->help("Arithmetic Shift Right. (ix)")
        ->example("asr @x")
        ->regnameind("x");

    insert(mnem("bcc", 2, "\x24\x00", "\xff\x00"))
        ->help("Branch if Carry Clear. (rel)")
        ->example("here: bcc here")
        ->prioritize()       //Dupe of BHS.
        ->rel("\x00\xff");

    insert(mnem("bclr", 2, "\x11\x00", "\xf1\x00"))
        ->help("Clear bit. (dir)")
        ->example("bclr bit4, @0xff")
        ->bit3("\x0e\x00")
        ->abs("\x00\xff");


    insert(mnem("bcs", 2, "\x25\x00", "\xff\x00"))
        ->help("Branch if Carry Set. (rel)")
        ->example("here: bcs here")
        ->prioritize()       //Dupe of BLO.
        ->rel("\x00\xff");
    insert(mnem("beq", 2, "\x27\x00", "\xff\x00"))
        ->help("Branch if Equal. (rel)")
        ->example("here: beq here")
        ->rel("\x00\xff");
    insert(mnem("bhcc", 2, "\x28\x00", "\xff\x00"))
        ->help("Branch if Half Carry Clear. (rel)")
        ->example("here: bhcc here")
        ->rel("\x00\xff");
    insert(mnem("bhcs", 2, "\x29\x00", "\xff\x00"))
        ->help("Branch if Half Carry Set. (rel)")
        ->example("here: bhcs here")
        ->rel("\x00\xff");
    insert(mnem("bhi", 2, "\x22\x00", "\xff\x00"))
        ->help("Branch if Higher. (rel)")
        ->example("here: bhi here")
        ->rel("\x00\xff");
    insert(mnem("bhs", 2, "\x24\x00", "\xff\x00"))  //Same BCLR.
        ->help("Branch if Higher or Same. (rel)")
        ->example("here: bhs here")
        ->rel("\x00\xff");
    insert(mnem("bih", 2, "\x2f\x00", "\xff\x00"))  //Same BCLR.
        ->help("Branch if IRQ High. (rel)")
        ->example("here: bih here")
        ->rel("\x00\xff");
    insert(mnem("bil", 2, "\x2e\x00", "\xff\x00"))  //Same BCLR.
        ->help("Branch if IRQ Low. (rel)")
        ->example("here: bil here")
        ->rel("\x00\xff");


    insert(mnem("bit", 2, "\xa5\x00", "\xff\x00"))
        ->help("Bit Immediate Test. (imm)")
        ->example("bit #0xff")
        ->imm("\x00\xff");
    insert(mnem("bit", 2, "\xb5\x00", "\xff\x00"))
        ->help("Bit Immediate Test. (dir)")
        ->example("bit @0xff")
        ->abs("\x00\xff");
    insert(mnem("bit", 3, "\xc5\x00\x00", "\xff\x00\x00"))
        ->help("Bit Immediate Test. (ext)")
        ->example("bit @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("bit", 3, "\xd5\x00\x00", "\xff\x00\x00"))
        ->help("Bit Immediate Test. (ix2)")
        ->example("bit @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("bit", 2, "\xe5\x00", "\xff\x00"))
        ->help("Bit Immediate Test. (ix1)")
        ->example("bit @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("bit", 1, "\xf5", "\xff"))
        ->help("Bit Immediate Test. (ix)")
        ->example("bit @x")
        ->regnameind("x");

    insert(mnem("blo", 2, "\x25\x00", "\xff\x00"))
        ->help("Branch if Lower. (rel)")
        ->example("here: blo here")
        ->rel("\x00\xff");
    insert(mnem("bls", 2, "\x23\x00", "\xff\x00"))
        ->help("Branch if Lower or Same. (rel)")
        ->example("here: bls here")
        ->rel("\x00\xff");
    insert(mnem("bmc", 2, "\x2c\x00", "\xff\x00"))
        ->help("Branch if Int Mask Clear. (rel)")
        ->example("here: bmc here")
        ->rel("\x00\xff");
    insert(mnem("bmi", 2, "\x2b\x00", "\xff\x00"))
        ->help("Branch if Minus. (rel)")
        ->example("here: bmi here")
        ->rel("\x00\xff");
    insert(mnem("bms", 2, "\x2d\x00", "\xff\x00"))
        ->help("Branch if Int Mask Set. (rel)")
        ->example("here: bms here")
        ->rel("\x00\xff");
    insert(mnem("bne", 2, "\x26\x00", "\xff\x00"))
        ->help("Branch if Not Equal. (rel)")
        ->example("here: bne here")
        ->rel("\x00\xff");
    insert(mnem("bpl", 2, "\x2a\x00", "\xff\x00"))
        ->help("Branch if Plus. (rel)")
        ->example("here: bpl here")
        ->rel("\x00\xff");


    // Sheet 2
    insert(mnem("bra", 2, "\x20\x00", "\xff\x00"))
        ->help("Branch Always. (rel)")
        ->example("here: bra here")
        ->rel("\x00\xff");
    insert(mnem("brclr", 3, "\x01\x00\x00", "\xf1\x00\x00"))
        ->help("Branch if bit Clear. (dir)")
        ->example("here: brclr bit4, @0xff, here")
        ->bit3("\x0e\x00\x00")
        ->abs("\x00\xff\x00")
        ->rel("\x00\x00\xff");
    insert(mnem("brn", 2, "\x21\x00", "\xff\x00"))
        ->help("Branch Never. (rel)")
        ->example("here: brn here")
        ->rel("\x00\xff");
    insert(mnem("brset", 3, "\x00\x00\x00", "\xf1\x00\x00"))
        ->help("Branch if bit Set. (dir)")
        ->example("here: brset bit4, @0xff, here")
        ->bit3("\x0e\x00\x00")
        ->abs("\x00\xff\x00")
        ->rel("\x00\x00\xff");
    insert(mnem("bset", 2, "\x10\x00", "\xf1\x00"))
        ->help("Bit Set. (dir)")
        ->example("bset bit4, @0xff")
        ->bit3("\x0e\x00")
        ->abs("\x00\xff");
    insert(mnem("bsr", 2, "\xad\x00", "\xff\x00"))
        ->help("Branch to Subroutine. (rel)")
        ->example("here: bsr here")
        ->rel("\x00\xff");
    insert(mnem("clc", 1, "\x98", "\xff"))
        ->help("Clear Carry Bit. (rel)")
        ->example("clc");
    insert(mnem("cli", 1, "\x9a", "\xff"))
        ->help("Clear Int Mask. (rel)")
        ->example("cli");

    insert(mnem("clr", 2, "\x3f\x00", "\xff\x00"))
        ->help("Clear. (dir)")
        ->example("clr @0xff")
        ->abs("\x00\xff");
    insert(mnem("clr", 1, "\x4f", "\xff"))
        ->help("Clear. (inh)")
        ->example("clr a")
        ->regname("a");
    insert(mnem("clr", 1, "\x5f", "\xff"))
        ->help("Clear. (inh)")
        ->example("clr x")
        ->regname("x");
    insert(mnem("clr", 2, "\x6f\x00", "\xff\x00"))
        ->help("Clear. (ix1)")
        ->example("clr @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("clr", 1, "\x7f", "\xff"))
        ->help("Clear. (ix)")
        ->example("clr @x")
        ->regnameind("x");

    insert(mnem("cmp", 2, "\xa1\x00", "\xff\x00"))
        ->help("Compare to Accumulator. (imm)")
        ->example("cmp #0xff")
        ->imm("\x00\xff");
    insert(mnem("cmp", 2, "\xb1\x00", "\xff\x00"))
        ->help("Compare to Accumulator. (dir)")
        ->example("cmp @0xff")
        ->abs("\x00\xff");
    insert(mnem("cmp", 3, "\xc1\x00\x00", "\xff\x00\x00"))
        ->help("Compare to Accumulator. (ext)")
        ->example("cmp @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("cmp", 3, "\xd1\x00\x00", "\xff\x00\x00"))
        ->help("Compare to Accumulator. (ix2)")
        ->example("cmp @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("cmp", 2, "\xe1\x00", "\xff\x00"))
        ->help("Compare to Accumulator. (ix1)")
        ->example("cmp @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("cmp", 1, "\xf1", "\xff"))
        ->help("Compare to Accumulator. (ix)")
        ->example("cmp @x")
        ->regnameind("x");


    insert(mnem("com", 2, "\x33\x00", "\xff\x00"))
        ->help("One's Complement. (dir)")
        ->example("com @0xff")
        ->abs("\x00\xff");
    insert(mnem("com", 1, "\x43", "\xff"))
        ->help("One's Complement. (inh)")
        ->example("com a")
        ->regname("a");
    insert(mnem("com", 1, "\x53", "\xff"))
        ->help("One's Complement. (inh)")
        ->example("com x")
        ->regname("x");
    insert(mnem("com", 2, "\x63\x00", "\xff\x00"))
        ->help("One's Complement. (ix1)")
        ->example("com @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("com", 1, "\x73", "\xff"))
        ->help("One's Complement. (ix)")
        ->example("com @x")
        ->regnameind("x");

    insert(mnem("cpx", 2, "\xa3\x00", "\xff\x00"))
        ->help("Compare to X. (imm)")
        ->example("cpx #0xff")
        ->imm("\x00\xff");
    insert(mnem("cpx", 2, "\xb3\x00", "\xff\x00"))
        ->help("Compare to X. (dir)")
        ->example("cpx @0xff")
        ->abs("\x00\xff");
    insert(mnem("cpx", 3, "\xc3\x00\x00", "\xff\x00\x00"))
        ->help("Compare to X. (ext)")
        ->example("cpx @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("cpx", 3, "\xd3\x00\x00", "\xff\x00\x00"))
        ->help("Compare to X. (ix2)")
        ->example("cpx @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("cpx", 2, "\xe3\x00", "\xff\x00"))
        ->help("Compare to X. (ix1)")
        ->example("cpx @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("cpx", 1, "\xf3", "\xff"))
        ->help("Compare to X. (ix)")
        ->example("cpx @x")
        ->regnameind("x");


    insert(mnem("dec", 2, "\x3a\x00", "\xff\x00"))
        ->help("Decrement. (dir)")
        ->example("dec @0xff")
        ->abs("\x00\xff");
    insert(mnem("dec", 1, "\x4a", "\xff"))
        ->help("Decrement. (inh)")
        ->example("dec a")
        ->regname("a");
    insert(mnem("dec", 1, "\x5a", "\xff"))
        ->help("Decrement. (inh)")
        ->example("dec x")
        ->regname("x");
    insert(mnem("dec", 2, "\x6a\x00", "\xff\x00"))
        ->help("Decrement. (ix1)")
        ->example("dec @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("dec", 1, "\x7a", "\xff"))
        ->help("Decrement. (ix)")
        ->example("dec @x")
        ->regnameind("x");

    //Sheet 3.

    insert(mnem("eor", 2, "\xa8\x00", "\xff\x00"))
        ->help("Exclusive OR Accumulator. (imm)")
        ->example("eor #0xff")
        ->imm("\x00\xff");
    insert(mnem("eor", 2, "\xb8\x00", "\xff\x00"))
        ->help("Exclusive OR Accumulator. (dir)")
        ->example("eor @0xff")
        ->abs("\x00\xff");
    insert(mnem("eor", 3, "\xc8\x00\x00", "\xff\x00\x00"))
        ->help("Exclusive OR Accumulator. (ext)")
        ->example("eor @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("eor", 3, "\xd8\x00\x00", "\xff\x00\x00"))
        ->help("Exclusive OR Accumulator. (ix2)")
        ->example("eor @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("eor", 2, "\xe8\x00", "\xff\x00"))
        ->help("Exclusive OR Accumulator. (ix1)")
        ->example("eor @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("eor", 1, "\xf8", "\xff"))
        ->help("Exclusive OR Accumulator. (ix)")
        ->example("eor @x")
        ->regnameind("x");


    insert(mnem("inc", 2, "\x3c\x00", "\xff\x00"))
        ->help("Increment. (dir)")
        ->example("inc @0xff")
        ->abs("\x00\xff");
    insert(mnem("inc", 1, "\x4c", "\xff"))
        ->help("Increment. (inh)")
        ->example("inc a")
        ->regname("a");
    insert(mnem("inc", 1, "\x5c", "\xff"))
        ->help("Increment. (inh)")
        ->example("inc x")
        ->regname("x");
    insert(mnem("inc", 2, "\x6c\x00", "\xff\x00"))
        ->help("Increment. (ix1)")
        ->example("inc @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("inc", 1, "\x7c", "\xff"))
        ->help("Increment. (ix)")
        ->example("inc @x")
        ->regnameind("x");


    // AC opcode is not a jump.
    insert(mnem("jmp", 2, "\xbc\x00", "\xff\x00"))
        ->help("Unconditional Jump. (dir)")
        ->example("jmp 0xff")
        ->adr("\x00\xff");
    insert(mnem("jmp", 3, "\xcc\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional Jump. (ext)")
        ->example("jmp 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jmp", 3, "\xdc\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional Jump. (ix2)")
        ->example("jmp @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("jmp", 2, "\xec\x00", "\xff\x00"))
        ->help("Unconditional Jump. (ix1)")
        ->example("jmp @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("jmp", 1, "\xfc", "\xff"))
        ->help("Unconditional Jump. (ix)")
        ->example("jmp @x")
        ->regnameind("x");

    // AD opcode is not a JSR.
    insert(mnem("jsr", 2, "\xbd\x00", "\xff\x00"))
        ->help("Jump to Subroutine. (dir)")
        ->example("jsr 0xff")
        ->adr("\x00\xff");
    insert(mnem("jsr", 3, "\xcd\x00\x00", "\xff\x00\x00"))
        ->help("Jump to Subroutine. (ext)")
        ->example("jsr 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jsr", 3, "\xdd\x00\x00", "\xff\x00\x00"))
        ->help("Jump to Subroutine. (ix2)")
        ->example("jsr @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("jsr", 2, "\xed\x00", "\xff\x00"))
        ->help("Jump to Subroutine. (ix1)")
        ->example("jsr @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("jsr", 1, "\xfd", "\xff"))
        ->help("Jump to Subroutine. (ix)")
        ->example("jsr @x")
        ->regnameind("x");


    insert(mnem("lda", 2, "\xa6\x00", "\xff\x00"))
        ->help("Load Accumulator. (imm)")
        ->example("lda #0xff")
        ->imm("\x00\xff");
    insert(mnem("lda", 2, "\xb6\x00", "\xff\x00"))
        ->help("Load Accumulator. (dir)")
        ->example("lda @0xff")
        ->abs("\x00\xff");
    insert(mnem("lda", 3, "\xc6\x00\x00", "\xff\x00\x00"))
        ->help("Load Accumulator. (ext)")
        ->example("lda @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("lda", 3, "\xd6\x00\x00", "\xff\x00\x00"))
        ->help("Load Accumulator. (ix2)")
        ->example("lda @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("lda", 2, "\xe6\x00", "\xff\x00"))
        ->help("Load Accumulator. (ix1)")
        ->example("lda @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("lda", 1, "\xf6", "\xff"))
        ->help("Load Accumulator. (ix)")
        ->example("lda @x")
        ->regnameind("x");


    insert(mnem("ldx", 2, "\xae\x00", "\xff\x00"))
        ->help("Load Index. (imm)")
        ->example("ldx #0xff")
        ->imm("\x00\xff");
    insert(mnem("ldx", 2, "\xbe\x00", "\xff\x00"))
        ->help("Load Index. (dir)")
        ->example("ldx @0xff")
        ->abs("\x00\xff");
    insert(mnem("ldx", 3, "\xce\x00\x00", "\xff\x00\x00"))
        ->help("Load Index. (ext)")
        ->example("ldx @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("ldx", 3, "\xde\x00\x00", "\xff\x00\x00"))
        ->help("Load Index. (ix2)")
        ->example("ldx @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("ldx", 2, "\xee\x00", "\xff\x00"))
        ->help("Load Index. (ix1)")
        ->example("ldx @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("ldx", 1, "\xfe", "\xff"))
        ->help("Load Index. (ix)")
        ->example("ldx @x")
        ->regnameind("x");


    //Same as ASL, because we're shifting into the carry bit.
    insert(mnem("lsl", 2, "\x38\x00", "\xff\x00"))
        ->help("Logical Shift Left. (dir)")
        ->example("lsl @0xff")
        ->prioritize() // Above ASL.
        ->abs("\x00\xff");
    insert(mnem("lsl", 1, "\x48", "\xff"))
        ->help("Logical Shift Left. (inh)")
        ->example("lsl a")
        ->prioritize() // Above ASL.
        ->regname("a");
    insert(mnem("lsl", 1, "\x58", "\xff"))
        ->help("Logical Shift Left. (inh)")
        ->example("lsl x")
        ->prioritize() // Above ASL.
        ->regname("x");
    insert(mnem("lsl", 2, "\x68\x00", "\xff\x00"))
        ->help("Logical Shift Left. (ix1)")
        ->example("lsl @0xff, x")
        ->prioritize() // Above ASL.
        ->absx("\x00\xff");
    insert(mnem("lsl", 1, "\x78", "\xff"))
        ->help("Logical Shift Left. (ix)")
        ->example("lsl @x")
        ->prioritize() // Above ASL.
        ->regnameind("x");

    insert(mnem("lsr", 2, "\x34\x00", "\xff\x00"))
        ->help("Logical Shift Right. (dir)")
        ->example("lsr @0xff")
        ->abs("\x00\xff");
    insert(mnem("lsr", 1, "\x44", "\xff"))
        ->help("Logical Shift Right. (inh)")
        ->example("lsr a")
        ->regname("a");
    insert(mnem("lsr", 1, "\x54", "\xff"))
        ->help("Logical Shift Right. (inh)")
        ->example("lsr x")
        ->regname("x");
    insert(mnem("lsr", 2, "\x64\x00", "\xff\x00"))
        ->help("Logical Shift Right. (ix1)")
        ->example("lsr @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("lsr", 1, "\x74", "\xff"))
        ->help("Logical Shift Right. (ix)")
        ->example("lsr @x")
        ->regnameind("x");

    insert(mnem("mul", 1, "\x42", "\xff"))
        ->help("Multiply. (ix)")
        ->example("mul a, x")
        ->regname("a")
        ->regname("x");


    insert(mnem("neg", 2, "\x30\x00", "\xff\x00"))
        ->help("Negate with Two's Complement. (dir)")
        ->example("neg @0xff")
        ->abs("\x00\xff");
    insert(mnem("neg", 1, "\x40", "\xff"))
        ->help("Negate with Two's Complement. (inh)")
        ->example("neg a")
        ->regname("a");
    insert(mnem("neg", 1, "\x50", "\xff"))
        ->help("Negate with Two's Complement. (inh)")
        ->example("neg x")
        ->regname("x");
    insert(mnem("neg", 2, "\x60\x00", "\xff\x00"))
        ->help("Negate with Two's Complement. (ix1)")
        ->example("neg @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("neg", 1, "\x70", "\xff"))
        ->help("Negate with Two's Complement. (ix)")
        ->example("neg @x")
        ->regnameind("x");


    insert(mnem("nop", 1, "\x9d", "\xff"))
        ->help("No Operation.")
        ->example("nop");



    insert(mnem("ora", 2, "\xaa\x00", "\xff\x00"))
        ->help("Inclusive OR Accumulator. (imm)")
        ->example("ora #0xff")
        ->imm("\x00\xff");
    insert(mnem("ora", 2, "\xba\x00", "\xff\x00"))
        ->help("Inclusive OR Accumulator. (dir)")
        ->example("ora @0xff")
        ->abs("\x00\xff");
    insert(mnem("ora", 3, "\xca\x00\x00", "\xff\x00\x00"))
        ->help("Inclusive OR Accumulator. (ext)")
        ->example("ora @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("ora", 3, "\xda\x00\x00", "\xff\x00\x00"))
        ->help("Inclusive OR Accumulator. (ix2)")
        ->example("ora @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("ora", 2, "\xea\x00", "\xff\x00"))
        ->help("Inclusive OR Accumulator. (ix1)")
        ->example("ora @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("ora", 1, "\xfa", "\xff"))
        ->help("Inclusive OR Accumulator. (ix)")
        ->example("ora @x")
        ->regnameind("x");


    insert(mnem("rol", 2, "\x39\x00", "\xff\x00"))
        ->help("Rotate Left Through Carry. (dir)")
        ->example("rol @0xff")
        ->abs("\x00\xff");
    insert(mnem("rol", 1, "\x49", "\xff"))
        ->help("Rotate Left Through Carry. (inh)")
        ->example("rol a")
        ->regname("a");
    insert(mnem("rol", 1, "\x59", "\xff"))
        ->help("Rotate Left Through Carry. (inh)")
        ->example("rol x")
        ->regname("x");
    insert(mnem("rol", 2, "\x69\x00", "\xff\x00"))
        ->help("Rotate Left Through Carry. (ix1)")
        ->example("rol @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("rol", 1, "\x79", "\xff"))
        ->help("Rotate Left Through Carry. (ix)")
        ->example("rol @x")
        ->regnameind("x");

    insert(mnem("ror", 2, "\x36\x00", "\xff\x00"))
        ->help("Rotate Right Through Carry. (dir)")
        ->example("ror @0xff")
        ->abs("\x00\xff");
    insert(mnem("ror", 1, "\x46", "\xff"))
        ->help("Rotate Right Through Carry. (inh)")
        ->example("ror a")
        ->regname("a");
    insert(mnem("ror", 1, "\x56", "\xff"))
        ->help("Rotate Right Through Carry. (inh)")
        ->example("ror x")
        ->regname("x");
    insert(mnem("ror", 2, "\x66\x00", "\xff\x00"))
        ->help("Rotate Right Through Carry. (ix1)")
        ->example("ror @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("ror", 1, "\x76", "\xff"))
        ->help("Rotate Right Through Carry. (ix)")
        ->example("ror @x")
        ->regnameind("x");


    insert(mnem("rsp", 1, "\x9c", "\xff"))
        ->help("Reset Stack Pointer.")
        ->example("rsp");
    insert(mnem("rti", 1, "\x80", "\xff"))
        ->help("Return from Interrupt.")
        ->example("rti");
    insert(mnem("rts", 1, "\x81", "\xff"))
        ->help("Return from Subroutine.")
        ->example("rts");



    insert(mnem("sbc", 2, "\xa2\x00", "\xff\x00"))
        ->help("Subtract and Carry from A. (imm)")
        ->example("sbc #0xff")
        ->imm("\x00\xff");
    insert(mnem("sbc", 2, "\xb2\x00", "\xff\x00"))
        ->help("Subtract and Carry from A. (dir)")
        ->example("sbc @0xff")
        ->abs("\x00\xff");
    insert(mnem("sbc", 3, "\xc2\x00\x00", "\xff\x00\x00"))
        ->help("Subtract and Carry from A. (ext)")
        ->example("sbc @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("sbc", 3, "\xd2\x00\x00", "\xff\x00\x00"))
        ->help("Subtract and Carry from A. (ix2)")
        ->example("sbc @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("sbc", 2, "\xe2\x00", "\xff\x00"))
        ->help("Subtract and Carry from A. (ix1)")
        ->example("sbc @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("sbc", 1, "\xf2", "\xff"))
        ->help("Subtract and Carry from A. (ix)")
        ->example("sbc @x")
        ->regnameind("x");

    insert(mnem("sec", 1, "\x99", "\xff"))
        ->help("Set Carry Bit.")
        ->example("sec");
    insert(mnem("sei", 1, "\x9b", "\xff"))
        ->help("Set Interrupt Mask.")
        ->example("sei");



    //A7 opcode is not STA.
    insert(mnem("sta", 2, "\xb7\x00", "\xff\x00"))
        ->help("Store Accumulator. (dir)")
        ->example("sta @0xff")
        ->abs("\x00\xff");
    insert(mnem("sta", 3, "\xc7\x00\x00", "\xff\x00\x00"))
        ->help("Store Accumulator. (ext)")
        ->example("sta @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("sta", 3, "\xd7\x00\x00", "\xff\x00\x00"))
        ->help("Store Accumulator. (ix2)")
        ->example("sta @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("sta", 2, "\xe7\x00", "\xff\x00"))
        ->help("Store Accumulator. (ix1)")
        ->example("sta @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("sta", 1, "\xf7", "\xff"))
        ->help("Store Accumulator. (ix)")
        ->example("sta @x")
        ->regnameind("x");


    //Stop
    insert(mnem("stop", 1, "\x8e", "\xff"))
        ->help("Enable !IRQ, stop oscilator.")
        ->example("stop");

    //STX
    insert(mnem("stx", 2, "\xbf\x00", "\xff\x00"))
        ->help("Store Index. (dir)")
        ->example("stx @0xff")
        ->abs("\x00\xff");
    insert(mnem("stx", 3, "\xcf\x00\x00", "\xff\x00\x00"))
        ->help("Store Index. (ext)")
        ->example("stx @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("stx", 3, "\xdf\x00\x00", "\xff\x00\x00"))
        ->help("Store Index. (ix2)")
        ->example("stx @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("stx", 2, "\xef\x00", "\xff\x00"))
        ->help("Store Index. (ix1)")
        ->example("stx @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("stx", 1, "\xff", "\xff"))
        ->help("Store Index. (ix)")
        ->example("stx @x")
        ->regnameind("x");


    insert(mnem("sub", 2, "\xa0\x00", "\xff\x00"))
        ->help("Subtract from A. (imm)")
        ->example("sub #0xff")
        ->imm("\x00\xff");
    insert(mnem("sub", 2, "\xb0\x00", "\xff\x00"))
        ->help("Subtract from A. (dir)")
        ->example("sub @0xff")
        ->abs("\x00\xff");
    insert(mnem("sub", 3, "\xc0\x00\x00", "\xff\x00\x00"))
        ->help("Subtract from A. (ext)")
        ->example("sub @0xffff")
        ->abs("\x00\xff\xff");
    insert(mnem("sub", 3, "\xd0\x00\x00", "\xff\x00\x00"))
        ->help("Subtract from A. (ix2)")
        ->example("sub @0xffff, x")
        ->absx("\x00\xff\xff");
    insert(mnem("sub", 2, "\xe0\x00", "\xff\x00"))
        ->help("Subtract from A. (ix1)")
        ->example("sub @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("sub", 1, "\xf0", "\xff"))
        ->help("Subtract from A. (ix)")
        ->example("sub @x")
        ->regnameind("x");


    insert(mnem("swi", 1, "\x83", "\xff"))
        ->help("Software Interrupt.")
        ->example("swi");
    insert(mnem("tax", 1, "\x97", "\xff"))
        ->help("Transfer A to X.")
        ->example("tax");



    insert(mnem("tst", 2, "\x3d\x00", "\xff\x00"))
        ->help("Test for Negative or Zero. (dir)")
        ->example("tst @0xff")
        ->abs("\x00\xff");
    insert(mnem("tst", 1, "\x4d", "\xff"))
        ->help("Test for Negative or Zero. (inh)")
        ->example("tst a")
        ->regname("a");
    insert(mnem("tst", 1, "\x5d", "\xff"))
        ->help("Test for Negative or Zero. (inh)")
        ->example("tst x")
        ->regname("x");
    insert(mnem("tst", 2, "\x6d\x00", "\xff\x00"))
        ->help("Test for Negative or Zero. (ix1)")
        ->example("tst @0xff, x")
        ->absx("\x00\xff");
    insert(mnem("tst", 1, "\x7d", "\xff"))
        ->help("Test for Negative or Zero. (ix)")
        ->example("tst @x")
        ->regnameind("x");

    insert(mnem("txa", 1, "\x9f", "\xff"))
        ->help("Transfer X to A.")
        ->example("txa");
    insert(mnem("wait", 1, "\x8f", "\xff"))
        ->help("Enable Interrupts and Halt CPU.")
        ->example("wait");
}





