#include "galangs2000.h"

#include "gamnemonic.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

/* This class represents the AMI S2000 series, which was
 * also sold as Iskra's EMZ1001 series in Yugoslovia.
 *
 * Definitions are on PDF page 23 (paper page 22) of
 * docs/AMI_S2000_Programming_Manual_Rev2_(june_1978).pdf
 *
 */

GALangS2000::GALangS2000() {
    /* The instruction set begins with opcodes that are exactly one
     * byte, having no parameters and a mask of 0xFF.  For convenience,
     * we define withese without explicitly defining the length and mask.
     */


    name="s2000"; //FIXME: Lacks emz1001 synonym.

    regnames.clear();
    regnames<<"a"<<"f1"<<"f2";


    //Basic instructions have no operands.
    insert(mnem(
        "nop",  // Mnemonic name.
        "\x00"  // Opcode byte.
        ))
        ->help("No operation.")  // Short help text for cheat sheet.
        ->example("nop");        // Minimal instruction for self-test.
    insert(mnem("brk","\x01"))
        ->help("Break.")
        ->example("brk");
    insert(mnem("rt","\x02"))
        ->help("Return from subroutine.")
        ->example("rt");
    insert(mnem("rts","\x03"))
        ->help("Return from subroutine and skip next instruction.")
        ->example("rts");
    insert(mnem("psh","\x04"))
        ->help("Preset high the master strobe addressed by BL.")
        ->example("psh");
    insert(mnem("psl","\x05"))
        ->help("Preset low the master strobe addressed by BL.")
        ->example("psl");
    insert(mnem("and","\x06"))
        ->help("Logical and, mem with Acc.")
        ->example("and");
    insert(mnem("sos","\x07"))
        ->help("Skip on seconds flag/timer output.")
        ->example("sos");
    insert(mnem("sbe","\x08"))
        ->help("Skip if BL=E.")
        ->example("sbe");
    insert(mnem("szc","\x09"))
        ->help("Skip if carry is zero.")
        ->example("szc");
    insert(mnem("stc","\x0a"))
        ->help("Set carry to 1.")
        ->example("stc");
    insert(mnem("rsc","\x0b"))
        ->help("Clear carry to 0.")
        ->example("rsc");
    insert(mnem("lae","\x0c"))
        ->help("Load Acc from E.")
        ->example("lae");
    insert(mnem("xae","\x0d"))
        ->help("Exchange Acc and E.")
        ->example("xae");
    insert(mnem("inp","\x0e"))
        ->help("Input D lines to Acc and mem.")
        ->example("inp");
    insert(mnem("eur","\x0f"))
        ->help("Set 50/60 Hz and display latch polarity.")
        ->example("eur a")
        ->regname("a");
    insert(mnem("cma","\x10"))
        ->help("Logical 1's complement Acc.  Acc=15-Acc.")
        ->example("cma");
    insert(mnem("xabu","\x11"))
        ->help("Exchange low bits of Acc with BU.")
        ->example("xabu");
    insert(mnem("lab","\x12"))
        ->help("Load Acc from BL.")
        ->example("lab");
    insert(mnem("xab","\x13"))
        ->help("Exchange Acc and BL.")
        ->example("xab");
    insert(mnem("adcs","\x14"))
        ->help("Add mem to Acc with carry.  Skip if sum<=15.")
        ->example("adcs");
    insert(mnem("xor","\x15"))
        ->help("Logical XOR.  Mem with Acc.")
        ->example("xor");
    insert(mnem("add","\x16"))
        ->help("Add mem to Acc.  Carry unaltered.")
        ->example("add");
    insert(mnem("sam","\x17"))
        ->help("Skip if Acc==mem.")
        ->example("sam");
    insert(mnem("disb","\x18"))
        ->help("Display data from Acc and mem.")
        ->example("disb");
    insert(mnem("mvs","\x19"))
        ->help("Moves A-line master strobe latch to A line.  Floats D.")
        ->example("mvs");
    insert(mnem("out","\x1a"))
        ->help("Output to D lines from Acc and mem.  EXT pulse at T7 time.")
        ->example("out");
    insert(mnem("disn","\x1b"))
        ->help("Display number in Acc, decimal point from carry.")
        ->example("disn");

    /* These instructions do have an immediate value, so we must
     * define the parameters and add an immediate.  Note that
     * we add the immediate *after* inserting the instruction,
     * thanks to returned pointers.
     */

    //Bit mask instructions.
    insert(mnem(
               "szm",   // Mnemonic name.
               1,       // Length.
               "\x1c",  // Opcode byte.
               "\xfc"   // Opcode bitmask, covering all but least two bits.
               ))
        ->help("Skip if memory bit is zero.")
        ->example("szm bit4")
        ->bit3("\x03");
    insert(mnem("stm",1,"\x20","\xfc"))
        ->help("Set immediate bit in memory.")
        ->example("stm bit4")
        ->bit3("\x03");
    insert(mnem("rsm",1,"\x24","\xfc"))
        ->help("Clear immediate bit in memory.")
        ->example("rsm bit4")
        ->bit3("\x03");


    //More simple instructions.
    insert(mnem("szk","\x28"))
        ->help("Skip if zero in K input.")
        ->example("szk");
    insert(mnem("szi","\x29"))
        ->help("Skip if zero in I input.")
        ->example("szi");
    insert(mnem("rf","\x2a"))
        ->help("Clear flag 1.")
        ->example("rf f1")
        ->regname("f1");
    insert(mnem("sf","\x2b"))
        ->help("Set flag 1.")
        ->example("sf f1")
        ->regname("f1");
    insert(mnem("rf","\x2c"))
        ->help("Clear flag 2.")
        ->example("rf f2")
        ->regname("f2");
    insert(mnem("sf","\x2d"))
        ->help("Set flag 2.")
        ->example("sf f2")
        ->regname("f2");
    insert(mnem("tf","\x2e"))
        ->help("Skip if flag 1 is set.")
        ->example("tf f1")
        ->regname("f1");
    insert(mnem("tf","\x2f"))
        ->help("Skip if flag 2 is set.")
        ->example("tf f2")
        ->regname("f2");

    //These four have inverted arguments.
    insert(mnem(
               "xci",    // Mnemonic name.
               1,        // Length.
               "\x30",   // Opcode byte.
               "\xfc",   // Opcode bitmask, covering all but least two bits.
               "\x03"    // Those two bits are inverted.
               ))
        ->help("Exchange Acc with mem, inc BL and modify BU.  Skip if ++BL==0.")
        ->example("xci #0x3")
        ->imm("\x03");   // And those two bits are an immediate argument.
    insert(mnem("xcd",1,"\x34","\xfc","\x03"))
        ->help("Exchange Acc with mem, dec BL and modify BU.  Skip if BL--==0.")
        ->example("xcd #0x3")
        ->imm("\x03");
    insert(mnem("xc",1,"\x38","\xfc","\x03"))
        ->help("Exchange Acc with mem, then modify BU.")
        ->example("xc #0x3")
        ->imm("\x03");
    insert(mnem("lam",1,"\x3c","\xfc","\x03"))
        ->help("Load Acc from mem, then modify BU.")
        ->example("lam #0x3")
        ->imm("\x03");

    //These arguments are not inverted.
    insert(mnem("lbz",1,"\x40","\xfc"))
        ->help("Load BL with 0 and BU with imm.")
        ->example("lbz #0x3")
        ->imm("\x03");
    insert(mnem("lbf",1,"\x44","\xfc"))
        ->help("Load BL with 15 and BU with imm.")
        ->example("lbf #0x3")
        ->imm("\x03");
    insert(mnem("lbe",1,"\x48","\xfc"))
        ->help("Load BL with E and BU with imm.")
        ->example("lbe #0x3")
        ->imm("\x03");
    insert(mnem("lbep",1,"\x4c","\xfc"))
        ->help("Load BL with E+1 and BU with imm.")
        ->example("lbep #0x3")
        ->imm("\x03");
    insert(mnem("adis",1,"\x50","\xf0"))
        ->help("Add imm to Acc.  Skip if sum<=15.  Carry unaltered.")
        ->example("adis #0xf")
        ->imm("\x0f");

    /* PP comes before a jump instruction.  The first PP sets the page,
     * while the second PP in a row sets the bank.  These and the jump
     * after them are all skipped as one unit.
     */
    //This is inverted.
    insert(mnem("pp",1,"\x60","\xf0", "\x0f"))
        ->help("Prepare page or bank register.")
        ->example("pp #0xf")
        ->imm("\x0f");  //Page Prepare;

    //Do not invert.
    insert(mnem("lai",1,"\x70","\xf0"))
        ->help("Load accumulator immediate.")
        ->example("lai #0xf")
        ->imm("\x0f");
    insert(mnem("jms",1,"\x80","\xc0"))
        //Should we mention the exception, when preceded by a PP instruction?
        ->help("Jump to immediate in page 15, saving state to stack.")
        ->example("jms #0x3f")
        ->imm("\x3f");
    insert(mnem("jmp",1,"\xc0","\xc0"))
        ->help("Jump to immediate in prepared page and bank.")
        ->example("jmp #0x3f")
        ->imm("\x3f");
}
