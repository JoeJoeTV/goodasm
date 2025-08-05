#include "galangst7.h"
#include "gamnemonic.h"


#define mnem new GAMnemonic

/* ST7 is largely a superset of 6805, but it adds some new opcode prefixes:
 * 90 -- Indexing by Y instead of by X.
 * 92 -- Double-indirect forms.
 * 91 -- Double-indirect on Y.
 */


//15 opcode arithmetic instruction, destination A.
void GALangST7::buildArithmetic15(uint8_t opcode,
                                  QString name,
                                  QString help,
                                  bool mode6805){
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
                                  QString help,
                                  bool mode6805){
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
GALangST7::GALangST7() {
    endian=BIG;
    name="st7";
    maxbytes=3;

    //Register names.
    regnames.clear();
    regnames<<"x"<<"y"<<"a";



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
    insert(mnem("bset", 2, "\x10\x00", "\xf1\x00"))
        ->help("Bit Set")
        ->example("bset @0xff, bit4")
        ->abs("\x00\xff")
        ->bit3("\x0e\x00");
    //BTJF
    //BTJT
    //CALL
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
    //MUL
    buildArithmetic11(0x00, "neg", "Negate");
    insert(mnem("nop", "\x9d"))->help("No Operation");
    buildArithmetic15(0x0a, "or", "or");
    //POP
    //PUSH
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






    /*
    //First half inherited from 6805.
    insert(mnem("adc", 2, "\xa9\x00", "\xff\x00"))            //Immediate
        ->help("Add to A with Carry. (imm)")
        ->example("adc a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("adc", 2, "\xb9\x00", "\xff\x00"))            //Short
        ->help("Add to A with Carry. (dir)")
        ->example("adc a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("adc", 3, "\xc9\x00\x00", "\xff\x00\x00"))    //Long
        ->help("Add to A with Carry. (ext)")
        ->example("adc a, @0xffff")
        ->regname("a")
        ->abs("\x00\xff\xff");
    insert(mnem("adc", 1, "\xf9", "\xff"))
        ->help("Add to A with Carry. (ix)")
        ->example("adc a, @x")
        ->regname("a")
        ->regnameind("x");
    insert(mnem("adc", 2, "\xe9\x00", "\xff\x00"))
        ->help("Add to A with Carry. (ix1)")
        ->example("adc a, (0xff, x)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\xff")
        ->regname("x");
    insert(mnem("adc", 3, "\xd9\x00\x00", "\xff\x00\x00"))
        ->help("Add to A with Carry. (ix2)")
        ->example("adc a, (0xffff, x)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\xff\xff")
        ->regname("x");
    //90 prefix, indexing by Y.
    insert(mnem("adc", 2, "\x90\xf9", "\xff\xff"))
        ->help("Add to A with Carry. (iy)")
        ->example("adc a, @y")
        ->regname("a")
        ->regnameind("y");
    insert(mnem("adc", 3, "\x90\xe9\x00", "\xff\xff\x00"))
        ->help("Add to A with Carry. (iy1)")
        ->example("adc a, (0xff, y)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\x00\xff")
        ->regname("y");
    insert(mnem("adc", 4, "\x90\xd9\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add to A with Carry. (iy2)")
        ->example("adc a, (0xffff, y)")
        ->regname("a")
        ->group('(')
        ->adr("\x00\x00\xff\xff")
        ->regname("y");



    //92 prefix, double-indirection
    insert(mnem("adc", 3, "\x92\xb9\x00", "\xff\xff\x00"))
        ->help("Add to A with Carry. (indir)")
        ->example("adc a, (@0xff)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff");
    insert(mnem("adc", 4, "\x92\xc9\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add to A with Carry. (indext)")
        ->example("adc a, (@0xffff)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff");
    insert(mnem("adc", 3, "\x92\xe9\x00", "\xff\xff\x00"))
        ->help("Add to A with Carry. (ind-ix1)")
        ->example("adc a, (@0xff, x)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("x");
    insert(mnem("adc", 4, "\x92\xd9\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add to A with Carry. (ind-ix2)")
        ->example("adc a, (@0xffff, x)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("x");
    //91 prefix, double-indirection on Y.
    insert(mnem("adc", 3, "\x91\xe9\x00", "\xff\xff\x00"))
        ->help("Add to A with Carry. (ind-iy1)")
        ->example("adc a, (@0xff, y)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff")
        ->regname("y");
    insert(mnem("adc", 4, "\x91\xd9\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add to A with Carry. (ind-ixy)")
        ->example("adc a, (@0xffff, y)")
        ->regname("a")
        ->group('(')
        ->abs("\x00\x00\xff\xff")
        ->regname("y");
*/
}
