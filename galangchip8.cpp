#include "gamnemonic.h"
#include "galangchip8.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

GALangChip8::GALangChip8() {
    endian=BIG;
    name="chip8";
    maxbytes=2;   // Longest instruction.
    align=2;      // Alignment is two bytes.

    regnames.clear();
    regnames<<"i"<<"dt"<<"st"<<"k"<<"f"<<"b"
             //FIXME: We use r0 to r15 instead of v0 to vf.
             <<"r0"<<"r1"<<"r2"<<"r3"<<"r4"<<"r5"<<"r6"<<"r7"
             <<"r8"<<"r9"<<"r10"<<"r11"<<"r12"<<"r13"<<"r14"<<"r15";


    //FIXME: Support incompatible variants.

    //Unique to Chip-8 Instruction Set, missing from Cowgod.
    insert(mnem("nop", 2, "\x00\x00", "\xff\xff"))
        ->help("No Operation")
        ->prioritize() // Could be considered a system call.
        ->example("nop");

    /* FIXME: Maybe support this but warn about it? */
    insert(mnem("sys", 2, "\x00\x00", "\xf0\x00"))
        ->help("System Call, ignored on modern machines.")
        ->example("sys #0x0fff")
        ->imm("\x0f\xff");

    insert(mnem("cls", 2, "\x00\xe0", "\xff\xff"))
        ->help("Clear the Display")
        ->prioritize() // Could be considered a system call.
        ->example("cls");
    insert(mnem("ret", 2, "\x00\xee", "\xff\xff"))
        ->help("Return from a Subroutine")
        ->prioritize() // Could be considered a system call.
        ->example("ret");

    insert(mnem("jp", 2, "\x10\x00", "\xf0\x00"))
        ->help("Jump to an address.")
        ->example("jp 0x0fff")
        ->adr("\x0f\xff");
    insert(mnem("call", 2, "\x20\x00", "\xf0\x00"))
        ->help("Call a subroutine at an address.")
        ->example("call 0x0fff")
        ->adr("\x0f\xff");

    insert(mnem("se", 2, "\x30\x00", "\xf0\x00"))
        ->help("Skip if register equals immediate.")
        ->example("se r0, #0xff")
        ->regdir("\x0f\x00")
        ->imm("\x00\xff");
    insert(mnem("sne", 2, "\x40\x00", "\xf0\x00"))
        ->help("Skip if register does not equal immediate.")
        ->example("sne r0, #0xff")
        ->regdir("\x0f\x00")
        ->imm("\x00\xff");
    insert(mnem("se", 2, "\x50\x00", "\xf0\x0f"))
        ->help("Skip if register equals register.")
        ->example("se r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("ld", 2, "\x60\x00", "\xf0\x00"))
        ->help("Loads a register with an immediate byte.")
        ->example("ld r0, #0xff")
        ->regdir("\x0f\x00")
        ->imm("\x00\xff");
    insert(mnem("add", 2, "\x70\x00", "\xf0\x00"))
        ->help("Adds an immediate byte into a register")
        ->example("add r0, #0xff")
        ->regdir("\x0f\x00")
        ->imm("\x00\xff");
    insert(mnem("ld", 2, "\x80\x00", "\xf0\x0f"))
        ->help("Loads the first register with the second.")
        ->example("ld r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("or", 2, "\x80\x01", "\xf0\x0f"))
        ->help("ORs the first register with the second.")
        ->example("or r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("and", 2, "\x80\x02", "\xf0\x0f"))
        ->help("ANDs the first register with the second.")
        ->example("and r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("xor", 2, "\x80\x03", "\xf0\x0f"))
        ->help("XORs the first register with the second.")
        ->example("xor r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("add", 2, "\x80\x04", "\xf0\x0f"))
        ->help("Adds the first register with the second, carry in VF.")
        ->example("add r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("sub", 2, "\x80\x05", "\xf0\x0f"))
        ->help("Subs the first register with the second, carry from VF.")
        ->example("sub r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("shr", 2, "\x80\x06", "\xf0\x0f"))
        ->help("FIXME: Explain this.")
        ->example("shr r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("subn", 2, "\x80\x07", "\xf0\x0f"))
        ->help("Subtract first param from second, storing result into first.  VF=not borrow.")
        ->example("subn r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("shl", 2, "\x80\x0e", "\xf0\x0f"))
        ->help("FIXME: Explain this.")
        ->example("shl r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("sne", 2, "\x90\x00", "\xf0\x0f"))
        ->help("Skip next if registers are not equal.")
        ->example("sne r0, r1")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0");
    insert(mnem("ld", 2, "\xa0\x00", "\xf0\x00"))
        ->help("Skip next if registers are not equal.")
        ->example("ld i, #0x025f")
        ->regname("i")
        ->imm("\x0f\xff");
    insert(mnem("jp", 2, "\xb0\x00", "\xf0\x00"))
        ->help("Jump to r0+adr.")
        ->example("jp r0, 0x0fff")
        ->regname("r0")
        ->adr("\x0f\xff");
    insert(mnem("rnd", 2, "\xc0\x00", "\xf0\x00"))
        ->help("Load register with random value AND mask.")
        ->example("rnd r0, #0xff")
        ->regdir("\x0f\x00")
        ->imm("\x00\xff");
    insert(mnem("drw", 2, "\xd0\x00", "\xf0\x00"))
        ->help("FIXME: Explain this.")
        ->example("drw r0, r1, #0xf")
        ->regdir("\x0f\x00")
        ->regdir("\x00\xf0")
        ->imm("\x00\xf");
    insert(mnem("skp", 2, "\xe0\x9e", "\xf0\xff"))
        ->help("Skip if key with register value is pressed.")
        ->example("skp r0")
        ->regdir("\x0f\x00");
    insert(mnem("sknp", 2, "\xe0\xa1", "\xf0\xff"))
        ->help("Skip if key with register value is not pressed.")
        ->example("sknp r0")
        ->regdir("\x0f\x00");
    insert(mnem("ld", 2, "\xf0\x07", "\xf0\xff"))
        ->help("Load reg with timer value.")
        ->example("ld r0, dt")
        ->regdir("\x0f\x00")
        ->regname("dt");
    insert(mnem("ld", 2, "\xf0\x0a", "\xf0\xff"))
        ->help("Load reg with keypress.")
        ->example("ld r0, k")
        ->regdir("\x0f\x00")
        ->regname("k");
    insert(mnem("ld", 2, "\xf0\x15", "\xf0\xff"))
        ->help("Load delay timer with reg.")
        ->example("ld dt, r0")
        ->regname("dt")
        ->regdir("\x0f\x00");
    insert(mnem("ld", 2, "\xf0\x18", "\xf0\xff"))
        ->help("Load sound timer with reg.")
        ->example("ld st, r0")
        ->regname("st")
        ->regdir("\x0f\x00");
    insert(mnem("add", 2, "\xf0\x1e", "\xf0\xff"))
        ->help("Add a reg to the index.")
        ->example("add i, r0")
        ->regname("i")
        ->regdir("\x0f\x00");

    insert(mnem("ld", 2, "\xf0\x29", "\xf0\xff"))
        ->help("Load index with the font digit of a reg.")
        ->example("ld f, r0")
        ->regname("f")
        ->regdir("\x0f\x00");
    insert(mnem("ld", 2, "\xf0\x33", "\xf0\xff"))
        ->help("Store BCD of reg to mem[i], mem[i+1], mem[i+2].")
        ->example("ld b, r0")
        ->regname("b")
        ->regdir("\x0f\x00");

    insert(mnem("ld", 2, "\xf0\x55", "\xf0\xff"))
        ->help("Load all registered up to chosen into memory at index.")
        ->example("ld @i, r0")
        ->regnameind("i")
        ->regdir("\x0f\x00");
    insert(mnem("ld", 2, "\xf0\x65", "\xf0\xff"))
        ->help("Load all registered up to chosen from memory at index.")
        ->example("ld r0, @i")
        ->regdir("\x0f\x00")
        ->regnameind("i");

}
