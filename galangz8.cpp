#include "galangz8.h"
#include "gamnemonic.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

/* These types are made to match the abreviations on page 148
 * of the Z8 CPU User Manual, table 37.
 */

//CC  -- Condition Code.
#define z8CC(x) insert(new GAParameterZ8CC((x)))
//r   -- 4-bit working register
#define z8r(x) insert(new GAParameterZ8Reg4((x), false, false))
//R   -- 8-bit register
#define z8R(x) insert(new GAParameterZ8Reg8((x), false, false))
//RR  -- Working register pair, even in the range 00 to FE.
#define z8RR(x) insert(new GAParameterZ8Reg8((x), false, true))
//ir  -- indirect 4-bit working register.
#define z8ir(x) insert(new GAParameterZ8Reg4((x), true, false))
//IR  -- indirect 8-bit register.
#define z8IR(x) insert(new GAParameterZ8Reg8((x), true, false))
//irr -- Indirect 4-bit register pair.
#define z8irr(x) insert(new GAParameterZ8Reg4((x), true, true))
//IRR -- Indirect 8-bit register pair.
#define z8IRR(x) insert(new GAParameterZ8Reg8((x), true, true))
//X   -- Indexed
//DA  -- Direct address, 16-bit
#define z8DA(x) insert(new GAParameterAddress((x)))
//RA  -- Relative addressed, -128 to +127.
#define z8RA(x) insert(new GAParameterRelative((x),2))
//IM  -- Immediate data.
#define z8IM(x) insert(new GAParameterImmediate((x)))


GALangZ8::GALangZ8() {
    endian=BIG;  // Higher bits come in earlier bytes.
    name="z8";
    maxbytes=4;

    //page 158
    insert(mnem("add", 2, "\x02\x00", "\xff\x00"))
        ->help("Add.")
        ->example("add r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("add", 2, "\x03\x00\x00", "\xff\x00\x00"))
        ->help("Add.")
        ->example("add r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("add", 3, "\x04\x00\x00", "\xff\x00\x00"))
        ->help("Add.")
        ->example("add r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("add", 3, "\x05\x00\x00", "\xff\x00\x00"))
        ->help("Add.")
        ->example("add r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("add", 3, "\x06\x00\x00", "\xff\x00\x00"))
        ->help("Add.")
        ->example("add r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("add", 3, "\x07\x00\x00", "\xff\x00\x00"))
        ->help("Add.")
        ->example("add @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    //page 161.
    //The docs list the opcodes incorrect at 02 to 07, examples show 12 to 17h.
    insert(mnem("adc", 2, "\x12\x00", "\xff\x00"))
        ->help("Add w/ carry.")
        ->example("adc r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("adc", 2, "\x13\x00\x00", "\xff\x00\x00"))
        ->help("Add w/ carry.")
        ->example("adc r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("adc", 3, "\x14\x00\x00", "\xff\x00\x00"))
        ->help("Add w/ carry.")
        ->example("adc r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("adc", 3, "\x15\x00\x00", "\xff\x00\x00"))
        ->help("Add w/ carry.")
        ->example("adc r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("adc", 3, "\x16\x00\x00", "\xff\x00\x00"))
        ->help("Add w/ carry.")
        ->example("adc r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("adc", 3, "\x17\x00\x00", "\xff\x00\x00"))
        ->help("Add w/ carry.")
        ->example("adc @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    //page 164
    insert(mnem("call", 3, "\xd6\x00\x00", "\xff\x00\x00"))
        ->help("Call a function.")
        ->example("call 0xdead")
        ->adr("\x00\xff\xff");
    insert(mnem("call", 2, "\xd4\x00", "\xff\x00"))
        ->help("Call a function.")
        ->example("call @r6")
        ->z8IRR("\x00\xff");

    //page 166
    insert(mnem("ccf", 1, "\xef", "\xff"))
        ->help("Complement carry flag.")
        ->example("ccf");
    \
    //page 167
    insert(mnem("clr", 2, "\xb0\x00", "\xff\x00"))
        ->help("Clear.")
        ->example("clr 0x34")
        ->z8R("\x00\xff");
    insert(mnem("clr", 2, "\xb1\x00", "\xff\x00"))
        ->help("Clear.")
        ->example("clr @0xA5")
        ->z8IR("\x00\xff");

    insert(mnem("com", 2, "\x60\x00", "\xff\x00"))
        ->help("Complement.")
        ->example("com 0x34")
        ->z8R("\x00\xff");
    insert(mnem("com", 2, "\x61\x00", "\xff\x00"))
        ->help("Complement.")
        ->example("com @0xA5")
        ->z8IR("\x00\xff");

    //page 170
    insert(mnem("cp", 2, "\xa2\x00", "\xff\x00"))
        ->help("Compare.")
        ->example("cp r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("cp", 2, "\xa3\x00\x00", "\xff\x00\x00"))
        ->help("Compare.")
        ->example("cp r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("cp", 3, "\xa4\x00\x00", "\xff\x00\x00"))
        ->help("Compare.")
        ->example("cp r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("cp", 3, "\xa5\x00\x00", "\xff\x00\x00"))
        ->help("Compare.")
        ->example("cp r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("cp", 3, "\xa6\x00\x00", "\xff\x00\x00"))
        ->help("Compare.")
        ->example("cp r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("cp", 3, "\xa7\x00\x00", "\xff\x00\x00"))
        ->help("Compare.")
        ->example("cp @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src


    insert(mnem("da", 2, "\x40\x00", "\xff\x00"))
        ->help("Decimal adjust.")
        ->example("da 0x34")
        ->z8R("\x00\xff");
    insert(mnem("da", 2, "\x41\x00", "\xff\x00"))
        ->help("Decimal adjust.")
        ->example("da @0xA5")
        ->z8IR("\x00\xff");

    insert(mnem("dec", 2, "\x00\x00", "\xff\x00"))
        ->help("Decrement.")
        ->example("dec 0x34")
        ->z8R("\x00\xff");
    insert(mnem("dec", 2, "\x01\x00", "\xff\x00"))
        ->help("Decrement.")
        ->example("dec @0xA5")
        ->z8IR("\x00\xff");

    //page 176

    insert(mnem("djnz", 2, "\x0a\x00", "\x0f\x00"))
        ->help("Decrement and jump if not zero.")
        ->example("loop: djnz r1, loop")
        ->z8r("\xf0\x00")  //reg
        ->rel("\x00\xff", 2); //offset=2 to account for length.


    insert(mnem("decw", 2, "\x80\x00", "\xff\x00"))
        ->help("Decrement word.")
        ->example("decw 0x34")
        ->z8RR("\x00\xff");
    insert(mnem("decw", 2, "\x81\x00", "\xff\x00"))
        ->help("Decrement word.")
        ->example("decw @0xA4")
        ->z8IRR("\x00\xff");


    insert(mnem("di", 1, "\x8f", "\xff"))
        ->help("Disable interrupts.")
        ->example("di");
    insert(mnem("ei", 1, "\x9f", "\xff"))
        ->help("Enable interrupts.")
        ->example("ei");
    insert(mnem("halt", 1, "\x7f", "\xff"))
        ->help("Halt until interrupt.  (NOP before this.)")
        ->example("halt");

    insert(mnem("inc", 1, "\x0e", "\x0f"))
        ->help("Increment")
        ->example("inc r7")
        ->z8r("\xf0");  //reg
    insert(mnem("inc", 2, "\x20\x00", "\xff\x00"))
        ->help("Increment.")
        ->example("inc 0x34")
        ->z8R("\x00\xff");
    insert(mnem("inc", 2, "\x21\x00", "\xff\x00"))
        ->help("Increment.")
        ->example("inc @0xA5")
        ->z8IR("\x00\xff");

    insert(mnem("incw", 2, "\xa0\x00", "\xff\x00"))
        ->help("Increment word.")
        ->example("incw 0x34")
        ->z8RR("\x00\xff");
    insert(mnem("incw", 2, "\xa1\x00", "\xff\x00"))
        ->help("Increment word.")
        ->example("incw @0x34")
        ->z8IR("\x00\xff");
    /* Bad example from page 185.
    insert(mnem("incw", 2, "\xa0\x00", "\xff\x00"))
        ->help("Increment word.")
        ->example("incw 0x34")
        ->z8R("\x00\xff"); */

    //Page 187 mistakenly says 8F as the opcode, but that is di.
    insert(mnem("iret", 1, "\xbf", "\xff"))
        ->help("Return from interrupt handler.")
        ->example("iret");

    insert(mnem("jp", 3, "\x0d\x00\x00", "\x0f\x00\x00"))
        ->help("Conditional jump.")
        ->example("loop: jp c, loop")
        ->z8CC("\xf0\x00\x00")  //Condition Code
        ->z8DA("\x00\xff\xff");
    insert(mnem("jp", 2, "\x30\x00", "\xff\x00"))
        ->help("Unconditional jump.")
        ->example("jp @r6")
        ->z8IRR("\x00\xff");

    insert(mnem("jr", 2, "\x0b\x00", "\x0f\x00"))
        ->help("Relative jump.")
        ->example("loop: jr t, loop")
        ->z8CC("\xf0\x00")  //Condition Code
        ->z8RR("\x00\xff"); //offset=2 to account for length.

    //page 191
    insert(mnem("ld", 2, "\x0c\x00", "\x0f\x00"))
        ->help("Load")
        ->example("ld r6, #0xff")
        ->z8r("\xf0\x00")   //dest
        ->z8IM("\x00\xff"); //src
    insert(mnem("ld", 2, "\x08\x00", "\x0f\x00"))
        ->help("Load")
        ->example("ld r6, 0xff")
        ->z8r("\xf0\x00")   //dest
        ->z8R("\x00\xff");  //src
    insert(mnem("ld", 2, "\x09\x00", "\x0f\x00"))
        ->help("Load")
        ->example("ld 0xff, r6")
        ->z8R("\x00\xff")   //dest
        ->z8r("\xf0\x00");  //src

    insert(mnem("ld", 2, "\xe3\x00", "\xff\x00"))
        ->help("Load")
        ->example("ld r7, @r6")
        ->z8r("\x00\xf0")   //dest
        ->z8ir("\x00\x0f");  //src
    insert(mnem("ld", 2, "\xf3\x00", "\xff\x00"))
        ->help("Load")
        ->example("ld @r7, r6")
        ->z8ir("\x00\xf0")   //dest
        ->z8r("\x00\x0f");  //src

    insert(mnem("ld", 3, "\xe4\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld 0x34, 0xff")
        ->z8R("\x00\x00\xff")   //dest
        ->z8R("\x00\xff\x00");  //src
    insert(mnem("ld", 3, "\xe5\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld 0x34, @0xff")
        ->z8R("\x00\x00\xff")   //dest
        ->z8IR("\x00\xff\x00");  //src

    insert(mnem("ld", 3, "\xe6\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld 0x34, #0xff")
        ->z8R("\x00\xff\x00")   //dest
        ->z8IM("\x00\x00\xff");  //src
    insert(mnem("ld", 3, "\xe7\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld @0x34, #0xff")
        ->z8IR("\x00\xff\x00")   //dest
        ->z8IM("\x00\x00\xff");  //src

    insert(mnem("ld", 3, "\xf5\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld @0x34, 0xff")
        ->z8IR("\x00\x00\xff")   //dest
        ->z8R("\x00\xff\x00");  //src


    insert(mnem("ld", 3, "\xc7\x00\x00", "\xff\x00\x00"))
        ->help("Load")
        ->example("ld r5, (0xff, r7)")
        ->z8r("\x00\xf0\x00")   //dest
        ->group('(') //src
        ->z8R("\x00\x00\xff")
        ->z8r("\x00\x0f\x00");
    auto m=insert(mnem("ld", 3, "\xd7\x00\x00", "\xff\x00\x00"))
                 ->help("Load")
                 ->example("ld (0xff, r7), r5");
    m->group('(') //dest
        ->z8R("\x00\x00\xff")
        ->z8r("\x00\x0f\x00");
    m->z8r("\x00\xf0\x00");   //dest

    //page 195

    insert(mnem("ldc", 2, "\xc2\x00", "\xff\x00"))
        ->help("Load Constant (Program Memory)")
        ->example("ldc r7, @rr8")
        ->z8r("\x00\xf0")   //dest
        ->z8irr("\x00\x0f");  //src
    insert(mnem("ldc", 2, "\xd2\x00", "\xff\x00"))
        ->help("Load Constant (Program Memory)")
        ->example("ldc @rr6, r8")
        ->z8irr("\x00\xf0")   //dest
        ->z8r("\x00\x0f");  //src
    insert(mnem("ldci", 2, "\xc3\x00", "\xff\x00"))
        ->help("Load Constant Autoincrement (Program Memory)")
        ->example("ldci @r7, @rr8")
        ->z8ir("\x00\xf0")   //dest
        ->z8irr("\x00\x0f");  //src
    insert(mnem("ldci", 2, "\xd3\x00", "\xff\x00"))
        ->help("Load Constant Autoincrement (Program Memory)")
        ->example("ldci @rr6, @r8")
        ->z8irr("\x00\xf0")   //dest
        ->z8ir("\x00\x0f");  //src

    //Load Constant Autoincrement

    insert(mnem("lde", 2, "\x82\x00", "\xff\x00"))
        ->help("Load External Data")
        ->example("lde r7, @rr8")
        ->z8r("\x00\xf0")     //dest
        ->z8irr("\x00\x0f");  //src
    insert(mnem("lde", 2, "\x92\x00", "\xff\x00"))
        ->help("Load External Data")
        ->example("lde @rr6, r8")
        ->z8irr("\x00\xf0")   //dest
        ->z8r("\x00\x0f");    //src
    insert(mnem("ldei", 2, "\x83\x00", "\xff\x00"))
        ->help("Load External Data Autoincrement")
        ->example("ldei @r7, @rr8")
        ->z8ir("\x00\xf0")    //dest
        ->z8irr("\x00\x0f");  //src
    insert(mnem("ldei", 2, "\x93\x00", "\xff\x00"))
        ->help("Load External Data Autoincrement")
        ->example("ldei @rr6, @r8")
        ->z8irr("\x00\xf0")   //dest
        ->z8ir("\x00\x0f");   //src

    insert(mnem("nop", 1, "\xff", "\xff"))
        ->help("No Operation")
        ->example("nop");

    //page 204

    insert(mnem("and", 2, "\x52\x00", "\xff\x00"))
        ->help("Logical AND.")
        ->example("and r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("and", 2, "\x53\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND.")
        ->example("and r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("and", 3, "\x54\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND.")
        ->example("and r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("and", 3, "\x55\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND.")
        ->example("and r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("and", 3, "\x56\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND.")
        ->example("and r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("and", 3, "\x57\x00\x00", "\xff\x00\x00"))
        ->help("Logical AND.")
        ->example("and @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    insert(mnem("or", 2, "\x42\x00", "\xff\x00"))
        ->help("Logical OR.")
        ->example("or r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("or", 2, "\x43\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR.")
        ->example("or r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("or", 3, "\x44\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR.")
        ->example("or r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("or", 3, "\x45\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR.")
        ->example("or r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("or", 3, "\x46\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR.")
        ->example("or r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("or", 3, "\x47\x00\x00", "\xff\x00\x00"))
        ->help("Logical OR.")
        ->example("or @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    //page 210
    insert(mnem("xor", 2, "\xb2\x00", "\xff\x00"))
        ->help("Logical XOR.")
        ->example("xor r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("xor", 2, "\xb3\x00\x00", "\xff\x00\x00"))
        ->help("Logical XOR.")
        ->example("xor r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("xor", 3, "\xb4\x00\x00", "\xff\x00\x00"))
        ->help("Logical XOR.")
        ->example("xor r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("xor", 3, "\xb5\x00\x00", "\xff\x00\x00"))
        ->help("Logical XOR.")
        ->example("xor r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("xor", 3, "\xb6\x00\x00", "\xff\x00\x00"))
        ->help("Logical XOR.")
        ->example("xor r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("xor", 3, "\xb7\x00\x00", "\xff\x00\x00"))
        ->help("Logical XOR.")
        ->example("xor @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src


    insert(mnem("pop", 2, "\x50\x00", "\xff\x00"))
        ->help("Pop from stack.")
        ->example("pop 0x34")
        ->z8R("\x00\xff");
    insert(mnem("pop", 2, "\x51\x00", "\xff\x00"))
        ->help("Pop from stack.")
        ->example("pop @0xA5")
        ->z8IR("\x00\xff");
    insert(mnem("push", 2, "\x70\x00", "\xff\x00"))
        ->help("Push to stack.")
        ->example("push 0x34")
        ->z8R("\x00\xff");
    insert(mnem("push", 2, "\x71\x00", "\xff\x00"))
        ->help("Push to stack.")
        ->example("push @0xA5")
        ->z8IR("\x00\xff");


    insert(mnem("rcf", 1, "\xcf", "\xff"))
        ->help("Reset Carry Flag")
        ->example("rcf");
    insert(mnem("ret", 1, "\xaf", "\xff"))
        ->help("Return")
        ->example("ret");


    insert(mnem("rl", 2, "\x90\x00", "\xff\x00"))
        ->help("Rotate Left.")
        ->example("rl 0x34")
        ->z8R("\x00\xff");
    insert(mnem("rl", 2, "\x91\x00", "\xff\x00"))
        ->help("Rotate Left.")
        ->example("rl @0xA5")
        ->z8IR("\x00\xff");
    insert(mnem("rlc", 2, "\x10\x00", "\xff\x00"))
        ->help("Rotate Left Through Carry.")
        ->example("rlc 0x34")
        ->z8R("\x00\xff");
    insert(mnem("rlc", 2, "\x11\x00", "\xff\x00"))
        ->help("Rotate Left Through Carry.")
        ->example("rlc @0xA5")
        ->z8IR("\x00\xff");

    insert(mnem("rr", 2, "\xe0\x00", "\xff\x00"))
        ->help("Rotate Right.")
        ->example("rr 0x34")
        ->z8R("\x00\xff");
    insert(mnem("rr", 2, "\xe1\x00", "\xff\x00"))
        ->help("Rotate Right.")
        ->example("rr @0xA5")
        ->z8IR("\x00\xff");
    insert(mnem("rrc", 2, "\xc0\x00", "\xff\x00"))
        ->help("Rotate Right Through Carry.")
        ->example("rrc 0x34")
        ->z8R("\x00\xff");
    insert(mnem("rrc", 2, "\xc1\x00", "\xff\x00"))
        ->help("Rotate Right Through Carry.")
        ->example("rrc @0xA5")
        ->z8IR("\x00\xff");

    //page 227
    insert(mnem("scf", 1, "\xdf", "\xff"))
        ->help("Set Carry Flag.")
        ->example("scf");

    insert(mnem("srp", 2, "\x31\x00", "\xff\x00"))
        ->help("Set Register Pointer.")
        ->example("srp #0xf0")
        ->z8IM("\x00\xff");


    insert(mnem("sra", 2, "\xd0\x00", "\xff\x00"))
        ->help("Shift Right Arithmetic.")
        ->example("sra 0x34")
        ->z8RR("\x00\xff");
    insert(mnem("sra", 2, "\xd1\x00", "\xff\x00"))
        ->help("Shift Right Arithmetic.")
        ->example("sra @0x34")
        ->z8IR("\x00\xff");

    insert(mnem("stop", 1, "\x6f", "\xff"))
        ->help("Stops main clock. (NOP first.)")
        ->example("stop");


    insert(mnem("sub", 2, "\x22\x00", "\xff\x00"))
        ->help("Subtract.")
        ->example("sub r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("sub", 2, "\x23\x00\x00", "\xff\x00\x00"))
        ->help("Subtract.")
        ->example("sub r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("sub", 3, "\x24\x00\x00", "\xff\x00\x00"))
        ->help("Subtract.")
        ->example("sub r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("sub", 3, "\x25\x00\x00", "\xff\x00\x00"))
        ->help("Subtract.")
        ->example("sub r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("sub", 3, "\x26\x00\x00", "\xff\x00\x00"))
        ->help("Subtract.")
        ->example("sub r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("sub", 3, "\x27\x00\x00", "\xff\x00\x00"))
        ->help("Subtract.")
        ->example("sub @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    insert(mnem("sbc", 2, "\x32\x00", "\xff\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("sbc", 2, "\x33\x00\x00", "\xff\x00\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("sbc", 3, "\x34\x00\x00", "\xff\x00\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("sbc", 3, "\x35\x00\x00", "\xff\x00\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("sbc", 3, "\x36\x00\x00", "\xff\x00\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("sbc", 3, "\x37\x00\x00", "\xff\x00\x00"))
        ->help("Subtract w/ carry.")
        ->example("sbc @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src


    insert(mnem("swap", 2, "\xf0\x00", "\xff\x00"))
        ->help("Swap Nybbles.")
        ->example("swap 0x34")
        ->z8RR("\x00\xff");
    insert(mnem("swap", 2, "\xf1\x00", "\xff\x00"))
        ->help("Swap Nybbles.")
        ->example("swap @0x34")
        ->z8IR("\x00\xff");


    insert(mnem("tcm", 2, "\x62\x00", "\xff\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("tcm", 2, "\x63\x00\x00", "\xff\x00\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("tcm", 3, "\x64\x00\x00", "\xff\x00\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("tcm", 3, "\x65\x00\x00", "\xff\x00\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("tcm", 3, "\x66\x00\x00", "\xff\x00\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("tcm", 3, "\x67\x00\x00", "\xff\x00\x00"))
        ->help("Test Complement under Mask.")
        ->example("tcm @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src

    insert(mnem("tm", 2, "\x72\x00", "\xff\x00"))
        ->help("Test under Mask.")
        ->example("tm r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("tm", 2, "\x73\x00\x00", "\xff\x00\x00"))
        ->help("Test under Mask.")
        ->example("tm r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("tm", 3, "\x74\x00\x00", "\xff\x00\x00"))
        ->help("Test under Mask.")
        ->example("tm r5, 0x26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("tm", 3, "\x75\x00\x00", "\xff\x00\x00"))
        ->help("Test under Mask.")
        ->example("tm r6, @0x22")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("tm", 3, "\x76\x00\x00", "\xff\x00\x00"))
        ->help("Test under Mask.")
        ->example("tm r6, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src
    insert(mnem("tm", 3, "\x77\x00\x00", "\xff\x00\x00"))
        ->help("Test under Mask.")
        ->example("tm @0x22, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->z8IM("\x00\x00\xff"); //src


    insert(mnem("wdt", 1, "\x5f", "\xff"))
        ->help("Watch Dog Timer.")
        ->example("wdt");
}


/* r, ir
 *
 * 4-bit registers on Z8 run from r0 to r15.  The might also be
 * indirect, such @r0 to @r15.
 */

GAParameterZ8Reg4::GAParameterZ8Reg4(const char* mask, bool indirect, bool pair){
    if(indirect) prefix="@";
    setMask(mask);
    this->pair=pair;
}
int GAParameterZ8Reg4::match(GAParserOperand *op, int len){
    if(op->prefix!=prefix)
        return 0;
    if(!op->value.startsWith(pair?"rr":"r"))
        return 0;

    QString v=op->value.mid(pair?2:1, op->value.length()-1);
    bool okay=false;
    uint64_t val=v.toInt(&okay);
    if(!okay) return 0;
    if(val<16) return 1;

    return 0;
}
QString GAParameterZ8Reg4::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    //Registers begin with r1.
    uint64_t r=rawdecode(lang,adr,bytes,inslen);
    assert(r<16); //Reg count on this architecture.

    QString rendering=prefix
                        +(pair?"rr":"r")+QString::number(r)
                        +suffix;
    return rendering;
}
void GAParameterZ8Reg4::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){

    QString v=op.value.mid(pair?2:1, op.value.length()-1);
    bool okay=false;
    uint64_t val=v.toInt(&okay);
    assert(okay);

    rawencode(lang,adr,bytes,op,inslen,val);
}


/* R, IR
 *
 * 8-bit registers on Z8 are a little weird.  They are
 * represented like an address (0x34, for example) unless
 * they refer to a 4-bit register, in which case they use the
 * 4-bit notation and the upper nybble is set to E.
 *
 * So 0x34 would be 34, but r15 would be 0xEF.
 */

GAParameterZ8Reg8::GAParameterZ8Reg8(const char* mask, bool indirect, bool pair){
    if(indirect) prefix="@";
    this->pair=pair;
    setMask(mask);
}
int GAParameterZ8Reg8::match(GAParserOperand *op, int len){
    if(op->prefix!=prefix)
        return 0;
    if(op->value.startsWith(pair?"rr":"r")){
        // 4-bit register, encoded as 0xE0|regnum.
        QString v=op->value.mid(pair?2:1, op->value.length()-1);
        bool okay=false;
        uint64_t val=v.toInt(&okay);
        if(!okay) return 0;
        if(pair && val&1) return 0;  //Pairs must be event to match.
        if(val<16) return 1;
    }else{
        // 8-bit register, encoded plain.
        uint64_t val=op->uint64(false);
        if(pair && val&1) return 0;  //Pairs must be event to match.
        if(val<256) return 1;
    }

    return 0;
}

QString GAParameterZ8Reg8::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    //Registers begin with r1.
    uint64_t r=rawdecode(lang,adr,bytes,inslen);
    assert(r<256); //Reg count on this architecture.

    QString rendering="ERROR";
    if((r&0xF0)==0xE0){
        //4-bit registers use E in the upper nybble when represented in 8 bits.
        r&=0x0f;
        return prefix
               +(pair?"rr":"r")
               +QString::number(r)
               +suffix;
    }else{
        //FIXME, should be cleaner.
        rendering=prefix
                    +QString::asprintf("0x%02x",(unsigned int) r)
                    +suffix;
    }
    return rendering;
}
void GAParameterZ8Reg8::encode(GALanguage *lang,
                               uint64_t adr, QByteArray &bytes,
                               GAParserOperand op,
                               int inslen
                               ){
    //4 bit.
    if(op.value.startsWith(pair?"rr":"r")){
        // 4-bit register, encoded as 0xE0|regnum.
        QString v=op.value.mid(pair?2:1, op.value.length()-1);
        bool okay=false;
        uint64_t val=v.toInt(&okay);
        assert(okay);
        assert(val<16);
        val|=0xE0;
        rawencode(lang,adr,bytes,op,inslen,val);
        return;
    }

    //8 bit, just the number.
    rawencode(lang, adr, bytes, op, inslen,
              op.uint64(false));
}


/* These are the condition codes, from page 147 of
 * the Z8 CPU User Manual.
 */
GAParameterZ8CC::GAParameterZ8CC(const char* mask){
    setMask(mask);
}
int GAParameterZ8CC::match(GAParserOperand *op, int len){
    QString s=op->value;

    //No modes on condition codes.
    if(prefix!="" || suffix!="") return 0;

    //Is the name in our list?
    for(int i=0; i<16; i++)
        if(s==names[i] || s==othernames[i]) return 1;

    //No match.
    return 0;
}
QString GAParameterZ8CC::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t r=rawdecode(lang,adr,bytes,inslen);
    assert(r<16);
    return names[r];
}
void GAParameterZ8CC::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){
    assert(prefix=="");
    assert(suffix=="");

    for(int i=0; i<16; i++)
        if(op.value==names[i] || op.value==othernames[i]){
            //8 bit, just the number.
            rawencode(lang, adr, bytes, op, inslen,
                      i);
            return;
        }

    //We shouldn't be able to get here.
    assert(0);
}
