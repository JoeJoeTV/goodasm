#include "galangscpu2.h"
#include "gamnemonic.h"

#include <QDebug>

//Just to keep things shorter.
#define mnem new GAMnemonic
//8-bit register.  &HL is handled elsewhere by overloads.
#define scpu2reg8(x) insert(new GAParameterSCPU2Reg8((x)))
//16-bit register for most instructions.
#define scpu2reg16(x) insert(new GAParameterSCPU2Reg16((x),false))
//16-bit register for push and pop, with AF in place of SP.
#define scpu2reg16stk(x) insert(new GAParameterSCPU2Reg16((x),true))
#define scpu2cond(x) insert(new GAParameterSCPU2Cond((x)))
#define scpu2rsthandler(x) insert(new GAParameterSCPU2ResetHandler((x)))


// This is a handy overview if you get lost.
// https://gbdev.io/pandocs/CPU_Instruction_Set.html


GALangSCPU2::GALangSCPU2() {
    GAMnemonic* m;
    endian=LITTLE;  // Higher bits come in earlier bytes.
    name="scpu2";
    maxbytes=3;


    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        "b" << "c" << "d" << "e" <<
        "f" << "h" << "l" << "a" <<

        "bc" << "de" << "hl" << "sp" << "af"
        ;


    //Chapter 2, page 11 of gbctr.pdf.
    //In examples, we use 'b' for a generic 8-bit register.


    //ld r, r'
    //This overlaps with ld r, @hl.  Priority sorts that out.
    insert(mnem("ld", 1, "\x40", "\xc0"))
        ->help("Copies one register's value into another.")
        ->example("ld b, b")
        ->scpu2reg8("\x38")
        ->scpu2reg8("\x07");

    //ld r, #imm
    insert(mnem("ld", 2, "\x06\x00", "\xc7\x00"))
        ->help("Loads an immediate into a reg8.")
        ->example("ld b, #0xff")
        ->scpu2reg8("\x38\x00")
        ->imm("\x00\xff");
    //ld r, @HL
    insert(mnem("ld", 1, "\x46", "\xc7"))
        ->help("Loads register with the value in memory at HL.")
        ->example("ld b, @hl")
        ->prioritize()
        ->scpu2reg8("\x38")
        ->regnameind("hl");
    //ld @HL, r
    insert(mnem("ld", 1, "\x70\x00", "\xf8\x00"))
        ->help("Loads memory at HL with the value from a register")
        ->example("ld @hl, b")
        ->prioritize()
        ->regnameind("hl")
        ->scpu2reg8("\x07\x00");
    //ld @HL, @0xff
    insert(mnem("ld", 2, "\x36\x00", "\xff\x00"))
        ->help("Loads memory at HL with an immediate.")
        ->example("ld @hl, #0xff")
        ->prioritize()
        ->regnameind("hl")
        ->imm("\x00\xff");
    //ld a, @bc
    insert(mnem("ld", 1, "\x0a\x00", "\xff\x00"))
        ->help("Loads the accumulator with the value at BC.")
        ->example("ld a, @bc")
        ->regname("a")
        ->regnameind("bc");
    //ld a, @de
    insert(mnem("ld", 1, "\x1a\x00", "\xff\x00"))
        ->help("Loads the accumulator with the value at DE.")
        ->example("ld a, @de")
        ->regname("a")
        ->regnameind("de");
    //ld @bc, a
    insert(mnem("ld", 1, "\x02", "\xff"))
        ->help("Loads the value at BC with the value in the accumulator.")
        ->example("ld @bc, a")
        ->regnameind("bc")
        ->regname("a");

    //Page 13

    //ld @de, a
    insert(mnem("ld", 1, "\x12\x00", "\xff\x00"))
        ->help("Loads the value at DE with the value in the accumulator.")
        ->example("ld @de, a")
        ->regnameind("de")
        ->regname("a");
    //ld a, @0xdead
    insert(mnem("ld", 3, "\xfa\x00\x00", "\xff\x00\x00"))
        ->help("Loads the accumulator with byte at an absolute address.")
        ->example("ld a, @0xdead")
        ->regname("a")
        ->abs("\x00\xff\xff");
    //ld @0xdead, a
    insert(mnem("ld", 3, "\xea\x00\x00", "\xff\x00\x00"))
        ->help("Loads to the absolute address the value in the accumulator.")
        ->example("ld @0xdead, a")
        ->abs("\x00\xff\xff")
        ->regname("a");

    //Page 14

    /* FIXME: LDH instructions get weird because 8 bits are an offset
     * added to 0xFF00.  We only support marking this offset in the
     * number and not with the register names.
     */

    insert(mnem("ldh", 1, "\xf2", "\xff"))
        ->help("Load A from mem[0xff00+C].")
        ->example("ldh a, @c")
        ->regname("a")
        ->regnameind("c");
    insert(mnem("ldh", 1, "\xe2", "\xff"))
        ->help("Load mem[0xff00+C] from A.")
        ->example("ldh @c, a")
        ->regnameind("c")
        ->regname("a");
    insert(mnem("ldh", 2, "\xf0\x00", "\xff\x00"))
        ->help("Load A with mem[0xff00+abs8].")
        ->example("ldh a, @0xffff")
        ->regname("a")
        ->abs("\x00\xff", 0xff00);
    insert(mnem("ldh", 2, "\xe0\x00", "\xff\x00"))
        ->help("Load mem[0xff00+abs8] with A.")
        ->example("ldh @0xffff, a")
        ->abs("\x00\xff", 0xff00)
        ->regname("a");

    // Page 15

    /* This becomes tricky because we need a post-decrement
     * of an indirect register.  Using grouping symbol
     * to avoid ambiguity, but this need support in the parser.
     */

    m=insert(mnem("ld", 1, "\x3a", "\xff"))
                        ->help("Load A with mem[HL], then decrement HL.")
                        ->example("ld a, (hl-)");
    m->regname("a");
    GAParameterGroup* g=m->group('(');
    g->regname("hl","","-");


    m=insert(mnem("ld", 1, "\x32", "\xff"))
                        ->help("Load mem[HL] with A, then decrement HL.")
                        ->example("ld (hl-), a");
    g=m->group('(');
    g->regname("hl","","-");
    m->regname("a");

    // Page 16

    m=insert(mnem("ld", 1, "\x2a", "\xff"))
                        ->help("Load A with mem[HL], then increment HL.")
                        ->example("ld a, (hl+)");
    m->regname("a");
    g=m->group('(');
    g->regname("hl","","+");


    m=insert(mnem("ld", 1, "\x22", "\xff"))
            ->help("Load mem[HL] with A, then decrement HL.")
            ->example("ld (hl+), a");
    g=m->group('(');
    g->regname("hl","","+");
    m->regname("a");


    insert(mnem("ld", 3, "\x01\x00\x00", "\xcf\x00\x00"))
        ->help("Load r16 with 16-bit immediate.")
        ->example("ld sp, #0xdead")
        ->scpu2reg16("\x30\x00\x00")
        ->imm("\x00\xff\xff");

    // Overload with instruction above to also allow addresses to be written to 16-bit register pairs
    insert(mnem("ld", 3, "\x01\x00\x00", "\xcf\x00\x00"))
        ->help("Load r16 with 16-bit address.")
        ->example("ld sp, 0xdead")
        ->scpu2reg16("\x30\x00\x00")
        ->adr("\x00\xff\xff");


    insert(mnem("ld", 3, "\x08\x00\x00", "\xff\x00\x00"))
        ->help("Load to 16-bit address the value of stack pointer.")
        ->example("ld @0xdead, sp")
        ->abs("\x00\xff\xff")
        ->regname("sp");

    insert(mnem("ld", 1, "\xf9", "\xff"))
        ->help("Load to SP the value of HL.")
        ->example("ld sp, hl")
        ->regname("sp")
        ->regname("hl");

    insert(mnem("push", 1, "\xc5", "\xcf"))
        ->help("Push a reg16stk to the stack.")
        ->example("push af")
        ->scpu2reg16stk("\x30");

    // Page 44 (nee 18)

    insert(mnem("pop", 1, "\xc1", "\xcf"))
        ->help("Pop a reg16stk from the stack.")
        ->example("pop af")
        ->scpu2reg16stk("\x30");
    insert(mnem("ld", 2, "\xf8\x00", "\xff\x00"))
        ->help("Loads SP+simm8 into HL.")
        ->example("ld hl, sp, #-15")
        ->regname("hl")
        ->regname("sp")
        ->simm("\x00\xff");

    //Page 19
    insert(mnem("add", 1, "\x80", "\xf8"))
        ->help("Adds a reg8 into the accumulator.")
        ->example("add b")
        ->scpu2reg8("\x07");
    insert(mnem("add", 1, "\x86", "\xff"))
        ->help("Adds mem[HL] into A.")
        ->example("add @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("add", 2, "\xc6\x00", "\xff\x00"))
        ->help("Adds imm8 into A.")
        ->example("add #0x35")
        ->imm("\x00\xff");

    //Page 20
    insert(mnem("adc", 1, "\x88", "\xf8"))
        ->help("Adds a reg8 and carry into A.")
        ->example("adc b")
        ->scpu2reg8("\x07");
    insert(mnem("adc", 1, "\x8e", "\xff"))
        ->help("Adds mem[HL] with carry into A.")
        ->example("adc @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("adc", 2, "\xce\x00", "\xff\x00"))
        ->help("Adds imm8 and carry into A.")
        ->example("adc #0x35")
        ->imm("\x00\xff");


    //Just like ADD.
    insert(mnem("sub", 1, "\x90", "\xf8"))
        ->help("Subtracts a reg8 out of A.")
        ->example("sub b")
        ->scpu2reg8("\x07");
    insert(mnem("sub", 1, "\x96", "\xff"))
        ->help("Subtracts mem[HL] out of A.")
        ->example("sub @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("sub", 2, "\xd6\x00", "\xff\x00"))
        ->help("Subtracts imm8 out of A.")
        ->example("sub #0x35")
        ->imm("\x00\xff");

    //Just like ADC
    insert(mnem("sbc", 1, "\x98", "\xf8"))
        ->help("Subtract w/C a reg8 out of A.")
        ->example("sbc b")
        ->scpu2reg8("\x07");
    insert(mnem("sbc", 1, "\x9e", "\xff"))
        ->help("Subtract w/C mem[HL] out of A.")
        ->example("sbc @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("sbc", 2, "\xde\x00", "\xff\x00"))
        ->help("Subtract w/C imm8 out of A.")
        ->example("sbc #0x35")
        ->imm("\x00\xff");

    //Page 23

    //Compare instructions, like SUB except they don't update A.
    insert(mnem("cp", 1, "\xb8", "\xf8"))
        ->help("Compare reg8 to A.")
        ->example("cp b")
        ->scpu2reg8("\x07");
    insert(mnem("cp", 1, "\xbe", "\xff"))
        ->help("Compare mem[HL] to A.")
        ->example("cp @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("cp", 2, "\xfe\x00", "\xff\x00"))
        ->help("Compare imm8 to A.")
        ->example("cp #0x35")
        ->imm("\x00\xff");

    //Page 25.

    //Increments.
    insert(mnem("inc", 1, "\x04", "\xc7"))
        ->help("Increment reg8.")
        ->example("inc b")
        ->scpu2reg8("\x38");
    insert(mnem("inc", 1, "\x34", "\xff"))
        ->help("Increment mem[HL].")
        ->example("inc @hl")
        ->prioritize()
        ->regnameind("hl");



    //Decrements
    insert(mnem("dec", 1, "\x35", "\xff"))
        ->help("Decrement mem[HL].")
        ->example("dec @hl")
        ->prioritize()          //More important than "dec b" when matching.
        ->regnameind("hl");
    insert(mnem("dec", 1, "\x05", "\xc7"))
        ->help("Decrement reg8.")
        ->example("dec b")
        ->scpu2reg8("\x38");


    // Page 26

    //Bitwise AND.
    insert(mnem("and", 1, "\xa0", "\xf8"))
        ->help("A&=reg8")
        ->example("and b")
        ->scpu2reg8("\x07");
    insert(mnem("and", 1, "\xa6", "\xff"))
        ->help("A&=mem[HL]")
        ->example("and @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("and", 2, "\xe6\x00", "\xff\x00"))
        ->help("A&=imm8")
        ->example("and #0x35")
        ->imm("\x00\xff");


    //OR
    insert(mnem("or", 1, "\xb0", "\xf8"))
        ->help("A|=reg8")
        ->example("or b")
        ->scpu2reg8("\x07");
    insert(mnem("or", 1, "\xb6", "\xff"))
        ->help("A|=mem[HL]")
        ->example("or @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("or", 2, "\xf6\x00", "\xff\x00"))
        ->help("A|=imm8")
        ->example("or #0x35")
        ->imm("\x00\xff");

    //XOR
    insert(mnem("xor", 1, "\xa8", "\xf8"))
        ->help("A^=reg8")
        ->example("xor b")
        ->scpu2reg8("\x07");
    insert(mnem("xor", 1, "\xae", "\xff"))
        ->help("A^=mem[HL]")
        ->example("xor @hl")
        ->prioritize()
        ->regnameind("hl");
    insert(mnem("xor", 2, "\xee\x00", "\xff\x00"))
        ->help("A^=imm8")
        ->example("xor #0x35")
        ->imm("\x00\xff");

    //Page 30

    insert(mnem("ccf", 1, "\x3f", "\xff"))
        ->help("Complement Carry Flag")
        ->example("ccf");
    insert(mnem("scf", 1, "\x37", "\xff"))
        ->help("Set Carry Flag")
        ->example("scf");
    // SCPU2 does not support DAA
    // insert(mnem("daa", 1, "\x27", "\xff"))
    //     ->help("Decimal Adjust Accumulator")
    //     ->example("daa");
    insert(mnem("cpl", 1, "\x2f", "\xff"))
        ->help("Complement Accumulator")
        ->example("cpl");

    //page 32

    //Jump instructions
    insert(mnem("jp", 3, "\xc3\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional Jump to a 16-bit address.")
        ->example("jp 0xdead")
        ->adr("\x00\xff\xff");
    insert(mnem("jp", 1, "\xe9", "\xff"))
        ->help("Jump to address in HL.")
        ->example("jp hl")
        ->regname("hl");
    insert(mnem("jp", 3, "\xc2\x00\x00", "\xe7\x00\x00"))
        ->help("Conditional (NZ,Z,NC,C) Jump to a 16-bit address.")
        ->example("jp nz, 0xdead")
        ->scpu2cond("\x18\x00\x00")
        ->adr("\x00\xff\xff");
    insert(mnem("jr", 2, "\x18\x00", "\xff\x00"))
        ->help("Relative Jump to an 8-bit offset.")
        ->example("jr 0xde")
        ->rel("\x00\xff", 2);  //Offset confirmed against IDA Pro.
    insert(mnem("jr", 2, "\x20\x00", "\xe7\x00"))
        ->help("Conditional (NZ,Z,NC,C) Relative Jump to an 8-bit offset.")
        ->example("jr nz, 0xde")
        ->scpu2cond("\x18\x00\x00")
        ->rel("\x00\xff", 2);  //Offset confirmed against IDA Pro.

    // Page 34

    //Calls
    insert(mnem("call", 3, "\xcd\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional call to a 16-bit address.")
        ->example("call 0xdead")
        ->adr("\x00\xff\xff");
    insert(mnem("call", 3, "\xc4\x00\x00", "\xe7\x00\x00"))
        ->help("Conditional call to a 16-bit address.")
        ->example("call nz, 0xdead")
        ->scpu2cond("\x18\x00\x00")
        ->adr("\x00\xff\xff");


    //Returns
    insert(mnem("ret", 1, "\xc9", "\xff"))
        ->help("Return from a function call.")
        ->example("ret");
    insert(mnem("ret", 1, "\xc0", "\xe7"))
        ->help("Conditional return.")
        ->example("ret nz")
        ->scpu2cond("\x18");
    insert(mnem("reti", 1, "\xd9", "\xff"))
        ->help("Return from an interrupt handler.")
        ->example("reti");

    //Page 36 / 123.
    insert(mnem("rst", 1, "\xc7", "\xc7"))
        ->help("Resets to a fixed handler. (FIXME)")
        ->example("rst 0x18") // Opcode of 0xdf.
        ->scpu2rsthandler("\x38")  // This should *not* be shifted like a regular parameter.
        ;


    insert(mnem("halt", 1, "\x76", "\xff"))
        ->help("Stops the CPU. Clocks still run. 1-byte version.")
        ->prioritize(2)     // Takes position of "ld @hl, @hl"
        ->example("halt");

    // SCPU2 does not support this instruction
    // insert(mnem("stop", 1, "\x10", "\xff"))
    //     ->help("Stops the CPU.")
    //     ->prioritize()
    //     ->example("stop");
    insert(mnem("nop", 1, "\x00", "\xff"))
        ->help("No operation.")
        ->prioritize()
        ->example("nop");

    insert(mnem("di", 1, "\xf3", "\xff"))
        ->help("Disable interrupts.")
        ->prioritize()
        ->example("di");
    insert(mnem("ei", 1, "\xfb", "\xff"))
        ->help("Enable interrupts.")
        ->prioritize()
        ->example("ei");


    //16-bit arithmetic.
    insert(mnem("inc", 1, "\x03", "\xcf"))
        ->help("Increment reg16.")
        ->example("inc bc")
        ->scpu2reg16("\x30");
    insert(mnem("dec", 1, "\x0b", "\xcf"))
        ->help("Decrement reg16.")
        ->example("dec bc")
        ->scpu2reg16("\x30");

    insert(mnem("add", 1, "\x09", "\xcf"))
        ->help("Add a reg16 into HL.")
        ->example("add hl, bc")
        ->regname("hl")
        ->scpu2reg16("\x30");
    insert(mnem("add", 2, "\xe8\x00", "\xff\x00"))
        ->help("Add a simm8 into reg16.")
        ->example("add sp, #-0x16")
        ->regname("sp")
        ->simm("\x00\xff");

    //Page 83

    insert(mnem("rlca", 1, "\x07", "\xff"))
        ->help("Rotate Left Circular Accumulator")
        ->example("rlca");

    insert(mnem("rrca", 1, "\x0f", "\xff"))
        ->help("Rotate Right Circular Accumulator")
        ->example("rrca");
    insert(mnem("rla", 1, "\x17", "\xff"))
        ->help("Rotate Left Accumulator")
        ->example("rla");
    insert(mnem("rra", 1, "\x1f", "\xff"))
        ->help("Rotate Right Accumulator")
        ->example("rra");

    //Page 87
    insert(mnem("rlc", 2, "\xcb\x00", "\xff\xf8"))
        ->help("Rotate Left Circular Register")
        ->example("rlc b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("rlc", 2, "\xcb\x06", "\xff\xff"))
        ->help("Rotate Left Circular mem[HL]")
        ->example("rlc @hl")
        ->prioritize()      //Prevents conflict with 'rlc h'
        ->regnameind("hl");
    insert(mnem("rrc", 2, "\xcb\x08", "\xff\xf8"))
        ->help("Rotate Right Circular Register")
        ->example("rrc b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("rrc", 2, "\xcb\x0e", "\xff\xff"))
        ->help("Rotate Right Circular mem[HL]")
        ->example("rrc @hl")
        ->prioritize()      //Prevents conflict with 'rrc h'
        ->regnameind("hl");


    insert(mnem("rl", 2, "\xcb\x10", "\xff\xf8"))
        ->help("Rotate Left Register")
        ->example("rl b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("rl", 2, "\xcb\x16", "\xff\xff"))
        ->help("Rotate Left mem[HL]")
        ->example("rl @hl")
        ->prioritize()      //Prevents conflict with 'rl l'
        ->regnameind("hl");
    insert(mnem("rr", 2, "\xcb\x18", "\xff\xf8"))
        ->help("Rotate Right Register")
        ->example("rr b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("rr", 2, "\xcb\x1e", "\xff\xff"))
        ->help("Rotate Right mem[HL]")
        ->example("rr @hl")
        ->prioritize()      //Prevents conflict with 'rr l'
        ->regnameind("hl");

    insert(mnem("sla", 2, "\xcb\x20", "\xff\xf8"))
        ->help("Shift Left Arithmetic Register")
        ->example("sla b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("sla", 2, "\xcb\x26", "\xff\xff"))
        ->help("Shift Left Arithmetic mem[HL]")
        ->example("sla @hl")
        ->prioritize()      //Prevents conflict with 'sla l'
        ->regnameind("hl");
    insert(mnem("sra", 2, "\xcb\x28", "\xff\xf8"))
        ->help("Shift Right Arithmetic Register")
        ->example("sra b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("sra", 2, "\xcb\x2e", "\xff\xff"))
        ->help("Shift Right Arithmetic mem[HL]")
        ->example("sra @hl")
        ->prioritize()      //Prevents conflict with 'sra l'
        ->regnameind("hl");

    //Page 99
    insert(mnem("swap", 2, "\xcb\x30", "\xff\xf8"))
        ->help("Swap Nybbles Register")
        ->example("swap b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("swap", 2, "\xcb\x36", "\xff\xff"))
        ->help("Swap Nybbles mem[HL]")
        ->example("swap @hl")
        ->prioritize()      //Prevents conflict with 'swap h'
        ->regnameind("hl");

    insert(mnem("srl", 2, "\xcb\x38", "\xff\xf8"))
        ->help("Shift Right Logical Register")
        ->example("srl b")
        ->scpu2reg8("\x00\x07");
    insert(mnem("srl", 2, "\xcb\x3e", "\xff\xff"))
        ->help("Shift Right Logical mem[HL]")
        ->example("srl @hl")
        ->prioritize()      //Prevents conflict with 'srl h'
        ->regnameind("hl");


    //page 103
    insert(mnem("bit", 2, "\xcb\x40", "\xff\xc0"))
        ->help("Bit Immediate Test of a Register")
        ->example("bit bit1, b")
        ->bit3("\x00\x38")
        ->scpu2reg8("\x00\x07");
    insert(mnem("res", 2, "\xcb\x80", "\xff\xc0"))
        ->help("Bit Reset of a Register")
        ->example("res bit1, b")
        ->bit3("\x00\x38")
        ->scpu2reg8("\x00\x07");
    insert(mnem("set", 2, "\xcb\xc0", "\xff\xc0"))
        ->help("Bit Set of a Register")
        ->example("set bit1, b")
        ->bit3("\x00\x38")
        ->scpu2reg8("\x00\x07");


    // MARK: prefix 2 extension (vram load instructions)

    insert(mnem("vld", 2, "\xd3\x02", "\xff\xff"))
        ->help("Loads the value at BC (VRAM) with the value in the accumulator.")
        ->example("vld @bc, a")
        ->regnameind("bc")
        ->regname("a");
    
    insert(mnem("vld", 2, "\xd3\x0a", "\xff\xff"))
        ->help("Loads the accumulator with the value at BC (VRAM).")
        ->example("vld a, @bc")
        ->regname("a")
        ->regnameind("bc");
    
    insert(mnem("vld", 2, "\xd3\x12", "\xff\xff"))
        ->help("Loads the value at DE (VRAM) with the value in the accumulator.")
        ->example("vld @de, a")
        ->regnameind("de")
        ->regname("a");
    
    insert(mnem("vld", 2, "\xd3\x1a", "\xff\xff"))
        ->help("Loads the accumulator with the value at DE (VRAM).")
        ->example("vld a, @de")
        ->regname("a")
        ->regnameind("de");

    m=insert(mnem("vld", 2, "\xd3\x22", "\xff\xff"))
            ->help("Load vram[HL] with A, then decrement HL.")
            ->example("vld (hl+), a");
    g=m->group('(');
    g->regname("hl","","+");
    m->regname("a");

    m=insert(mnem("vld", 2, "\xd3\x2a", "\xff\xff"))
                        ->help("Load A with vram[HL], then increment HL.")
                        ->example("vld a, (hl+)");
    m->regname("a");
    g=m->group('(');
    g->regname("hl","","+");

    m=insert(mnem("vld", 2, "\xd3\x32", "\xff\xff"))
                        ->help("Load vram[HL] with A, then decrement HL.")
                        ->example("vld (hl-), a");
    g=m->group('(');
    g->regname("hl","","-");
    m->regname("a");

    insert(mnem("vld", 3, "\xd3\x36\x00", "\xff\xff\x00"))
        ->help("Loads vram at HL with an immediate.")
        ->example("vld @hl, #0xff")
        ->prioritize()
        ->regnameind("hl")
        ->imm("\x00\x00\xff");

    m=insert(mnem("vld", 2, "\xd3\x3a", "\xff\xff"))
                        ->help("Load A with vram[HL], then decrement HL.")
                        ->example("vld a, (hl-)");
    m->regname("a");
    g=m->group('(');
    g->regname("hl","","-");

    insert(mnem("vld", 2, "\xd3\x46", "\xff\xc7"))
        ->help("Loads register with the value in vram at HL.")
        ->example("vld b, @hl")
        ->prioritize()
        ->scpu2reg8("\x00\x38")
        ->regnameind("hl");

    insert(mnem("vld", 2, "\xd3\x70", "\xff\xf8"))
        ->help("Loads vram at HL with the value from a register")
        ->example("vld @hl, b")
        ->prioritize()
        ->regnameind("hl")
        ->scpu2reg8("\x00\x07");

    insert(mnem("vldh", 3, "\xd3\xe0\x00", "\xff\xff\x00"))
        ->help("Load vram[0xff00+abs8] with A.")
        ->example("vldh @0xffff, a")
        ->abs("\x00\x00\xff", 0xff00)
        ->regname("a");

    insert(mnem("vldh", 2, "\xd3\xe2", "\xff\xff"))
        ->help("Load vram[0xff00+C] from A.")
        ->example("vldh @c, a")
        ->regnameind("c")
        ->regname("a");
    
    insert(mnem("vld", 4, "\xd3\xea\x00\x00", "\xff\xff\x00\x00"))
        ->help("Loads to the absolute address (VRAM) the value in the accumulator.")
        ->example("vld @0xdead, a")
        ->abs("\x00\x00\xff\xff")
        ->regname("a");

    insert(mnem("vldh", 3, "\xd3\xf0\x00", "\xff\xff\x00"))
        ->help("Load A with vram[0xff00+abs8].")
        ->example("vldh a, @0xffff")
        ->regname("a")
        ->abs("\x00\x00\xff", 0xff00);

    insert(mnem("vldh", 2, "\xd3\xf2", "\xff\xff"))
        ->help("Load A from vram[0xff00+C].")
        ->example("vldh a, @c")
        ->regname("a")
        ->regnameind("c");

    insert(mnem("vld", 4, "\xd3\xfa\x00\x00", "\xff\xff\x00\x00"))
        ->help("Loads the accumulator with byte at an absolute address (VRAM).")
        ->example("vld a, @0xdead")
        ->regname("a")
        ->abs("\x00\x00\xff\xff");
}

/* SCPU2 has rather unique register names.  We define support here,
 * and use the preprocessor so that they needn't be added to the
 * GAParameterGroup class. */
GAParameterSCPU2Reg8::GAParameterSCPU2Reg8(const char* mask){
    setMask(mask);
}
int GAParameterSCPU2Reg8::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(op->value.length()!=1)
        return 0; //Wrong size.
    char c=op->value[0].toLatin1();
    switch(c){
        //FIXME: The SCPU2 Register Order might be B, C, D, E, H, L, [HL], A
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'h':
    case 'l':
    case 'a':
        return 1;  //Legal register name.
    default:
        return 0;  //Not an 8-bit register name.
    }
}

QString GAParameterSCPU2Reg8::decode(GALanguage *lang,
                                    uint64_t adr, const char *bytes,
                                    int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<8); //Reg count on this architecture.

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameterSCPU2Reg8::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen){
    uint64_t val;

    QString v=op.value;
    for(int i=0; i<8; i++)
        if(v==regnames[i]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}

/* These are the same, except for 16-bit register pairs.
 */


GAParameterSCPU2Reg16::GAParameterSCPU2Reg16(const char* mask, bool stk){
    setMask(mask);
    if(stk)
        regnames[3]="af";
}
int GAParameterSCPU2Reg16::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(op->value.length()!=2)
        return 0; //Wrong size.

    //This set is right for r16, wrong for r16stk, r16mem.
    for(int i=0; i<4; i++)
        if(op->value==regnames[i]) return 1;
    return 0;
}

QString GAParameterSCPU2Reg16::decode(GALanguage *lang,
                                    uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<4); //Reg count on this architecture.
    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameterSCPU2Reg16::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen){
    uint64_t val;
    for(int i=0; i<4; i++){
        if(op.value==regnames[i]) val=i;
    }

    rawencode(lang,adr,bytes,op,inslen,val);
}


// Conditions in a jump.

GAParameterSCPU2Cond::GAParameterSCPU2Cond(const char* mask){
    setMask(mask);
}
int GAParameterSCPU2Cond::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.

    //Is the condition in the list?
    for(int i=0; i<4; i++)
        if(op->value==condnames[i]) return 1;
    return 0;
}

QString GAParameterSCPU2Cond::decode(GALanguage *lang,
                                     uint64_t adr, const char *bytes,
                                    int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<4); //Reg count on this architecture.
    QString rendering=prefix
                        +condnames[p]
                        +suffix;
    return rendering;
}
void GAParameterSCPU2Cond::encode(GALanguage *lang,
                                  uint64_t adr, QByteArray &bytes,
                                  GAParserOperand op,
                                  int inslen){
    uint64_t val;
    for(int i=0; i<4; i++){
        if(op.value==condnames[i]) val=i;
    }

    rawencode(lang,adr,bytes,op,inslen,val);
}

/* Restart handler addresses, which are oddly
 * like addresses except unshifted.
 */

GAParameterSCPU2ResetHandler::GAParameterSCPU2ResetHandler(const char* mask){
    setMask(mask);
}
int GAParameterSCPU2ResetHandler::match(GAParserOperand *op, int len){
    int64_t val=op->uint64(false); //False on a match.

    //Reject values that don't fit.
    if(val&(0xc7)){
        qDebug()<<"Not a valid SCPU2 restart target address: "<<op->value;
        return 0;
    }

    return 1;
}
QString GAParameterSCPU2ResetHandler::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen)
                 <<3;   //Shift to expand it.
    return QString::asprintf("0x%02x",(unsigned int) p);
}
void GAParameterSCPU2ResetHandler::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){
    int64_t val=op.uint64(true)
                  >>3;   //Shift to shrink it.
    rawencode(lang,adr,bytes,op,inslen,val);
}
