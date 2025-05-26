#include "galangtms320c28x.h"
#include "gamnemonic.h"
#include "gaparameter.h"

#include <iostream>

// Just to keep things shorter.
#define mnem new GAMnemonic

// TMS320C28x 32-bit data access (Direct/Stack/Indirect/Register).
#define loc32(x) insert(new GAParameterTMS320C28xReg32((x)))

// TMS320C28x 16-bit data access (Direct/Stack/Indirect/Register).
#define loc16(x) insert(new GAParameterTMS320C28xReg16((x)))

// TMS320C28x 22-bit immediate.
#define imm22(x) insert(new GAParameterTMS320C28xImm22((x)))

// TMS320C28x 16-bit SP-relative address.
#define rel16(x) insert(new GAParameterTMS320C28xRel16((x)))

// TMS320C28x constant.
#define const(x) insert(new GAParameterTMS320C28xConst((x)))

// TMS320C28x Product Mode shift bits.
#define pm(x) insert(new GAParameterTMS320C28xPM((x)))

// TMS320C28x Status mode bits.
#define status(x) insert(new GAParameterTMS320C28xStatus((x)))

// TMS320C28x Condition code bits.
#define cond(x) insert(new GAParameterTMS320C28xCondition((x), false))
#define condr(x) insert(new GAParameterTMS320C28xCondition((x), true))

/* This class represents the TI TMS320C28x DSP.
 *
 * Opcodes taken from here:
 * https://www.ti.com/lit/ug/spru430f/spru430f.pdf
 */

GALangTMS320C28x::GALangTMS320C28x()
{
    endian = BIG;
    name = "tms320c28x";
    wordsize = 2;
    align = 2;
    epsilon = 2; // TODO: wrong results with --base <offset>
    minbytes = 2;
    maxbytes = 4;
    swapwordonload = true; // File is little endian, but we work in big endian.

    // Register names.
    regnames.clear();
    regnames
        << "acc" << "xar1" << "xar2" << "xar3" << "xar4" << "xar5" << "xar6" << "xar7" << "p" << "xt"     // 32-bit
        << "ah" << "al" << "xar0" << "ar0" << "ar1" << "ar2" << "ar3" << "ar4" << "ar5" << "ar6" << "ar7" // 16-bit
        << "dp" << "ifr" << "ier" << "dbgier" << "ph" << "pl" << "sp" << "st0" << "st1" << "t" << "tl"    // 16-bit
        << "al.msb" << "ah.msb" << "al.lsb" << "ah.lsb"                                                   // 8-bit
        << "acc:p" << "ar1:ar0" << "ar3:ar2" << "ar5:ar4" << "ar1h:ar0h" << "t:st0" << "dp:st1"           // Combined
        << "ovc" << "objmode" << "m0m1map" << "xf" << "amode"                                             // Status
        << "pc" << "rpc";                                                                                 // Special

    GAMnemonic *m = 0;
    GAParameterGroup *pg = 0;

    // Instructions begin on page 124 with ABORTI.

    insert(mnem("aborti", 2, "\x00\x01", "\xff\xff"))
        ->help("Abort Interrupt")
        ->example("aborti");
    insert(mnem("abs", 2, "\xff\x56", "\xff\xff"))
        ->help("Absolute Value of Accumulator")
        ->example("abs acc")
        ->regname("acc");
    insert(mnem("abstc", 2, "\x56\x5f", "\xff\xff"))
        ->help("Absolute Value of Accumulator and Load TC")
        ->example("abstc acc")
        ->regname("acc");
    // TODO: ADD ACC,#16bit<<#0..15 — Add Value to Accumulator
    // TODO: ADD ACC,loc16 <<T — Add Value to Accumulator
    // TODO: ADD ACC,loc16 << #0..16 — Add Value to Accumulator
    insert(mnem("add", 2, "\x94\x00", "\xff\x00"))
        ->help("Add Value to AL")
        ->example("mov al, @var")
        ->regname("al")
        ->loc16("\x00\xff");
    insert(mnem("add", 2, "\x95\x00", "\xff\x00"))
        ->help("Add Value to AH")
        ->example("andb ah, @var")
        ->regname("ah")
        ->loc16("\x00\xff");
    insert(mnem("add", 2, "\x72\x00", "\xff\x00"))
        ->help("Add AL to Specified Location")
        ->example("mov al, @var")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("add", 2, "\x73\x00", "\xff\x00"))
        ->help("Add AH to Specified Location")
        ->example("add ah, @var")
        ->loc16("\x00\xff")
        ->regname("ah");
    insert(mnem("add", 4, "\x08\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Add Constant to Specified Location")
        ->example("add @var, #10")
        ->loc16("\x00\xff\x00\x00")
        ->simm("\x00\x00\xff\xff");
    insert(mnem("addb", 2, "\x09\x00", "\xff\x00"))
        ->help("Add 8-bit Constant to Accumulator")
        ->example("addb acc, #1")
        ->regname("acc")
        ->imm("\x00\xff");
    insert(mnem("addb", 2, "\x9c\x00", "\xff\x00"))
        ->help("Add 8-bit Constant to AL")
        ->example("addb al, #-3")
        ->regname("al")
        ->simm("\x00\xff");
    insert(mnem("addb", 2, "\x9d\x00", "\xff\x00"))
        ->help("Add 8-bit Constant to AH")
        ->example("addb ah, #2")
        ->regname("ah")
        ->simm("\x00\xff");
    insert(mnem("addb", 2, "\xfe\x00", "\xff\x80"))
        ->help("Add 7-bit Constant to Stack Pointer")
        ->example("addb sp, #6")
        ->regname("sp")
        ->imm("\x00\x7f");
    // TODO: ADDB XARn, #7bit — Add 7-bit Constant to Auxiliary Register
    insert(mnem("addcl", 4, "\x56\x40\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add 32-bit Value Plus Carry to Accumulator")
        ->example("addcl acc, @var")
        ->regname("acc")
        ->loc32("\x00\x00\x00\xff");
    insert(mnem("addcu", 2, "\x0c\x00", "\xff\x00"))
        ->help("Add Unsigned Value Plus Carry to Accumulator")
        ->example("addcu acc, @varlow")
        ->regname("acc")
        ->loc16("\x00\xff");
    insert(mnem("addl", 2, "\x07\x00", "\xff\x00"))
        ->help("Add 32-bit Value to Accumulator")
        ->example("addl acc, @var")
        ->regname("acc")
        ->loc32("\x00\xff");
    // TODO: ADDL ACC,P << PM — Add Shifted P to Accumulator
    insert(mnem("addl", 4, "\x56\x01\x00\x00", "\xff\xff\xff\x00"))
        ->help("Add Accumulator to Specified Location")
        ->example("addl @var, acc")
        ->loc32("\x00\x00\x00\xff")
        ->regname("acc");
    insert(mnem("addu", 2, "\x0d\x00", "\xff\x00"))
        ->help("Add Unsigned Value to Accumulator")
        ->example("addu acc, @varlow")
        ->regname("acc")
        ->loc16("\x00\xff");
    insert(mnem("addul", 4, "\x56\x57\x00\x00", "\xff\xff\xff\x00"))
        ->help("Add 32-bit Unsigned Value to P")
        ->example("addul p, @var")
        ->regname("p")
        ->loc32("\x00\x00\x00\xff");
    insert(mnem("addul", 4, "\x56\x53\x00\x00", "\xff\xff\x00\x00"))
        ->help("Add 32-bit Unsigned Value to Accumulator")
        ->example("addul acc, @var")
        ->regname("acc")
        ->loc32("\x00\x00\x00\xff");
    insert(mnem("adrk", 2, "\xfc\x00", "\xff\x00"))
        ->help("Add to Current Auxiliary Register")
        ->example("adrk #2")
        ->imm("\x00\xff");
    // TODO: AND ACC,#16bit << #0..16 — Description
    insert(mnem("and", 2, "\x89\x00", "\xff\x00"))
        ->help("Bitwise AND")
        ->example("and acc, @var")
        ->regname("acc")
        ->loc16("\x00\xff");
    insert(mnem("and", 4, "\xcc\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Bitwise AND")
        ->example("and al, @var, 0x0084")
        ->regname("al")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("and", 4, "\xcd\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Bitwise AND")
        ->example("and ah, @var, 0x0084")
        ->regname("ah")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("and", 4, "\x76\x26\x00\x00", "\xff\xff\x00\x00"))
        ->help("Bitwise AND to Disable Specified CPU Interrupts")
        ->example("and ier, #0xffde")
        ->regname("ier")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("and", 4, "\x76\x2f\x00\x00", "\xff\xff\x00\x00"))
        ->help("Bitwise AND to Clear Pending CPU Interrupts")
        ->example("and ifr, #0x0000")
        ->regname("ifr")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("and", 2, "\xc0\x00", "\xff\x00"))
        ->help("Bitwise AND")
        ->example("and @var, al")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("and", 2, "\xc1\x00", "\xff\x00"))
        ->help("Bitwise AND")
        ->example("and @var, ah")
        ->loc16("\x00\xff")
        ->regname("ah");
    insert(mnem("and", 2, "\xce\x00", "\xff\x00"))
        ->help("Bitwise AND")
        ->example("and al, @var")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("and", 2, "\xcf\x00", "\xff\x00"))
        ->help("Bitwise AND")
        ->example("and ah, @var")
        ->loc16("\x00\xff")
        ->regname("ah");
    insert(mnem("and", 4, "\x18\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Bitwise AND")
        ->example("and @var, #~(1 << 3 | 1)")
        ->loc16("\x00\xff\x00\x00")
        ->simm("\x00\x00\xff\xff");
    insert(mnem("andb", 2, "\x90\x00", "\xff\x00"))
        ->help("Bitwise AND 8-bit Value")
        ->example("andb al, 0xff")
        ->regname("al")
        ->imm("\x00\xff");
    insert(mnem("andb", 2, "\x91\x00", "\xff\x00"))
        ->help("Bitwise AND 8-bit Value")
        ->example("andb ah, 0xff")
        ->regname("ah")
        ->imm("\x00\xff");
    insert(mnem("asp", 2, "\x76\x1b", "\xff\xff"))
        ->help("Align Stack Pointer")
        ->example("asp");
    insert(mnem("asr", 2, "\xff\xa0", "\xff\xf0"))
        ->help("Arithmetic Shift Right")
        ->example("asr al, #2")
        ->regname("al")
        ->imm("\x00\x0f");
    insert(mnem("asr", 2, "\xff\xb0", "\xff\xf0"))
        ->help("Arithmetic Shift Right")
        ->example("asr ah, #2")
        ->regname("ah")
        ->imm("\x00\x0f");
    insert(mnem("asr", 2, "\xff\x64", "\xff\xff"))
        ->help("Arithmetic Shift Right")
        ->example("asr al, t")
        ->regname("al")
        ->regname("t");
    insert(mnem("asr", 2, "\xff\x65", "\xff\xff"))
        ->help("Arithmetic Shift Right")
        ->example("asr ah, t")
        ->regname("ah")
        ->regname("t");
    insert(mnem("asr64", 2, "\x56\x80", "\xff\xf0"))
        ->help("Arithmetic Shift Right of 64-bit Value")
        ->example("asr64 acc:p, #10")
        ->regname("acc:p")
        ->imm("\x00\x0f");
    insert(mnem("asr64", 2, "\x56\x23", "\xff\xff"))
        ->help("Arithmetic Shift Right of 64-bit Value")
        ->example("asr64 acc:p, t")
        ->regname("acc:p")
        ->regname("t");
    insert(mnem("asrl", 2, "\x56\x10", "\xff\xff"))
        ->help("Arithmetic Shift Right of Accumulator")
        ->example("asrl acc, t")
        ->regname("acc")
        ->regname("t");
    insert(mnem("b", 4, "\xff\x70\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Branch")
        ->example("b #0x0002, eq")
        ->rel16("\x00\x00\xff\xff") // TODO: 16-bit SP-relative broken?
        ->cond("\x00\x0f\x00\x00");
    // TODO: BANZ 16bitOffset,ARn− − — Branch if Auxiliary Register Not Equal to Zero
    // TODO: BAR 16bitOffset,ARn,ARm,EQ/NEQ — Branch on Auxiliary Register Comparison
    insert(mnem("bf", 4, "\x56\xc0\x00\x00", "\xff\xf0\x00\x00"))
        ->help("Branch Fast")
        ->example("bf 2, unc")
        ->rel16("\x00\x00\xff\xff") // TODO: 16-bit SP-relative broken?
        ->cond("\x00\x0f\x00\x00");
    insert(mnem("c27map", 2, "\x56\x3f", "\xff\xff"))
        ->help("Clear the M0M1MAP Bit")
        ->example("c27map")
        ->prioritize(); // Remove conflict: clrc m0m1map
    insert(mnem("c27obj", 2, "\x56\x36", "\xff\xff"))
        ->help("Clear the Objmode Bit")
        ->example("c27obj")
        ->prioritize(); // Remove conflict: clrc objmode
    insert(mnem("c28addr", 2, "\x56\x16", "\xff\xff"))
        ->help("Clear the AMODE Status Bit")
        ->example("c28addr")
        ->prioritize(); // Remove conflict: clrc amode
    insert(mnem("c28map", 2, "\x56\x1a", "\xff\xff"))
        ->help("Set the M0M1MAP Bit")
        ->example("c28map")
        ->prioritize(); // Remove conflict: setc m0m1map
    insert(mnem("c28obj", 2, "\x56\x1f", "\xff\xff"))
        ->help("Set the Objmode Bit")
        ->example("c28obj")
        ->prioritize(); // Remove conlict: setc objmode
    insert(mnem("clrc amode", 2, "\x56\x16", "\xff\xff"))
        ->help("Clear the AMODE Bit")
        ->example("clrc amode")
        ->regname("amode");
    insert(mnem("clrc m0m1map", 2, "\x56\x3f", "\xff\xff"))
        ->help("Clear the M0M1MAP Bit")
        ->example("clrc m0m1map")
        ->regname("m0m1map");
    insert(mnem("clrc objmode", 2, "\x56\x36", "\xff\xff"))
        ->help("Clear the Objmode Bit")
        ->example("clrc objmode")
        ->regname("objmode");
    insert(mnem("clrc", 2, "\x56\x5c", "\xff\xff"))
        ->help("Clear Overflow Counter")
        ->example("clrc ovc")
        ->regname("ovc");
    insert(mnem("clrc", 2, "\x56\x1b", "\xff\xff"))
        ->help("Clear XF Status Bit")
        ->example("clrc xf")
        ->regname("xf");
    insert(mnem("clrc", 2, "\x29\x00", "\xff\x00"))
        ->help("Clear Status Bits")
        ->example("clrc tc, c, sxm, ovm")
        ->status("\x00\xff");
    insert(mnem("cmp", 2, "\x54\x00", "\xff\x00"))
        ->help("Compare")
        ->example("cmp al, @var")
        ->regname("al")
        ->loc16("\x00\xff");
    insert(mnem("cmp", 2, "\x55\x00", "\xff\x00"))
        ->help("Compare")
        ->example("cmp ah, @var")
        ->regname("ah")
        ->loc16("\x00\xff");
    insert(mnem("cmp", 4, "\x1b\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Compare")
        ->example("cmp @var, #20")
        ->loc16("\x00\xff\x00\x00")
        ->simm("\x00\x00\xff\xff");
    insert(mnem("cmp64", 2, "\x56\x5e", "\xff\xff"))
        ->help("Compare 64-bit Value")
        ->example("cmp64 acc:p")
        ->regname("acc:p");
    insert(mnem("cmpb", 2, "\x52\x00", "\xff\x00"))
        ->help("Compare 8-bit Value")
        ->example("cmpb al, @var")
        ->regname("al")
        ->imm("\x00\xff");
    insert(mnem("cmpb", 2, "\x53\x00", "\xff\x00"))
        ->help("Compare 8-bit Value")
        ->example("cmpb ah, @var")
        ->regname("ah")
        ->imm("\x00\xff");
    insert(mnem("cmpl", 2, "\x0f\x00", "\xff\x00"))
        ->help("Compare 32-bit Value")
        ->example("cmpl acc, @p")
        ->regname("acc")
        ->loc32("\x00\xff");
    // TODO: CMPL ACC,P << PM — Compare 32-bit Value
    insert(mnem("cmpr", 2, "\x56\x1d", "\xff\xff"))
        ->help("Compare Auxiliary Registers")
        ->example("cmpr 0")
        ->const(0);
    insert(mnem("cmpr", 2, "\x56\x19", "\xff\xff"))
        ->help("Compare Auxiliary Registers")
        ->example("cmpr 1")
        ->const(1);
    insert(mnem("cmpr", 2, "\x56\x18", "\xff\xff"))
        ->help("Compare Auxiliary Registers")
        ->example("cmpr 2")
        ->const(2);
    insert(mnem("cmpr", 2, "\x56\x1c", "\xff\xff"))
        ->help("Compare Auxiliary Registers")
        ->example("cmpr 3")
        ->const(3);
    insert(mnem("csb", 2, "\x56\x35", "\xff\xff"))
        ->help("Count Sign Bits")
        ->example("csb acc")
        ->regname("acc");
    insert(mnem("dec", 2, "\x0b\x00", "\xff\x00"))
        ->help("Decrement by 1")
        ->example("dec @acc")
        ->loc16("\x00\xff");
    insert(mnem("dint", 2, "\x3b\x10", "\xff\xff"))
        ->help("Disable Maskable Interrupts (Set INTM Bit)")
        ->example("dint")
        ->prioritize(); // Remove alias conflict: setc intm
    // TODO: DMAC ACC:P,loc32,*XAR7/++ — 16-Bit Dual Multiply and Accumulate
    insert(mnem("dmov", 2, "\xa5\x00", "\xff\x00"))
        ->help("Data Move Contents of 16-bit Location")
        ->example("dmov @x+1")
        ->loc16("\x00\xff");
    insert(mnem("eallow", 2, "\x76\x22", "\xff\xff"))
        ->help("Enable Write Access to Protected Space")
        ->example("eallow");
    insert(mnem("edis", 2, "\x76\x1a", "\xff\xff"))
        ->help("Disable Write Access to Protected Registers")
        ->example("edis");
    insert(mnem("eint", 2, "\x29\x10", "\xff\xff"))
        ->help("Enable Maskable Interrupts (Clear INTM Bit)")
        ->example("eint")
        ->prioritize(); // Remove alias conflict: clrc intm
    insert(mnem("estop0", 2, "\x76\x25", "\xff\xff"))
        ->help("Emulation Stop 0")
        ->example("estop0");
    insert(mnem("estop1", 2, "\x76\x24", "\xff\xff"))
        ->help("Emulation Stop 1")
        ->example("estop1");
    insert(mnem("ffc", 4, "\x00\xc0\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Fast Function Call")
        ->example("ffc xar7, func")
        ->regname("xar7")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("flip", 2, "\x56\x70", "\xff\xff"))
        ->help("Flip Order of Bits in AL Register")
        ->example("flip al")
        ->regname("al");
    insert(mnem("flip", 2, "\x56\x71", "\xff\xff"))
        ->help("Flip Order of Bits in AH Register")
        ->example("flip ah")
        ->regname("ah");
    insert(mnem("iack", 4, "\x76\x3f\x00\x00", "\xff\xff\x00\x00"))
        ->help("Interrupt Acknowledge")
        ->example("iack #0xffff")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("idle", 2, "\x76\x21", "\xff\xff"))
        ->help("Put Processor in Idle Mode")
        ->example("idle");
    // TODO: IMACL P,loc32,*XAR7/++ — Signed 32 X 32-Bit Multiply and Accumulate (Lower Half)
    // TODO: IMPYAL P,XT,loc32 — Signed 32-Bit Multiply (Lower Half) and Add Previous P
    // TODO: IMPYL ACC,XT,loc32 — Signed 32 X 32-Bit Multiply (Lower Half)
    // TODO: IMPYL P,XT,loc32 — Signed 32 X 32-Bit Multiply (Lower Half)
    // TODO: IMPYSL P,XT,loc32 — Signed 32-Bit Multiply (Low Half) and Subtract P
    // TODO: IMPYXUL P,XT,loc32 — Signed 32 X Unsigned 32-Bit Multiply (Lower Half)
    // TODO: IN loc16,*(PA) — Input Data From Port
    insert(mnem("inc", 2, "\x0a\x00", "\xff\x00"))
        ->help("Increment by 1")
        ->example("inc @acc")
        ->loc16("\x00\xff");
    // TODO: INTR — Emulate Hardware Interrupt
    insert(mnem("iret", 2, "\x76\x02", "\xff\xff"))
        ->help("Interrupt Return")
        ->example("iret");
    // TODO: LB *XAR7 — Long Indirect Branch
    // TODO: LB 22bit — Long Branch
    // TODO: LC *XAR7 — Long Indirect Call
    // TODO: LC 22bit — Long Call
    insert(mnem("lcr", 4, "\x76\x40\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Long Call Using RPC")
        ->example("lcr func")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("lcr", 2, "\x3e\x60", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar0")
        ->regnameind("xar0");
    insert(mnem("lcr", 2, "\x3e\x61", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar1")
        ->regnameind("xar1");
    insert(mnem("lcr", 2, "\x3e\x62", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar2")
        ->regnameind("xar2");
    insert(mnem("lcr", 2, "\x3e\x63", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar3")
        ->regnameind("xar3");
    insert(mnem("lcr", 2, "\x3e\x64", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar4")
        ->regnameind("xar4");
    insert(mnem("lcr", 2, "\x3e\x65", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar5")
        ->regnameind("xar5");
    insert(mnem("lcr", 2, "\x3e\x66", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar6")
        ->regnameind("xar6");
    insert(mnem("lcr", 2, "\x3e\x67", "\xff\xf8"))
        ->help("Long Indirect Call Using RPC")
        ->example("lcr *xar7")
        ->regnameind("xar7");
    insert(mnem("loopnz", 4, "\x2e\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Loop While Not Zero")
        ->example("loopnz @t, #0x0004")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("loopz", 4, "\x2c\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Loop While Zero")
        ->example("loopz @t, 0x0004")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("lpaddr", 2, "\x56\x1e", "\xff\xff"))
        ->help("Set the AMODE Bit")
        ->example("lpaddr");
    insert(mnem("lret", 2, "\x76\x14", "\xff\xff"))
        ->help("Long Return")
        ->example("lret");
    insert(mnem("lrete", 2, "\x76\x10", "\xff\xff"))
        ->help("Long Return and Enable Interrupts")
        ->example("lrete");
    insert(mnem("lretr", 2, "\x00\x06", "\xff\xff"))
        ->help("Long Return Using RPC")
        ->example("lretr");
    // TODO: LSL ACC,#1..16 — Logical Shift Left
    // TODO: LSL ACC,T — Logical Shift Left by T(3:0)
    // TODO: LSL AX,#1...16 — Logical Shift Left
    // TODO: LSL AX,T — Logical Shift Left by T(3:0)
    // TODO: LSL64 ACC:P,#1..16 — Logical Shift Left
    // TODO: LSL64 ACC:P,T — 64-Bit Logical Shift Left by T(5:0)
    // TODO: LSLL ACC,T — Logical Shift Left by T (4:0)
    insert(mnem("lsr", 2, "\xff\xc0", "\xff\xf0"))
        ->help("Logical Shift Right")
        ->example("lsr al, #1")
        ->regname("al")
        ->imm("\x00\x0f");
    insert(mnem("lsr", 2, "\xff\xd0", "\xff\xf0"))
        ->help("Logical Shift Right")
        ->example("lsr ah, #1")
        ->regname("ah")
        ->imm("\x00\x0f");
    // TODO: LSR AX,T — Logical Shift Right by T(3:0)
    // TODO: LSR64 ACC:P,#1..16 — 64-Bit Logical Shift Right
    // TODO: LSR64 ACC:P,T — 64-Bit Logical Shift Right by T(5:0)
    // TODO: LSRL ACC,T — Logical Shift Right by T (4:0)
    // TODO: MAC P,loc16,0:pma — Multiply and Accumulate
    // TODO: MAC P ,loc16,*XAR7/++ — Multiply and Accumulate
    // TODO: MAX AX, loc16 — Find the Maximum
    // TODO: MAXCUL P,loc32 — Conditionally Find the Unsigned Maximum
    // TODO: MAXL ACC,loc32 — Find the 32-bit Maximum
    // TODO: MIN AX, loc16 — Find the Minimum
    // TODO: MINCUL P,loc32 — Conditionally Find the Unsigned Minimum
    // TODO: MINL ACC,loc32 — Find the 32-bit Minimum
    // TODO: MOV *(0:16bit), loc16 — Move Value
    // TODO: MOV ACC,#16bit<<#0..15 — Load Accumulator With Shift
    // TODO: MOV ACC,loc16<<T — Load Accumulator With Shift
    insert(mnem("mov", 2, "\x85\x00", "\xff\x00"))
        ->help("Load Accumulator With Shift")
        ->example("mov acc, @var << #0")
        ->regname("acc")
        ->loc16("\x00\xff"); // TODO: shift, objmode differences
    insert(mnem("mov", 4, "\x56\x03\x00\x00", "\xff\xff\xf0\x00"))
        ->help("Load Accumulator With Shift")
        ->example("mov acc, @var << #10")
        ->regname("acc")
        ->loc16("\x00\x00\x00\xff")
        ->imm("\x00\x00\x0f\x00"); // TODO: shift, objmode differences
    insert(mnem("mov", 2, "\x25\x00", "\xff\x00"))
        ->help("Load Accumulator With Shift")
        ->example("mov acc, @var << #16")
        ->regname("acc")
        ->loc16("\x00\xff"); // TODO: shift, objmode differences
    // TODO: MOV AR6/7, loc16 — Load Auxiliary Register
    insert(mnem("mov", 2, "\x92\x00", "\xff\x00"))
        ->help("Load AL")
        ->example("mov al, *+xar0[0]")
        ->regname("al")
        ->loc16("\x00\xff");
    insert(mnem("mov", 2, "\x93\x00", "\xff\x00"))
        ->help("Load AH")
        ->example("mov ah, *+xar0[0]")
        ->regname("ah")
        ->loc16("\x00\xff");
    // TODO: MOV DP, #10bit — Load Data-Page Pointer
    // TODO: MOV IER,loc16 — Load the Interrupt-Enable Register
    insert(mnem("mov", 4, "\x28\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Save 16-bit Constant")
        ->example("mov @xar2++ #0xffff")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    // TODO: MOV loc16, *(0:16bit) — Move Value
    insert(mnem("mov", 2, "\x2b\x00", "\xff\x00"))
        ->help("Clear 16-bit Location")
        ->example("mov *xar2++, #0")
        ->loc16("\x00\xff")
        ->const(0);
    // TODO: MOV loc16,ACC << 1..8 — Save Low Word of Shifted Accumulator
    // TODO: MOV loc16, ARn — Store 16-bit Auxiliary Register
    insert(mnem("mov", 2, "\x96\x00", "\xff\x00"))
        ->help("Store AL")
        ->example("mov *xar2++, al")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("mov", 2, "\x97\x00", "\xff\x00"))
        ->help("Store AH")
        ->example("mov *xar2++, ah")
        ->loc16("\x00\xff")
        ->regname("ah");
    // TODO: MOV loc16, AX, COND — Store AX Register Conditionally
    insert(mnem("mov", 2, "\x20\x00", "\xff\x00"))
        ->help("Store Interrupt-Enable Register")
        ->example("mov *sp++, ier")
        ->loc16("\x00\xff")
        ->regname("ier");
    insert(mnem("mov", 4, "\x56\x29\x00\x00", "\xff\xff\x00\xff"))
        ->help("Store the Overflow Counter")
        ->example("mov *sp++, ovc")
        ->loc16("\x00\x00\x00\xff")
        ->regname("ovc");
    insert(mnem("mov", 2, "\x3f\x00", "\xff\x00"))
        ->help("Store Lower Half of Shifted P Register")
        ->example("mov @y32+0, p")
        ->loc16("\x00\xff")
        ->regname("p");
    insert(mnem("mov", 2, "\x21\x00", "\xff\x00"))
        ->help("Store the T Register")
        ->example("mov @y32+0, p")
        ->loc16("\x00\xff")
        ->regname("t");
    // TODO: MOV OVC, loc16 — Load the Overflow Counter
    // TODO: MOV PH, loc16 — Load the High Half of the P Register
    // TODO: MOV PL, loc16 — Load the Low Half of the P Register
    // TODO: MOV PM, AX — Load Product Shift Mode
    // TODO: MOV T, loc16 — Load the Upper Half of the XT Register
    // TODO: MOV TL, #0 — Clear the Lower Half of the XT Register
    // TODO: MOV XARn, PC — Save the Current Program Counter
    // TODO: MOVA T,loc16 — Load T Register and Add Previous Product
    // TODO: MOVAD T, loc16 — Load T Register
    insert(mnem("movb", 2, "\x02\x00", "\xff\x00"))
        ->help("Load Accumulator With 8-bit Value")
        ->example("movb acc, #1")
        ->regname("acc")
        ->imm("\x00\xff");
    // TODO: MOVB AR6/7, #8bit — Load Auxiliary Register With an 8-bit Constant
    insert(mnem("movb", 2, "\x9a\x00", "\xff\x00"))
        ->help("Load AL With 8-bit Constant")
        ->example("movb al, #0xf0")
        ->regname("al")
        ->imm("\x00\xff");
    insert(mnem("movb", 2, "\x9b\x00", "\xff\x00"))
        ->help("Load AH With 8-bit Constant")
        ->example("movb ah, #0xf0")
        ->regname("ah")
        ->imm("\x00\xff");
    insert(mnem("movb", 2, "\xc6\x00", "\xff\x00"))
        ->help("Load Byte Value")
        ->example("movb al.lsb, *+xar2[3]")
        ->regname("al.lsb")
        ->loc16("\x00\xff");
    insert(mnem("movb", 2, "\xc7\x00", "\xff\x00"))
        ->help("Load Byte Value")
        ->example("movb ah.lsb, *+xar2[3]")
        ->regname("ah.lsb")
        ->loc16("\x00\xff");
    insert(mnem("movb", 2, "\x38\x00", "\xff\x00"))
        ->help("Load Byte Value")
        ->example("movb al.msb, *+xar2[1]")
        ->regname("al.msb")
        ->loc16("\x00\xff");
    insert(mnem("movb", 2, "\x39\x00", "\xff\x00"))
        ->help("Load Byte Value")
        ->example("movb ah.msb, *+xar2[1]")
        ->regname("ah.msb")
        ->loc16("\x00\xff");
    // TODO: MOVB loc16,#8bit,COND — Conditionally Save 8-bit Constant
    // TODO: MOVB loc16, AX.LSB — Store LSB of AX Register
    // TODO: MOVB loc16, AX.MSB — Store MSB of AX Register
    // TODO: MOVB XARn, #8bit — Load Auxiliary Register With 8-bit Value
    // TODO: MOVDL XT,loc32 — Store XT and Load New XT
    // TODO: MOVH loc16,ACC << 1..8 — Description
    // TODO: MOVH loc16, P — Save High Word of the P Register
    insert(mnem("movl", 2, "\x06\x00", "\xff\x00"))
        ->help("Load Accumulator With 32 Bits")
        ->example("movl acc, @var")
        ->regname("acc")
        ->loc32("\x00\xff");
    // TODO: MOVL ACC,P << PM — Load the Accumulator With Shifted P
    insert(mnem("movl", 2, "\x1e\x00", "\xff\x00"))
        ->help("Store 32-bit Accumulator")
        ->example("movl @var, acc")
        ->loc32("\x00\xff")
        ->regname("acc");
    // TODO: MOVL loc32,ACC,COND — Conditionally Store the Accumulator
    // TODO: MOVL loc32,P — Store the P Register
    // TODO: MOVL loc32, XARn — Store 32-bit Auxiliary Register
    // TODO: MOVL loc32,XT — Store the XT Register
    // TODO: MOVL P,ACC — Load P From the Accumulator
    // TODO: MOVL P,loc32 — Load the P Register
    // TODO: MOVL XARn, loc32 — Load 32-bit Auxiliary Register
    insert(mnem("movl", 2, "\x8e\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar0, @acc")
        ->regname("xar0")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\x8b\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar1, @acc")
        ->regname("xar1")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\x86\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar2, @acc")
        ->regname("xar2")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\x82\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar3, @acc")
        ->regname("xar3")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\x8a\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar4, @acc")
        ->regname("xar4")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\x83\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar5, @acc")
        ->regname("xar5")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\xc4\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar6, @acc")
        ->regname("xar6")
        ->loc32("\x00\xff");
    insert(mnem("movl", 2, "\xc5\x00", "\xff\x00"))
        ->help("Load 32-bit Auxiliary Register")
        ->example("movl xar7, @acc")
        ->regname("xar7")
        ->loc32("\x00\xff");
    insert(mnem("movl", 4, "\x8d\x00\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar0, #0")
        ->regname("xar0")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x8d\x40\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar1, #0")
        ->regname("xar1")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x8d\x80\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar2, #0")
        ->regname("xar2")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x8d\xc0\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar3, #0")
        ->regname("xar3")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x8f\x00\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar4, #0")
        ->regname("xar4")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x8f\x40\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar5, #0")
        ->regname("xar5")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x76\x80\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar6, #0")
        ->regname("xar6")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 4, "\x76\xc0\x00\x00", "\xff\xc0\x00\x00"))
        ->help("Load 32-bit Auxiliary Register With Constant Value")
        ->example("movl xar7, #0")
        ->regname("xar7")
        ->imm22("\x00\x3f\xff\xff");
    insert(mnem("movl", 2, "\x87\x00", "\xff\x00"))
        ->help("Load the XT Register")
        ->example("movl xt, @x2")
        ->regname("xt")
        ->loc32("\x00\xff");
    // TODO: MOVP T,loc16 — Load the T Register and Store P in the Accumulator
    // TODO: MOVS T,loc16 — Load T and Subtract P From the Accumulator
    insert(mnem("movu", 2, "\x0e\x00", "\xff\x00"))
        ->help("Load Accumulator With Unsigned Word")
        ->example("movu acc, @al, @x2")
        ->regname("acc")
        ->loc16("\x00\xff");
    // TODO: MOVU loc16,OVC — Store the Unsigned Overflow Counter
    // TODO: MOVU OVC,loc16 — Load Overflow Counter With Unsigned Value
    insert(mnem("movw", 4, "\x76\x1f\x00\x00", "\xff\xff\x00\x00"))
        ->help("Load the Entire Data Page")
        ->example("movw dp, #0xf012")
        ->regname("dp")
        ->imm("\x00\x00\xff\xff");
    // TODO: MOVX TL,loc16 — Load Lower Half of XT With Sign Extension
    insert(mnem("movz", 2, "\x58\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR0 and Clear Upper Half")
        ->example("movz ar0, *+xar2[0]")
        ->regname("ar0")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x59\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR1 and Clear Upper Half")
        ->example("movz ar1, *+xar2[0]")
        ->regname("ar1")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x5a\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR2 and Clear Upper Half")
        ->example("movz ar2, *+xar2[0]")
        ->regname("ar2")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x5b\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR3 and Clear Upper Half")
        ->example("movz ar3, *+xar2[0]")
        ->regname("ar3")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x5c\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR4 and Clear Upper Half")
        ->example("movz ar4, *+xar2[0]")
        ->regname("ar4")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x5d\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR5 and Clear Upper Half")
        ->example("movz ar5, *+xar2[0]")
        ->regname("ar5")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x88\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR6 and Clear Upper Half")
        ->example("movz ar6, *+xar2[0]")
        ->regname("ar6")
        ->loc16("\x00\xff");
    insert(mnem("movz", 2, "\x80\x00", "\xff\x00"))
        ->help("Load Lower Half of XAR7 and Clear Upper Half")
        ->example("movz ar7, *+xar2[0]")
        ->regname("ar7")
        ->loc16("\x00\xff");
    // TODO: MOVZ DP, #10bit — Load Data Page and Clear High Bits
    // TODO: MPY ACC,loc16, #16bit — 16 X 16-bit Multiply
    // TODO: MPY ACC, T, loc16 — 16 X 16-bit Multiply
    // TODO: MPY P,loc16,#16bit — 16 X 16-Bit Multiply
    // TODO: MPY P,T,loc16 — 16 X 16 Multiply
    // TODO: MPYA P,loc16,#16bit — 16 X 16-Bit Multiply and Add Previous Product
    // TODO: MPYA P,T,loc16 — 16 X 16-bit Multiply and Add Previous Product
    // TODO: MPYB ACC,T,#8bit — Multiply by 8-bit Constant
    // TODO: MPYB P,T,#8bit — Multiply Signed Value by Unsigned 8-bit Constant
    // TODO: MPYS P,T,loc16 — 16 X 16-bit Multiply and Subtract
    // TODO: MPYU P,T,loc16 — Unsigned 16 X 16 Multiply
    // TODO: MPYU ACC,T,loc16 — 16 X 16-bit Unsigned Multiply
    // TODO: MPYXU ACC, T, loc16 — Multiply Signed Value by Unsigned Value
    // TODO: MPYXU P,T,loc16 — Multiply Signed Value by Unsigned Value
    // TODO: NASP — Unalign Stack Pointer
    // TODO: NEG ACC — Negate Accumulator
    // TODO: NEG AX — Negate AX Register
    // TODO: NEG64 ACC:P — Negate Accumulator Register and Product Register
    // TODO: NEGTC ACC — If TC is Equivalent to 1, Negate ACC
    insert(mnem("nop", 2, "\x77\x00", "\xff\x00"))
        ->help("No Operation With Optional Indirect Address Modification")
        ->example("nop *,arp2")
        ->loc16("\x00\xff"); // TODO: This needs help
    // TODO: NORM ACC, *ind — Normalize ACC and Modify Selected Auxiliary Register
    // TODO: NORM ACC,XARn++/− − — Normalize ACC and Modify Selected Auxiliary Register
    // TODO: NOT ACC — Complement Accumulator
    // TODO: NOT AX — Complement AX Register
    // TODO: OR ACC, loc16 — Bitwise OR
    // TODO: OR ACC,#16bit << #0..16 — Bitwise OR
    // TODO: OR AX, loc16 — Bitwise OR
    // TODO: OR IER,#16bit — Bitwise OR
    // TODO: OR IFR,#16bit — Bitwise OR
    insert(mnem("or", 4, "\x1a\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Bitwise OR")
        ->example("or @var, #(1 << 4 | 1 << 7)")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("or", 2, "\x98\x00", "\xff\x00"))
        ->help("Bitwise OR")
        ->example("or @var, al")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("or", 2, "\x99\x00", "\xff\x00"))
        ->help("Bitwise OR")
        ->example("or @var, ah")
        ->loc16("\x00\xff")
        ->regname("ah");
    // TODO: ORB AX,#8bit — Bitwise OR 8-bit Value
    // TODO: OUT *(PA),loc16 — Output Data to Port
    insert(mnem("pop", 2, "\x06\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to Accumulator")
        ->example("pop acc")
        ->prioritize() // Remove conflict: movl acc, @sp
        ->regname("acc");
    insert(mnem("pop", 2, "\x07\x76", "\xff\xff"))
        ->help("Pop Top of Stack to 16-bit Auxiliary Registers")
        ->example("pop ar1:ar0")
        ->prioritize() // Remove conflict: movl acc, @var
        ->regname("ar1:ar0");
    insert(mnem("pop", 2, "\x05\x76", "\xff\xff"))
        ->help("Pop Top of Stack to 16-bit Auxiliary Registers")
        ->example("pop ar3:ar2")
        ->prioritize() // Remove conflict: movl acc, @var
        ->regname("ar3:ar2");
    insert(mnem("pop", 2, "\x06\x76", "\xff\xff"))
        ->help("Pop Top of Stack to 16-bit Auxiliary Registers")
        ->example("pop ar5:ar4")
        ->prioritize() // Remove conflict: movl acc, @var
        ->regname("ar5:ar4");
    insert(mnem("pop", 2, "\x03\x00", "\xff\xff"))
        ->help("Pop Top of Stack to Upper Half of Auxiliary Registers")
        ->example("pop ar1h:ar0h")
        ->prioritize() // Remove conflict: movl acc, @var
        ->regname("ar1h:ar0h");
    insert(mnem("pop", 2, "\x76\x12", "\xff\xff"))
        ->help("Pop Top of Stack to DBGIER Register")
        ->example("pop dbgier")
        ->regname("dbgier");
    insert(mnem("pop", 2, "\x76\x03", "\xff\xff"))
        ->help("Pop Top of Stack to DP Register")
        ->example("pop dp")
        ->regname("dp");
    insert(mnem("pop", 2, "\x76\x01", "\xff\xff"))
        ->help("Pop Top of Stack to DP and ST1 Registers")
        ->example("pop dp:st1")
        ->regname("dp:st1");
    insert(mnem("pop", 2, "\x00\x02", "\xff\xff"))
        ->help("Pop Top of Stack to IFR Register")
        ->example("pop ifr")
        ->regname("ifr");
    insert(mnem("pop", 2, "\x2a\x00", "\xff\x00"))
        ->help("Pop Top of Stack to 16-bit Value")
        ->example("pop @t")
        ->loc16("\x00\xff");
    insert(mnem("pop", 2, "\x76\x11", "\xff\xff"))
        ->help("Pop top of Stack to P Register")
        ->example("pop p")
        ->regname("p");
    insert(mnem("pop", 2, "\x00\x07", "\xff\xff"))
        ->help("Pop top of Stack to RPC Register")
        ->example("pop rpc")
        ->regname("rpc");
    insert(mnem("pop", 2, "\x76\x13", "\xff\xff"))
        ->help("Pop Top of Stack to ST0 Register")
        ->example("pop st0")
        ->regname("st0");
    insert(mnem("pop", 2, "\x76\x00", "\xff\xff"))
        ->help("Pop Top of Stack to ST1 Register")
        ->example("pop st1")
        ->regname("st1");
    insert(mnem("pop", 2, "\x76\x15", "\xff\xff"))
        ->help("Pop Top of Stack to T and ST0 Registers")
        ->example("pop t:st0")
        ->regname("t:st0");
    insert(mnem("pop", 2, "\x8e\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar0")
        ->prioritize() // Remove conflict: movl xar0, @acc
        ->regname("xar0");
    insert(mnem("pop", 2, "\x8b\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar1")
        ->prioritize() // Remove conflict: movl xar1, @acc
        ->regname("xar1");
    insert(mnem("pop", 2, "\x86\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar2")
        ->prioritize() // Remove conflict: movl xar2, @acc
        ->regname("xar2");
    insert(mnem("pop", 2, "\x82\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar3")
        ->prioritize() // Remove conflict: movl xar3, @acc
        ->regname("xar3");
    insert(mnem("pop", 2, "\x8a\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar4")
        ->prioritize() // Remove conflict: movl xar4, @acc
        ->regname("xar4");
    insert(mnem("pop", 2, "\x83\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar5")
        ->prioritize() // Remove conflict: movl xar5, @acc
        ->regname("xar5");
    insert(mnem("pop", 2, "\xc4\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar6")
        ->prioritize() // Remove conflict: movl xar6, @acc
        ->regname("xar6");
    insert(mnem("pop", 2, "\xc5\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to 32-bit Auxiliary Register")
        ->example("pop xar7")
        ->prioritize() // Remove conflict: movl xar7, @acc
        ->regname("xar7");
    insert(mnem("pop", 2, "\x87\xbe", "\xff\xff"))
        ->help("Pop Top of Stack to XT Register")
        ->example("pop xt")
        ->prioritize() // Remove conflict: movl xt, @sp
        ->regname("xt");
    // TODO: PREAD loc16,*XAR7 — Read From Program Memory
    insert(mnem("push", 2, "\x1e\xbd", "\xff\xff"))
        ->help("Push Accumulator Onto Stack")
        ->example("push acc")
        ->prioritize() // Remove conlict: movl loc32, acc
        ->regname("acc");
    insert(mnem("push", 2, "\x76\x0d", "\xff\xff"))
        ->help("Push 16-bit Auxiliary Registers Onto Stack")
        ->example("push ar1:ar0")
        ->regname("ar1:ar0");
    insert(mnem("push", 2, "\x76\x0f", "\xff\xff"))
        ->help("Push 16-bit Auxiliary Registers Onto Stack")
        ->example("push ar3:ar2")
        ->regname("ar3:ar2");
    insert(mnem("push", 2, "\x76\x0c", "\xff\xff"))
        ->help("Push 16-bit Auxiliary Registers Onto Stack")
        ->example("push ar5:ar4")
        ->regname("ar5:ar4");
    insert(mnem("push", 2, "\x00\x05", "\xff\xff"))
        ->help("Push AR1H and AR0H Registers Onto Stack")
        ->example("push ar1h:ar0h")
        ->regname("ar1h:ar0h");
    insert(mnem("push", 2, "\x76\x0e", "\xff\xff"))
        ->help("Push DBGIER Register Onto Stack")
        ->example("push dbgier")
        ->regname("dbgier");
    insert(mnem("push", 2, "\x76\x0b", "\xff\xff"))
        ->help("Push DP Register Onto Stack")
        ->example("push dp")
        ->regname("dp");
    insert(mnem("push", 2, "\x76\x09", "\xff\xff"))
        ->help("Push DP and ST1 Registers Onto Stack")
        ->example("push dp:st1")
        ->regname("dp:st1");
    insert(mnem("push", 2, "\x76\x0a", "\xff\xff"))
        ->help("Push IFR Register Onto Stack")
        ->example("push ifr")
        ->regname("ifr");
    insert(mnem("push", 2, "\x22\x00", "\xff\x00"))
        ->help("Push 16-bit Value Onto Stack")
        ->example("push @t")
        ->loc16("\x00\xff");
    insert(mnem("push", 2, "\x76\x1d", "\xff\xff"))
        ->help("Push P Register Onto Stack")
        ->example("push p")
        ->regname("p");
    insert(mnem("push", 2, "\x00\x04", "\xff\xff"))
        ->help("Push RPC Register Onto Stack")
        ->example("push rpc")
        ->regname("rpc");
    insert(mnem("push", 2, "\x76\x18", "\xff\xff"))
        ->help("Push ST0 Register Onto Stack")
        ->example("push st0")
        ->regname("st0");
    insert(mnem("push", 2, "\x76\x08", "\xff\xff"))
        ->help("Push ST1 Register Onto Stack")
        ->example("push st1")
        ->regname("st1");
    insert(mnem("push", 2, "\x76\x19", "\xff\xff"))
        ->help("Push T and ST0 Registers Onto Stack")
        ->example("push t:st0")
        ->regname("t:st0");
    insert(mnem("push", 2, "\x3a\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar0")
        ->regname("xar0");
    insert(mnem("push", 2, "\xb2\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar1")
        ->regname("xar1");
    insert(mnem("push", 2, "\xaa\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar2")
        ->regname("xar2");
    insert(mnem("push", 2, "\xa2\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar3")
        ->regname("xar3");
    insert(mnem("push", 2, "\xa8\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar4")
        ->regname("xar4");
    insert(mnem("push", 2, "\xa0\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar5")
        ->regname("xar5");
    insert(mnem("push", 2, "\xc2\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar6")
        ->regname("xar6");
    insert(mnem("push", 2, "\xc3\xbd", "\xff\xff"))
        ->help("Push 32-bit Auxiliary Register Onto Stack")
        ->example("push xar7")
        ->regname("xar7");
    insert(mnem("push", 2, "\xab\xbd", "\xff\xff"))
        ->help("Push XT Register Onto Stack")
        ->example("push xt")
        ->regname("xt");
    // TODO: PWRITE *XAR7,loc16 — Write to Program Memory
    // TODO: QMACL P,loc32,*XAR7/++ — Signed 32 X 32-bit Multiply and Accumulate (Upper Half)
    // TODO: QMPYAL P,XT,loc32 — Signed 32-bit Multiply (Upper Half) and Add Previous P
    // TODO: QMPYL P,XT,loc32 — Signed 32 X 32-bit Multiply (Upper Half)
    // TODO: QMPYL ACC,XT,loc32 — Signed 32 X 32-bit Multiply (Upper Half)
    // TODO: QMPYSL P,XT,loc32 — Signed 32-bit Multiply (Upper Half) and Subtract Previous P
    // TODO: QMPYUL P,XT,loc32 — Unsigned 32 X 32-bit Multiply (Upper Half)
    // TODO: QMPYXUL P,XT,loc32 — Signed X Unsigned 32-bit Multiply (Upper Half)
    // TODO: ROL ACC — Rotate Accumulator Left
    // TODO: ROR ACC — Rotate Accumulator Right
    insert(mnem("rpt", 2, "\xf6\x00", "\xff\x00"))
        ->help("Repeat Next Instruction")
        ->example("rpt #10")
        ->imm("\x00\xff");
    insert(mnem("rpt", 2, "\xf7\x00", "\xff\x00"))
        ->help("Repeat Next Instructionh")
        ->example("rpt @var")
        ->loc16("\x00\xff");
    // TODO: SAT ACC — Saturate Accumulator
    // TODO: SAT64 ACC:P — Saturate 64-bit Value ACC:P
    insert(mnem("sb", 2, "\x60\x00", "\xf0\x00"))
        ->help("Short Conditional Branch")
        ->example("sb #2, eq")
        ->rel("\x00\xff")
        ->cond("\x0f\x00");
    // TODO: SBBU ACC,loc16 — Subtract Unsigned Value Plus Inverse Borrow
    insert(mnem("sbf", 2, "\xec\x00", "\xff\x00"))
        ->help("Short Branch Fast")
        ->example("sbf #2, eq")
        ->rel("\x00\xff")
        ->condr("\x03\x00");
    insert(mnem("sbf", 2, "\xed\x00", "\xff\x00"))
        ->help("Short Branch Fast")
        ->example("sbf #2, neq")
        ->rel("\x00\xff")
        ->condr("\x03\x00");
    insert(mnem("sbf", 2, "\xee\x00", "\xff\x00"))
        ->help("Short Branch Fast")
        ->example("sbf #2, tc")
        ->rel("\x00\xff")
        ->condr("\x03\x00");
    insert(mnem("sbf", 2, "\xef\x00", "\xff\x00"))
        ->help("Short Branch Fast")
        ->example("sbf #2, ntc")
        ->rel("\x00\xff")
        ->condr("\x03\x00");
    // TODO: SBRK #8bit — Subtract From Current Auxiliary Register
    insert(mnem("setc", 2, "\x3b\x00", "\xff\x00"))
        ->help("Set Multiple Status Bits")
        ->example("setc intm, dbgm")
        ->status("\x00\xff");
    insert(mnem("setc", 2, "\x56\x1a", "\xff\xff"))
        ->help("Set the M0M1MAP Status Bit")
        ->example("setc m0m1map")
        ->regname("m0m1map");
    insert(mnem("setc", 2, "\x56\x1f", "\xff\xff"))
        ->help("Set the Objmode Status Bit")
        ->example("setc objmode")
        ->regname("objmode");
    insert(mnem("setc", 2, "\x56\x26", "\xff\xff"))
        ->help("Set XF Bit and Output Signal")
        ->example("setc xf")
        ->regname("xf");
    insert(mnem("sfr", 2, "\xff\x40", "\xff\xf0"))
        ->help("Shift Accumulator Right")
        ->example("sfr acc, #10")
        ->regname("acc")
        ->imm("\x00\x0f");
    insert(mnem("sfr", 2, "\xff\x51", "\xff\xff"))
        ->help("Shift Accumulator Right")
        ->example("sfr acc, t")
        ->regname("acc")
        ->regname("t");
    insert(mnem("spm", 2, "\xff\x68", "\xff\xf8"))
        ->help("Set Product Mode Shift Bits")
        ->example("spm 0")
        ->pm("\x00\x0f");
    insert(mnem("sqra", 4, "\x56\x15\x00\x00", "\xff\xff\xff\x00"))
        ->help("Square Value and Add P to ACC")
        ->example("sqra *xar2++")
        ->loc16("\x00\x00\x00\xff");
    insert(mnem("sqrs", 4, "\x56\x11\x00\x00", "\xff\xff\x00\x00"))
        ->help("Square Value and Subtract P From ACC")
        ->example("sqrs *xar2++")
        ->loc16("\x00\x00\x00\xff");
    // TODO: SUB ACC,loc16 << #0..16 — Subtract Shifted Value From Accumulator
    // TODO: SUB ACC,loc16 << T — Subtract Shifted Value From Accumulator
    // TODO: SUB ACC,#16bit << #0..15 — Subtract Shifted Value From Accumulator
    insert(mnem("sub", 2, "\x9e\x00", "\xff\x00"))
        ->help("Subtract Specified Location From AL")
        ->example("sub al, @var")
        ->regname("al")
        ->imm("\x00\xff");
    insert(mnem("sub", 2, "\x9f\x00", "\xff\x00"))
        ->help("Subtract Specified Location From AH")
        ->example("sub ah, @var")
        ->regname("ah")
        ->imm("\x00\xff");
    insert(mnem("sub", 2, "\x74\x00", "\xff\x00"))
        ->help("Reverse-Subtract Specified Location From AL")
        ->example("sub @var, al")
        ->imm("\x00\xff")
        ->regname("al");
    insert(mnem("sub", 2, "\x75\x00", "\xff\x00"))
        ->help("Reverse-Subtract Specified Location From AH")
        ->example("sub @var, ah")
        ->imm("\x00\xff")
        ->regname("ah");
    insert(mnem("subb", 2, "\x19\x00", "\xff\x00"))
        ->help("Subtract 8-bit Value")
        ->example("subb acc, #1")
        ->regname("acc")
        ->imm("\x00\xff");
    insert(mnem("subb", 2, "\xfe\x80", "\xff\x80"))
        ->help("Subtract 7-bit Value")
        ->example("subb sp, #2")
        ->regname("sp")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xd8\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar0, #0x10")
        ->regname("xar0")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xd9\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar1, #0x10")
        ->regname("xar1")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xda\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar2, #0x10")
        ->regname("xar2")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xdb\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar3, #0x10")
        ->regname("xar3")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xdc\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar4, #0x10")
        ->regname("xar4")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xdd\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar5, #0x10")
        ->regname("xar5")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xde\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar6, #0x10")
        ->regname("xar6")
        ->imm("\x00\x7f");
    insert(mnem("subb", 2, "\xdf\x80", "\xff\x80"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subb xar7, #0x10")
        ->regname("xar7")
        ->imm("\x00\x7f");
    insert(mnem("subbl", 4, "\x56\x54\x00\x00", "\xff\xff\xff\x00"))
        ->help("Subtract 32-bit Value Plus Inverse Borrow")
        ->example("subbl acc, @var+2")
        ->regname("acc")
        ->loc32("\x00\x00\x00\xff");
    insert(mnem("subcu", 2, "\x1f\x00", "\xff\x00"))
        ->help("Subtract 7-Bit From Auxiliary Register")
        ->example("subcu acc, @var")
        ->regname("acc")
        ->loc16("\x00\xff");
    insert(mnem("subcul", 4, "\x56\x17\x00\x00", "\xff\xff\xff\x00"))
        ->help("Subtract Conditional 32 Bits")
        ->example("subcul acc, @var")
        ->regname("acc")
        ->loc32("\x00\x00\x00\xff");
    insert(mnem("subl", 2, "\x03\x00", "\xff\x00"))
        ->help("Subtract 32-bit Value")
        ->example("subl acc, @var")
        ->regname("acc")
        ->loc32("\x00\xff");
    // TODO: SUBL ACC,P << PM — Subtract 32-bit Value
    insert(mnem("subl", 4, "\x56\x41\x00\x00", "\xff\xff\xff\x00"))
        ->help("Subtract 32-bit Value")
        ->example("subl @var, acc")
        ->loc32("\x00\x00\x00\xff")
        ->regname("acc");
    // TODO: SUBR loc16,AX — Reverse-Subtract Specified Location From AX
    // TODO: SUBRL loc32, ACC — Reverse-Subtract Specified Location From ACC
    // TODO: SUBU ACC, loc16 — Subtract Unsigned 16-bit Value
    // TODO: SUBUL ACC, loc32 — Subtract Unsigned 32-bit Value
    // TODO: SUBUL P,loc32 — Subtract Unsigned 32-bit Value
    insert(mnem("tbit", 2, "\x40\x00", "\xf0\x00"))
        ->help("Test Specified Bit")
        ->example("tbit @var, #4")
        ->loc16("\x00\xff")
        ->imm("\x0f\x00");
    insert(mnem("tbit", 4, "\x56\x25\x00\x00", "\xff\xff\xff\x00"))
        ->help("Test Bit Specified by Register")
        ->example("tbit @var, t")
        ->loc16("\x00\x00\x00\xff")
        ->regname("t");
    insert(mnem("tclr", 4, "\x56\x09\x00\x00", "\xff\xff\xf0\x00"))
        ->help("Test and Clear Specified Bit")
        ->example("tset @var, #6")
        ->loc16("\x00\x00\x00\xff")
        ->imm("\x00\x00\x0f\x00");
    insert(mnem("test", 2, "\xff\x58", "\xff\xff"))
        ->help("Test for Accumulator Equal to Zero")
        ->example("test acc")
        ->regname("acc");
    insert(mnem("trap", 2, "\x00\x20", "\xff\xe0"))
        ->help("Software Trap")
        ->example("trap #0")
        ->imm("\x00\x1f");
    insert(mnem("tset", 4, "\x56\x0d\x00\x00", "\xff\xff\xf0\x00"))
        ->help("Test and Set Specified Bit")
        ->example("tset @var, #6")
        ->loc16("\x00\x00\x00\xff")
        ->imm("\x00\x00\x0f\x00");
    // TODO: UOUT *(PA),loc16 — Unprotected Output Data to I/O Port
    insert(mnem("xb", 2, "\x56\x14", "\xff\xff"))
        ->help("C2 x LP Source-Compatible Indirect Branch")
        ->example("xb *al")
        ->regnameind("al");
    // TODO: XB pma,*,ARPn — C2xLP Source-Compatible Branch with ARP Modification
    // TODO: XB pma,COND — C2 xLP Source-Compatible Branch
    // TODO: XBANZ pma,*ind{,ARPn} — C2 x LP Source-Compatible Branch If ARn Is Not Zero
    insert(mnem("xcall", 2, "\x56\x34", "\xff\xff"))
        ->help("C2 x LP Source-Compatible Function Call")
        ->example("xb *al")
        ->regnameind("al");
    // TODO: XCALL pma,*,ARPn — C2 x LP Source-Compatible Function Call
    // TODO: XCALL pma,COND — C2xLP Source-Compatible Function Call
    // TODO: XMAC P,loc16,*(pma) — C2xLP Source-compatible Multiply and Accumulate
    // TODO: XMACD P,loc16,*(pma) — C2xLP Source-Compatible Multiply and Accumulate With Data Move
    insert(mnem("xor", 2, "\xb7\x00", "\xff\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor al, @var")
        ->regname("acc")
        ->loc16("\x00\xff");
    // TODO: XOR ACC,#16bit << #0..16 — Bitwise Exclusive OR
    insert(mnem("xor", 2, "\x70\x00", "\xff\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor al, @var")
        ->regname("al")
        ->loc16("\x00\xff");
    insert(mnem("xor", 2, "\x71\x00", "\xff\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor ah, @var")
        ->regname("ah")
        ->loc16("\x00\xff");
    insert(mnem("xor", 2, "\xf2\x00", "\xff\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor @var, al")
        ->loc16("\x00\xff")
        ->regname("al");
    insert(mnem("xor", 2, "\xf3\x00", "\xff\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor @var, ah")
        ->loc16("\x00\xff")
        ->regname("ah");
    insert(mnem("xor", 2, "\x1c\x00\x00\x00", "\xff\x00\x00\x00"))
        ->help("Bitwise Exclusive OR")
        ->example("xor @var, #(1 << 2 | 1 << 14)")
        ->loc16("\x00\xff\x00\x00")
        ->imm("\x00\x00\xff\xff");
    insert(mnem("xorb", 2, "\xf0\x00", "\xff\xff"))
        ->help("Bitwise Exclusive OR 8-bit Value")
        ->example("xorb al, #0x80")
        ->imm("\x00\xff");
    insert(mnem("xorb", 2, "\xf1\x00", "\xff\xff"))
        ->help("Bitwise Exclusive OR 8-bit Value")
        ->example("xorb ah, #0x80")
        ->imm("\x00\xff");
    // TODO: XPREAD loc16, *(pma) — C2xLP Source-Compatible Program Read
    // TODO: XPREAD loc16, *AL — C2xLP Source-Compatible Program Read
    // TODO: XPWRITE *A,loc16 — C2xLP Source-Compatible Program Write
    insert(mnem("xret", 2, "\x56\xff", "\xff\xff"))
        ->help("C2xLP Source-Compatible Return")
        ->example("xret")
        ->prioritize(); // Remove alias conflict: xretc unc
    insert(mnem("xretc", 2, "\x56\xf0", "\xff\xf0"))
        ->help("C2xLP Source-Compatible Conditional Return")
        ->example("xretc eq")
        ->cond("\x00\x0f");
    insert(mnem("zalr", 4, "\x56\x13\x00\x00", "\xff\xff\xff\x00"))
        ->help("Zero AL and Load AH With Rounding")
        ->example("zalr acc, @var")
        ->regname("acc")
        ->loc16("\x00\x00\x00\xff");
    insert(mnem("zap", 2, "\x56\x5c", "\xff\xff"))
        ->help("Clear Overflow Counter")
        ->example("zap ovc")
        ->prioritize() // Remove alias conflict: clrc ovc
        ->regname("ovc");
    insert(mnem("zapa", 2, "\x56\x33", "\xff\xff"))
        ->help("Zero Accumulator and P Register")
        ->example("zapa");
}

// TMS320C28x 16-Bit Register Access Decoding.
GAParameterTMS320C28xReg16::GAParameterTMS320C28xReg16(const char *mask)
{
    setMask(mask);
}

int GAParameterTMS320C28xReg16::match(GAParserOperand *op, int len)
{
    if (op->prefix != this->prefix || op->suffix != this->suffix)
    {
        return 0;
    }

    if (regnames.key(op->value))
    {
        return 1;
    }

    return 0;
}

QString GAParameterTMS320C28xReg16::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    QString rendering;

    if (p >= 0b0'0'000'000 && p <= 0b0'0'111'111)
    {
        // Direct
        prefix = "@";
        suffix = "";
        rendering = prefix + QString::asprintf("%d", (unsigned int)p);
    }
    else if (p >= 0b0'1'000'000 && p <= 0b0'1'111'111)
    {
        // Stack
        prefix = "*-";
        suffix = QString::asprintf("[%d]", (unsigned int)p);
        rendering = prefix + "sp" + suffix;
    }
    else if (p == 0b1'0'111'101)
    {
        // Stack
        prefix = "*";
        suffix = "++";
        rendering = prefix + "sp" + suffix;
    }
    else if (p == 0b1'0'111'110)
    {
        // Stack
        prefix = "*--";
        suffix = "";
        rendering = prefix + "sp" + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'000'000)
    {
        // C28x Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'001'000)
    {
        // C28x Indirect
        prefix = "*--";
        suffix = "";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'010'000)
    {
        // C28x Indirect
        prefix = "*+";
        suffix = "[ar0]";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'011'000)
    {
        // C28x Indirect
        prefix = "*+";
        suffix = "[ar1]";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'000'000) == 0b1'1'000'000)
    {
        // C28x Indirect
        uint8_t iii = (p & 0b111'000) >> 3;
        if (iii == 0)
        {
            prefix = "*";
            suffix = "";
        }
        else
        {
            prefix = "*+";
            suffix = QString::asprintf("[%d]", (unsigned int)iii);
        }
        rendering = prefix + regnames.value(p & 0b111) + suffix;
    }
    else if (p == 0b1'0'111'000)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'001)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'010)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'011)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + "0" + suffix;
    }
    else if (p == 0b1'0'111'100)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + "0" + suffix;
    }
    else if (p == 0b1'0'101'110)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + "br0" + suffix;
    }
    else if (p == 0b1'0'101'111)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + "br0" + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'110'000)
    {
        // C2xLP Indirect
        uint8_t rrr = p & 0b111;
        prefix = "*";
        suffix = QString::asprintf(",arp%d", (unsigned int)rrr);
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'111)
    {
        // Circular Indirect
        prefix = "*";
        suffix = "%++";
        rendering = prefix + "ar6" + suffix;
    }
    else if ((p & 0b1'1'110'000) == 0b1'0'100'000)
    {
        prefix = "@";
        suffix = "";
        rendering = prefix + regnames.value(p & 0b1'111) + suffix;
    }

    return rendering;
}

void GAParameterTMS320C28xReg16::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    uint64_t val = regnames.key(op.value);

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x 32-Bit Register Access Decoding.
GAParameterTMS320C28xReg32::GAParameterTMS320C28xReg32(const char *mask)
{
    setMask(mask);
}

int GAParameterTMS320C28xReg32::match(GAParserOperand *op, int len)
{
    if (op->prefix != this->prefix || op->suffix != this->suffix)
    {
        return 0;
    }

    if (regnames.key(op->value))
    {
        return 1;
    }

    return 0;
}

QString GAParameterTMS320C28xReg32::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    QString rendering;

    if (p >= 0b0'0'000'000 && p <= 0b0'0'111'111)
    {
        // Direct
        prefix = "@";
        suffix = "";
        rendering = prefix + QString::asprintf("%d", (unsigned int)p);
    }
    else if (p >= 0b0'1'000'000 && p <= 0b0'1'111'111)
    {
        // Stack
        prefix = "*-";
        suffix = QString::asprintf("[%d]", (unsigned int)p);
        rendering = prefix + "sp" + suffix;
    }
    else if (p == 0b1'0'111'101)
    {
        // Stack
        prefix = "*";
        suffix = "++";
        rendering = prefix + "sp" + suffix;
    }
    else if (p == 0b1'0'111'110)
    {
        // Stack
        prefix = "*--";
        suffix = "";
        rendering = prefix + "sp" + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'000'000)
    {
        // C28x Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'001'000)
    {
        // C28x Indirect
        prefix = "*--";
        suffix = "";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'010'000)
    {
        // C28x Indirect
        prefix = "*+";
        suffix = "[ar0]";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'011'000)
    {
        // C28x Indirect
        prefix = "*+";
        suffix = "[ar1]";
        rendering = prefix + regnames.value(p & 0b1111) + suffix;
    }
    else if ((p & 0b1'1'000'000) == 0b1'1'000'000)
    {
        // C28x Indirect
        uint8_t iii = (p & 0b111'000) >> 3;
        if (iii == 0)
        {
            prefix = "*";
            suffix = "";
        }
        else
        {
            prefix = "*+";
            suffix = QString::asprintf("[%d]", (unsigned int)iii);
        }
        rendering = prefix + regnames.value(p & 0b111) + suffix;
    }
    else if (p == 0b1'0'111'000)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'001)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'010)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'011)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + "0" + suffix;
    }
    else if (p == 0b1'0'111'100)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + "0" + suffix;
    }
    else if (p == 0b1'0'101'110)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "++";
        rendering = prefix + "br0" + suffix;
    }
    else if (p == 0b1'0'101'111)
    {
        // C2xLP Indirect
        prefix = "*";
        suffix = "--";
        rendering = prefix + "br0" + suffix;
    }
    else if ((p & 0b1'1'111'000) == 0b1'0'110'000)
    {
        // C2xLP Indirect
        uint8_t rrr = p & 0b111;
        prefix = "*";
        suffix = QString::asprintf(",arp%d", (unsigned int)rrr);
        rendering = prefix + suffix;
    }
    else if (p == 0b1'0'111'111)
    {
        // Circular Indirect
        prefix = "*";
        suffix = "%++";
        rendering = prefix + "ar6" + suffix;
    }
    else if ((p & 0b1'1'110'000) == 0b1'0'100'000)
    {
        prefix = "@";
        suffix = "";
        rendering = prefix + regnames.value(p & 0b1'111) + suffix;
    }

    return rendering;
}

void GAParameterTMS320C28xReg32::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    uint64_t val = regnames.key(op.value);

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x 22-bit immediate.
GAParameterTMS320C28xImm22::GAParameterTMS320C28xImm22(const char *mask)
{
    setMask(mask);
}

int GAParameterTMS320C28xImm22::match(GAParserOperand *op, int len)
{
    int64_t val = op->uint64(false); // False on a match.

    if (op->prefix != prefix || op->suffix != suffix)
        return 0;

    // Reject values that don't fit.
    if (val > 0x3FFFFF)
    {
        return 0;
    }

    return 1;
}

QString GAParameterTMS320C28xImm22::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    return QString::asprintf("0x%02x", (unsigned int)p);
}

void GAParameterTMS320C28xImm22::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    int64_t val = op.uint64(true);

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x 16-bit SP-relative address.
GAParameterTMS320C28xRel16::GAParameterTMS320C28xRel16(const char *mask)
{
    setMask(mask);
}

QString GAParameterTMS320C28xRel16::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    int64_t p = rawdecode(lang, adr, bytes, inslen);

    return QString::asprintf("0x%02x", (unsigned int)(p));
}

void GAParameterTMS320C28xRel16::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    int64_t val = op.uint64(true);

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x constant operand.
GAParameterTMS320C28xConst::GAParameterTMS320C28xConst(unsigned int value)
{
    const_value = value;
}

int GAParameterTMS320C28xConst::match(GAParserOperand *op, int len)
{
    if (op->prefix != prefix || op->suffix != suffix)
        return 0;

    if (op->value.toInt() != const_value)
        return 0;

    return 1;
}

QString GAParameterTMS320C28xConst::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    return QString::asprintf("#%d", (unsigned int)const_value);
}

void GAParameterTMS320C28xConst::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    rawencode(lang, adr, bytes, op, inslen, const_value);
}

// TMS320C28x 3-bit Product Mode shift bits.
GAParameterTMS320C28xPM::GAParameterTMS320C28xPM(const char *mask)
{
    setMask(mask);
}

int GAParameterTMS320C28xPM::match(GAParserOperand *op, int len)
{
    if (op->prefix != this->prefix)
        return 0; // Wrong type.

    // Is the shift in the list?
    for (auto it = shifts.begin(); it != shifts.end(); ++it)
    {
        if (op->value == it.value())
            return 1;
    }

    return 0;
}

QString GAParameterTMS320C28xPM::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    QString shift;

    for (auto it = shifts.begin(); it != shifts.end(); ++it)
    {
        if (p & it.key())
        {
            shift = it.value();
            break;
        }
    }

    QString rendering = prefix + shift + suffix;

    return rendering;
}

void GAParameterTMS320C28xPM::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    uint64_t val;

    for (auto it = shifts.begin(); it != shifts.end(); ++it)
    {
        if (op.value == it.value())
        {
            val += it.key();
            break;
        }
    }

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x 8-bit Status mode bits.
GAParameterTMS320C28xStatus::GAParameterTMS320C28xStatus(const char *mask)
{
    setMask(mask);
}

int GAParameterTMS320C28xStatus::match(GAParserOperand *op, int len)
{
    if (op->prefix != this->prefix)
        return 0; // Wrong type.

    // Is the status in the list?
    for (auto it = status.begin(); it != status.end(); ++it)
    {
        if (op->value == it.value())
            return 1;
    }

    return 0;
}

QString GAParameterTMS320C28xStatus::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    QString bits;

    for (auto it = status.begin(); it != status.end(); ++it)
    {
        if (p & it.key())
        {
            if (!bits.isEmpty())
            {
                bits.append(",");
            }
            bits.append(it.value());
        }
    }

    QString rendering = prefix + bits + suffix;

    return rendering;
}

void GAParameterTMS320C28xStatus::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    uint64_t val;

    for (auto it = status.begin(); it != status.end(); ++it)
    {
        if (op.value == it.value())
        {
            val += it.key();
        }
    }

    rawencode(lang, adr, bytes, op, inslen, val);
}

// TMS320C28x 8-bit Condition code bits.
GAParameterTMS320C28xCondition::GAParameterTMS320C28xCondition(const char *mask, bool reduced)
{
    setMask(mask);
    this->reduced = reduced;
}

int GAParameterTMS320C28xCondition::match(GAParserOperand *op, int len)
{
    if (op->prefix != this->prefix || op->suffix != this->suffix)
    {
        return 0;
    }

    if (this->reduced && conds_reduced.contains(op->value))
    {
        return 1;
    }
    else if (conds.contains(op->value))
    {
        return 1;
    }

    return 0;
}

QString GAParameterTMS320C28xCondition::decode(GALanguage *lang, uint64_t adr,
                                               const char *bytes, int inslen)
{
    uint64_t p = rawdecode(lang, adr, bytes, inslen);

    QString cond;

    if (this->reduced)
    {
        cond = conds_reduced[p];
    }
    else
    {
        cond = conds[p];
    }

    return cond;
}

void GAParameterTMS320C28xCondition::encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen)
{
    int64_t val = op.uint64(true);

    rawencode(lang, adr, bytes, op, inslen, val);
}
