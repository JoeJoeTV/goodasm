#include "galangh83.h"
#include "gaparameter.h"
#include "gamnemonic.h"
#include "goodasm.h"

#include <QDebug>

// Keep things tidy.
#define mnem new GAMnemonic

// H83 Constant Operand.
#define h83_const(x) insert(new GAParameterH83Const(x))

// H83 8-Bit Register.
#define regh83_8(x) insert(new GAParameterH83Reg8((x), false))
#define regh83_8ind(x) insert(new GAParameterH83Reg8((x), true))

// H83 16-Bit Register.
#define regh83_16(x) insert(new GAParameterH83Reg16((x), false))
#define regh83_16ind(x) insert(new GAParameterH83Reg16((x), true))

// H83 32-Bit Register.
#define regh83_32(x) insert(new GAParameterH83Reg32((x), false, false, false))
#define regh83_32ind(x) insert(new GAParameterH83Reg32((x), true, false, false))

// H83 32-Bit Register Indirect Access with Post-Increment/Pre-Decremenent.
#define regh83_32indinc(x) insert(new GAParameterH83Reg32((x), true, true, false))
#define regh83_32inddec(x) insert(new GAParameterH83Reg32((x), true, false, true))

/* This class represents the H83.
 *
 * Opcodes taken from here (Section 2.4):
 * https://www.renesas.com/us/en/document/mah/h8s2245-group-hardware-manual
 */

GALangH83::GALangH83() {
    endian=BIG;
    name="h83";
    maxbytes=10;

    regnames.clear();
    regnames
            << "er0"<< "er1"<< "er2"<< "er3"<< "er4"<< "er5"<< "er6"<< "er7"    // 32-bit
            << "r0"<<  "r1"<<  "r2"<<  "r3"<<  "r4"<<  "r5"<<  "r6"<<  "r7"     // 16-bit
            << "e0"<<  "e1"<<  "e2"<<  "e3"<<  "e4"<<  "e5"<<  "e6"<<  "e7"     // 16-bit
            << "r0h"<< "r1h"<< "r2h"<< "r3h"<< "r4h"<< "r5h"<< "r6h"<< "r7h"    // 8-bit
            << "r0l"<< "r1l"<< "r2l"<< "r3l"<< "r4l"<< "r5l"<< "r6l"<< "r7l"    // 8-bit
            << "pc"<< "ccr"<< "exr"<< "mach"<< "macl";                          // Special

    // TODO: threshold

    GAMnemonic *m=0;
    GAParameterGroup *pg=0;

    // ADD
    insert(mnem("add.b", 2, "\x80\x00", "\xf0\x00"))
        ->help("Add byte immediate to 8-bit register: add.b #xx:8, rd")
        ->example("add.b #0x12, r2h")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("add.b", 2, "\x08\x00", "\xff\x00"))
        ->help("Add byte in 8-bit register to 8-bit register: add.b rs, rd")
        ->example("add.b r5h, r3l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("add.w", 4, "\x79\x10\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Add word immediate to 16-bit register: add.w #xx:16, rd")
        ->example("add.w #0x1234, r5")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("add.w", 2, "\x09\x00", "\xff\x00"))
        ->help("Add word in 16-bit register to 16-bit register: add.w rs, rd")
        ->example("add.w r4, e7")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("add.l", 6, "\x7a\x10\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("Add long immediate to 32-bit register: add.l #xx:32, erd")
        ->example("add.l #0x12345678, er4")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("add.l", 2, "\x0a\x80", "\xff\x88"))
        ->help("Add long in 32-bit register to 32-bit register: add.l ers, erd")
        ->example("add.l er1, er4")
        ->regh83_32("\x00\x70")
        ->regh83_32("\x00\x07");

    // ADDS
    insert(mnem("adds", 2, "\x0b\x00", "\xff\xf8"))
        ->help("Add 1 to 32-bit register: adds #1, erd")
        ->example("adds #1, er2")
        ->h83_const(1)
        ->regh83_32("\x00\x07");
    insert(mnem("adds", 2, "\x0b\x80", "\xff\xf8"))
        ->help("Add 2 to 32-bit register: adds #2, erd")
        ->example("adds #2, er3")
        ->h83_const(2)
        ->regh83_32("\x00\x07");
    insert(mnem("adds", 2, "\x0b\x90", "\xff\xf8"))
        ->help("Add 4 to 32-bit register: adds #4, erd")
        ->example("adds #4, er4")
        ->h83_const(4)
        ->regh83_32("\x00\x07");

    // ADDX
    insert(mnem("addx", 2, "\x90\x00", "\xf0\x00"))
        ->help("Add byte with carry to 8-bit register: addx #xx:8, rd")
        ->example("addx #0x12, r2l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("addx", 2, "\x0e\x00", "\xff\x00"))
        ->help("Add byte in 8-bit register with carry to 8-bit register: addx rs, rd")
        ->example("addx r6h, r2l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");

    // AND
    insert(mnem("and.b", 2, "\xe0\x00", "\xf0\x00"))
        ->help("AND 8-bit register with byte immediate: and.b #xx:8, rd")
        ->example("and.b #0x12, r3l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("and.b", 2, "\x16\x00", "\xff\x00"))
        ->help("AND 8-bit register with 8-bit register: and.b rs, rd")
        ->example("and.b r5h, r3l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("and.w", 4, "\x79\x60\x00\x00", "\xff\xf0\x00\x00"))
        ->help("AND 16-bit register with word immediate: and.w #xx:16, rd")
        ->example("and.w #0x1234, e4")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("and.w", 2, "\x66\x00", "\xff\x00"))
        ->help("AND 16-bit register with 16-bit register: and.w rs, rd")
        ->example("and.w r2, e7")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("and.l", 6, "\x7a\x60\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("AND 32-bit register with long immediate: and.l #xx:32, erd")
        ->example("and.l #0x12345678, er2")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("and.l", 4, "\x01\xf0\x66\x00", "\xff\xff\xff\x88"))
        ->help("AND 32-bit register with 32-bit register: and.l rs, rd")
        ->example("and.l er2, er3")
        ->regh83_32("\x00\x00\x00\x70")
        ->regh83_32("\x00\x00\x00\x07");

    // ANDC
    insert(mnem("andc", 2, "\x06\x00", "\xff\x00"))
        ->help("AND CCR register with byte immediate: andc #xx:8, ccr")
        ->example("andc #0x12, ccr")
        ->imm("\x00\xff")
        ->regname("ccr");
    insert(mnem("andc", 4, "\x01\x41\x06\x00", "\xff\xff\xff\x00"))
        ->help("AND EXR register with byte immediate: andc #xx:8, exr")
        ->example("andc #0x12, exr")
        ->imm("\x00\x00\x00\xff")
        ->regname("exr");

    // BAND
    insert(mnem("band", 2, "\x76\x00", "\xff\x80"))
        ->help("AND Carry with bit in 16-bit register: band #xx:3, rd")
        ->example("band #6, r4")
        ->imm("\x00\x70")
        ->regh83_16("\x00\x0f");
    insert(mnem("band", 4, "\x7c\x00\x76\x00", "\xff\x8f\xff\x8f"))
        ->help("AND Carry with bit in address stored in 32-bit register: band #xx:3, @erd")
        ->example("band #1, @er1")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("band", 4, "\x7e\x00\x76\x00", "\xff\x00\xff\x8f"))
        ->help("AND Carry with bit at address given by 8-bit absolute: band #xx:3, @aa:8")
        ->example("band #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("band", 6, "\x6a\x10\x00\x00\x76\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("AND Carry with bit at address given by 16-bit absolute: band #xx:3, @aa:16")
        ->example("band #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("band", 8, "\x6a\x30\x00\x00\x00\x00\x76\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("AND Carry with bit at address given by 32-bit absolute: band #xx:3, @aa:32")
        ->example("band #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BCC
    insert(mnem("bra", 2, "\x40\x00", "\xff\x00"))
        ->help("Branch Always to 8-bit displacement: bra d:8")
        ->example("bra #0x12")
        ->imm("\x00\xff");
    insert(mnem("bra", 4, "\x58\x00\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch Always to 16-bit displacement: bra d:16")
        ->example("bra #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("brn", 2, "\x41\x00", "\xff\x00"))
        ->help("Branch Never to 8-bit displacement: brn d:8")
        ->example("brn #0x12")
        ->imm("\x00\xff");
    insert(mnem("brn", 4, "\x58\x10\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch Never 16-bit displacement: brn d:16")
        ->example("brn #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bhi", 2, "\x42\x00", "\xff\x00"))
        ->help("Branch If High (Carry OR Zero == 0) to 8-bit displacement: bhi d:8")
        ->example("bhi #0x12")
        ->imm("\x00\xff");
    insert(mnem("bhi", 4, "\x58\x20\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If High (Carry OR Zero == 0) 16-bit displacement: bhi d:16")
        ->example("bhi #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bls", 2, "\x43\x00", "\xff\x00"))
        ->help("Branch If Low or Same (Carry OR Zero == 1) to 8-bit displacement: bls d:8")
        ->example("bls #0x12")
        ->imm("\x00\xff");
    insert(mnem("bls", 4, "\x58\x30\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Low or Same (Carry OR Zero == 0) to 16-bit displacement: bls d:16")
        ->example("bls #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bcc", 2, "\x44\x00", "\xff\x00"))
        ->help("Branch If Carry Clear (Carry == 0) to 8-bit displacement: bcc d:8")
        ->example("bcc #0x12")
        ->imm("\x00\xff");
    insert(mnem("bcc", 4, "\x58\x40\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Carry Clear (Carry == 0) to 16-bit displacement: bcc d:16")
        ->example("bcc #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bcs", 2, "\x45\x00", "\xff\x00"))
        ->help("Branch If Carry Set (Carry == 1) to 8-bit displacement: bcs d:8")
        ->example("bcs #0x12")
        ->imm("\x00\xff");
    insert(mnem("bcs", 4, "\x58\x50\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Carry Set (Carry == 1) to 16-bit displacement: bcs d:16")
        ->example("bcs #0x1234")
        ->imm("\x00\x00\xff\xff");

    insert(mnem("bne", 2, "\x46\x00", "\xff\x00"))
        ->help("Branch If Not Equal (Zero == 0) to 8-bit displacement: bne d:8")
        ->example("bne #0x12")
        ->imm("\x00\xff");
    insert(mnem("bne", 4, "\x58\x60\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Not Equal (Zero == 0) to 16-bit displacement: bne d:16")
        ->example("bne #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("beq", 2, "\x47\x00", "\xff\x00"))
        ->help("Branch If Equal (Zero == 1) to 8-bit displacement: beq d:8")
        ->example("beq #0x12")
        ->imm("\x00\xff");
    insert(mnem("beq", 4, "\x58\x70\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Equal (Zero == 1) to 16-bit displacement: beq d:16")
        ->example("beq #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bvc", 2, "\x48\x00", "\xff\x00"))
        ->help("Branch If Overflow Clear (Overflow == 0) to 8-bit displacement: bvc d:8")
        ->example("bvc #0x12")
        ->imm("\x00\xff");
    insert(mnem("bvc", 4, "\x58\x80\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Overflow Clear (Overflow == 0) to 16-bit displacement: bvc d:16")
        ->example("bvc #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bvs", 2, "\x49\x00", "\xff\x00"))
        ->help("Branch If Overflow Set (Overflow == 1) to 8-bit displacement: bvs d:8")
        ->example("bvs #0x12")
        ->imm("\x00\xff");
    insert(mnem("bvs", 4, "\x58\x90\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Overflow Set (Overflow == 1) to 16-bit displacement: bvs d:16")
        ->example("bvs #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bpl", 2, "\x4a\x00", "\xff\x00"))
        ->help("Branch If Plus (Negative == 0) to 8-bit displacement: bpl d:8")
        ->example("bpl #0x12")
        ->imm("\x00\xff");
    insert(mnem("bpl", 4, "\x58\xa0\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Plus (Negative == 0) to 16-bit displacement: bpl d:16")
        ->example("bpl #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bmi", 2, "\x4b\x00", "\xff\x00"))
        ->help("Branch If Minus (Negative == 0) to 8-bit displacement: bmi d:8")
        ->example("bmi #0x12")
        ->imm("\x00\xff");
    insert(mnem("bmi", 4, "\x58\xb0\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Minus (Negative == 0) to 16-bit displacement: bmi d:16")
        ->example("bmi #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bge", 2, "\x4c\x00", "\xff\x00"))
        ->help("Branch If Greater Or Equal (Negative XOR Overflow == 0) to 8-bit displacement: bge d:8")
        ->example("bge #0x12")
        ->imm("\x00\xff");
    insert(mnem("bge", 4, "\x58\x5c\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Greater Or Equal (Negative XOR Overflow == 0) to 16-bit displacement: bge d:16")
        ->example("bge #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("blt", 2, "\x4d\x00", "\xff\x00"))
        ->help("Branch If Less Than (Negative XOR Overflow == 1) to 8-bit displacement: blt d:8")
        ->example("blt #0x12")
        ->imm("\x00\xff");
    insert(mnem("blt", 4, "\x58\xd0\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Less Than (Negative XOR Overflow == 1) to 16-bit displacement: blt d:16")
        ->example("blt #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("bgt", 2, "\x4e\x00", "\xff\x00"))
        ->help("Branch If Greater Than (Zero OR (Negative XOR Overflow) == 0) to 8-bit displacement: bgt d:8")
        ->example("bgt #0x12")
        ->imm("\x00\xff");
    insert(mnem("bgt", 4, "\x58\xe0\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Greater Than (Zero OR (Negative XOR Overflow) == 0) to 16-bit displacement: bgt d:16")
        ->example("bgt #0x1234")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("ble", 2, "\x4f\x00", "\xff\x00"))
        ->help("Branch If Less Than Or Equal (Zero OR (Negative XOR Overflow) == 1) to 8-bit displacement: ble d:8")
        ->example("ble #0x12")
        ->imm("\x00\xff");
    insert(mnem("ble", 4, "\x58\xf0\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch If Less Than Or Equal (Zero OR (Negative XOR Overflow) == 1) to 16-bit displacement: ble d:16")
        ->example("ble #0x1234")
        ->imm("\x00\x00\xff\xff");

    // BCLR
    insert(mnem("bclr", 2, "\x72\x00", "\xff\x80"))
        ->help("Clear bit specified in 8-bit register: bclr #xx:3, rd")
        ->example("bclr #4, r5l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bclr", 4, "\x7d\x00\x72\x00", "\xff\x8f\xff\x8f"))
        ->help("Clear bit specified at address given by 32-bit register: bclr #xx:3, @erd")
        ->example("bclr #1, @er2")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bclr", 4, "\x7f\x00\x72\x00", "\xff\x00\xff\x8f"))
        ->help("Clear bit specified at address given by 8-bit absolute: clr #xx:3, @aa:8")
        ->example("bclr #5, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bclr", 6, "\x6a\x18\x00\x00\x72\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Clear bit specified at address given by 16-bit absolute: clr #xx:3, @aa:16")
        ->example("bclr #5, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bclr", 8, "\x6a\x38\x00\x00\x00\x00\x72\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Clear bit specified at address given by 32-bit absolute: bclr #xx:3, @aa:32")
        ->example("bclr #5, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");
    insert(mnem("bclr", 2, "\x62\x00", "\xff\x00"))
        ->help("Clear bit specified by 8-bit register in an 8-bit register: bclr rn, rd")
        ->example("bclr r2l, r0h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("bclr", 4, "\x7d\x00\x62\x00", "\xff\x8f\xff\x0f"))
        ->help("Clear bit specified by 8-bit register in 32-bit register: bclr rn, @erd")
        ->example("bclr r3l, @er6")
        ->regh83_8("\x00\x70\x00\x00")
        ->regh83_32ind("\x00\x00\x00\xf0");
    insert(mnem("bclr", 4, "\x7f\x00\x62\x00", "\xff\x00\xff\x0f"))
        ->help("Clear bit specified by 8-bit register at address given by 8-bit absolute: bclr rn, @aa:8")
        ->example("bclr r0h, @0x12")
        ->regh83_8("\x00\x00\x00\xf0")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bclr", 6, "\x6a\x18\x00\x00\x62\x00", "\xff\xff\x00\x00\xff\x0f"))
        ->help("Clear bit specified by 8-bit register at address given by 16-bit absolute: bclr rn, @aa:16")
        ->example("bclr r0h, @0x1234")
        ->regh83_8("\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bclr", 8, "\x6a\x38\x00\x00\x00\x00\x62\x00", "\xff\xff\x00\x00\x00\x00\xff\x0f"))
        ->help("Clear bit specified by 8-bit register at address given by 32-bit absolute: bclr rn, @aa:32")
        ->example("bclr r0h, @0x12345678")
        ->regh83_8("\x00\x00\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BIAND
    insert(mnem("biand", 2, "\x76\x80", "\xff\x80"))
        ->help("AND Carry with inverse a specified bit in an 8-bit register: biand #xx:3, rd")
        ->example("biand #1, r2l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("biand", 4, "\x7c\x00\x76\x80", "\xff\x8f\xff\x8f"))
        ->help("AND Carry with inverse of a specified bit at address given by 32-bit register: biand #xx:3, @erd")
        ->example("biand #2, @er1")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("biand", 4, "\x7e\x00\x76\x80", "\xff\x00\xff\x8f"))
        ->help("AND Carry with inverse of a specified bit at address given by 8-bit absolute: biand #xx:3, @aa:8")
        ->example("biand #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("biand", 6, "\x6a\x10\x00\x00\x76\x80", "\xff\xff\x00\x00\xff\x8f"))
        ->help("AND Carry with inverse of a specified bit at address given by 16-bit absolute: biand #xx:3, @aa:16")
        ->example("biand #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("biand", 8, "\x6a\x30\x00\x00\x00\x00\x76\x80", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("AND Carry with inverse of a specified bit at address given by 32-bit absolute: biand #xx:3, @aa:32")
        ->example("biand #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BILD
    insert(mnem("bild", 2, "\x77\x80", "\xff\x80"))
        ->help("Set Carry to inverse of a specified bit in an 8-bit register: bild #xx:3, rd")
        ->example("bild #1, r3l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bild", 4, "\x7c\x00\x77\x80", "\xff\x8f\xff\x8f"))
        ->help("Set Carry to inverse of a specified bit at address given by 32-bit register: bild #xx:3, @erd")
        ->example("bild #2, @er4")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bild", 4, "\x7e\x00\x77\x80", "\xff\x00\xff\x8f"))
        ->help("Set Carry to inverse of a specified bit at address given by 8-bit absolute: bild #xx:3, @aa:8")
        ->example("bild #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bild", 6, "\x6a\x10\x00\x00\x77\x80", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Set Carry to inverse of a specified bit at address given by 16-bit absolute: bild #xx:3, @aa:16")
        ->example("bild #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bild", 8, "\x6a\x30\x00\x00\x00\x00\x77\x80", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Set Carry to inverse of a specified bit at address given by 32-bit absolute: bild #xx:3, @aa:32")
        ->example("bild #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BIOR
    insert(mnem("bior", 2, "\x74\x80", "\xff\x80"))
        ->help("OR Carry with inverse a specified bit in an 8-bit register: bior #xx:3, rd")
        ->example("bior #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bior", 4, "\x7c\x00\x74\x80", "\xff\x8f\xff\x8f"))
        ->help("OR Carry with inverse of a specified bit at address given by 32-bit register: bior #xx:3, @erd")
        ->example("bior #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bior", 4, "\x7e\x00\x74\x80", "\xff\x00\xff\x8f"))
        ->help("OR Carry with inverse of a specified bit in 8-bit absolute: bior #xx:3, @aa:8")
        ->example("bior #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bior", 6, "\x6a\x10\x00\x00\x74\x80", "\xff\xff\x00\x00\xff\x8f"))
        ->help("OR Carry with inverse of a specified bit in 16-bit absolute: bior #xx:3, @aa:16")
        ->example("bior #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bior", 8, "\x6a\x30\x00\x00\x00\x00\x74\x80", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("OR Carry with inverse of a specified bit in 32-bit absolute: bior #xx:3, @aa:32")
        ->example("bior #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BIST
    insert(mnem("bist", 2, "\x67\x80", "\xff\x80"))
        ->help("Set 8-bit register to inverse of a specified bit in Carry: bist #xx:3, rd")
        ->help("bist #xx:3, rd")
        ->example("bist #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bist", 4, "\x7d\x00\x67\x80", "\xff\x8f\xff\x8f"))
        ->help("Set memory given by 32-bit register to inverse of a specified bit in Carry: bist #xx:3, @erd")
        ->help("bist #xx:3, @erd")
        ->example("bist #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bist", 4, "\x7f\x00\x67\x80", "\xff\x00\xff\x8f"))
        ->help("Set memory given by 8-bit absolute to inverse of a specified bit in Clear: bist #xx:3, @aa:8")
        ->help("bist #xx:3, @aa:8")
        ->example("bist #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bist", 6, "\x6a\x18\x00\x00\x67\x80", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Set memory given by 16-bit absolute to inverse of a specified bit in Clear: bist #xx:3, @aa:16")
        ->example("bist #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bist", 8, "\x6a\x38\x00\x00\x00\x00\x67\x80", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Set memory given by 32-bit absolute to inverse of a specified bit in Clear: bist #xx:3, @aa:32")
        ->example("bist #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BIXOR
    insert(mnem("bixor", 2, "\x75\x80", "\xff\x80"))
        ->help("XOR Carry with inverse of a specified bit in an 8-bit register: bixor #xx:3, rd")
        ->example("bixor #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bixor", 4, "\x7c\x00\x75\x80", "\xff\x8f\xff\x8f"))
        ->help("XOR Carry with inverse of a specified bit at address given by 32-bit register: bixor #xx:3, @erd")
        ->example("bixor #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bixor", 4, "\x7e\x00\x75\x80", "\xff\x00\xff\x8f"))
        ->help("XOR Carry with inverse of a specified bit in 8-bit absolute: bixor #xx:3, @aa:8")
        ->example("bixor #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bixor", 6, "\x6a\x10\x00\x00\x75\x80", "\xff\xff\x00\x00\xff\x8f"))
        ->help("XOR Carry with inverse of a specified bit in 16-bit absolute: bixor #xx:3, @aa:16")
        ->example("bixor #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bixor", 8, "\x6a\x10\x00\x00\x00\x00\x75\x80", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("XOR Carry with inverse of a specified bit in 32-bit absolute: bixor #xx:3, @aa:32")
        ->example("bixor #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BLD
    insert(mnem("bld", 2, "\x77\x00", "\xff\x80"))
        ->help("Set Carry to a specified bit in an 8-bit register: bld #xx:3, rd")
        ->example("bld #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bld", 4, "\x7c\x00\x77\x00", "\xff\x8f\xff\x8f"))
        ->help("Set Carry to a specified bit at address given by 32-bit register: bld #xx:3, @erd")
        ->example("bld #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bld", 4, "\x7e\x00\x77\x00", "\xff\x00\xff\x8f"))
        ->help("Set Carry to a specified bit at address given by 8-bit absolute: bld #xx:3, @aa:8")
        ->example("bld #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bld", 6, "\x6a\x10\x00\x00\x77\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Set Carry to a specified bit at address given by 16-bit absolute: bld #xx:3, @aa:16")
        ->example("bld #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bld", 8, "\x6a\x10\x00\x00\x00\x00\x77\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Set Carry to a specified bit at address given by 32-bit absolute: bld #xx:3, @aa:32")
        ->example("bld #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BNOT
    insert(mnem("bnot", 2, "\x71\x00", "\xff\x80"))
        ->help("Invert a specified bit in an 8-bit register: bnot #xx:3, rd")
        ->example("bnot #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bnot", 4, "\x7d\x00\x71\x00", "\xff\x8f\xff\x8f"))
        ->help("Invert a specified bit at address given by 32-bit register: bnot #xx:3, @erd")
        ->example("bnot #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bnot", 4, "\x7f\x00\x71\x00", "\xff\x00\xff\x8f"))
        ->help("Invert a specified bit at address given by 8-bit absolute: bnot #xx:3, @aa:8")
        ->example("bnot #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bnot", 6, "\x6a\x18\x00\x00\x71\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Invert a specified bit at address given by 16-bit absolute: bnot #xx:3, @aa:16")
        ->example("bnot #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bnot", 8, "\x6a\x38\x00\x00\x00\x00\x71\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Invert a specified bit at address given by 32-bit absolute: bnot #xx:3, @aa:32")
        ->example("bnot #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");
    insert(mnem("bnot", 2, "\x61\x00", "\xff\x00"))
        ->help("Invert a bit given by an 8-bit register in an 8-bit register: bnot rn, rd")
        ->example("bnot r2l, r3h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("bnot", 4, "\x7d\x00\x61\x00", "\xff\x8f\xff\x0f"))
        ->help("Invert a bit given by an 8-bit register at address given by 32-bit register: bnot rn, @erd")
        ->help("bnot rn, @erd")
        ->example("bnot r2l, @er4")
        ->regh83_8("\x00\x00\x00\xf0")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bnot", 4, "\x7f\x00\x61\x00", "\xff\x00\xff\x0f"))
        ->help("Invert a bit given by an 8-bit register at address given by 8-bit absolute: bnot rn, @aa:8")
        ->example("bnot r2l, @0x12")
        ->regh83_8("\x00\x00\x00\xf0")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bnot", 6, "\x6a\x18\x00\x00\x61\x00", "\xff\xff\x00\x00\xff\x0f"))
        ->help("Invert a bit given by an 8-bit register at address given by 16-bit absolute: bnot rn, @aa:16")
        ->example("bnot r3h, @0x1234")
        ->regh83_8("\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bnot", 8, "\x6a\x38\x00\x00\x00\x00\x61\x00", "\xff\xff\x00\x00\x00\x00\xff\x0f"))
        ->help("Invert a bit given by an 8-bit register at address given by 32-bit absolute: bnot rn, @aa:32")
        ->example("bnot r4l, @0x12345678")
        ->regh83_8("\x00\x00\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BOR
    insert(mnem("bor", 2, "\x74\x00", "\xff\x80"))
        ->help("OR Carry with a specified bit in an 8-bit register: bor #xx:3, rd")
        ->example("bor #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bor", 4, "\x7c\x00\x74\x00", "\xff\x8f\xff\x8f"))
        ->help("OR Carry with a specified bit at address given by 32-bit register: bor #xx:3, @erd")
        ->example("bor #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bor", 4, "\x7e\x00\x74\x00", "\xff\x00\xff\x8f"))
        ->help("OR Carry with a specified bit in 8-bit absolute: bor #xx:3, @aa:8")
        ->example("bor #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bor", 6, "\x6a\x10\x00\x00\x74\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("OR Carry with a specified bit in 16-bit absolute: bor #xx:3, @aa:16")
        ->example("bor #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bor", 8, "\x6a\x30\x00\x00\x00\x00\x74\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("OR Carry with a specified bit in 32-bit absolute: bor #xx:3, @aa:32")
        ->example("bor #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BSET
    insert(mnem("bset", 2, "\x70\x00", "\xff\x80"))
        ->help("Set a specified bit in an 8-bit register: bset #xx:3, rd")
        ->example("bset #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bset", 4, "\x7d\x00\x70\x00", "\xff\x8f\xff\x8f"))
        ->help("Set a specified bit at address given by 32-bit register: bset #xx:3, @erd")
        ->example("bset #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bset", 4, "\x7f\x00\x70\x00", "\xff\x00\xff\x8f"))
        ->help("Set a specified bit at address given by 8-bit absolute: bset #xx:3, @aa:8")
        ->example("bset #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bset", 6, "\x6a\x18\x00\x00\x70\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Set a specified bit at address given by 16-bit absolute: bset #xx:3, @aa:16")
        ->example("bset #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bset", 8, "\x6a\x38\x00\x00\x00\x00\x70\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Set a specified bit at address given by 32-bit absolute: bset #xx:3, @aa:32")
        ->example("bset #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");
    insert(mnem("bset", 2, "\x60\x00", "\xff\x00"))
        ->help("Set a bit given by an 8-bit register in an 8-bit register: bset rn, rd")
        ->example("bset r2l, r3h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("bset", 4, "\x7d\x00\x60\x00", "\xff\x8f\xff\x0f"))
        ->help("Set a bit given by an 8-bit register at address given by 32-bit register: bset rn, @erd")
        ->example("bset r2l, @er4")
        ->regh83_8("\x00\x00\x00\xf0")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bset", 4, "\x7f\x00\x60\x00", "\xff\x00\xff\x0f"))
        ->help("Set a bit given by an 8-bit register at address given by 8-bit absolute: bset rn, @aa:8")
        ->example("bset r2l, @0x12")
        ->regh83_8("\x00\x00\x00\xf0")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bset", 6, "\x6a\x18\x00\x00\x60\x00", "\xff\xff\x00\x00\xff\x0f"))
        ->help("Set a bit given by an 8-bit register at address given by 16-bit absolute: bset rn, @aa:16")
        ->example("bset r2l, @0x1234")
        ->regh83_8("\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bset", 8, "\x6a\x38\x00\x00\x00\x00\x60\x00", "\xff\xff\x00\x00\x00\x00\xff\x0f"))
        ->help("Set a bit given by an 8-bit register at address given by 32-bit absolute: bset rn, @aa:32")
        ->example("bset r2l, @0x12345678")
        ->regh83_8("\x00\x00\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BSR
    insert(mnem("bsr", 2, "\x55\x00", "\xff\x00"))
        ->help("Branch To Subroutine at 8-bit displacement: bsr d:8")
        ->example("bsr #0x12")
        ->imm("\x00\xff");
    insert(mnem("bsr", 4, "\x5c\x00\x00\x00", "\xff\xff\x00\x00"))
        ->help("Branch To Subroutine at 16-bit displacement: bsr d:16")
        ->example("bsr #0x1234")
        ->imm("\x00\x00\xff\xff");

    // BST
    insert(mnem("bst", 2, "\x67\x00", "\xff\x80"))
        ->help("Set 8-bit register to a specified bit in Carry: bst #xx:3, rd")
        ->example("bst #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bst", 4, "\x7d\x00\x67\x00", "\xff\x8f\xff\x8f"))
        ->help("Set memory given by 32-bit register to a specified bit in Carry: bst #xx:3, @erd")
        ->example("bst #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bst", 4, "\x7f\x00\x67\x00", "\xff\x00\xff\x8f"))
        ->help("Set memory given by 8-bit absolute to a specified bit in Clear: bst #xx:3, @aa:8")
        ->example("bst #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bst", 6, "\x6a\x18\x00\x00\x67\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Set memory given by 16-bit absolute to a specified bit in Clear: bst #xx:3, @aa:16")
        ->example("bst #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bst", 8, "\x6a\x38\x00\x00\x00\x00\x67\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Set memory given by 32-bit absolute to a specified bit in Clear: bst #xx:3, @aa:32")
        ->help("bst #xx:3, @aa:32")
        ->example("bst #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BTST
    insert(mnem("btst", 2, "\x73\x00", "\xff\x80"))
        ->help("Test a specified bit in an 8-bit register: btst #xx:3, rd")
        ->example("btst #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("btst", 4, "\x7c\x00\x73\x00", "\xff\x8f\xff\x8f"))
        ->help("Test a specified bit at address given by 32-bit register: btst #xx:3, @erd")
        ->help("btst #xx:3, @erd")
        ->example("btst #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("btst", 4, "\x7e\x00\x73\x00", "\xff\x00\xff\x8f"))
        ->help("Test a specified bit at address given by 8-bit absolute: btst #xx:3, @aa:8")
        ->example("btst #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("btst", 6, "\x6a\x10\x00\x00\x73\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("Test a specified bit at address given by 16-bit absolute: btst #xx:3, @aa:16")
        ->example("btst #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("btst", 8, "\x6a\x30\x00\x00\x00\x00\x73\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("Test a specified bit at address given by 32-bit absolute: btst #xx:3, @aa:32")
        ->example("btst #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");
    insert(mnem("btst", 2, "\x63\x00", "\xff\x00"))
        ->help("Test a bit given by an 8-bit register in an 8-bit register: btst rn, rd")
        ->example("btst r2l, r3h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("btst", 4, "\x7c\x00\x63\x00", "\xff\x8f\xff\x0f"))
        ->help("Test a bit given by an 8-bit register at address given by 32-bit register: btst rn, @erd")
        ->example("btst r2l, @er4")
        ->regh83_8("\x00\x00\x00\xf0")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("btst", 4, "\x7e\x00\x63\x00", "\xff\x00\xff\x0f"))
        ->help("Test a bit given by an 8-bit register at address given by 8-bit absolute: btst rn, @aa:8")
        ->example("btst r2l, @0x12")
        ->regh83_8("\x00\x00\x00\xf0")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("btst", 6, "\x6a\x10\x00\x00\x63\x00", "\xff\xff\x00\x00\xff\x0f"))
        ->help("Test a bit given by an 8-bit register at address given by 16-bit absolute: btst rn, @aa:16")
        ->example("btst r2l, @0x1234")
        ->regh83_8("\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("btst", 8, "\x6a\x30\x00\x00\x00\x00\x63\x00", "\xff\xff\x00\x00\x00\x00\xff\x0f"))
        ->help("Test a bit given by an 8-bit register at address given by 32-bit absolute: btst rn, @aa:32")
        ->example("btst r2l, @0x12345678")
        ->regh83_8("\x00\x00\x00\x00\x00\x00\x00\xf0")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // BXOR
    insert(mnem("bxor", 2, "\x75\x00", "\xff\x80"))
        ->help("XOR Carry with a specified bit in an 8-bit register: bxor #xx:3, rd")
        ->example("bxor #1, r7l")
        ->imm("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("bxor", 4, "\x7c\x00\x75\x00", "\xff\x8f\xff\x8f"))
        ->help("XOR Carry with a specified bit at address given by 32-bit register: bxor #xx:3, @erd")
        ->example("bxor #2, @er0")
        ->imm("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x70\x00\x00");
    insert(mnem("bxor", 4, "\x7e\x00\x75\x00", "\xff\x00\xff\x8f"))
        ->help("XOR Carry with a specified bit in 8-bit absolute: bxor #xx:3, @aa:8")
        ->example("bxor #3, @0x12")
        ->imm("\x00\x00\x00\x70")
        ->abs("\x00\xff\x00\x00");
    insert(mnem("bxor", 6, "\x6a\x10\x00\x00\x75\x00", "\xff\xff\x00\x00\xff\x8f"))
        ->help("XOR Carry with a specified bit in 16-bit absolute: bxor #xx:3, @aa:16")
        ->example("bxor #3, @0x1234")
        ->imm("\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\x00\x00");
    insert(mnem("bxor", 8, "\x6a\x30\x00\x00\x00\x00\x75\x00", "\xff\xff\x00\x00\x00\x00\xff\x8f"))
        ->help("XOR Carry with a specified bit in 32-bit absolute: bxor #xx:3, @aa:32")
        ->example("bxor #3, @0x12345678")
        ->imm("\x00\x00\x00\x00\x00\x00\x00\x70")
        ->abs("\x00\x00\xff\xff\xff\xff\x00\x00");

    // CLRMAC
    insert(mnem("clrmac", 2, "\x01\xa0", "\xff\xff"))
        ->help("Sets MAC to 0: clrmac")
        ->example("clrmac");

    // CMP
    insert(mnem("cmp.b", 2, "\xa0\x00", "\xf0\x00"))
        ->help("Compare byte immediate with 8-bit register: cmp.b #xx:8, rd")
        ->example("cmp.b #12, r4l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("cmp.b", 2, "\x1c\x00", "\xff\x00"))
        ->help("Compare 8-bit register with 8-bit register: cmp.b rs, rd")
        ->example("cmp.b r0h, r4l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("cmp.w", 4, "\x79\x20\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Compare word immediate with 16-bit register: cmp.w #xx:16, rd")
        ->example("cmp.w #0x1234, r6")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("cmp.w", 2, "\x1d\x00", "\xff\x00"))
        ->help("Compare 16-bit register with 16-bit register: cmp.w rs, rd")
        ->example("cmp.w e2, r6")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("cmp.l", 6, "\x7a\x20\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("Compare long immediate with 32-bit register: cmp.l #xx:32, erd")
        ->example("cmp.l #0x12345678, er3")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("cmp.l", 2, "\x1f\x80", "\xff\x88"))
        ->help("Compare 32-bit register with 32-bit register: cmp.l ers, erd")
        ->example("cmp.l er2, er5")
        ->regh83_32("\x00\x70")
        ->regh83_32("\x00\x07");

    // DAA
    insert(mnem("daa", 2, "\x0f\x00", "\xff\xf0"))
        ->help("Decimal Adjust Add to BCD 8-bit register: daa rd")
        ->example("daa r5l")
        ->regh83_8("\x00\x0f");

    // DAS
    insert(mnem("das", 2, "\x1f\x00", "\xff\xf0"))
        ->help("Decimal Adjust Subtract to BCD 8-bit register: das rd")
        ->example("das r5l")
        ->regh83_8("\x00\x0f");

    // DEC
    insert(mnem("dec.b", 2, "\x1a\x00", "\xff\xf0"))
        ->help("Decrement 8-bit register by 1: dec.b rd")
        ->example("dec.b r0l")
        ->regh83_8("\x00\x0f");
     insert(mnem("dec.w", 2, "\x1b\x50", "\xff\xf0"))
        ->help("Decrement 16-bit register by 1: dec.w #1, rd")
        ->example("dec.w #1, r2")
        ->h83_const(1)
        ->regh83_16("\x00\x0f");
      insert(mnem("dec.w", 2, "\x1b\xd0", "\xff\xf0"))
        ->help("Decrement 16-bit register by 2: dec.w #2, rd")
        ->example("dec.w #2, r4")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
      insert(mnem("dec.l", 2, "\x1b\x70", "\xff\xf8"))
        ->help("Decrement 32-bit register by 1: dec.l #1, erd")
        ->example("dec.l #1, er2")
        ->h83_const(1)
        ->regh83_32("\x00\x07");
      insert(mnem("dec.l", 2, "\x1b\xf0", "\xff\xf8"))
        ->help("Decrement 32-bit register by 2: dec.l #2, erd")
        ->example("dec.l #2, er4")
        ->h83_const(2)
        ->regh83_32("\x00\x07");
 
    // DIVXS
    insert(mnem("divxs.b", 4, "\x01\xd0\x51\x00", "\xff\xff\xff\x00"))
        ->help("Signed division of 8-bit register by 8-bit register: divxs.b rs, rd")
        ->example("divxs.b r0l, r1h")
        ->regh83_8("\x00\x00\x00\xf0")
        ->regh83_8("\x00\x00\x00\x0f");
    insert(mnem("divxs.w", 4, "\x01\xd0\x53\x00", "\xff\xff\xff\x08"))
        ->help("Signed division of 16-bit register by 16-bit register: divxs.w rs, rd")
        ->example("divxs.w r4, e0")
        ->regh83_16("\x00\x00\x00\xf0")
        ->regh83_16("\x00\x00\x00\x07");

    // DIVXU
    insert(mnem("divxu.b", 2, "\x51\x00", "\xff\x00"))
        ->help("Unsigned division of 8-bit register by 8-bit register: divxu.b rs, rd")
        ->example("divxu.b r0l, r1h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("divxu.w", 2, "\x53\x00", "\xff\x08"))
        ->help("Unsigned division of 16-bit register by 16-bit register: divxu.w rs, rd")
        ->example("divxu.w r4, e0")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x07");

    // EEPMOV
    insert(mnem("eepmov.b", 4, "\x7b\x5c\x59\x8f", "\xff\xff\xff\xff"))
        ->help("Move byte from address in ER5 to address in ER6, decrement R4L, repeat until R4L == 0: eepmov.b")
        ->example("eepmov.b");
    insert(mnem("eepmov.w", 4, "\x7b\xd4\x59\x8f", "\xff\xff\xff\xff"))
        ->help("Move byte from address in ER5 to address in ER6, decrement R4, repeat until R4 == 0: eepmov.w")
        ->example("eepmov.w");

    // EXTS
    insert(mnem("exts.w", 2, "\x17\xd0", "\xff\xf0"))
        ->help("Sign extend lower 8-bits of 16-bit register: exts.w rd")
        ->example("exts.w e3")
        ->regh83_16("\x00\x0f");
    insert(mnem("exts.l", 2, "\x17\xf0", "\xff\xf8"))
        ->help("Sign extend lower 16-bits of 32-bit register: exts.l erd")
        ->example("exts.l er2")
        ->regh83_32("\x00\x07");

    // EXTU
    insert(mnem("extu.w", 2, "\x17\x50", "\xff\xf0"))
        ->help("Zero extend lower 8-bits of 16-bit register: extu.w rd")
        ->example("extu.w e3")
        ->regh83_16("\x00\x0f");
    insert(mnem("extu.l", 2, "\x17\x70", "\xff\xf8"))
        ->help("Zero extend lower 16-bits of 32-bit register: extu.l erd")
        ->example("extu.l er2")
        ->regh83_32("\x00\x07");

    // INC
    insert(mnem("inc.b", 2, "\x0a\x00", "\xff\xf0"))
        ->help("Increment 8-bit register by 1: inc.b rd")
        ->example("inc.b r2l")
        ->regh83_8("\x00\x0f");
     insert(mnem("inc.w", 2, "\x0b\x50", "\xff\xf0"))
        ->help("Increment 16-bit register by 1: inc.w #1, rd")
        ->example("inc.w #1, r3")
        ->h83_const(1)
        ->regh83_16("\x00\x0f");
      insert(mnem("inc.w", 2, "\x0b\xd0", "\xff\xf0"))
        ->help("Increment 16-bit register by 2: inc.w #2, rd")
        ->example("inc.w #2, r6")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
      insert(mnem("inc.l", 2, "\x0b\x70", "\xff\xf8"))
        ->help("Increment 32-bit register by 1: inc.l #1, erd")
        ->example("inc.l #1, er3")
        ->h83_const(1)
        ->regh83_32("\x00\x07");
      insert(mnem("inc.l", 2, "\x0b\xf0", "\xff\xf8"))
        ->help("Increment 32-bit register by 2: inc.l #2, erd")
        ->example("inc.l #2, er6")
        ->h83_const(2)
        ->regh83_32("\x00\x07");
 
    // JMP
    insert(mnem("jmp", 2, "\x59\x00", "\xff\x8f"))
        ->help("Branch unconditionally to address in 32-bit register: jmp @ern")
        ->example("jmp @er3")
        ->regh83_32ind("\x00\x70");
    insert(mnem("jmp", 4, "\x5a\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Branch unconditionally to 24-bit absolute: jmp @aa:24")
        ->example("jmp @0x123456")
        ->abs("\x00\xff\xff\xff");
    insert(mnem("jmp", 2, "\x5b\x00", "\xff\x00"))
        ->help("Branch unconditionally to 8-bit absolute: jmp @aa:8")
        ->example("jmp @0x12")
        ->abs("\x00\xff");

    // JSR
    insert(mnem("jsr", 2, "\x5d\x00", "\xff\x8f"))
        ->help("Branch to Subroutine at address in 32-bit register: jsr @ern")
        ->example("jsr @er3")
        ->regh83_32ind("\x00\x70");
    insert(mnem("jsr", 4, "\x5e\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Branch to Subroutine at 24-bit absolute: jsr @aa:24")
        ->example("jsr @0x123456")
        ->abs("\x00\xff\xff\xff");
    insert(mnem("jsr", 2, "\x5f\x00", "\xff\x00"))
        ->help("Branch to Subroutine at 8-bit absolute: jsr @aa:8")
        ->example("jsr @0x12")
        ->abs("\x00\xff");

    // LDC
    insert(mnem("ldc", 2, "\x07\x00", "\xff\x00"))
        ->help("Set CCR to byte immediate: ldc #xx:8, ccr")
        ->example("ldc #0x12, ccr")
        ->imm("\x00\xff")
        ->regname("ccr");
    insert(mnem("ldc", 4, "\x01\x41\x07\x00", "\xff\xff\xff\x00"))
        ->help("Set EXR to byte immediate: ldc #xx:8, exr")
        ->example("ldc #0x12, exr")
        ->imm("\x00\x00\x00\xff")
        ->regname("exr");
    insert(mnem("ldc", 2, "\x03\x00", "\xff\xf0"))
        ->help("Set CCR to 8-bit register: ldc rs, ccr")
        ->example("ldc r2l, ccr")
        ->regh83_8("\x00\x0f")
        ->regname("ccr");
    insert(mnem("ldc", 2, "\x03\x10", "\xff\xf0"))
        ->help("Set EXT to 8-bit register: ldc rs, exr")
        ->example("ldc r2l, exr")
        ->regh83_8("\x00\x0f")
        ->regname("exr");
    insert(mnem("ldc", 4, "\x01\x40\x69\x00", "\xff\xff\xff\x8f"))
        ->help("Set CCR to byte at address given by 32-bit register: ldc @ers, ccr")
        ->example("ldc @er4, ccr")
        ->regh83_32ind("\x00\x00\x00\x70")
        ->regname("ccr");
    insert(mnem("ldc", 4, "\x01\x41\x69\x00", "\xff\xff\xff\x8f"))
        ->help("Set EXR to byte at address given by 32-bit register: ldc @ers, exr")
        ->example("ldc @er4, exr")
        ->regh83_32ind("\x00\x00\x00\x70")
        ->regname("exr");
    // TODO: Should be `ldc @(d:16, ers), ccr`.
    m = insert(mnem("ldc", 6, "\x01\x40\x6f\x00\x00\x00", "\xff\xff\xff\x8f\x00\x00"))
        ->help("Set CCR to byte at address given by 32-bit register with 16-bit offset: ldc (d:16, ers), ccr")
        ->example("ldc (#0x1234, er2), ccr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    m->regname("ccr");
    // TODO: Should be `ldc @(d:16, ers), exr`.
    m = insert(mnem("ldc", 6, "\x01\x41\x6f\x00\x00\x00", "\xff\xff\xff\x8f\x00\x00"))
        ->help("Set EXR to byte at address given by 32-bit register with 16-bit offset: ldc (d:16, ers), exr")
        ->example("ldc (#0x1234, er2), exr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    m->regname("exr");
    // TODO: Should be `ldc @(d:32, ers), ccr`.
    m = insert(mnem("ldc", 10, "\x01\x40\x78\x00\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xff\x00\x00\x00\x00"))
        ->help("Set CCR to byte at address given by 32-bit register with 32-bit offset: ldc (d:32, ers), ccr")
        ->example("ldc (#0x12345678, er2), ccr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    m->regname("ccr");
    // TODO: Should be `ldc @(d:32, ers), exr`.
    m = insert(mnem("ldc", 10, "\x01\x41\x78\x00\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xff\x00\x00\x00\x00"))
        ->help("Set EXR to byte at address given by 32-bit register with 32-bit offset: ldc (d:32, ers), exr")
        ->example("ldc (#0x12345678, er2), exr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    m->regname("exr");
    insert(mnem("ldc", 4, "\x01\x40\x6d\x00", "\xff\xff\xff\x8f"))
        ->help("Set CCR to byte at address given by 32-bit address with increment: ldc @ers+, ccr")
        ->example("ldc @er0+, ccr")
        ->regh83_32indinc("\x00\x00\x00\x70")
        ->regname("ccr");
    insert(mnem("ldc", 4, "\x01\x41\x6d\x00", "\xff\xff\xff\x8f"))
        ->help("Set EXR to byte at address given by 32-bit address with increment: ldc @ers+, exr")
        ->example("ldc @er0+, exr")
        ->regh83_32indinc("\x00\x00\x00\x70")
        ->regname("exr");
    insert(mnem("ldc", 6, "\x01\x40\x6b\x00\x00\x00", "\xff\xff\xff\xff\x00\x00"))
        ->help("Set CCR to byte at address given by 16-bit absolute: ldc @aa:16, ccr")
        ->example("ldc @0x1234, ccr")
        ->abs("\x00\x00\x00\x00\xff\xff")
        ->regname("ccr");
    insert(mnem("ldc", 6, "\x01\x41\x6b\x00\x00\x00", "\xff\xff\xff\xff\x00\x00"))
        ->help("Set EXR to byte at address given by 16-bit absolute: ldc @aa:16, exr")
        ->example("ldc @0x1234, exr")
        ->abs("\x00\x00\x00\x00\xff\xff")
        ->regname("exr");
    insert(mnem("ldc", 8, "\x01\x40\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\xff\x00\x00\x00\x00"))
        ->help("Set CCR to byte at address given by 32-bit absolute: ldc @aa:32, ccr")
        ->example("ldc @0x12345678, ccr")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff")
        ->regname("ccr");
    insert(mnem("ldc", 8, "\x01\x41\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\xff\x00\x00\x00\x00"))
        ->help("Set EXR to byte at address given by 32-bit absolute: ldc @aa:32, exr")
        ->example("ldc @0x12345678, exr")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff")
        ->regname("exr");

    // TODO: LDM

    // LDMAC
    insert(mnem("ldmac", 2, "\x03\x20", "\xff\xf8"))
        ->help("Move 32-bit register into MACH: ldmac ers, mach")
        ->example("ldmac er3, mach")
        ->regh83_32("\x00\x07")
        ->regname("mach");
    insert(mnem("ldmac", 2, "\x03\x30", "\xff\xf8"))
        ->help("Move 32-bit register into MACL: ldmac ers, macl")
        ->example("ldmac er3, macl")
        ->regh83_32("\x00\x07")
        ->regname("macl");

    // MAC
    insert(mnem("mac", 4, "\x01\x60\x6d\x00", "\xff\xff\xff\x88"))
        ->help("Add result of signed multiplication of addresses in 32-bit registers to MAC: mac @ern+, @erm+")
        ->example("mac @er3+, @er6+")
        ->regh83_32indinc("\x00\x00\x00\x70")
        ->regh83_32indinc("\x00\x00\x00\x07");

    // MOV
    insert(mnem("mov.b", 2, "\xf0\x00", "\xf0\x00"))
        ->help("Move byte immediate into 8-bit register: mov.b #xx:8, rd")
        ->example("mov.b #0x12, r4l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("mov.b", 2, "\x0c\x00", "\xff\x00"))
        ->help("Move byte from 8-bit register to 8-bit register: mov.b rs, rd")
        ->example("mov.b r3h, r4l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("mov.b", 2, "\x68\x00", "\xff\x80"))
        ->help("Move byte from address given by 32-bit register into 8-bit register: mov.b @ers, rd")
        ->example("mov.b @er2, r6h")
        ->regh83_32ind("\x00\x70")
        ->regh83_8("\x00\x0f");
    // TODO: Should be `mov.b @(d:16, ers), rd`.
    m = insert(mnem("mov.b", 4, "\x6e\x00\x00\x00", "\xff\x80\x00\x00"))
        ->help("Move byte from address given by 32-bit register with 16-bit offset into 8-bit register: mov.b (d:16, ers), rd")
        ->example("mov.b (#0x1234, er5), r1h");
    pg = m->group('(');
    pg->imm("\x00\x00\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00");
    m->regh83_8("\x00\x0f\x00\x00");
    // TODO: Should be `mov @(d:32, ers), rd`.
    m = insert(mnem("mov.b", 8, "\x78\x00\x6a\x20\x00\x00\x00\x00", "\xff\x8f\xff\xf0\x00\x00\x00\x00"))
        ->help("Move byte from address given by 32-bit register with 32-bit offset into 8-bit register: mov.b (d:32, ers), rd")
        ->example("mov.b (#0x12345678, er0), r7h");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00\x00\x00\x00\x00");
    m->regh83_8("\x00\x00\x00\x0f\x00\x00\x00\x00");
    insert(mnem("mov.b", 2, "\x6c\x00", "\xff\x80"))
        ->help("Move byte from address given by 32-bit register into 8-bit register with increment: mov.b @ers+, rd")
        ->example("mov.b @er2+, r3l")
        ->regh83_32indinc("\x00\x70")
        ->regh83_8("\x00\x0f");
    insert(mnem("mov.b", 2, "\x20\x00", "\xf0\x00"))
        ->help("MOve byte from address given by 8-bit absolute into 8-bit register: mov.b @aa:8, rd")
        ->example("mov.b @0x12, r0l")
        ->abs("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("mov.b", 4, "\x6a\x00\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Move byte from address given by 16-bit absolute into 8-bit register: mov.b @aa:16, rd")
        ->example("mov.b @0x1234, r2h")
        ->abs("\x00\x00\xff\xff")
        ->regh83_8("\x00\x0f\x00\x00");
    insert(mnem("mov.b", 6, "\x6a\x20\x00\x00\x00\x00", "\xff\xf0\x00\x00\x00\x00"))
        ->help("Move byte from address given by 32-bit absolute into 8-bit register: mov.b @aa:32, rd")
        ->example("mov.b @0x12345678, r5l")
        ->abs("\x00\x00\xff\xff\xff\xff")
        ->regh83_8("\x00\x0f\x00\x00\x00\x00");
    insert(mnem("mov.b", 2, "\x68\x80", "\xff\x80"))
        ->help("Move byte from 8-bit register into address given by 32-bit register: mov.b rs, @erd")
        ->example("mov.b r5l, @er0")
        ->regh83_8("\x00\x0f")
        ->regh83_32ind("\x00\x70");
    // TODO: Should be `mov.b @(d:16, ers), erd`.
    m = insert(mnem("mov.b", 4, "\x6e\x80\x00\x00", "\xff\x80\x00\x00"))
        ->help("Move byte from 8-bit register into address given by 32-bit register and 16-bit offset: mov.b rs, (d:16, erd)")
        ->example("mov.b r6h, (#0x1234, er5)");
    m->regh83_8("\x00\x0f\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00");
    // TODO: Should be `mov.b @(d:32, ers), erd`.
    m = insert(mnem("mov.b", 8, "\x78\x00\x6a\xa0\x00\x00\x00\x00", "\xff\x8f\xff\xf0\x00\x00\x00\x00"))
        ->help("Move byte from 8-bit register into address given by 32-bit register and 32-bit offset: mov.b rs, (d:32, erd)")
        ->example("mov.b r7h, (#0x12345678, er0)");
    m->regh83_8("\x00\x00\x00\x0f\x00\x00\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00\x00\x00\x00\x00");
    insert(mnem("mov.b", 2, "\x6c\x80", "\xff\x80"))
        ->help("Move byte from 8-bit register into address given by 32-bit register with decrement: mov.b rs, @-erd")
        ->example("mov.b r1h, @-er0")
        ->regh83_8("\x00\x0f")
        ->regh83_32inddec("\x00\x70");
    insert(mnem("mov.b", 2, "\x30\x00", "\xf0\x00"))
        ->help("Move byte from 8-bit register into address given by 8-bit absolute: mov.b rs, @aa:8")
        ->example("mov.b r1h, @0x12")
        ->regh83_8("\x0f\x00")
        ->abs("\x00\xff");
    insert(mnem("mov.b", 4, "\x6a\x80\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Move byte from 8-bit register into address given by 16-bit absolute: mov.b rs, @aa:16")
        ->example("mov.b r4l, @0x1234")
        ->regh83_8("\x00\x0f\x00\x00")
        ->abs("\x00\x00\xff\xff");
    insert(mnem("mov.b", 6, "\x6a\xa0\x00\x00\x00\x00", "\xff\xf0\x00\x00\x00\x00"))
        ->help("Move byte from 8-bit register into address given by 32-bit absolute: mov.b rs, @aa:32")
        ->example("mov.b r7l, @0x12345678")
        ->regh83_8("\x00\x0f\x00\x00\x00\x00")
        ->abs("\x00\x00\xff\xff\xff\xff");
    insert(mnem("mov.w", 4, "\x79\x00\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Move word immediate into 16-bit register: mov.w #xx:16, rd")
        ->example("mov.w #0x1234, e2")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("mov.w", 2, "\x0d\x00", "\xff\x00"))
        ->help("Move word from 16-bit register into 16-bit register: mov.w rs, rd")
        ->example("mov.w r3, r4")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("mov.w", 2, "\x69\x00", "\xff\x80"))
        ->help("Move word from address given by 32-bit register into 16-bit register: mov.w @ers, rd")
        ->example("mov.w @er3, r4")
        ->regh83_32ind("\x00\x70")
        ->regh83_16("\x00\x0f");
    // TODO: Should be `mov.w @(d:16, ers), rd`.
    m = insert(mnem("mov.w", 4, "\x6f\x00\x00\x00", "\xff\x80\x00\x00"))
        ->help("Move word from address given by 32-bit register with 16-bit offset into 16-bit register: mov.w (d:16, ers), rd")
        ->example("mov.w (#0x1234, er5), r3l");
    pg = m->group('(');
    pg->imm("\x00\x00\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00");
    m->regh83_8("\x00\x0f\x00\x00");
    // TODO: Should be `mov.w @(d:32, ers), rd`.
    m = insert(mnem("mov.w", 8, "\x78\x00\x6b\x20\x00\x00\x00\x00", "\xff\x8f\xff\xf0\x00\x00\x00\x00"))
        ->help("Move word from address given by 32-bit register eith 32-bit offset into 16-bit register: mov.w (d:32, ers), rd")
        ->example("mov.w (#0x12345678, er3), r5l");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00\x00\x00\x00\x00");
    m->regh83_8("\x00\x00\x00\x0f\x00\x00\x00\x00");
    insert(mnem("mov.w", 2, "\x6d\x00", "\xff\x80"))
        ->help("Move word from address given by 32-bit register into 16-bit register with increment: mov.w @ers+, rd")
        ->example("mov.w @er4+, r3")
        ->regh83_32indinc("\x00\x70")
        ->regh83_16("\x00\x0f");
    insert(mnem("mov.w", 4, "\x6b\x00\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Move word from address given by 16-bit absolute into 16-bit register: mov.w @aa:16, rd")
        ->example("mov.w @1234, r0")
        ->abs("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("mov.w", 6, "\x6b\x20\x00\x00\x00\x00", "\xff\xf0\x00\x00\x00\x00"))
        ->help("Move word from address given by 32-bit absolute into 16-bit register: mov.w @aa:32, rd")
        ->example("mov.w @12345678, r6")
        ->abs("\x00\x00\xff\xff\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00\x00\x00");
    insert(mnem("mov.w", 2, "\x69\x80", "\xff\x80"))
        ->help("Move word from 16-bit register into address given by 32-bit register: mov.w rs, @erd")
        ->example("mov.w r3, @er5")
        ->regh83_16("\x00\x0f")
        ->regh83_32ind("\x00\x70");
    // TODO: Should be `mov.w rs, @(d:16, erd)`
    m = insert(mnem("mov.w", 4, "\x6f\x80\x00\x00", "\xff\x80\x00\x00"))
        ->help("Move word from 16-bit register into address given by 32-bit register with 16-bit offset: mov.w rs, (d:16, erd)")
        ->example("mov.w r1h, (#0x1234, er2)");
    m->regh83_8("\x00\x0f\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00");
    // TODO: Should be `mov.w rs, @(d:32, erd)`
    m = insert(mnem("mov.w", 8, "\x78\x80\x6b\xa0\xff\x00\x00\x00", "\xff\x8f\xff\xf0\x00\x00\x00\x00"))
        ->help("Move word from 16-bit register into address given by 32-bit register with 32-bit offset: mov.w rs, (d:32, erd)")
        ->example("mov.w r2h, (#0x12345678, er1)");
    m->regh83_8("\x00\x00\x00\x0f\x00\x00\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x70\x00\x00\x00\x00\x00\x00");
    insert(mnem("mov.w", 2, "\x6d\x80", "\xff\x80"))
        ->help("Move word from 16-bit register to address given by 32-bit register with decrement: mov.w rs, @-erd")
        ->example("mov.w r1, @-er3")
        ->regh83_16("\x00\x0f")
        ->regh83_32inddec("\x00\x70");
    insert(mnem("mov.w", 4, "\x6b\x80\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Move word from 16-bit register to address given by 16-bit absolute: mov.w rs, @aa:16")
        ->example("mov.w r2, @0x1234")
        ->regh83_16("\x00\x0f\x00\x00")
        ->abs("\x00\x00\xff\xff");
    insert(mnem("mov.w", 6, "\x6b\xa0\x00\x00\x00\x00", "\xff\xf0\x00\x00\x00\x00"))
        ->help("Move word from 16-bit register to address given by 32-bit absolute: mov.w rs, @aa:32")
        ->example("mov.w r4, @0x12345678")
        ->regh83_16("\x00\x0f\x00\x00\x00\x00")
        ->abs("\x00\x00\xff\xff\xff\xff");
    insert(mnem("mov.l", 6, "\x7a\x00\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("Move long immediate into 32-bit register: mov.l #xx:32, erd")
        ->example("mov.l #0x12345678, er2")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("mov.l", 2, "\x0f\x80", "\xff\x88"))
        ->help("Move long from 32-bit register into 32-bit register: mov.l ers, erd")
        ->example("mov.l er0, er2")
        ->regh83_32("\x00\x70")
        ->regh83_32("\x00\x07");
    insert(mnem("mov.l", 4, "\x01\x00\x69\x00", "\xff\xff\xff\x88"))
        ->help("Move long from address given by 32-bit register into 32-bit register: mov.l @ers, erd")
        ->example("mov.l @er2, er0")
        ->regh83_32ind("\x00\x00\x00\x70")
        ->regh83_32("\x00\x00\x00\x07");
    // TODO: Should be `mov.l @(d:16, ers), rd`.
    m = insert(mnem("mov.l", 6, "\x01\x00\x6f\x00\x00\x00", "\xff\xff\xff\x88\x00\x00"))
        ->help("Move long from address given by 32-bit register with 16-bit offset into 32-bit register: mov.l (d:16, ers), erd")
        ->example("mov.l (#0x1234, er3), er6");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    m->regh83_32("\x00\x00\x00\x07\x00\x00");
    // TODO: Should be `mov.l @(d:32, ers), rd`.
    m = insert(mnem("mov.l", 10, "\x01\x00\x78\x00\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xf8\x00\x00\x00\x00"))
        ->help("Move long from address given by 32-bit register with 32-bit offset into 32-bit register: mov.l (d:32, ers), erd")
        ->example("mov.l (#0x123456, er6), er1");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    m->regh83_32("\x00\x00\x00\x00\x00\x07\x00\x00\x00\x00");
    insert(mnem("mov.l", 4, "\x01\x00\x6d\x00", "\xff\xff\xff\x88"))
        ->help("Move long from address given by 32-bit register into 32-bit register with increment: mov.l @ers+, erd")
        ->example("mov.l @er0+, er2")
        ->regh83_32indinc("\x00\x00\x00\x70")
        ->regh83_32("\x00\x00\x00\x07");
    insert(mnem("mov.l", 6, "\x01\x00\x6b\x00\x00\x00", "\xff\xff\xff\xf8\x00\x00"))
        ->help("Move long from address given by 16-bit absolute into 32-bit register: mov.l @aa:16, erd")
        ->example("mov.l @0x1234, er3")
        ->abs("\x00\x00\x00\x00\xff\xff")
        ->regh83_32("\x00\x00\x00\x07\x00\x00");
    insert(mnem("mov.l", 8, "\x01\x00\x6b\x20\x00\x00\x00\x00", "\xff\xff\xff\xf8\x00\x00\x00\x00"))
        ->help("Move long from address given by 32-bit absolute into 32-bit register: mov.l @aa:32, erd")
        ->example("mov.l @0x12345678, er3")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x00\x00\x07\x00\x00\x00\x00");
    insert(mnem("mov.l", 4, "\x01\x00\x69\x80", "\xff\xff\xff\x88"))
        ->help("Move long from 32-bit register into adderss given by 32-bit register: mov.l ers, @erd")
        ->example("mov.l er5, @er0")
        ->regh83_32("\x00\x00\x00\x70")
        ->regh83_32ind("\x00\x00\x00\x07");
    // TODO: Should be `mov.l ers, @(d:16, erd)`.
    m = insert(mnem("mov.l", 6, "\x01\x00\x6f\x80\x00\x00", "\xff\xff\xff\x88\x00\x00"))
        ->help("Move long from 32-bit register into address given by 32-bit register with 16-bit offset: mov.l ers, (d:16, erd)")
        ->example("mov.l er6, (#0x1234, er3)");
    m->regh83_32("\x00\x00\x00\x07\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    // TODO: Should be `mov.l ers, @(d:32, erd)`.
    m = insert(mnem("mov.l", 10, "\x01\x00\x78\x00\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xf8\x00\x00\x00\x00"))
        ->help("Move long from 32-bit register into address given by 32-bit register with 32-bit offset: mov.l ers, (d:32, erd)")
        ->example("mov.l er4, (#0x12345678, er2)");
    m->regh83_32("\x00\x00\x00\x00\x00\x07\x00\x00\x00\x00");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    insert(mnem("mov.l", 4, "\x01\x00\x6d\x80", "\xff\xff\xff\x88"))
        ->help("Move long from 32-bit register into address given by 32-bit register with decrement: mov.l ers, @-erd")
        ->example("mov.l er0, @-er5")
        ->regh83_32("\x00\x00\x00\x70")
        ->regh83_32inddec("\x00\x00\x00\x07");
    insert(mnem("mov.l", 6, "\x01\x00\x6b\x80\x00\x00", "\xff\xff\xff\xf8\x00\x00"))
        ->help("Move long from 32-bit register into address given by 16-bit absolute: mov.l ers, @aa:16")
        ->example("mov.l er1, @0x1234")
        ->regh83_32("\x00\x00\x00\x07\x00\x00")
        ->abs("\x00\x00\x00\x00\xff\xff");
    insert(mnem("mov.l", 8, "\x01\x00\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\xf8\x00\x00\x00\x00"))
        ->help("Move long from 32-bit register into address given by 32-bit absolute: mov.l ers, @aa:32")
        ->example("mov.l er6, @0x12345678")
        ->regh83_32("\x00\x00\x00\x07\x00\x00\x00\x00")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff");

    // MOVFPE
    insert(mnem("movfpe", 4, "\x6a\x40\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Synchronised transfer memory from address given by 16-bit absolute to 8-bit register: movfpe @aa:16, rd")
        ->example("movfpe @0x1234, r2")
        ->abs("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");

    // MOVTPE
    insert(mnem("movtpe", 4, "\x6a\xc0\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Synchronised transfer memory from 8-bit register to address given by 16-bit absolute: movtpe rs, @aa:16")
        ->example("movtpe r4, @0x1234")
        ->regh83_16("\x00\x0f\x00\x00")
        ->abs("\x00\x00\xff\xff");

    // MULXS
    insert(mnem("mulxs.b", 4, "\x01\xc0\x50\x00", "\xff\xff\xff\x00"))
        ->help("Signed multiplication of 8-bit register by 8-bit register: mulxs.b rs, rd")
        ->example("mulxs.b r0l, r4h")
        ->regh83_8("\x00\x00\x00\xf0")
        ->regh83_8("\x00\x00\x00\x0f");
    insert(mnem("mulxs.w", 4, "\x01\xc0\x52\x00", "\xff\xff\xff\x08"))
        ->help("Signed multiplication of 16-bit register by 16-bit register: mulxs.w rs, erd")
        ->example("mulxs.w r5, er2")
        ->regh83_16("\x00\x00\x00\xf0")
        ->regh83_32("\x00\x00\x00\x07");

    // MULXU
    insert(mnem("mulxu.b", 2, "\x50\x00", "\xff\x00"))
        ->help("Unsigned multiplcation of 16-bit register by 16-bit register: mulxu.b rs, rd")
        ->example("mulxu.b r0l, r4h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("mulxu.w", 2, "\x52\x00", "\xff\x08"))
        ->help("Unsigned multiplcation of 16-bit register by 16-bit register: mulxu.w rs, erd")
        ->example("mulxu.w r5, er2")
        ->regh83_16("\x00\xf0")
        ->regh83_32("\x00\x07");

    // NEG
    insert(mnem("neg.b", 2, "\x17\x80", "\xff\xf0"))
        ->help("2s complement of 8-bit register: neg.b rd")
        ->example("neg.b r5h")
        ->regh83_8("\x00\x0f");
    insert(mnem("neg.w", 2, "\x17\x90", "\xff\xf0"))
        ->help("2s complement of 16-bit register: neg.w rd")
        ->example("neg.w e1")
        ->regh83_16("\x00\x0f");
    insert(mnem("neg.l", 2, "\x17\xb0", "\xff\xf8"))
        ->help("2s complement of 32-bit register: neg.l rd")
        ->example("neg.l er7")
        ->regh83_32("\x00\x07");

    // NOP
    insert(mnem("nop", 2, "\x00\x00", "\xff\xff"))
        ->help("No-Operation: nop")
        ->example("nop");

    // NOT
    insert(mnem("not.b", 2, "\x17\x00", "\xff\xf0"))
        ->help("1s complement of 8-bit register: not.b rd")
        ->example("not.b r5h")
        ->regh83_8("\x00\x0f");
    insert(mnem("not.w", 2, "\x17\x10", "\xff\xf0"))
        ->help("1s complement of 16-bit register: not.w rd")
        ->example("not.w e1")
        ->regh83_16("\x00\x0f");
    insert(mnem("not.l", 2, "\x17\x30", "\xff\xf8"))
        ->help("1s complement of 16-bit register: not.l rd")
        ->example("not.l er7")
        ->regh83_32("\x00\x07");

    // OR
    insert(mnem("or.b", 2, "\xc0\x00", "\xf0\x00"))
        ->help("OR byte immediate with 8-bit register: or.b #xx:8, rd")
        ->example("or.b #0x12, r5h")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("or.b", 2, "\x14\x00", "\xff\x00"))
        ->help("OR 8-bit register with 8-bit register: or.b rs, rd")
        ->example("or.b r2h, r5h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("or.w", 4, "\x79\x40\x00\x00", "\xff\xf0\x00\x00"))
        ->help("OR word immediate with 16-bit register: or.w #xx:16, rd")
        ->example("or.w #0x1234, r2")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("or.w", 2, "\x64\x00", "\xff\x00"))
        ->help("OR 16-bit register with 16-bit register: or.w rs, rd")
        ->example("or.w r2, r5")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("or.l", 6, "\x7a\x40\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("OR long immediate with 32-bit register: or.l #xx:32, rd")
        ->example("or.l #0x12345678, er1")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("or.l", 4, "\x01\xf0\x64\x00", "\xff\xff\xff\x88"))
        ->help("OR 32-bit register with 32-bit register: or.l ers, erd")
        ->example("or.l er2, er5")
        ->regh83_32("\x00\x00\x00\x70")
        ->regh83_32("\x00\x00\x00\x07");

    // ORC
    insert(mnem("orc", 2, "\x04\x00", "\xff\x00"))
        ->help("OR CCR with byte immediate: orc #xx:8, ccr")
        ->example("orc #0x12, ccr")
        ->imm("\x00\xff")
        ->regname("ccr");
    insert(mnem("orc", 4, "\x01\x41\x04\x00", "\xff\xff\xff\x00"))
        ->help("OR EXR with byte immediate: orc #xx:8, exr")
        ->example("orc #0x12, exr")
        ->imm("\x00\x00\x00\xff")
        ->regname("exr");

    // POP: pseudo-instructions of `mov.x @ers+, rd`.
    insert(mnem("pop.w", 2, "\x6d\x70", "\xff\xf0"))
        ->help("Pop word off stack into 16-bit register: pop.w rn")
        ->example("pop.w r0")
        ->prioritize()
        ->regh83_16("\x00\x0f");
    insert(mnem("pop.l", 4, "\x01\x00\x6d\70", "\xff\xff\xff\xf8"))
        ->help("Pop long off stack into 32-bit register: pop.l ern")
        ->example("pop.l er3")
        ->prioritize()
        ->regh83_32("\x00\x00\x00\x07");

    // PUSH: pseudo-instructions of `mov.x @-ers, rd`.
    insert(mnem("push.w", 2, "\x6d\xf0", "\xff\xf0"))
        ->help("Push word onto stack from 16-bit register: push.w rn")
        ->example("push.w r0")
        ->prioritize()
        ->regh83_16("\x00\x0f");
    insert(mnem("push.l", 4, "\x01\x00\x6d\xf0", "\xff\xff\xff\xf8"))
        ->help("Push long onto stack from 32-bit register: push.l ern")
        ->example("push.l er3")
        ->prioritize()
        ->regh83_32("\x00\x00\x00\x07");

    // ROTL
    insert(mnem("rotl.b", 2, "\x12\x80", "\xff\xf0"))
        ->help("Rotate left 8-bit register: rotl.b rd")
        ->example("rotl.b r4l")
        ->regh83_8("\x00\x0f");
    insert(mnem("rotl.b", 2, "\x12\xc0", "\xff\xf0"))
        ->help("Rotate left 8-bit register twice: rotl.b #2, rd")
        ->example("rotl.b #2, r4l")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("rotl.w", 2, "\x12\x90", "\xff\xf0"))
        ->help("Rotate left 16-bit register: rotl.w rd")
        ->example("rotl.w e0")
        ->regh83_16("\x00\x0f");
    insert(mnem("rotl.w", 2, "\x12\xd0", "\xff\xf0"))
        ->help("Rotate left 16-bit register twice: rotl.w #2, rd")
        ->example("rotl.w #2, e0")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("rotl.l", 2, "\x12\xb0", "\xff\xf8"))
        ->help("Rotate left 32-bit register: rotl.l rd")
        ->example("rotl.l er2")
        ->regh83_32("\x00\x07");
    insert(mnem("rotl.l", 2, "\x12\xf0", "\xff\xf8"))
        ->help("Rotate left 32-bit register twice: rotl.l #2, rd")
        ->example("rotl.l #2, er2")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // ROTR
    insert(mnem("rotr.b", 2, "\x13\x80", "\xff\xf0"))
        ->help("Rotate right 8-bit register: rotr.b rd")
        ->example("rotr.b r4l")
        ->regh83_8("\x00\x0f");
    insert(mnem("rotr.b", 2, "\x13\xc0", "\xff\xf0"))
        ->help("Rotate right 8-bit register twice: rotr.b #2, rd")
        ->example("rotr.b #2, r4l")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("rotr.w", 2, "\x13\x90", "\xff\xf0"))
        ->help("Rotate right 16-bit register: rotr.w rd")
        ->example("rotr.w e0")
        ->regh83_16("\x00\x0f");
    insert(mnem("rotr.w", 2, "\x13\xd0", "\xff\xf0"))
        ->help("Rotate right 16-bit register twice: rotr.w #2, rd")
        ->example("rotr.w #2, e0")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("rotr.l", 2, "\x13\xb0", "\xff\xf8"))
        ->help("Rotate right 32-bit register: rotr.l rd")
        ->example("rotr.l er2")
        ->regh83_32("\x00\x07");
    insert(mnem("rotr.l", 2, "\x13\xf0", "\xff\xf8"))
        ->help("Rotate right 32-bit register twice: rotr.l #2, rd")
        ->example("rotr.l #2, er2")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // ROTXL
    insert(mnem("rotxl.b", 2, "\x12\x00", "\xff\xf0"))
        ->help("Rotate left through Carry 8-bit register: rotxl.b rd")
        ->example("rotxl.b r4l")
        ->regh83_8("\x00\x0f");
    insert(mnem("rotxl.b", 2, "\x12\x40", "\xff\xf0"))
        ->help("Rotate left through Carry 8-bit register twice: rotxl.b #2, rd")
        ->example("rotxl.b #2, r4l")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("rotxl.w", 2, "\x12\x10", "\xff\xf0"))
        ->help("Rotate left through Carry 16-bit register: rotxl.w rd")
        ->example("rotxl.w e0")
        ->regh83_16("\x00\x0f");
    insert(mnem("rotxl.w", 2, "\x12\x50", "\xff\xf0"))
        ->help("Rotate left through Carry 16-bit register twice: rotxl.w #2, rd")
        ->example("rotxl.w #2, e0")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("rotxl.l", 2, "\x12\x30", "\xff\xf8"))
        ->help("Rotate left through Carry 32-bit register: rotxl.l rd")
        ->example("rotxl.l er2")
        ->regh83_32("\x00\x07");
    insert(mnem("rotxl.l", 2, "\x12\x70", "\xff\xf8"))
        ->help("Rotate left through Carry 32-bit register twice: rotxl.l #2, rd")
        ->example("rotxl.l #2, er2")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // ROTXR
    insert(mnem("rotxr.b", 2, "\x13\x00", "\xff\xf0"))
        ->help("Rotate right through Carry 8-bit register: rotxr.b rd")
        ->example("rotxr.b r4l")
        ->regh83_8("\x00\x0f");
    insert(mnem("rotxr.b", 2, "\x13\x40", "\xff\xf0"))
        ->help("Rotate right through Carry 8-bit register twice: rotxr.b #2, rd")
        ->example("rotxr.b #2, r4l")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("rotxr.w", 2, "\x13\x10", "\xff\xf0"))
        ->help("Rotate right through Carry 16-bit register: rotxr.w rd")
        ->example("rotxr.w e0")
        ->regh83_16("\x00\x0f");
    insert(mnem("rotxr.w", 2, "\x13\x50", "\xff\xf0"))
        ->help("Rotate right through Carry 16-bit register twice: rotxr.w #2, rd")
        ->example("rotxr.w #2, e0")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("rotxr.l", 2, "\x13\x30", "\xff\xf8"))
        ->help("Rotate right through Carry 32-bit register: rotxr.l rd")
        ->example("rotxr.l er2")
        ->regh83_32("\x00\x07");
    insert(mnem("rotxr.l", 2, "\x13\x70", "\xff\xf8"))
        ->help("Rotate right through Carry 32-bit register twice: rotxr.l #2, rd")
        ->example("rotxr.l #2, er2")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // RTE
    insert(mnem("rte", 2, "\x56\x70", "\xff\xff"))
        ->help("Return from Exception handler: rte")
        ->example("rte");

    // RTS
    insert(mnem("rts", 2, "\x54\x70", "\xff\xff"))
        ->help("Return from Subroutine: rts")
        ->example("rts");

    // SHAL
    insert(mnem("shal.b", 2, "\x10\x80", "\xff\xf0"))
        ->help("Arithmetic shift left 8-bit register: shal.b rd")
        ->example("shal.b r2h")
        ->regh83_8("\x00\x0f");
    insert(mnem("shal.b", 2, "\x10\xc0", "\xff\xf0"))
        ->help("Arithmetic shift left 8-bit register twice: shal.b #2, rd")
        ->example("shal.b #2, r2h")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("shal.w", 2, "\x10\x90", "\xff\xf0"))
        ->help("Arithmetic shift left 16-bit register: shal.w rd")
        ->example("shal.w r5")
        ->regh83_16("\x00\x0f");
    insert(mnem("shal.w", 2, "\x10\xd0", "\xff\xf0"))
        ->help("Arithmetic shift left 16-bit register twice: shal.w #2, rd")
        ->example("shal.w #2, r5")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("shal.l", 2, "\x10\xb0", "\xff\xf8"))
        ->help("Arithmetic shift left 32-bit register: shal.l erd")
        ->example("shal.l er1")
        ->regh83_32("\x00\x07");
    insert(mnem("shal.l", 2, "\x10\xf0", "\xff\xf8"))
        ->help("Arithmetic shift left 32-bit register twice: shal.l #2, erd")
        ->example("shal.l #2, er1")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // SHAR
    insert(mnem("shar.b", 2, "\x11\x80", "\xff\xf0"))
        ->help("Arithmetic shift right 8-bit register: shar.b rd")
        ->example("shar.b r2h")
        ->regh83_8("\x00\x0f");
    insert(mnem("shar.b", 2, "\x11\xc0", "\xff\xf0"))
        ->help("Arithmetic shift right 8-bit register twice: shar.b #2, rd")
        ->example("shar.b #2, r2h")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("shar.w", 2, "\x11\x90", "\xff\xf0"))
        ->help("Arithmetic shift right 16-bit register: shar.w rd")
        ->example("shar.w r5")
        ->regh83_16("\x00\x0f");
    insert(mnem("shar.w", 2, "\x11\xd0", "\xff\xf0"))
        ->help("Arithmetic shift right 16-bit register twice: shar.w #2, rd")
        ->example("shar.w #2, r5")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("shar.l", 2, "\x11\xb0", "\xff\xf8"))
        ->help("Arithmetic shift right 32-bit register: shar.l erd")
        ->example("shar.l er1")
        ->regh83_32("\x00\x07");
    insert(mnem("shar.l", 2, "\x11\xf0", "\xff\xf8"))
        ->help("Arithmetic shift right 32-bit register twice: shar.l #2, erd")
        ->example("shar.l #2, er1")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // SHLL
    insert(mnem("shll.b", 2, "\x10\x00", "\xff\xf0"))
        ->help("Logical shift left 8-bit register: shll.b rd")
        ->example("shll.b r2h")
        ->regh83_8("\x00\x0f");
    insert(mnem("shll.b", 2, "\x10\x40", "\xff\xf0"))
        ->help("Logical shift left 8-bit register twice: shll.b #2, rd")
        ->example("shll.b #2, r2h")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("shll.w", 2, "\x10\x10", "\xff\xf0"))
        ->help("Logical shift left 16-bit register: shll.w rd")
        ->example("shll.w r5")
        ->regh83_16("\x00\x0f");
    insert(mnem("shll.w", 2, "\x10\x50", "\xff\xf0"))
        ->help("Logical shift left 16-bit register twice: shll.w #2, rd")
        ->example("shll.w #2, r5")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("shll.l", 2, "\x10\x30", "\xff\xf8"))
        ->help("Logical shift left 32-bit register: shll.l erd")
        ->example("shll.l er1")
        ->regh83_32("\x00\x07");
    insert(mnem("shll.l", 2, "\x10\x70", "\xff\xf8"))
        ->help("Logical shift left 32-bit register twice: shll.l #2, erd")
        ->example("shll.l #2, er1")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // SHLR
    insert(mnem("shlr.b", 2, "\x11\x00", "\xff\xf0"))
        ->help("Logical shift right 8-bit register: shlr.b rd")
        ->example("shlr.b r2h")
        ->regh83_8("\x00\x0f");
    insert(mnem("shlr.b", 2, "\x11\x40", "\xff\xf0"))
        ->help("Logical shift right 8-bit register twice: shlr.b #2, rd")
        ->example("shlr.b #2, r2h")
        ->h83_const(2)
        ->regh83_8("\x00\x0f");
    insert(mnem("shlr.w", 2, "\x11\x10", "\xff\xf0"))
        ->help("Logical shift right 16-bit register: shlr.w rd")
        ->example("shlr.w r5")
        ->regh83_16("\x00\x0f");
    insert(mnem("shlr.w", 2, "\x11\x50", "\xff\xf0"))
        ->help("Logical shift right 16-bit register twice: shlr.w #2, rd")
        ->example("shlr.w #2, r5")
        ->h83_const(2)
        ->regh83_16("\x00\x0f");
    insert(mnem("shlr.l", 2, "\x11\x30", "\xff\xf8"))
        ->help("Logical shift right 32-bit register: shlr.l erd")
        ->example("shlr.l er1")
        ->regh83_32("\x00\x07");
    insert(mnem("shlr.l", 2, "\x11\x70", "\xff\xf8"))
        ->help("Logical shift right 32-bit register twice: shlr.l #2, erd")
        ->example("shlr.l #2, er1")
        ->h83_const(2)
        ->regh83_32("\x00\x07");

    // SLEEP
    insert(mnem("sleep", 2, "\x01\x80", "\xff\xff"))
        ->help("Transition to power-down state: sleep")
        ->example("sleep");

    // STC
    insert(mnem("stc", 2, "\x02\x00", "\xff\xf0"))
        ->help("Store CCR in 8-bit register: stc ccr, rd")
        ->example("stc ccr, r1h")
        ->regname("ccr")
        ->regh83_8("\x00\x0f");
    insert(mnem("stc", 2, "\x02\x10", "\xff\xf0"))
        ->help("Store EXR in 8-bit register: stc exr, rd")
        ->example("stc exr, r1h")
        ->regname("exr")
        ->regh83_8("\x00\x0f");
    insert(mnem("stc", 4, "\x01\x40\x69\x80", "\xff\xff\xff\x8f"))
        ->help("Store CCR in address given by 32-bit register: stc ccr, @erd")
        ->example("stc ccr, @er0")
        ->regname("ccr")
        ->regh83_32ind("\x00\x00\x00\x70");
    insert(mnem("stc", 4, "\x01\x41\x69\x80", "\xff\xff\xff\x8f"))
        ->help("Store EXR in address given by 32-bit register: stc exr, @erd")
        ->example("stc exr, @er0")
        ->regname("exr")
        ->regh83_32ind("\x00\x00\x00\x70");
    // TODO: Should be `stc ccr, @(d:16, erd)`.
    m = insert(mnem("stc", 6, "\x01\x40\x6f\x80\x00\x00", "\xff\xff\xff\x8f\x00\x00"))
        ->help("Store CCR in address given by 32-bit register with 16-bit offset: stc ccr, (d:16, erd)")
        ->example("stc ccr, (#0x1234, er5)");
    m->regname("ccr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    // TODO: Should be `stc exr, @(d:16, erd)`.
    m = insert(mnem("stc", 6, "\x01\x41\x6f\x80\x00\x00", "\xff\xff\xff\x8f\x00\x00"))
        ->help("Store EXR in address given by 32-bit register with 16-bit offset: stc exr, (d:16, erd)")
        ->example("stc exr, (#0x1234, er5)");
    m->regname("exr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00");
    // TODO: Should be `stc ccr, @(d:32, erd)`.
    m = insert(mnem("stc", 10, "\x01\x40\x78\x00\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xff\x00\x00\x00\x00"))
        ->help("Store CCR in address given by 32-bit register with 32-bit offset: stc ccr, (d:32, erd)")
        ->example("stc ccr, (#0x12345678, er0)");
    m->regname("ccr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    // TODO: Should be `stc exr, @(d:32, erd)`.
    m = insert(mnem("stc", 10, "\x01\x41\x78\x00\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\x8f\xff\xff\x00\x00\x00\x00"))
        ->help("Store EXR in address given by 32-bit address with 32-bit offset: stc exr, (d:32, erd)")
        ->example("stc exr, (#0x12345678, er0)");
    m->regname("exr");
    pg = m->group('(');
    pg->imm("\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");
    pg->regh83_32("\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00");
    insert(mnem("stc", 4, "\x01\x40\x6d\x80", "\xff\xff\xff\x8f"))
        ->help("Store CCR in address given by 32-bit register with decrement: stc ccr, @-erd")
        ->example("stc ccr, @-er1")
        ->regname("ccr")
        ->regh83_32inddec("\x00\x00\x00\x70");
    insert(mnem("stc", 4, "\x01\x41\x6d\x80", "\xff\xff\xff\x8f"))
        ->help("Store EXR in address given by 32-bit register with decrement: stc exr, @-erd")
        ->example("stc exr, @-er1")
        ->regname("exr")
        ->regh83_32inddec("\x00\x00\x00\x70");
    insert(mnem("stc", 6, "\x01\x40\x6b\x80\x00\x00", "\xff\xff\xff\xff\x00\x00"))
        ->help("Store CCR in address given by 16-bit absolute: stc ccr, @aa:16")
        ->example("stc ccr, @0x1234")
        ->regname("ccr")
        ->abs("\x00\x00\x00\x00\xff\xff");
    insert(mnem("stc", 6, "\x01\x41\x6b\x80\x00\x00", "\xff\xff\xff\xff\x00\x00"))
        ->help("Store EXR in address given by 16-bit absolute: stc exr, @aa:16")
        ->example("stc exr, @0x1234")
        ->regname("exr")
        ->abs("\x00\x00\x00\x00\xff\xff");
    insert(mnem("stc", 8, "\x01\x40\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\xff\x00\x00\x00\x00"))
        ->help("Store CCR in address given by 32-bit absolute: stc ccr, @aa:32")
        ->example("stc ccr, @0x12345678")
        ->regname("ccr")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff");
    insert(mnem("stc", 8, "\x01\x41\x6b\xa0\x00\x00\x00\x00", "\xff\xff\xff\xff\x00\x00\x00\x00"))
        ->help("Store EXR in address given by 32-bit absolute: stc exr, @aa:32")
        ->example("stc exr, @0x12345678")
        ->regname("exr")
        ->abs("\x00\x00\x00\x00\xff\xff\xff\xff");

    // TODO: STM

    // SUB
    insert(mnem("sub.b", 2, "\x18\x00", "\xff\x00"))
        ->help("Subtract 8-bit register from 8-bit register: sub.b rs, rd")
        ->example("sub.b r4l, r2h")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("sub.w", 4, "\x79\x30\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Subtract word immediate from 16-bit register: sub.w #xx:16, rd")
        ->example("sub.w #0x1234, r3")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("sub.w", 2, "\x19\x00", "\xff\x00"))
        ->help("Subtract 16-bit register from 16-bit register: sub.w rs, rd")
        ->example("sub.w e4, r3")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("sub.l", 6, "\x7a\x30\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("Subtract long immediate from 32-bit register: sub.l #xx;32, erd")
        ->example("sub.l #0x12345678, er0")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("sub.l", 2, "\x1a\x80", "\xff\x88"))
        ->help("Subtract 32-bit register from 32-bit register: sub.l ers, erd")
        ->example("sub.l er4, er5")
        ->regh83_32("\x00\x70")
        ->regh83_32("\x00\x07");

    // SUBS
    insert(mnem("subs", 2, "\x1b\x00", "\xff\xf8"))
        ->help("Subtract 1 from 32-bit register: subs #1, erd")
        ->example("subs #1, er2")
        ->h83_const(1)
        ->regh83_32("\x00\x07");
    insert(mnem("subs", 2, "\x1b\x80", "\xff\xf8"))
        ->help("Subtract 2 from 32-bit register: subs #2, erd")
        ->example("subs #2, er5")
        ->h83_const(2)
        ->regh83_32("\x00\x07");
    insert(mnem("subs", 2, "\x1b\x90", "\xff\xf8"))
        ->help("Subtract 4 from 32-bit register: subs #4, erd")
        ->example("subs #4, er0")
        ->h83_const(4)
        ->regh83_32("\x00\x07");

    // SUBX
    insert(mnem("subx", 2, "\xb0\x00", "\xf0\x00"))
        ->help("Subtract with carry byte immediate from 8-bit register: subx #xx:8, rd")
        ->example("subx #0x12, r2l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("subx", 2, "\x1e\x00", "\xff\x00"))
        ->help("Subtract with carry 16-bit register from 16-bit register: subx rs, rd")
        ->example("subx r7h, r2l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");

    // TRAPA
    insert(mnem("trapa", 2, "\x57\x00", "\xff\xcf"))
        ->help("Start trap exception handling: trapa #xx:2")
        ->example("trapa #1")
        ->imm("\x00\x30");

    // XOR
    insert(mnem("xor.b", 2, "\xd0\x00", "\xf0\x00"))
        ->help("XOR byte immediate with 8-bit register: xor.b #xx:8, rd")
        ->example("xor.b #0x12, r5l")
        ->imm("\x00\xff")
        ->regh83_8("\x0f\x00");
    insert(mnem("xor.b", 2, "\x15\x00", "\xff\x00"))
        ->help("XOR 8-bit register with 8-bit register: xor.b rs, rd")
        ->example("xor.b r1h, r5l")
        ->regh83_8("\x00\xf0")
        ->regh83_8("\x00\x0f");
    insert(mnem("xor.w", 4, "\x79\x50\x00\x00", "\xff\xf0\x00\x00"))
        ->help("XOR word immediate with 16-bit register: xor.w #xx:16, rd")
        ->example("xor.w #0x1234, r5")
        ->imm("\x00\x00\xff\xff")
        ->regh83_16("\x00\x0f\x00\x00");
    insert(mnem("xor.w", 2, "\x65\x00", "\xff\x00"))
        ->help("XOR 16-bit register with 16-bit register: xor.w rs, rd")
        ->example("xor.w r1, r5")
        ->regh83_16("\x00\xf0")
        ->regh83_16("\x00\x0f");
    insert(mnem("xor.l", 6, "\x7a\x50\x00\x00\x00\x00", "\xff\xf8\x00\x00\x00\x00"))
        ->help("XOR long immediate with 32-bit register: xor.l #xx:32, rd")
        ->example("xor.l #0x12345678, er2")
        ->imm("\x00\x00\xff\xff\xff\xff")
        ->regh83_32("\x00\x07\x00\x00\x00\x00");
    insert(mnem("xor.l", 4, "\x01\xf0\x65\x00", "\xff\xff\xff\x88"))
        ->help("XOR 32-bit register with 32-bit register: xor.l ers, erd")
        ->example("xor.l er1, er5")
        ->regh83_32("\x00\x00\x00\x70")
        ->regh83_32("\x00\x00\x00\x07");

    // XORC
    insert(mnem("xorc", 2, "\x05\x00", "\xff\x00"))
        ->help("XOR CCR with byte immediate: xorc #xx:8, ccr")
        ->example("xorc #0x12, ccr")
        ->imm("\x00\xff")
        ->regname("ccr");
    insert(mnem("xorc", 4, "\x01\x41\x05\x00", "\xff\xff\xff\x00"))
        ->help("XOR EXR with byte immediate: xorc #xx:8, exr")
        ->example("xorc #0x12, exr")
        ->imm("\x00\x00\x00\xff")
        ->regname("exr");
};

// H83 Constant Operand.
GAParameterH83Const::GAParameterH83Const(unsigned int value) {
    const_value = value;
}

int GAParameterH83Const::match(GAParserOperand *op, int len) {
    if (op->prefix != prefix || op->suffix != suffix)
        return 0;

    if (op->value.toInt() != const_value)
        return 0;

    return 1;
}

QString GAParameterH83Const::decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) {
    return QString::asprintf("#%d", (unsigned int)const_value);
}

void GAParameterH83Const::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) {
    rawencode(lang,adr,bytes,op,inslen,const_value);
}

// H83 8-Bit Register Access Decoding.
GAParameterH83Reg8::GAParameterH83Reg8(const char* mask, bool indirect) {
    setMask(mask);
    if (indirect)
        prefix = "@";
}

int GAParameterH83Reg8::match(GAParserOperand *op, int len) {
    if (op->prefix != this->prefix || op->suffix != this->suffix) {
        return 0;
    }

    for ( int i = 0b0000 ; i <= 0b1111 ; i++ ) {
        if (op->value == regnames[i])
            return 1;
    }

    return 0;
}

QString GAParameterH83Reg8::decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) {
    uint64_t p = rawdecode(lang, adr, bytes, inslen);
    assert(p<16);    // Number of 8-bit registers: R0H-R7H,R0L-R7H.

    QString rendering = prefix + regnames[p] + suffix;

    return rendering;
}

void GAParameterH83Reg8::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) {
    uint64_t val;

    QString v = op.value;
    for ( int i = 0b0000 ; i <= 0b1111 ; i++ ) {
        if ( v == regnames[i] )
            val = i;
    }

    rawencode(lang, adr, bytes, op, inslen, val);
}

// H83 16-Bit Register Access Decoding.
GAParameterH83Reg16::GAParameterH83Reg16(const char* mask, bool indirect) {
    setMask(mask);
    if (indirect)
        prefix = "@";
}

int GAParameterH83Reg16::match(GAParserOperand *op, int len) {
    if (op->prefix != this->prefix || op->suffix != this->suffix) {
        return 0;
    }

    for ( int i = 0b0000 ; i <= 0b1111 ; i++ ) {
        if (op->value == regnames[i])
            return 1;
    }

    return 0;
}

QString GAParameterH83Reg16::decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) {
    uint64_t p = rawdecode(lang, adr, bytes, inslen);
    assert(p<16);    // Number of 16-bit registers: R0-R7,E0-E7.

    QString rendering = prefix + regnames[p] + suffix;

    return rendering;
}

void GAParameterH83Reg16::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) {
    uint64_t val;

    QString v = op.value;
    for ( int i = 0b0000 ; i <= 0b1111 ; i++ ) {
        if ( v == regnames[i] )
            val = i;
    }

    rawencode(lang, adr, bytes, op, inslen, val);
}

// H83 32-Bit Register Access Decoding.
GAParameterH83Reg32::GAParameterH83Reg32(const char* mask, bool indirect, bool postinc, bool predec) {
    setMask(mask);
    if (indirect)
        prefix = "@";
    // @ERs+
    if (postinc) {
        prefix = "@";
        suffix = "+";
    }
    // @-ERs
    if (predec)
        prefix = "@-";
}

int GAParameterH83Reg32::match(GAParserOperand *op, int len) {
    if (op->prefix != this->prefix || op->suffix != this->suffix) {
        return 0;
    }

    for ( int i = 0b000 ; i <= 0b111 ; i++ ) {
        if (op->value == regnames[i])
            return 1;
    }

    return 0;
}

QString GAParameterH83Reg32::decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) {
    uint64_t p = rawdecode(lang, adr, bytes, inslen);
    assert(p<8);    // Number of 32-bit registers: ER0-ER7.

    QString rendering = prefix + regnames[p] + suffix;

    return rendering;
}

void GAParameterH83Reg32::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) {
    uint64_t val;

    QString v = op.value;
    for ( int i = 0b000 ; i <= 0b111 ; i++ ) {
        if ( v == regnames[i] )
            val = i;
    }

    rawencode(lang, adr, bytes, op, inslen, val);
}
