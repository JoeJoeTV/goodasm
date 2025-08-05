#include "galangst7.h"
#include "gamnemonic.h"


#define mnem new GAMnemonic

/* ST7 is largely a superset of 6805, but it adds some new opcode prefixes:
 * 90 -- Indexing by Y instead of by X.
 * 92 -- Double-indirect forms.
 * 91 -- Double-indirect on Y.
 *
 * In addition to the prefixes, opcodes have been added for pushing and
 * popping some registers.
 *
 * Y is a new index register, never existing on 6805.
 */

//CALL and JP.
void GALangST7::buildCallJump(uint8_t opcode,
                              QString name,
                              QString help){
    /* Opcode is C for JP and D for CALL.
     * We exit early in 6805 mode to avoid
     * extra definitions.
     */
    assert((opcode&0xf)==opcode);

    char modeb[]="\xb0\x00";
    modeb[0]|=opcode;
    insert(mnem(name, 2, modeb, "\xff\x00"))  //short
        ->help(help)
        ->example(name+" 0xab")
        ->adr("\x00\xff");
    char modec[]="\xc0\x00\x00";
    modec[0]|=opcode;
    insert(mnem(name, 3, modec, "\xff\x00\x00"))  //long
        ->help(help)
        ->example(name+" 0xdead")
        ->adr("\x00\xff\xff");
    char modef[]="\xf0";
    modef[0]|=opcode;
    insert(mnem(name, 1, modef, "\xff"))
        ->help(help)
        ->example(name+" @x")
        ->regnameind("x");
    char modee[]="\xe0\x00";
    modee[0]|=opcode;
    insert(mnem(name, 2, modee, "\xff\x00"))
        ->help(help)
        ->example(name+" (0xed, x)")
        ->group('(')
        ->adr("\x00\xff")
        ->regname("x");
    char moded[]="\xd0\x00\x00";
    moded[0]|=opcode;
    insert(mnem(name, 3, moded, "\xff\x00"))
        ->help(help)
        ->example(name+" (0xdead, x)")
        ->group('(')
        ->adr("\x00\xff\xff")
        ->regname("x");


    //Bail out if we don't want extended opcodes.
    if(mode6805) return;

    char mode90f[]="\x90\xf0";
    mode90f[1]|=opcode;
    insert(mnem(name, 2, mode90f, "\xff\xff"))
        ->help(help)
        ->example(name+" @y")
        ->regnameind("y");
    char mode90e[]="\x90\xe0\x00";
    mode90e[1]|=opcode;
    insert(mnem(name, 3, mode90e, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (0xed, y)")
        ->group('(')
        ->adr("\x00\x00\xff")
        ->regname("y");
    char mode90d[]="\x90\xd0\x00\x00";
    mode90d[1]|=opcode;
    insert(mnem(name, 4, mode90d, "\xff\xff\x00\x00"))
        ->help(help)
        ->example(name+" (0xdead, y)")
        ->group('(')
        ->adr("\x00\x00\xff\xff")
        ->regname("y");

    char mode92b[]="\x92\xb0\x00";
    mode92b[1]|=opcode;
    insert(mnem(name, 3, mode92b, "\xff\xff\x00"))  //short
        ->help(help)
        ->example(name+" @0xab")
        ->abs("\x00\x00\xff");
    char mode92c[]="\x92\xc0\x00\x00";
    mode92c[1]|=opcode;
    insert(mnem(name, 4, mode92c, "\xff\xff\x00\x00"))  //long
        ->help(help)
        ->example(name+" @0xdead")
        ->abs("\x00\x00\xff\xff");
    char mode92e[]="\x92\xe0\x00";
    mode92e[1]|=opcode;
    insert(mnem(name, 3, mode92e, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (@0xed, x)")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("x");
    char mode92d[]="\x92\xd0\x00\x00";
    mode92d[1]|=opcode;
    insert(mnem(name, 4, mode92d, "\xff\xff\x00\x00"))
        ->help(help)
        ->example(name+" (@0xdead, x)")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("x");

    char mode91e[]="\x91\xe0\x00";
    mode91e[1]|=opcode;
    insert(mnem(name, 3, mode91e, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (@0xed, y)")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("y");
    char mode91d[]="\x91\xd0\x00\x00";
    mode91d[1]|=opcode;
    insert(mnem(name, 4, mode91d, "\xff\xff\x00\x00"))
        ->help(help)
        ->example(name+" (@0xdead, y)")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("y");
}

//PUSH and POP on ST17.  Not on 6805.
void GALangST7::buildStackOp(uint8_t opcode,
                             QString name,
                             QString help){
    /* 6805 has JSR and RTS, but ST7 extends that with
     * support for pushing and popping X, Y and the
     * condition codes.
     *
     * Opcode parameter is the lowest opcode:
     * 84 for pop, 88 for push.
     */
    if(mode6805) return;

    char str[2];
    str[0]=opcode++;
    insert(mnem(name, 1, str, "\xff"))
        ->help(help)
        ->example(name+" a")
        ->regname("a");
    str[0]=opcode;
    insert(mnem(name, 1, str, "\xff"))
        ->help(help)
        ->example(name+" x")
        ->regname("x");
    str[0]=0x90;
    str[1]=opcode++;
    insert(mnem(name, 2, str, "\xff\xff"))
        ->help(help)
        ->example(name+" y")
        ->regname("y");
    str[0]=opcode;
    insert(mnem(name, 1, str, "\xff"))
        ->help(help)
        ->example(name+" cc")
        ->regname("cc");
}


//15 opcode arithmetic instruction, destination A.
void GALangST7::buildArithmetic15(uint8_t opcode,
                                  QString name,
                                  QString help){
    /* The 15-opcode operations have the accumulator (A)
     * as the destination register.
     */


    assert(!(opcode&0xF0)); //Only lower nybble should be set.

    //First half inherited from 6805.
    char typea[]="\xa0\x00";  //A900 for ADC, for example.
    typea[0]|=opcode;
    insert(mnem(name, 2, typea, "\xff\x00"))            //Immediate
        ->help(help+" (imm)")
        ->example(name+" a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    char typeb[]="\xb0\x00";
    typeb[0]|=opcode;
    insert(mnem(name, 2, typeb, "\xff\x00"))            //Short
        ->help(help+" (dir)")
        ->example(name+" a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    char typec[]="\xc0\x00\x00";
    typec[0]|=opcode;
    insert(mnem(name, 3, typec, "\xff\x00\x00"))    //Long
        ->help(help+" (ext)")
        ->example(name+" a, @0xffff")
        ->regname("a")
        ->abs("\x00\xff\xff");
    char typef[]="\xf0";
    typef[0]|=opcode;
    insert(mnem(name, 1, typef, "\xff"))
        ->help(help+" (ix)")
        ->example(name+" a, @x")
        ->regname("a")
        ->regnameind("x");
    char typee[]="\xe0\x00";
    typee[0]|=opcode;
    insert(mnem(name, 2, typee, "\xff\x00"))
        ->help(help+" (ix1)")
        ->example(name+" a, (0xff, x)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\xff")
        ->regname("x");
    char typed[]="\xd0\x00\x00";
    typed[0]|=opcode;
    insert(mnem(name, 3, typed, "\xff\x00\x00"))
        ->help(help+" (ix2)")
        ->example(name+" a, (0xffff, x)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\xff\xff")
        ->regname("x");

    //Exit early if not using extended instructions.
    if(mode6805) return;

    //90 prefix, indexing by Y.
    char type90f[]="\x90\xf0";
    type90f[1]|=opcode;
    insert(mnem(name, 2, type90f, "\xff\xff"))
        ->help(help+" (iy)")
        ->example(name+" a, @y")
        ->regname("a")
        ->regnameind("y");
    char type90e[]="\x90\xe0\x00";
    type90e[1]|=opcode;
    insert(mnem(name, 3, type90e, "\xff\xff\x00"))
        ->help(help+" (iy1)")
        ->example(name+" a, (0xff, y)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\x00\xff")
        ->regname("y");
    char type90d[]="\x90\xd0\x00\x00";
    type90d[1]|=opcode;
    insert(mnem(name, 4, type90d, "\xff\xff\x00\x00"))
        ->help(help+" (iy2)")
        ->example(name+" a, (0xffff, y)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\x00\xff\xff")
        ->regname("y");
    //92 prefix, double-indirection
    char type92b[]="\x92\xb0\x00";
    type92b[1]|=opcode;
    insert(mnem(name, 3, type92b, "\xff\xff\x00"))
        ->help(help+" (indir)")
        ->example(name+" a, (@0xff)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff");
    char type92c[]="\x92\xc0\x00\x00";
    type92c[1]|=opcode;
    insert(mnem(name, 4, type92c, "\xff\xff\x00\x00"))
        ->help(help+" (indext)")
        ->example(name+" a, (@0xffff)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff");
    char type92e[]="\x92\xe0\x00";
    type92e[1]|=opcode;
    insert(mnem(name, 3, type92e, "\xff\xff\x00"))
        ->help(help+" (ind-ix1)")
        ->example(name+" a, (@0xff, x)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("x");
    char type92d[]="\x92\xd0\x00\x00";
    type92d[1]|=opcode;
    insert(mnem(name, 4, type92d, "\xff\xff\x00\x00"))
        ->help(help+" (ind-ix2)")
        ->example(name+" a, (@0xffff, x)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("x");
    //91 prefix, double-indirection on Y.
    char type91e[]="\x91\xe0\x00";
    type91e[1]|=opcode;
    insert(mnem(name, 3, type91e, "\xff\xff\x00"))
        ->help(help+" (ind-iy1)")
        ->example(name+" a, (@0xff, y)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("y");
    char type01d[]="\x91\xd0\x00\x00";
    type01d[1]|=opcode;
    insert(mnem(name, 4, type01d, "\xff\xff\x00\x00"))
        ->help(help+" (ind-ixy)")
        ->example(name+" a, (@0xffff, y)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("y");
}


//11 opcode arithmetic instruction.  (not A)
void GALangST7::buildArithmetic11(uint8_t opcode,
                                  QString name,
                                  QString help){
    /* The 11-opcode operations are things which have the
     * same source and destination.  Inversion, clearing,
     * etc.
     */

    assert(!(opcode&0xF0)); //Only lower nybble should be set.

    //First half inherited from 6805.
    char type4[]="\x40";
    type4[0]|=opcode;
    insert(mnem(name, 1, type4, "\xff"))
        ->help(help)
        ->example(name+" a")
        ->regname("a");
    char type5[]="\x50";
    type5[0]|=opcode;
    insert(mnem(name, 1, type5, "\xff"))
        ->help(help)
        ->example(name+" x")
        ->regname("x");

    char type3[]="\x30\x00";
    type3[0]|=opcode;
    insert(mnem(name, 2, type3, "\xff\x00"))
        ->help(help)
        ->example(name+" @0x00")
        ->abs("\x00\xff");

    char type7[]="\x70";
    type7[0]|=opcode;
    insert(mnem(name, 1, type7, "\xff"))
        ->help(help+" (ix)")
        ->example(name+" @x")
        ->regnameind("x");


    char type6[]="\x60\x00";
    type6[0]|=opcode;
    insert(mnem(name, 2, type6, "\xff\x00"))
        ->help(help)
        ->example(name+" (0xff, x)")
        ->group('(')
        ->adr("\x00\xff")
        ->regname("x");


    //Exit early if 6805 mode.
    if(mode6805) return;

    char type905[]="\x90\x50";
    type905[1]|=opcode;
    insert(mnem(name, 2, type905, "\xff\xff"))
        ->help(help)
        ->example(name+" y")
        ->regname("y");

    char type907[]="\x90\x70";
    type907[1]|=opcode;
    insert(mnem(name, 2, type907, "\xff\xff"))
        ->help(help)
        ->example(name+" @y")
        ->regnameind("y");

    char type906[]="\x90\x60\x00";
    type906[1]|=opcode;
    insert(mnem(name, 3, type906, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (0xff, y)")
        ->group('(')
        ->adr("\x00\x00\xff")
        ->regname("y");


    char type923[]="\x92\x30\x00";
    type923[1]|=opcode;
    insert(mnem(name, 3, type923, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (@0x00)")
        ->group('(')
        ->abs("\x00\x00\xff");

    char type926[]="\x92\x60\x00";
    type926[1]|=opcode;
    insert(mnem(name, 3, type926, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (@0xff, x)")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("x");

    char type916[]="\x91\x60\x00";
    type916[1]|=opcode;
    insert(mnem(name, 3, type916, "\xff\xff\x00"))
        ->help(help)
        ->example(name+" (@0xff, y)")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("y");


}


//ST7 is a superset of 6805, so we include all of the 6805 stuff.
GALangST7::GALangST7(bool mode6805) {
    this->mode6805=mode6805;
    endian=BIG;
    name="st7";
    maxbytes=4;

    //Register names.
    regnames.clear();
    regnames<<"a"<<"x";
    if(!mode6805)
        regnames<<"y"<<"cc";

    //ADC, page 33
    buildArithmetic15(0x09, "adc", "Add w/ Carry");
    buildArithmetic15(0x0b, "add", "Add");
    buildArithmetic15(0x04, "and", "Bitwise AND");
    buildArithmetic15(0x05, "bcp", "Logical Bit Compare");
    insert(mnem("bres", 2, "\x11\x00", "\xf1\x00"))
        ->help("Bit Reset")
        ->example("bres @0xff, bit4")
        ->abs("\x00\xff")
        ->bit3("\x0e\x00");
    if(!mode6805){
        auto a=insert(mnem("bres", 3, "\x92\x11\x00", "\xff\xf1\x00"))
            ->help("Bit Reset")
                     ->example("bres (@0xff), bit4");
        a->group('(')
            ->abs("\x00\x00\xff");
        a->bit3("\x00\x0e\x00");
    }
    insert(mnem("bset", 2, "\x10\x00", "\xf1\x00"))
        ->help("Bit Set")
        ->example("bset @0xff, bit4")
        ->abs("\x00\xff")
        ->bit3("\x0e\x00");
    if(!mode6805){
        auto a=insert(mnem("bset", 3, "\x92\x10\x00", "\xff\xf1\x00"))
        ->help("Bit Set")
            ->example("bset (@0xff), bit4");
        a->group('(')
            ->abs("\x00\x00\xff");
        a->bit3("\x00\x0e\x00");
    }
    //BTJF
    //BTJT
    //CALL
    buildCallJump(0x0d, "call", "Call Subroutine");
    //CALLR
    buildArithmetic11(0x0f, "clr", "Clear");
    buildArithmetic15(0x01, "cp", "Compare");
    buildArithmetic11(0x03, "cpl", "Logical 1-Complement");
    buildArithmetic11(0x0a, "dec", "Decrement");
    insert(mnem("halt", "\x8e"))->help("Stop Oscillator Until Interrupt");
    buildArithmetic11(0x0c, "inc", "Increment");
    insert(mnem("iret", "\x80"))->help("Interrupt Return");
    //JP
    //JRA
    //JRxx -- Conditional Jump
    buildArithmetic15(0x06, "ld", "Load");

    /* Mul isn't really a part of 6805, but we don't cut this
     * out in 6805 mode because many 6805 chips like the ST16C54
     * include mul a, x.
     */
    insert(mnem("mul", 1, "\x42", "\xff"))
        ->help("Multiply.")
        ->example("mul x, a")
        ->regname("x")
        ->regname("a");
    if(!mode6805)
        insert(mnem("mul", 2, "\x90\x42", "\xff\xff"))
            ->help("Multiply.")
            ->example("mul y, a")
            ->regname("y")
            ->regname("a");
    buildArithmetic11(0x00, "neg", "Negate");
    insert(mnem("nop", "\x9d"))->help("No Operation");
    buildArithmetic15(0x0a, "or", "or");

    buildStackOp(0x84, "pop", "Pop from Stack");
    buildStackOp(0x88, "push", "Push to Stack");

    insert(mnem("rcf", "\x98"))->help("Reset Carry Flag");
    insert(mnem("ret", "\x81"))->help("Return From Subroutine");
    insert(mnem("rim", "\x9a"))->help("Reset Interrupt Mask (IE)");
    buildArithmetic11(0x09, "rlc", "Rotate Left Logical through Carry");
    buildArithmetic11(0x06, "rrc", "Rotate Right Logical through Carry");
    insert(mnem("rsp", "\x9c"))->help("Reset Stack Pointer");
    buildArithmetic15(0x02, "sbc", "Subtraction with Carry");
    insert(mnem("scf", "\x99"))->help("Set Carry Flag");
    insert(mnem("sim", "\x9b"))->help("Set Interrupt Mask (DI)");
    buildArithmetic11(0x08, "sla", "Shift Left Arithmetic");
    //buildArithmetic11(0x08, "sll", "Shift Left Arithmetic");  //Synonym for SLA.
    buildArithmetic11(0x07, "sra", "Shift Right Arithmetic");
    buildArithmetic11(0x04, "srl", "Shift Right Logical");
    buildArithmetic15(0x00, "sub", "Subtraction");
    buildArithmetic11(0x0e, "swap", "Swap Nybbles");
    buildArithmetic11(0x0d, "tnz", "Test for Negative or Zero");
    insert(mnem("trap", "\x83"))->help("Software Interrupt");
    insert(mnem("wfi", "\x8f"))->help("Wait for Interrupt");
    buildArithmetic15(0x08, "xor", "Logical Exclusive Or");
}
