#include "galangucom43.h"

#include "gamnemonic.h"

/* This module intentionally avoids abstractions to keep
 * the mechanism of the code visible.  In writing other modules,
 * I expect that you will make a few helper functions rather than
 * instantiate GAMnemonic directly.
 *
 * The uCOM43 instruction set is defined in PDF page 32 of docs/ucom43.pdf.
 */

GALangUcom43::GALangUcom43() {
    /* These two are defined the long way:
     * mnem, length, opcode bytes, and opcode mask.
     */
    name="ucom43";
    endian=GALanguage::BIG;

    /* Duplicate of "li 0".
    insert(new GAMnemonic(
        "cla",  // Clear Acc
        1,      // Length
        "\x90", // 0x90 as the opcode.
        "\xff"  // Mask.  All bits are used in the opcode.
        ))
        //Help string, for the cheat sheet.
        ->help("Clears accumulator.");
    */
    insert(new GAMnemonic("clc", "\x0b"))
        ->help("Reset carry f/f.");

    /* We can also define them the short way, with just
     * a name and a byte.  A length of 1 and a mask of FF are implied.
     */
    insert(new GAMnemonic("cma", "\x10"))
        ->help("Complement A.");
    insert(new GAMnemonic("cia","\x11"))
        ->help("Complement A, increment A.");
    insert(new GAMnemonic("inc","\x0d"))
        ->help("Increment A, skip next on carry.");
    insert(new GAMnemonic("dec","\x0f"))
        ->help("Decrement A, skip next on carry.");
    insert(new GAMnemonic("stc","\x1b"))
        ->help("Set carry f/f.");
    insert(new GAMnemonic("xc","\x1a"))
        ->help("Exchange carry f/f with carry save f/f.");
    insert(new GAMnemonic("rar","\x30"))
        ->help("Rotate Acc right thorugh carry f/f.");
    insert(new GAMnemonic("inm","\x1d"))
        ->help("Incrmeent RAM, skip next on carry.");
    insert(new GAMnemonic("dem","\x1f"))
        ->help("Decrement RAM, skip next on carry.");
    insert(new GAMnemonic("ad","\x08"))
        ->help("Add RAM to A, skip next on carry.");
    insert(new GAMnemonic("ads","\x09"))
        ->help("Add RAM and carry to A, store carry f/f, skip next on carry.");
    insert(new GAMnemonic("adc","\x19"))
        ->help("Add RAM and carry to A, store carry f/f.");
    insert(new GAMnemonic("daa","\x06"))
        ->help("Add 6 to A to adjust decimal for BCD addition.");
    insert(new GAMnemonic("das","\x0a"))
        ->help("Add 10 to A to adjust decimal for BCD substraction.");
    insert(new GAMnemonic("exl","\x18"))
        ->help("Xor A with RAM.");

    /* Parameters are added after the GAMnemonic is created.  They appear
     * in order in assembly language and are defined by their bitmask in
     * machine language.
     *
     * imm() is the simplest parameter, just a number as an immediate value.
     * Addresses will be much more complicated.
     */

    //Only the first of these is executed in a sequence.
    insert((new GAMnemonic("li",1,"\x90","\xf0")))
        ->help("Load immediate into A.")
        ->example("li #0xf")
        ->imm("\x0f");
    insert(new GAMnemonic("s","\x02"))
        ->help("Store A into RAM.");

    /* Some of these collide because the programmer's manual lists
     * a different opcode for when the argument is zero.  Here
     * we comment the duplicates so that the extras dissappear.
     */
    //insert(new GAMnemonic("l","\x38"));
    insert((new GAMnemonic("lm", 1, "\x38", "\xfc")))
        ->help("Load Acc with RAM, xor DP with immediate.")
        ->example("lm #0x3")
        ->imm("\x03");

    //Also collide
    //insert(new GAMnemonic("x","\x28"));
    insert((new GAMnemonic("xm", 1, "\x28", "\xfc")))
        ->help("Exchange A with RAM, xor DPh with immediate.")
        ->example("xm #0x3")
        ->imm("\x03");

    //Also collide.
    //insert(new GAMnemonic("xd","\x2c"));
    insert((new GAMnemonic("xmd", 1, "\x2c", "\xfc")))
        ->help("Exchange A with RAM, xor DPh with X, DPl--, skip next on carry.")
        ->example("xmd #0x3")
        ->imm("\x03");

    //As do these.
    //insert(new GAMnemonic("xi","\x3c"));
    insert((new GAMnemonic("xmi", 1, "\x3c", "\xfc")))
        ->help("Exchange A with RAM, xor DPh with imm, DPl++, skip next on carry.")
        ->example("xmi #0x3")
        ->imm("\x03");

    insert((new GAMnemonic("ldi", 2, "\x15\x00", "\xff\x80")))
        ->help("Load DP with immedate.")
        ->example("ldi #0x7f")
        ->imm("\x00\x7f");
    insert((new GAMnemonic("ldz", 1, "\x80", "\xf0")))
        ->help("Load DPh with 0, DPl with immediate.")
        ->example("ldz #0xf")
        ->imm("\x0f");

    insert(new GAMnemonic("ded","\x13"))
        ->help("Decrement DPl, skip next on carry.");
    insert(new GAMnemonic("ind","\x33"))
        ->help("Increment DPl, skip next on carry.");
    insert(new GAMnemonic("tal","\x07"))
        ->help("Transfer A into DPl.");
    insert(new GAMnemonic("tla","\x12"))
        ->help("Transfer DPl into A.");
    insert(new GAMnemonic("xhx","\x4f"))
        ->help("Exchange DPh with X.");
    insert(new GAMnemonic("xly","\x4e"))
        ->help("Exchange DPl with Y.");
    insert(new GAMnemonic("thx","\x47"))
        ->help("Transfer DPh to X.");
    insert(new GAMnemonic("tly","\x46"))
        ->help("Transfer DPl to Y.");
    insert(new GAMnemonic("xaz","\x4a"))
        ->help("Exchange Acc with Z.");
    insert(new GAMnemonic("xaw","\x4b"))
        ->help("Exchange Acc with W.");
    insert(new GAMnemonic("taz","\x42"))
        ->help("Transfer Acc to Z.");
    insert(new GAMnemonic("taw","\x43"))
        ->help("Transfer Acc to W.");
    insert(new GAMnemonic("xhr","\x4d"))
        ->help("Exchange DPh with R.");
    insert(new GAMnemonic("xls","\x4c"))
        ->help("Exchange DPl with S.");

    /* FIXME: All of these bit instructions need support
     * for a bit type, which should be different from an
     * immediate.
     */
    insert((new GAMnemonic("smb", 1, "\x78", "\xfc")))
        ->help("Set a single bit of RAM.")
        ->example("smb #0x3")
        ->imm("\x03"); //FIXME: Bit definitions.
    insert((new GAMnemonic("rmb", 1, "\x68", "\xfc")))
        ->help("Reset a single bit of RAM.")
        ->example("rmb #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("tmb", 1, "\x58", "\xfc")))
        ->help("Skip next on bit(RAM).")
        ->example("tmb #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("tab", 1, "\x24", "\xfc")))
        ->help("Skip next on bit(A).")
        ->example("tab #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("cmb", 1, "\x34", "\xfc")))
        ->help("Skip next if the immediate bit of A equals the same bit in RAM.")
        ->example("cmb #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("sfb", 1, "\x7c", "\xfc")))
        ->help("Set a single bit of Flag.")
        ->example("sfb #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("rfb", 1, "\x6c", "\xfc")))
        ->help("Reset a single bit of Flag.")
        ->example("rfb #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("fbt", 1, "\x5c", "\xfc")))
        ->help("Skip next on bit test of Flag.")
        ->example("fbt #0x3")
        ->imm("\x03");
    insert((new GAMnemonic("fbf", 1, "\x20", "\xfc")))
        ->help("Skip next on not bit test of Flag.")
        ->example("fbf #0x3")
        ->imm("\x03");

    insert(new GAMnemonic("cm","\x0c"))
        ->help("Skip next if A equals RAM.");

    insert((new GAMnemonic("ci", 2, "\x17\xc0", "\xff\xf0")))
        ->help("Skip next if A equals immediate.")
        ->example("ci #0xf")
        ->imm("\x00\x0f");
    insert((new GAMnemonic("cli", 2, "\x16\xe0", "\xff\xf0")))
        ->help("Skip next if DPl equals immediate.")
        ->example("cli #0xf")
        ->imm("\x00\x0f");

    insert(new GAMnemonic("tc","\x04"))
        ->help("Skip next on carry f/f.");
    insert(new GAMnemonic("ttm","\x05"))
        ->help("Skip next on timer f/f.");
    insert(new GAMnemonic("tit","\x03"))
        ->help("Skip next on interrupt f/f, reset interrupt f/f.");

    insert((new GAMnemonic("jcp", 1, "\xc0", "\xc0")))
        ->help("Jump to address in current page.")
        ->example("jcp 0x3f")
        ->pageadr("\x3f", 0x3f); //FIXME: Page address.
    insert((new GAMnemonic("jmp", 2, "\xa0\x00", "\xf8\x00")))
        ->help("Jump to address.")
        ->example("jmp 0x7ff")
        ->adr("\x07\xff");

    //Page 35
    insert(new GAMnemonic("jpa","\x41"))
        ->help("Jump to Acc in current page.");
    insert(new GAMnemonic("ei","\x31"))
        ->help("Set interrupt enable f/f.");
    insert(new GAMnemonic("di","\x01"))
        ->help("Reset interrupt enable f/f.");
    insert((new GAMnemonic("czp", 1, "\xb0", "\xf0")))
        ->help("Call Address (Zero Page)")
        ->example("czp #0xf")
        ->imm("\x0f");
    insert((new GAMnemonic("cal", 2, "\xa8\x00", "\xf8\x00")))
        ->help("Call Address.")
        ->example("cal 0x7ff")
        ->adr("\x07\xff");
    insert(new GAMnemonic("rt","\x48"))
        ->help("Return from subroutine.");
    insert(new GAMnemonic("rts","\x49"))
        ->help("Return from subroutine, skip next.");

    //FIXME: More bit fields.
    insert((new GAMnemonic("seb", 1, "\x74", "\xfc")))
        ->help("Single bit set on port E.")
        ->example("seb #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("reb", 1, "\x64", "\xfc"))
        ->help("Single bit reset on port E.")
        ->example("reb #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("spb", 1, "\x70", "\xfc"))
        ->help("Single bit set of output port DP1.")
        ->example("spb #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("rpb", 1, "\x60", "\xfc"))
        ->help("Single bit reset of ourput port DP1.")
        ->example("rpb #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("tpa", 1, "\x54", "\xfc"))
        ->help("Skip next on bit of input port A.")
        ->example("tpa #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("tpb", 1, "\x50", "\xfc"))
        ->help("Skip next on bit of input port DPl.")
        ->example("tpb #0x3")
        ->imm("\x03");
    insert(new GAMnemonic("oe","\x44"))
        ->help("4-bit parallel output from Acc to port E.");
    insert(new GAMnemonic("op","\x0e"))
        ->help("4-bit parallel output from Acc to port DPl.");
    insert(new GAMnemonic("ocd", 2, "\x1e\x00", "\xff\x00"))
        ->help("8-bit immediate output from immediate to ports C and D.")
        ->example("ocd #0xff")
        ->imm("\x00\xff");
    insert(new GAMnemonic("ia","\x40"))
        ->help("4-bit parallel input from port A to Acc.");
    insert(new GAMnemonic("ip","\x32"))
        ->help("4-bit parallel input from port DPl to Acc.");
    insert((new GAMnemonic("stm", 2, "\x14\x80", "\xff\xc0")))
        ->help("Timer Set")
        ->example("stm #0x3f")
        ->imm("\x00\x3f");
    insert(new GAMnemonic("nop","\x00"))
        ->help("No operation.");
}
