#include "galang8051.h"
#include "gaparameter.h"
#include "gamnemonic.h"
#include "goodasm.h"

#include <QDebug>

//Just to keep things shorter.
#define mnem new GAMnemonic
//11 bit address field, kinda like an in-page branch.
#define addr11(x) insert(new GAParameter8051Addr11((x)))
//3 bit register number
#define reg8051(x) insert(new GAParameter8051Reg((x), false))
//1 bit register number, indirected to the address in that reg.
#define reg8051ind(x) insert(new GAParameter8051Reg((x), true))
//Bits can be individually addressed by some instructions.  / prefix denotes inversion.
#define bitadr(x) insert(new GAParameter8051Bit((x), false))
#define bitadrnot(x) insert(new GAParameter8051Bit((x), true))

/* 8051 driver, based on the Microchip (née Atmel) documentation.
 *
 * https://ww1.microchip.com/downloads/en/DeviceDoc/doc4316.pdf
 *
 *
 * Oddities:
 * 1. Absolute Call (ACALL) takes low bits of address after PC increment, leaves high ones intact.
 * 2. "Bit addresses" are different from main memory space.  Inverted bits marked with / prefix.
 * 3. A5 opcode is intentionally unused.  It's a breakpoint in the TI/Chipcon implementation.
 */

GALang8051::GALang8051(){
    endian=BIG;  // Higher bits come in earlier bytes.
    name="8051";
    maxbytes=3;


    regnames.clear();
    regnames
            <<"a"<<"b"<<"c"
            <<"dptr"<<"pc"
            <<"r0"<<"r1"<<"r2"<<"r3"<<"r4"<<"r5"<<"r6"<<"r7"
        ;

    /* This won't work for 8051 because all opcodes except
     * A5 are meaningful.  Not yet sure what to do instead.
     */
    threshold=0.90;

    GAMnemonic *m=0;
    GAParameterGroup *pg=0;

    //Page 25 of Atmel 8051 Microcontrollers Hardware Manual

    //acall 0xdead
    insert(mnem("acall", 2, "\x11\x00", "\x1f\x00"))
        ->help("Calls a subroutine within 2kB.")
        ->example("loop: acall loop")
        ->addr11("\xe0\xff");

    //add a, Rn
    insert(mnem("add", 1, "\x28", "\xf8"))
        ->help("Adds a register into the Accumulator.")
        ->example("add a, r0")
        ->regname("a")
        ->reg8051("\x07");
    //add a, @dir
    insert(mnem("add", 2, "\x25\x00", "\xff\x00"))
        ->help("Adds an immediate byte to the Accumulator.")
        ->example("add a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    //add a, @Rn
    insert(mnem("add", 1, "\x26", "\xfe"))
        ->help("Adds a register into the Accumulator.")
        ->example("add a, @r0")
        ->regname("a")
        ->reg8051ind("\x01");
    //add a, #imm
    insert(mnem("add", 2, "\x24\x00", "\xff\x00"))
        ->help("Adds an immediate byte to the Accumulator.")
        ->example("add a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");



    //addc a, Rn
    insert(mnem("addc", 1, "\x38", "\xf8"))
        ->help("Adds a register into the Accumulator with carry.")
        ->example("addc a, r0")
        ->regname("a")
        ->reg8051("\x07");
    //addc a, @dir
    insert(mnem("addc", 2, "\x35\x00", "\xff\x00"))
        ->help("Adds an immediate byte to the Accumulator with carry.")
        ->example("addc a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    //addc a, @Rn
    insert(mnem("addc", 1, "\x36", "\xfe"))
        ->help("Adds a register into the Accumulator with carry.")
        ->example("addc a, @r0")
        ->regname("a")
        ->reg8051ind("\x01");
    //addc a, #imm
    insert(mnem("addc", 2, "\x34\x00", "\xff\x00"))
        ->help("Adds an immediate byte to the Accumulator with carry.")
        ->example("addc a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");



    //Page 28


    //acall 0xdead
    insert(mnem("ajmp", 2, "\x01\x00", "\x1f\x00"))
        ->help("Jumps within 2kB.")
        ->example("loop: ajmp loop")
        ->addr11("\xe0\xff");


    //Page 29


    //anl a, Rn
    insert(mnem("anl", 1, "\x58", "\xf8"))
        ->help("Logical AND of register into accumulator.")
        ->example("anl a, r0")
        ->regname("a")
        ->reg8051("\x07");
    //anl a, @dir
    insert(mnem("anl", 2, "\x55\x00", "\xff\x00"))
        ->help("Logical AND of absolute byte into accumulator.")
        ->example("anl a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    //anl a, @Rn
    insert(mnem("anl", 1, "\x56", "\xfe"))
        ->help("Logical AND of byte at mem[r0] or mem[r1] into A.")
        ->example("anl a, @r0")
        ->regname("a")
        ->reg8051ind("\x01");
    //anl a, #imm
    insert(mnem("anl", 2, "\x54\x00", "\xff\x00"))
        ->help("Logical AND of immediate into A.")
        ->example("anl a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //anl @dir, a
    insert(mnem("anl", 2, "\x52\x00", "\xff\x00"))
        ->help("Logical AND of accumulator into absolute.")
        ->example("anl @0xff, a")
        ->abs("\x00\xff")
        ->regname("a");
    //anl @dir, #imm
    insert(mnem("anl", 3, "\x53\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND of immediate into absolute.")
        ->example("anl @0xff, #0xff")
        ->abs("\x00\xff\x00")
        ->imm("\x00\x00\xff");
    //anl c, bitadr
    insert(mnem("anl", 2, "\x82\x00", "\xff\x00"))
        ->help("Logical AND of absolute byte into accumulator.")
        ->example("anl c, 0xff")
        ->regname("c")
        ->bitadr("\x00\xff");
    //anl c, /bitadr
    insert(mnem("anl", 2, "\xb0\x00", "\xff\x00"))
        ->help("Logical AND of absolute byte into accumulator.")
        ->example("anl c, /0xff")
        ->regname("c")
        ->bitadrnot("\x00\xff");


    // Page 31
    insert(mnem("cjne", 3, "\xb5\x00\x00", "\xff\x00\x00"))
        ->help("Compare and jump if not equal.")
        ->example("loop: cjne a, @0xff, loop")
        ->regname("a")
        ->abs("\x00\xff\x00")
        ->rel("\x00\x00\xff", 3);
    insert(mnem("cjne", 3, "\xb4\x00\x00", "\xff\x00\x00"))
        ->help("Compare and jump if not equal.")
        ->example("loop: cjne a, #0xff, loop")
        ->regname("a")
        ->imm("\x00\xff\x00")
        ->rel("\x00\x00\xff", 3);
    insert(mnem("cjne", 3, "\xb8\x00\x00", "\xf8\x00\x00"))
        ->help("Compare and jump if not equal.")
        ->example("loop: cjne r7, #0xff, loop")
        ->reg8051("\x07\x00\x00")
        ->imm("\x00\xff\x00")
        ->rel("\x00\x00\xff", 3);
    insert(mnem("cjne", 3, "\xb6\x00\x00", "\xfe\x00\x00"))
        ->help("Compare and jump if not equal.")
        ->example("loop: cjne @r1, #0xff, loop")
        ->reg8051ind("\x01\x00\x00")
        ->imm("\x00\xff\x00")
        ->rel("\x00\x00\xff", 3);

    //Page 33
    insert(mnem("clr", 1, "\xe4", "\xff"))
        ->help("Clear accumulator.")
        ->example("clr a")
        ->regname("a");
    insert(mnem("clr", 1, "\xc3", "\xff"))
        ->help("Clear carry.")
        ->example("clr c")
        ->regname("c");
    insert(mnem("clr", 2, "\xc2\x00", "\xff\x00"))
        ->help("Clear bit address.")
        ->example("clr 0xff")
        ->bitadr("\x00\xff");


    insert(mnem("cpl", 1, "\xf4", "\xff"))
        ->help("Complement accumulator.")
        ->example("cpl a")
        ->regname("a");
    insert(mnem("cpl", 1, "\xb3", "\xff"))
        ->help("Complement carry.")
        ->example("cpl c")
        ->regname("c");
    insert(mnem("cpl", 2, "\xb2\x00", "\xff\x00"))
        ->help("Complement bit address.")
        ->example("cpl 0xff")
        ->bitadr("\x00\xff");

    //Page 35

    insert(mnem("da", 1, "\xd4", "\xff"))
        ->help("Decimal adjust accumulator for addition.")
        ->example("da a")
        ->regname("a");

    insert(mnem("dec", 1, "\x14", "\xff"))
        ->help("Decrement accumulator.")
        ->example("dec a")
        ->regname("a");
    insert(mnem("dec", 1, "\x18", "\xf8"))
        ->help("Decrement register.")
        ->example("dec r7")
        ->reg8051("\x07");
    insert(mnem("dec", 2, "\x15\x00", "\xff\x00"))
        ->help("Decrement memory entry.")
        ->example("dec @0xff")
        ->abs("\x00\xff");
    insert(mnem("dec", 1, "\x16", "\xfe"))
        ->help("Decrement memory at register address.")
        ->example("dec @r1")
        ->reg8051ind("\x01");


    insert(mnem("div", 1, "\x84", "\xff"))
        ->help("Divide A by B.  A gets integer result, B gets remainder.")
        ->example("div a, b")
        ->regname("a")
        ->regname("b");

    // Page 38

    insert(mnem("djnz", 2, "\xd8\x00", "\xf8\x00"))
        ->help("Decrement and Jump if Not Zero.")
        ->example("loop: djnz r7, loop")
        ->reg8051("\x07\x00")
        ->rel("\x00\xff", 2);
    insert(mnem("djnz", 3, "\xd5\x00\x00", "\xff\x00\x00"))
        ->help("Decrement and Jump if Not Zero.")
        ->example("loop: djnz @0xff, loop")
        ->abs("\x00\xff\x00")
        ->rel("\x00\x00\xff", 3);


    insert(mnem("inc", 1, "\x04", "\xff"))
        ->help("Increment accumulator.")
        ->example("inc a")
        ->regname("a");
    insert(mnem("inc", 1, "\x08", "\xf8"))
        ->help("Increment register.")
        ->example("inc r7")
        ->reg8051("\x07");
    insert(mnem("inc", 2, "\x05\x00", "\xff\x00"))
        ->help("Increment memory entry.")
        ->example("inc @0xff")
        ->abs("\x00\xff");
    insert(mnem("inc", 1, "\x06", "\xfe"))
        ->help("Increment memory at register address.")
        ->example("inc @r1")
        ->reg8051ind("\x01");
    insert(mnem("inc", 1, "\xa3", "\xff"))
        ->help("Increment data pointer.")
        ->example("inc dptr")
        ->regname("dptr");



    insert(mnem("jb", 3, "\x20\x00\x00", "\xff\x00\x00"))
        ->help("Jump if bit set.")
        ->example("loop: jb 0xff, loop")
        ->bitadr("\x00\xff\x00")
        ->rel("\x00\x00\xff",3);
    insert(mnem("jbc", 3, "\x10\x00\x00", "\xff\x00\x00"))
        ->help("Jump if bit clear.")
        ->example("loop: jbc 0xff, loop")
        ->bitadr("\x00\xff\x00")
        ->rel("\x00\x00\xff",3);


    insert(mnem("jc", 2, "\x40\x00", "\xff\x00"))
        ->help("Jump if carry bit is set.")
        ->example("loop: jc loop")
        ->rel("\x00\xff",2);

    insert(mnem("jmp", 1, "\x73", "\xff"))
        ->help("PC=A+DPTR")
        ->example("jmp (a, dptr)")
        ->group('(')
        ->regname("a")
        ->regname("dptr");

    insert(mnem("jnb", 3, "\x30\x00\x00", "\xff\x00\x00"))
        ->help("Jump if bit not set.")
        ->example("loop: jnb 0xff, loop")
        ->bitadr("\x00\xff\x00")
        ->rel("\x00\x00\xff",3);


    insert(mnem("jnc", 2, "\x50\x00", "\xff\x00"))
        ->help("Jump if carry bit is clear.")
        ->example("loop: jnc loop")
        ->rel("\x00\xff",2);

    insert(mnem("jnz", 2, "\x70\x00", "\xff\x00"))
        ->help("Jump if Accumulator Not Zero.")
        ->example("loop: jnz loop")
        ->rel("\x00\xff",2);
    insert(mnem("jz", 2, "\x60\x00", "\xff\x00"))
        ->help("Jump if Accumulator Zero.")
        ->example("loop: jz loop")
        ->rel("\x00\xff",2);

    insert(mnem("lcall", 3, "\x12\x00\x00", "\xff\x00\x00"))
        ->help("Long call to a subroutine.")
        ->example("lcall 0xffff")
        // Much simpler than ACALL, just the sixteen bits in big endian order.
        ->adr("\x00\xff\xff");
    insert(mnem("ljmp", 3, "\x02\x00\x00", "\xff\x00\x00"))
        ->help("Long jump to a subroutine.")
        ->example("ljmp 0xffff")
        // Much simpler than AJMP, just the sixteen bits in big endian order.
        ->adr("\x00\xff\xff");


    //Page 45

    insert(mnem("mov", 1, "\xe8", "\xf8"))
        ->help("Move register into accumulator.")
        ->example("mov a, r7")
        ->regname("a")
        ->reg8051("\x07");
    insert(mnem("mov", 2, "\xe5\x00", "\xff\x00"))
        ->help("Move mem[adr] into accumulator.")
        ->example("mov a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("mov", 1, "\xe6", "\xfe"))
        ->help("Move mem[r0] or mem[r1] into accumulator.")
        ->example("mov a, @r1")
        ->regname("a")
        ->reg8051ind("\x01");
    insert(mnem("mov", 2, "\x74\x00", "\xff\x00"))
        ->help("Move immediate into accumulator.")
        ->example("mov a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("mov", 1, "\xf8", "\xf8"))
        ->help("Move accumulator into register.")
        ->example("mov r7, a")
        ->reg8051("\x07")
        ->regname("a");
    insert(mnem("mov", 2, "\xa8\x00", "\xf8\x00"))
        ->help("Move mem[adr] into register.")
        ->example("mov r7, @0xff")
        ->reg8051("\x07")
        ->abs("\x00\xff");
    insert(mnem("mov", 2, "\x78\x00", "\xf8\x00"))
        ->help("Move immediate into register.")
        ->example("mov r7, #0xff")
        ->reg8051("\x07")
        ->imm("\x00\xff");
    insert(mnem("mov", 2, "\xf5\x00", "\xff\x00"))
        ->help("Move accumulator into mem[adr].")
        ->example("mov @0xff, a")
        ->abs("\x00\xff")
        ->regname("a");
    insert(mnem("mov", 2, "\x88\x00", "\xf8\x00"))
        ->help("Move register into mem[adr].")
        ->example("mov @0xff, r7")
        ->abs("\x00\xff")
        ->reg8051("\x07");
    insert(mnem("mov", 3, "\x85\x00\x00", "\xff\x00\x00"))
        ->help("Move mem[adr] to mem[adr].  Dst first.")
        ->example("mov @0xff, @0xff")
        ->abs("\x00\x00\xff")  //Destination
        ->abs("\x00\xff\x00"); //Source
    insert(mnem("mov", 2, "\x86\x00", "\xfe\x00"))
        ->help("Move mem[r0] or mem[r1] into mem[adr].")
        ->example("mov @0xff, @r1")
        ->abs("\x00\xff")
        ->reg8051ind("\x01\x00");
    insert(mnem("mov", 3, "\x75\x00\x00", "\xff\x00\x00"))
        ->help("Move immediate to mem[adr].")
        ->example("mov @0xff, #0xff")
        ->abs("\x00\xff\x00")  //Destination absolute.
        ->imm("\x00\x00\xff"); //Source immediate.
    insert(mnem("mov", 1, "\xf6", "\xfe"))
        ->help("Move accumulator into mem[r0] or mem[r1].")
        ->example("mov @r1, a")
        ->reg8051ind("\x01")
        ->regname("a");
    insert(mnem("mov", 2, "\xa6\x00", "\xfe\x00"))
        ->help("Move memory entry into mem[r0] or mem[r1].")
        ->example("mov @r1, @0xff")
        ->reg8051ind("\x01")
        ->abs("\x00\xff");
    insert(mnem("mov", 2, "\x76\x00", "\xfe\x00"))
        ->help("Move byte into mem[r0] or mem[r1].")
        ->example("mov @r1, #0xff")
        ->reg8051ind("\x01")
        ->imm("\x00\xff");

    //Page 48

    insert(mnem("mov", 2, "\xa2\x00", "\xff\x00"))
        ->help("Move bit data into Carry.")
        ->example("mov c, 0xff")
        ->regname("c")
        ->bitadr("\x00\xff");
    insert(mnem("mov", 2, "\x92\x00", "\xff\x00"))
        ->help("Move Carry into bit data.")
        ->example("mov 0xff, c")
        ->bitadr("\x00\xff")
        ->regname("c");


    insert(mnem("mov", 3, "\x90\x00\x00", "\xff\x00\x00"))
        ->help("Load DPTR with 16-bit number.")
        ->example("mov dptr, #0xffff")
        ->regname("dptr")
        ->imm("\x00\xff\xff");



    //Page 49

    insert(mnem("movc", 1, "\x93", "\xff"))
        ->help("Load CODEMEM[a+dptr] into accumulator.")
        ->example("movc a, (a, dptr)")
        ->regname("a")
        ->group('(')
        ->regname("a")
        ->regname("dptr");
    insert(mnem("movc", 1, "\x83", "\xff"))
        ->help("Load CODEMEM[a+pc+1] into accumulator.")
        ->example("movc a, (a, pc)")
        ->regname("a")
        ->group('(')
        ->regname("a")
        ->regname("pc");

    // Page 50


    insert(mnem("movx", 1, "\xe2", "\xfe"))
        ->help("Move xmem[r0] or xmem[r1] into accumulator.")
        ->example("movx a, @r1")
        ->regname("a")
        ->reg8051ind("\x01");
    insert(mnem("movx", 1, "\xe0", "\xff"))
        ->help("Move xmem[dptr] into accumulator.")
        ->example("movx a, @dptr")
        ->regname("a")
        ->regnameind("dptr");
    insert(mnem("movx", 1, "\xf2", "\xfe"))
        ->help("Move accumulator into xmem[r0] or xmem[r1].")
        ->example("movx @r1, a")
        ->reg8051ind("\x01")
        ->regname("a");
    insert(mnem("movx", 1, "\xf0", "\xff"))
        ->help("Move accumulator into xmem[dptr].")
        ->example("movx @dptr, a")
        ->regnameind("dptr")
        ->regname("a");

    //Page 51

    insert(mnem("mul", 1, "\xa4", "\xff"))
        ->help("Multiply A by B.  High byte of result in B, low in A.")
        ->example("mul a, b")
        ->regname("a")
        ->regname("b");

    insert(mnem("nop", 1, "\x00", "\xff"))
        ->help("No operation.")
        ->example("nop");

    // Page 53

    //orl a, Rn
    insert(mnem("orl", 1, "\x48", "\xf8"))
        ->help("Logical AND of register into accumulator.")
        ->example("orl a, r0")
        ->regname("a")
        ->reg8051("\x07");
    //orl a, @dir
    insert(mnem("orl", 2, "\x45\x00", "\xff\x00"))
        ->help("Logical OR of absolute byte into accumulator.")
        ->example("orl a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    //orl a, @Rn
    insert(mnem("orl", 1, "\x46", "\xfe"))
        ->help("Logical OR of byte at mem[r0] or mem[r1] into A.")
        ->example("orl a, @r0")
        ->regname("a")
        ->reg8051ind("\x01");
    //orl a, #imm
    insert(mnem("orl", 2, "\x44\x00", "\xff\x00"))
        ->help("Logical OR of immediate into A.")
        ->example("orl a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //orl @dir, a
    insert(mnem("orl", 2, "\x42\x00", "\xff\x00"))
        ->help("Logical OR of accumulator into absolute.")
        ->example("orl @0xff, a")
        ->abs("\x00\xff")
        ->regname("a");
    //orl @dir, #imm
    insert(mnem("orl", 3, "\x43\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR of immediate into absolute.")
        ->example("orl @0xff, #0xff")
        ->abs("\x00\xff\x00")
        ->imm("\x00\x00\xff");
    //orl c, bitadr
    insert(mnem("orl", 2, "\x72\x00", "\xff\x00"))
        ->help("Logical OR of absolute byte into accumulator.")
        ->example("orl c, 0xff")
        ->regname("c")
        ->bitadr("\x00\xff");
    //orl c, /bitadr
    insert(mnem("orl", 2, "\xa0\x00", "\xff\x00"))
        ->help("Logical OR of absolute byte into accumulator.")
        ->example("orl c, /0xff")
        ->regname("c")
        ->bitadrnot("\x00\xff");

    //Page 56.



    insert(mnem("pop", 2, "\xd0\x00", "\xff\x00"))
        ->help("Pop from stack to mem[adr].")
        ->example("pop @0xff")
        ->abs("\x00\xff");
    insert(mnem("push", 2, "\xc0\x00", "\xff\x00"))
        ->help("Push to stack from mem[adr].")
        ->example("push @0xff")
        ->abs("\x00\xff");



    insert(mnem("ret", 1, "\x22", "\xff"))
        ->help("Return from function.")
        ->example("ret");
    insert(mnem("reti", 1, "\x32", "\xff"))
        ->help("Return from interrupt.")
        ->example("reti");


    insert(mnem("rl", 1, "\x23", "\xff"))
        ->help("Rotate accumulator left.")
        ->example("rl a")
        ->regname("a");
    insert(mnem("rlc", 1, "\x33", "\xff"))
        ->help("Rotate accumulator left through carry.")
        ->example("rlc a")
        ->regname("a");

    insert(mnem("rr", 1, "\x03", "\xff"))
        ->help("Rotate accumulator right.")
        ->example("rr a")
        ->regname("a");
    insert(mnem("rrc", 1, "\x13", "\xff"))
        ->help("Rotate accumulator right through carry.")
        ->example("rrc a")
        ->regname("a");

    //Page 60

    insert(mnem("setb", 1, "\xd3", "\xff"))
        ->help("Set carry bit")
        ->example("setb c")
        ->regname("c");
    insert(mnem("setb", 2, "\xd2\x00", "\xff\x00"))
        ->help("Set addressed bit")
        ->example("setb 0xff")
        ->bitadr("\x00\xff");

    insert(mnem("sjmp", 2, "\x80\x00", "\xff\x00"))
        ->help("Short Jump.")
        ->example("loop: sjmp loop")
        ->rel("\x00\xff",2);

    //Page 61

    insert(mnem("subb", 1, "\x98", "\xf8"))
        ->help("Subtract with borrow a register from A.")
        ->example("subb a, r7")
        ->regname("a")
        ->reg8051("\x07");
    insert(mnem("subb", 2, "\x95\x00", "\xff\x00"))
        ->help("Subtract with borrow a memory byte from A.")
        ->example("subb a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("subb", 1, "\x96", "\xfe"))
        ->help("Subtract with borrow mem[r0] or mem[r1] from A.")
        ->example("subb a, @r1")
        ->regname("a")
        ->reg8051ind("\x01");
    insert(mnem("subb", 2, "\x94\x00", "\xff\x00"))
        ->help("Subtract with borrow an immediate from A.")
        ->example("subb a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");

    // Page 62

    insert(mnem("swap", 1, "\xc4", "\xff"))
        ->help("Swap nybbles of the accumulator.")
        ->example("swap a")
        ->regname("a");


    insert(mnem("xch", 1, "\xc8", "\xf8"))
        ->help("Exchanges accumulator with register.")
        ->example("xch a, r7")
        ->regname("a")
        ->reg8051("\x07");
    insert(mnem("xch", 2, "\xc5\x00", "\xff\x00"))
        ->help("Exchanges accumulator with memory byte.")
        ->example("xch a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("xch", 1, "\xc6", "\xfe"))
        ->help("Exchanges accumulator with mem[r0] or mem[r1].")
        ->example("xch a, @r1")
        ->regname("a")
        ->reg8051ind("\x01");


    insert(mnem("xchd", 1, "\xd6", "\xfe"))
        ->help("Exchanges digit (low nybble) of A with mem[r0] or mem[r1].")
        ->example("xchd a, @r1")
        ->regname("a")
        ->reg8051ind("\x01");



    //Page 64

    //xrl a, Rn
    insert(mnem("xrl", 1, "\x68", "\xf8"))
        ->help("XOR of register into accumulator.")
        ->example("xrl a, r7")
        ->regname("a")
        ->reg8051("\x07");
    //xrl a, @dir
    insert(mnem("xrl", 2, "\x65\x00", "\xff\x00"))
        ->help("XOR of absolute byte into accumulator.")
        ->example("xrl a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    //xrl a, @Rn
    insert(mnem("xrl", 1, "\x66", "\xfe"))
        ->help("XOR of byte at mem[r0] or mem[r1] into A.")
        ->example("xrl a, @r0")
        ->regname("a")
        ->reg8051ind("\x01");
    //xrl a, #imm
    insert(mnem("xrl", 2, "\x64\x00", "\xff\x00"))
        ->help("XOR of immediate into A.")
        ->example("xrl a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //xrl @dir, a
    insert(mnem("xrl", 2, "\x62\x00", "\xff\x00"))
        ->help("XOR of accumulator into absolute.")
        ->example("xrl @0xff, a")
        ->abs("\x00\xff")
        ->regname("a");
    //xrl @dir, #imm
    insert(mnem("xrl", 3, "\x63\x00\x00", "\xff\x00\x00"))
        ->help("XOR of immediate into absolute.")
        ->example("xrl @0xff, #0xff")
        ->abs("\x00\xff\x00")
        ->imm("\x00\x00\xff");
}


GAParameter8051Addr11::GAParameter8051Addr11(const char* mask){
    setMask(mask);
}
int GAParameter8051Addr11::match(GAParserOperand *op, int len){
    int64_t val=op->uint64(false); //False on a match.

    if(op->prefix!=prefix || op->suffix!=suffix)
        return 0;

    //Reject values that don't fit.
    if(((val)&0xF800) != ((2+op->goodasm->address())&0xf800)){
        op->goodasm->error("8051 ADDR11 is out of range.");
        return 0;
    }

    return 1;
}

QString GAParameter8051Addr11::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    p+=2;       // Add 2 for PC advance.
    p&=~0xf800; //Mask off high bits.
    p|=(adr+2)&0xf800; // Mask in the relevant PC bits.

    return QString::asprintf("0x%02x",(unsigned int) p);
}
void GAParameter8051Addr11::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){
    int64_t val=op.uint64(true);
    val-=2;       // PC+2 will be applied when the CPU decodes the instruction.
    val&=~0xf800; // Mask off upper bits, which are not encoded.

    rawencode(lang,adr,bytes,op,inslen,val);
}




GAParameter8051Reg::GAParameter8051Reg(const char* mask, bool indirect){
    setMask(mask);
    if(indirect)
        prefix="@";
}
int GAParameter8051Reg::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix || op->suffix!=this->suffix)
        return 0; //Wrong type.

    for(int i=0; i<8; i++)
        if(op->value == regnames[i])
            return 1;

    return 0;
}

QString GAParameter8051Reg::decode(GALanguage *lang,
                                    uint64_t adr, const char *bytes,
                                    int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<8); //Reg count on this architecture.

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameter8051Reg::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen){
    uint64_t val;

    QString v=op.value;
    for(int i=0; i<8; i++)
        if(v==regnames[i]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}




GAParameter8051Bit::GAParameter8051Bit(const char* mask, bool inverted){
    setMask(mask);
    if(inverted)
        this->prefix="/";
}
