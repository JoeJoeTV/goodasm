#include "galang6502.h"

#include "gamnemonic.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

/* This class represents the 6502 with only legal, documented
 * instructions.  An extension might later support undocumented
 * additions.
 *
 * Opcodes taken from here:
 * https://www.pagetable.com/c64ref/6502/?tab=2#
 */

GALang6502::GALang6502() {
    endian=LITTLE;
    name="6502";
    maxbytes=3;

    /* Fraction of legal instructions.
     * C64 basic.bin  is 0.941673
     * C64 kernal.bin is 0.956533
     */
    threshold=0.90;

    GAMnemonic *m=0;
    GAParameterGroup *pg=0;

    //Register names.
    regnames.clear();
    regnames<<"x"<<"y"<<"a";



    //ADC variants
    insert(mnem("adc", 2, "\x69\x00", "\xff\x00"))
        ->help("Add to A with Carry. (immediate)")
        ->example("adc #0xff")
        ->imm("\x00\xff");
    insert(mnem("adc", 3, "\x6d\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (absolute)")
        ->example("adc @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("adc", 3, "\x7d\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (X-indexed absolute)")
        ->example("adc @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("adc", 3, "\x79\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (Y-indexed absolute)")
        ->example("adc @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("adc", 2, "\x65\x00", "\xff\x00"))
        ->help("Add to A with Carry. (zero-page)")
        ->example("adc @0x35")
        ->abs("\x00\xff");
    insert(mnem("adc", 2, "\x75\x00", "\xff\x00"))
        ->help("Add to A with Carry. (X-indexed zero-page)")
        ->example("adc @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("adc", 2, "\x61\x00", "\xff\x00"))
            ->help("Add to A with Carry. (X-Indexed Zero Page Indirect)")
            ->example("adc (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("adc", 2, "\x71\x00", "\xff\x00"))
            ->help("Add to A with Carry. (Zero Page Indirect Y-Indexed)")
            ->example("adc (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");

    //AND variants
    insert(mnem("and", 2, "\x29\x00", "\xff\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and #0xff")
        ->imm("\x00\xff");
    insert(mnem("and", 3, "\x2d\x00\x00", "\xff\x00\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("and", 3, "\x3d\x00\x00", "\xff\x00\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("and", 3, "\x39\x00\x00", "\xff\x00\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("and", 2, "\x25\x00", "\xff\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and @0x35")
        ->abs("\x00\xff");
    insert(mnem("and", 2, "\x35\x00", "\xff\x00"))
        ->help("AND Memory with Accumulator")
        ->example("and @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("and", 2, "\x21\x00", "\xff\x00"))
            ->example("and (@0x35, x)")
            ->help("AND Memory with Accumulator");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("and", 2, "\x31\x00", "\xff\x00"))
            ->example("and (@0x35), y")
            ->help("AND Memory with Accumulator");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");


    //ASL -- Arithmetic Shift Left
    insert(mnem("asl", 1, "\x0a", "\xff"))
        ->help("Arithmetic Shift Left")
        ->example("asl a")
        ->regname("a");
    insert(mnem("asl", 3, "\x0e\x00\x00", "\xff\x00\x00"))
        ->help("Arithmetic Shift Left")
        ->example("asl @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("asl", 3, "\x1e\x00\x00", "\xff\x00\x00"))
        ->help("Arithmetic Shift Left")
        ->example("asl @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("asl", 2, "\x06\x00", "\xff\x00"))
        ->help("Arithmetic Shift Left")
        ->example("asl @0x35")
        ->abs("\x00\xff");
    insert(mnem("asl", 2, "\x16\x00", "\xff\x00"))
        ->help("Arithmetic Shift Left")
        ->example("asl @0x35, x")
        ->absx("\x00\xff");

    // Branches
    insert(mnem("bcc", 2, "\x90\x00", "\xff\x00"))
        ->help("Branch on Carry Clear")
        ->example("loop: bcc loop")
        ->rel("\x00\xff",2); // Relative, offset to next instruction.
    insert(mnem("bcs", 2, "\xb0\x00", "\xff\x00"))
        ->help("Branch on Carry Set")
        ->example("loop: bcs loop")
        ->rel("\x00\xff",2);
    insert(mnem("beq", 2, "\xf0\x00", "\xff\x00"))
        ->help("Branch on Result Zero")
        ->example("loop: beq loop")
        ->rel("\x00\xff",2);
    insert(mnem("bmi", 2, "\x30\x00", "\xff\x00"))
        ->help("Branch on Result Minus")
        ->example("loop: bmi loop")
        ->rel("\x00\xff",2);
    insert(mnem("bne", 2, "\xd0\x00", "\xff\x00"))
        ->help("Branch on Result Not Zero")
        ->example("loop: bne loop")
        ->rel("\x00\xff",2);
    insert(mnem("bpl", 2, "\x10\x00", "\xff\x00"))
        ->help("Branch on Result Plus")
        ->example("loop: bpl loop")
        ->rel("\x00\xff",2);
    insert(mnem("bvc", 2, "\x50\x00", "\xff\x00"))
        ->help("Branch on Overlfow Clear")
        ->example("loop: bvc loop")
        ->rel("\x00\xff",2);
    insert(mnem("bvs", 2, "\x70\x00", "\xff\x00"))
        ->help("Branch on Overflow Set")
        ->example("loop: bvs loop")
        ->rel("\x00\xff",2);


    // Bit Immediate Test with Accumulator
    insert(mnem("bit", 3, "\x2c\x00\x00", "\xff\x00\x00"))
        ->help("Bit Immediate Test with A (Absolute)")
        ->example("bit @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("bit", 2, "\x24\x00", "\xff\x00"))
        ->help("Bit Immediate Test with A (Zero Page)")
        ->example("bit @0x35")
        ->abs("\x00\xff");

    //Break Command
    insert(mnem("brk", 1, "\x00", "\xff"))
        ->help("Break Command")
        ->example("brk");

    //Clear instructions.
    insert(mnem("clc", 1, "\x18", "\xff"))
        ->help("Clear Carry Flag")
        ->example("clc");
    insert(mnem("cld", 1, "\xd8", "\xff"))
        ->help("Clear Decimal Mode")
        ->example("cld");
    insert(mnem("cli", 1, "\x58", "\xff"))
        ->help("Clear Interrupt Disable")
        ->example("cli");
    insert(mnem("clv", 1, "\xb8", "\xff"))
        ->help("Clear Overflow Flag")
        ->example("clv");

    //Compare Memory and Accumulator
    insert(mnem("cmp", 2, "\xc9\x00", "\xff\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp #0xff")
        ->imm("\x00\xff");
    insert(mnem("cmp", 3, "\xcd\x00\x00", "\xff\x00\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("cmp", 3, "\xdd\x00\x00", "\xff\x00\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("cmp", 3, "\xd9\x00\x00", "\xff\x00\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("cmp", 2, "\xc5\x00", "\xff\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp @0x35")
        ->abs("\x00\xff");
    insert(mnem("cmp", 2, "\xd5\x00", "\xff\x00"))
        ->help("Compare with Accumulator")
        ->example("cmp @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("cmp", 2, "\xc1\x00", "\xff\x00"))
            ->help("Compare with Accumulator")
            ->example("cmp (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("cmp", 2, "\xd1\x00", "\xff\x00"))
            ->help("Compare with Accumulator")
            ->example("cmp (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");


    // CPX -- Compare Index Register X to Memory
    insert(mnem("cpx", 2, "\xe0\x00", "\xff\x00"))
        ->help("Compare X to Memory")
        ->example("cpx #0xff")
        ->imm("\x00\xff");
    insert(mnem("cpx", 3, "\xec\x00\x00", "\xff\x00\x00"))
        ->help("Compare X to Memory")
        ->example("cpx @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("cpx", 2, "\xe4\x00", "\xff\x00"))
        ->help("Compare X to Memory")
        ->example("cpx @0x35")
        ->abs("\x00\xff");

    // CPY -- Compare Index Register Y to Memory
    insert(mnem("cpy", 2, "\xc0\x00", "\xff\x00"))
        ->help("Compare Y to Memory")
        ->example("cpy #0xff")
        ->imm("\x00\xff");
    insert(mnem("cpy", 3, "\xcc\x00\x00", "\xff\x00\x00"))
        ->help("Compare Y to Memory")
        ->example("cpy @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("cpy", 2, "\xc4\x00", "\xff\x00"))
        ->help("Compare Y to Memory")
        ->example("cpy @0x35")
        ->abs("\x00\xff");

    // DEC -- Decrement Memory by One
    insert(mnem("dec", 3, "\xce\x00\x00", "\xff\x00\x00"))
        ->help("Decrement Memory by One")
        ->example("dec @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("dec", 3, "\xde\x00\x00", "\xff\x00\x00"))
        ->help("Decrement Memory by One")
        ->example("dec @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("dec", 2, "\xc6\x00", "\xff\x00"))
        ->help("Decrement Memory by One")
        ->example("dec @0x35")
        ->abs("\x00\xff");
    insert(mnem("dec", 2, "\xd6\x00", "\xff\x00"))
        ->help("Decrement Memory by One")
        ->example("dec @0x35, x")
        ->absx("\x00\xff");

    // Decrement registers by one.
    insert(mnem("dex", 1, "\xca", "\xff"))
        ->help("Decrement Index Register X by One")
        ->example("dex");
    insert(mnem("dey", 1, "\x88", "\xff"))
        ->help("Decrement Index Register Y by One")
        ->example("dey");

    // EOR -- Exclusive OR Memory with Accumulator
    insert(mnem("eor", 2, "\x49\x00", "\xff\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor #0xff")
        ->imm("\x00\xff");
    insert(mnem("eor", 3, "\x4d\x00\x00", "\xff\x00\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("eor", 3, "\x5d\x00\x00", "\xff\x00\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("eor", 3, "\x59\x00\x00", "\xff\x00\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("eor", 2, "\x45\x00", "\xff\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor @0x35")
        ->abs("\x00\xff");
    insert(mnem("eor", 2, "\x55\x00", "\xff\x00"))
        ->help("Exclusive OR with Accumulator")
        ->example("eor @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("eor", 2, "\x41\x00", "\xff\x00"))
            ->help("Exclusive OR with Accumulator")
            ->example("eor (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("eor", 2, "\x51\x00", "\xff\x00"))
            ->help("Exclusive OR with Accumulator")
            ->example("eor (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");

    // INC -- Increment Memory by One
    insert(mnem("inc", 3, "\xee\x00\x00", "\xff\x00\x00"))
        ->help("Increment Memory by One")
        ->example("inc @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("inc", 3, "\xfe\x00\x00", "\xff\x00\x00"))
        ->help("Increment Memory by One")
        ->example("inc @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("inc", 2, "\xe6\x00", "\xff\x00"))
        ->help("Increment Memory by One")
        ->example("inc @0x35")
        ->abs("\x00\xff");
    insert(mnem("inc", 2, "\xf6\x00", "\xff\x00"))
        ->help("Increment Memory by One")
        ->example("inc @0x35, x")
        ->absx("\x00\xff");

    // Increment registers by one.
    insert(mnem("inx", 1, "\xe8", "\xff"))
        ->help("Increment Index Register X by One")
        ->example("inx");
    insert(mnem("iny", 1, "\xc8", "\xff"))
        ->help("Increment Index Register Y by One")
        ->example("iny");

    // JMP -- Jump
    insert(mnem("jmp", 3, "\x4c\x00\x00", "\xff\x00\x00"))
        ->help("Jump")
        ->example("jmp 0xdead")
        ->adr("\x00\xff\xff");
    insert(mnem("jmp", 3, "\x6c\x00\x00", "\xff\x00\x00"))
        ->help("Indirect Jump")
        ->example("jmp @0xdead")
        ->abs("\x00\xff\xff");

    // JSR -- Jump to Subroutine
    insert(mnem("jsr", 3, "\x20\x00\x00", "\xff\x00\x00"))
        ->help("Jump to Subroutine")
        ->example("jsr 0xdead")
        ->adr("\x00\xff\xff");


    // LDA -- Load Accumulator with Memory
    insert(mnem("lda", 2, "\xa9\x00", "\xff\x00"))
        ->help("Load Accumulator")
        ->example("lda #0xff")
        ->imm("\x00\xff");
    insert(mnem("lda", 3, "\xad\x00\x00", "\xff\x00\x00"))
        ->help("Load Accumulator with Memory")
        ->example("lda @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("lda", 3, "\xbd\x00\x00", "\xff\x00\x00"))
        ->help("Load Accumulator with Memory")
        ->example("lda @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("lda", 3, "\xb9\x00\x00", "\xff\x00\x00"))
        ->help("Load Accumulator with Memory")
        ->example("lda @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("lda", 2, "\xa5\x00", "\xff\x00"))
        ->help("Load Accumulator with Memory")
        ->example("lda @0x35")
        ->abs("\x00\xff");
    insert(mnem("lda", 2, "\xb5\x00", "\xff\x00"))
        ->help("Load Accumulator with Memory")
        ->example("lda @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("lda", 2, "\xa1\x00", "\xff\x00"))
            ->help("Load Accumulator with Memory")
            ->example("lda (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("lda", 2, "\xb1\x00", "\xff\x00"))
            ->help("Load Accumulator with Memory")
            ->example("lda (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");


    // LDX -- Load Index Register X from Memory
    insert(mnem("ldx", 2, "\xa2\x00", "\xff\x00"))
        ->help("Load X")
        ->example("ldx #0xff")
        ->imm("\x00\xff");
    insert(mnem("ldx", 3, "\xae\x00\x00", "\xff\x00\x00"))
        ->help("Load X from Memory")
        ->example("ldx @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("ldx", 3, "\xbe\x00\x00", "\xff\x00\x00"))
        ->help("Load X from Memory")
        ->example("ldx @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("ldx", 2, "\xa6\x00", "\xff\x00"))
        ->help("Load X from Memory")
        ->example("ldx @0x35")
        ->abs("\x00\xff");
    insert(mnem("ldx", 2, "\xb6\x00", "\xff\x00"))
        ->help("Load X from Memory")
        ->example("ldx @0x35, y")
        ->absy("\x00\xff");


    // LDY -- Load Index Register Y from Memory
    insert(mnem("ldy", 2, "\xa0\x00", "\xff\x00"))
        ->help("Load Y")
        ->example("ldy #0xff")
        ->imm("\x00\xff");
    insert(mnem("ldy", 3, "\xac\x00\x00", "\xff\x00\x00"))
        ->help("Load Y from Memory")
        ->example("ldy @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("ldy", 3, "\xbc\x00\x00", "\xff\x00\x00"))
        ->help("Load Y from Memory")
        ->example("ldy @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("ldy", 2, "\xa4\x00", "\xff\x00"))
        ->help("Load Y from Memory")
        ->example("ldy @0x35")
        ->abs("\x00\xff");
    insert(mnem("ldy", 2, "\xb4\x00", "\xff\x00"))
        ->help("Load Y from Memory")
        ->example("ldy @0x35, x")
        ->absx("\x00\xff");

    // LSR -- Logical Shift Right
    insert(mnem("lsr", 1, "\x4a", "\xff"))
        ->help("Logical Shift Right")
        ->example("lsr a")
        ->regname("a");
    insert(mnem("lsr", 3, "\x4e\x00\x00", "\xff\x00\x00"))
        ->help("Logical Shift Right")
        ->example("lsr @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("lsr", 3, "\x5e\x00\x00", "\xff\x00\x00"))
        ->help("Logical Shift Right")
        ->example("lsr @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("lsr", 2, "\x46\x00", "\xff\x00"))
        ->help("Logical Shift Right")
        ->example("lsr @0x35")
        ->abs("\x00\xff");
    insert(mnem("lsr", 2, "\x56\x00", "\xff\x00"))
        ->help("Logical Shift Right")
        ->example("lsr @0x35, x")
        ->absx("\x00\xff");

    // NOP -- No Operation
    insert(mnem("nop", 1, "\xea", "\xff"))
        ->help("No Operation")
        ->example("nop");


    // OR A -- OR Memory with Accumulator
    insert(mnem("ora", 2, "\x09\x00", "\xff\x00"))
        ->help("OR with Accumulator")
        ->example("ora #0xff")
        ->imm("\x00\xff");
    insert(mnem("ora", 3, "\x0d\x00\x00", "\xff\x00\x00"))
        ->help("OR with Accumulator")
        ->example("ora @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("ora", 3, "\x1d\x00\x00", "\xff\x00\x00"))
        ->help("OR with Accumulator")
        ->example("ora @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("ora", 3, "\x19\x00\x00", "\xff\x00\x00"))
        ->help("OR with Accumulator")
        ->example("ora @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("ora", 2, "\x05\x00", "\xff\x00"))
        ->help("OR with Accumulator")
        ->example("ora @0x35")
        ->abs("\x00\xff");
    insert(mnem("ora", 2, "\x15\x00", "\xff\x00"))
        ->help("OR with Accumulator")
        ->example("ora @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("ora", 2, "\x01\x00", "\xff\x00"))
            ->help("OR with Accumulator")
            ->example("ora (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("ora", 2, "\x11\x00", "\xff\x00"))
            ->help("OR with Accumulator")
            ->example("ora (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");

    //Stack Operations
    insert(mnem("pha", 1, "\x48", "\xff"))
        ->help("Push Accumulator on Stack")
        ->example("pha");
    insert(mnem("php", 1, "\x08", "\xff"))
        ->help("Push Processor Status on Stack")
        ->example("php");
    insert(mnem("pla", 1, "\x68", "\xff"))
        ->help("Pull Accumultor from Stack")
        ->example("pla");
    insert(mnem("plp", 1, "\x28", "\xff"))
        ->help("Pull Processor Status from Stack")
        ->example("plp");


    // Rotate Left
    insert(mnem("rol", 1, "\x2a", "\xff"))
        ->help("Rotate Left")
        ->example("rol a")
        ->regname("a");
    insert(mnem("rol", 3, "\x2e\x00\x00", "\xff\x00\x00"))
        ->help("Rotate Left")
        ->example("rol @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("rol", 3, "\x3e\x00\x00", "\xff\x00\x00"))
        ->help("Rotate Left")
        ->example("rol @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("rol", 2, "\x26\x00", "\xff\x00"))
        ->help("Rotate Left")
        ->example("rol @0x35")
        ->abs("\x00\xff");
    insert(mnem("rol", 2, "\x36\x00", "\xff\x00"))
        ->help("Rotate Left")
        ->example("rol @0x35, x")
        ->absx("\x00\xff");


    //Rotate Right
    insert(mnem("ror", 1, "\x6a", "\xff"))
        ->help("Rotate Right")
        ->example("ror a")
        ->regname("a");
    insert(mnem("ror", 3, "\x6e\x00\x00", "\xff\x00\x00"))
        ->help("Rotate Right")
        ->example("ror @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("ror", 3, "\x7e\x00\x00", "\xff\x00\x00"))
        ->help("Rotate Right")
        ->example("ror @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("ror", 2, "\x66\x00", "\xff\x00"))
        ->help("Rotate Right")
        ->example("ror @0x35")
        ->abs("\x00\xff");
    insert(mnem("ror", 2, "\x76\x00", "\xff\x00"))
        ->help("Rotate Right")
        ->example("ror @0x35, x")
        ->absx("\x00\xff");

    //Returns
    insert(mnem("rti", 1, "\x40", "\xff"))
        ->help("Return from Interrupt")
        ->example("rti");
    insert(mnem("rts", 1, "\x60", "\xff"))
        ->help("Return from Subroutine")
        ->example("rts");

    // SBC -- Subtract Memory from Accumulator with Borrow
    insert(mnem("sbc", 2, "\xe9\x00", "\xff\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc #0xff")
        ->imm("\x00\xff");
    insert(mnem("sbc", 3, "\xed\x00\x00", "\xff\x00\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("sbc", 3, "\xfd\x00\x00", "\xff\x00\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("sbc", 3, "\xf9\x00\x00", "\xff\x00\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("sbc", 2, "\xe5\x00", "\xff\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc @0x35")
        ->abs("\x00\xff");
    insert(mnem("sbc", 2, "\xf5\x00", "\xff\x00"))
        ->help("Subtract from Accumulator")
        ->example("sbc @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("sbc", 2, "\xe1\x00", "\xff\x00"))
            ->help("Subtract from Accumulator")
            ->example("sbc (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("sbc", 2, "\xf1\x00", "\xff\x00"))
            ->help("Subtract from Accumulator")
            ->example("sbc (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");


    // Setting modes
    insert(mnem("sec", 1, "\x38", "\xff"))
        ->help("Set Carry Flag")
        ->example("sec");
    insert(mnem("sed", 1, "\xf8", "\xff"))
        ->help("Set Decimal Mode")
        ->example("sed");
    insert(mnem("sei", 1, "\x78", "\xff"))
        ->help("Set Interupt Disable")
        ->example("sei");

    // STA -- Store Accumulator in Memory
    insert(mnem("sta", 3, "\x8d\x00\x00", "\xff\x00\x00"))
        ->help("Store Accumulator to Memory")
        ->example("sta @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("sta", 3, "\x9d\x00\x00", "\xff\x00\x00"))
        ->help("Store Accumulator to Memory")
        ->example("sta @0xdead, x")
        ->absx("\x00\xff\xff");
    insert(mnem("sta", 3, "\x99\x00\x00", "\xff\x00\x00"))
        ->help("Store Accumulator to Memory")
        ->example("sta @0xdead, y")
        ->absy("\x00\xff\xff");
    insert(mnem("sta", 2, "\x85\x00", "\xff\x00"))
        ->help("Store Accumulator to Memory")
        ->example("sta @0x35")
        ->abs("\x00\xff");
    insert(mnem("sta", 2, "\x95\x00", "\xff\x00"))
        ->help("Store Accumulator to Memory")
        ->example("sta @0x35, x")
        ->absx("\x00\xff");
    m=insert(mnem("sta", 2, "\x81\x00", "\xff\x00"))
            ->help("Store Accumulator to Memory")
            ->example("sta (@0x35, x)");
    pg=m->group('(');
    pg->absx("\x00\xff");
    m=insert(mnem("sta", 2, "\x91\x00", "\xff\x00"))
            ->help("Store Accumulator to Memory")
            ->example("sta (@0x35), y");
    pg=m->group('(');
    pg->abs("\x00\xff");
    m->regname("y");



    // STX -- Store Index Register X to Memory
    insert(mnem("stx", 3, "\x8e\x00\x00", "\xff\x00\x00"))
        ->help("Store X to Memory")
        ->example("stx @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("stx", 2, "\x86\x00", "\xff\x00"))
        ->help("Store X to Memory")
        ->example("stx @0x35")
        ->abs("\x00\xff");
    insert(mnem("stx", 2, "\x96\x00", "\xff\x00"))
        ->help("Store X to Memory")
        ->example("stx @0x35, y")
        ->absy("\x00\xff");

    // STY -- Store Index Register Y to Memory
    insert(mnem("sty", 3, "\x8c\x00\x00", "\xff\x00\x00"))
        ->help("Store Y to Memory")
        ->example("sty @0xdead")
        ->abs("\x00\xff\xff");
    insert(mnem("sty", 2, "\x84\x00", "\xff\x00"))
        ->help("Store Y to Memory")
        ->example("sty @0x35")
        ->abs("\x00\xff");
    insert(mnem("sty", 2, "\x94\x00", "\xff\x00"))
        ->help("Store Y to Memory")
        ->example("sty @0x35, x")
        ->absx("\x00\xff");

    // Transfer instructions
    insert(mnem("tax", 1, "\xaa", "\xff"))
        ->help("Transfer Accumulator to X")
        ->example("tax");
    insert(mnem("tay", 1, "\xa8", "\xff"))
        ->help("Transfer Accumulator to Y")
        ->example("tay");
    insert(mnem("tsx", 1, "\xba", "\xff"))
        ->help("Transfer Stack Pointer to X")
        ->example("tsx");
    insert(mnem("txa", 1, "\x8a", "\xff"))
        ->help("Transfer X to Accumulator")
        ->example("txa");
    insert(mnem("txs", 1, "\x9a", "\xff"))
        ->help("Transfer X to Stack Pointer")
        ->example("txs");
    insert(mnem("tya", 1, "\x98", "\xff"))
        ->help("Transfer Y to Accumulator")
        ->example("tya");
}
