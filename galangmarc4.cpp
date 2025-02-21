#include "gamnemonic.h"
#include "galangmarc4.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

GALangMARC4::GALangMARC4(){
    //Begin at page 25/471 of marc4.pdf.
    name="marc4";

    //TODO Define endianness or max bytes?
    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        "ccr" <<
        "esp" << //Expression Stack Pointer
        "rsp" << //Return Stack Pointer
        "x"<<"y"<<
        "adr"  // Working Address
        ;


    //Basic instructions have no operands.
    insert(mnem(
               "add",  // Mnemonic name.
               "\x00"  // Opcode byte.
               ))
        ->help("Add the top two stack digits.")  // Short help text for cheat sheet.
        ->example("add");
    insert(mnem(
               "addc",  // Mnemonic name.
               "\x01"  // Opcode byte.
               ))
        ->help("Add with carry the top two stack digits.")
        ->example("addc");
    insert(mnem(
               "sub",  // Mnemonic name.
               "\x02"  // Opcode byte.
               ))
        ->help("2's complement subtraction of the top two digits.")
        ->example("sub");
    insert(mnem(
               "subb",  // Mnemonic name.
               "\x03"  // Opcode byte.
               ))
        ->help("1's complement subtraction of the top two digits.")
        ->example("subb");
    insert(mnem(
               "xor",  // Mnemonic name.
               "\x04"  // Opcode byte.
               ))
        ->help("XOR top two stack digits.")
        ->example("xor");
    insert(mnem(
               "and",  // Mnemonic name.
               "\x05"  // Opcode byte.
               ))
        ->help("Bitwise AND top two stack digits.")
        ->example("and");
    insert(mnem(
               "cmp_eq",  // Mnemonic name.
               "\x06"  // Opcode byte.
               ))
        ->help("Equality test for top two stack digits.")
        ->example("cmp_eq");
    insert(mnem(
               "cmp_ne",  // Mnemonic name.
               "\x07"  // Opcode byte.
               ))
        ->help("Uniequality test for top two stack digits.")
        ->example("cmp_ne");
    insert(mnem(
               "cmp_lt",  // Mnemonic name.
               "\x08"  // Opcode byte.
               ))
        ->help("Less-than test for top two stack digits.")
        ->example("cmp_lt");
    insert(mnem(
               "cmp_le",  // Mnemonic name.
               "\x09"  // Opcode byte.
               ))
        ->help("Less-or-equal for top two stack digits.")
        ->example("cmp_le");
    insert(mnem(
               "cmp_gt",  // Mnemonic name.
               "\x0a"  // Opcode byte.
               ))
        ->help("Greater-than for top two stack digits.")
        ->example("cmp_gt");
    insert(mnem(
               "cmp_ge",  // Mnemonic name.
               "\x0b"  // Opcode byte.
               ))
        ->help("Greater-or-equal for top two stack digits.")
        ->example("cmp_ge");
    insert(mnem(
               "or",  // Mnemonic name.
               "\x0c"  // Opcode byte.
               ))
        ->help("Bitwise OR top two stack digits.")
        ->example("or");
    insert(mnem(
               "push",  // Mnemonic name.
               "\x0d"  // Opcode byte.
               ))
        ->help("Copy condition code into TOS.  CCR@")
        ->example("push ccr")
        ->regname("ccr");
    insert(mnem(
               "pop",  // Mnemonic name.
               "\x0e"  // Opcode byte.
               ))
        ->help("Restore condition codes. CCR!")
        ->example("pop ccr")
        ->regname("ccr");
    insert(mnem(
               "sleep",  // Mnemonic name.
               "\x0f"  // Opcode byte.
               ))
        ->help("CPU in sleep mode, interrupts enabled.")
        ->example("sleep");


    //Page 26.
    insert(mnem(
               "shl",  // Mnemonic name.
               "\x10"  // Opcode byte.
               ))
        ->help("Shift TOS left into carry.")
        ->example("shl");
    insert(mnem(
               "roll",  // Mnemonic name.
               "\x11"  // Opcode byte.
               ))
        ->help("Rotate TOS left through carry.")
        ->example("roll");
    insert(mnem(
               "shr",  // Mnemonic name.
               "\x12"  // Opcode byte.
               ))
        ->help("Shift TOS right into carry.")
        ->example("shr");
    insert(mnem(
               "ror",  // Mnemonic name.
               "\x13"  // Opcode byte.
               ))
        ->help("Rotate TOS right through carry.")
        ->example("ror");
    insert(mnem(
               "inc",  // Mnemonic name.
               "\x14"  // Opcode byte.
               ))
        ->help("Increment TOS.")
        ->example("inc");
    insert(mnem(
               "dec",  // Mnemonic name.
               "\x15"  // Opcode byte.
               ))
        ->help("Decrement TOS.")
        ->example("dec");
    insert(mnem(
               "daa",  // Mnemonic name.
               "\x16"  // Opcode byte.
               ))
        ->help("Decimal adjust for addition in BCD arithmetic.")
        ->example("daa");
    insert(mnem(
               "not",  // Mnemonic name.
               "\x17"  // Opcode byte.
               ))
        ->help("1's complement of TOS.")
        ->example("not");
    insert(mnem(
               "tog_bf",  // Mnemonic name.
               "\x18"  // Opcode byte.
               ))
        ->help("Toggle Branch flag.")
        ->example("tog_bf");
    insert(mnem(
               "set_bcf",  // Mnemonic name.
               "\x19"  // Opcode byte.
               ))
        ->help("Set Branch and Carry flag.")
        ->example("set_bcf");
    insert(mnem(
               "di",  // Mnemonic name.
               "\x1a"  // Opcode byte.
               ))
        ->help("Disable all interrupts.")
        ->example("di");
    insert(mnem(
               "in",  // Mnemonic name.
               "\x1b"  // Opcode byte.
               ))
        ->help("Read data from 4-bit I/O port.")
        ->example("in");
    insert(mnem(
               "decr",  // Mnemonic name.
               "\x1c"  // Opcode byte.
               ))
        ->help("Decrement index on Return Stack.")
        ->example("decr");
    insert(mnem(
               "rti",  // Mnemonic name.
               "\x1d"  // Opcode byte.
               ))
        ->help("Return from interrupt routine; enable all interrupts.")
        ->example("rti");
    insert(mnem(
               "swi",  // Mnemonic name.
               "\x1e"  // Opcode byte.
               ))
        ->help("Software interrupt.")
        ->example("swi");
    insert(mnem(
               "out",  // Mnemonic name.
               "\x1f"  // Opcode byte.
               ))
        ->help("Write data to 4-bit I/O port.")
        ->example("out");
    insert(mnem(
               "table",  // Mnemonic name.
               1,        // Length
               "\x20",   // Opcode byte.
               "\xfe"    // Mask FE
               ))
        ->help("Fetch an 8-bit ROM constant and performs an EXIT to Ret_PC.")
        ->example("table")
        ->dontcare("\x01"); //We don't care about this bit, but we still have to define it.
    insert(mnem(
               "movidxstack",  // Mnemonic name.
               "\x22"  // Opcode byte.
               ))
        ->help("Move (loop) index onto Return Stack.")
        ->example("movidxstack");
    insert(mnem(
               //Also known as "r@"
               "i",  // Mnemonic name.
               "\x23"  // Opcode byte.
               ))
        ->help("Copy (loop) index from the Return Stack onto TOS. I or R@")
        ->example("i");
    insert(mnem(
               "exit",   // Mnemonic name.
               1,        // Length
               "\x24",   // Opcode byte.
               "\xfe"    //Mask
               ))
        ->help("Return from subroutine.")
        ->example("exit")
        ->dontcare("\x01"); //We don't care about this bit, but we still have to define it.


    //Page 27
    insert(mnem(
               "swap",  // Mnemonic name.
               "\x26"  // Opcode byte.
               ))
        ->help("Exchange the top two digits.")
        ->example("swap");
    insert(mnem(
               "over",  // Mnemonic name.
               "\x27"  // Opcode byte.
               ))
        ->help("Push a copy of TOS-1 onto TOS.")
        ->example("over");
    insert(mnem(
               "mov2r",  // Mnemonic name.
               "\x28"  // Opcode byte.
               ))
        ->help("Move top two digits onto Return Stack.")
        ->example("mov2r");
    insert(mnem(
               "mov3r",  // Mnemonic name.
               "\x29"  // Opcode byte.
               ))
        ->help("Move top two digits onto Return Stack.")
        ->example("mov3r");
    insert(mnem(
               "cp2rexp",  // Mnemonic name.
               "\x2a"  // Opcode byte.
               ))
        ->help("Copy two digits from Return to Expression Stack.")
        ->example("cp2rexp");
    insert(mnem(
               "cp3rexp",  // Mnemonic name.
               "\x2b"  // Opcode byte.
               ))
        ->help("Copy three digits from Return to Expression Stack.");
    insert(mnem(
               "rot",  // Mnemonic name.
               "\x2c"  // Opcode byte.
               ))
        ->help("Move third digit onto TOS.")
        ->example("rot");
    insert(mnem(
               "dup",  // Mnemonic name.
               "\x2d"  // Opcode byte.
               ))
        ->help("Duplicate TOS digit.")
        ->example("dup");
    insert(mnem(
               "drop",  // Mnemonic name.
               "\x2e"  // Opcode byte.
               ))
        ->help("Remove TOS digit from the Expression Stack.")
        ->example("drop");
    insert(mnem(
               "dropr",  // Mnemonic name.
               "\x2f"  // Opcode byte.
               ))
        ->help("Remove one entry from the Return Stack.")
        ->example("dropr");
    insert(mnem(
               "ifetchx",  // Mnemonic name.
               "\x30"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the X register.");
    insert(mnem(
               "ifetchppx",  // Mnemonic name.
               "\x31"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the pre-incremented X register.");
    insert(mnem(
               "ifetchxmm",  // Mnemonic name.
               "\x32"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the post-decremented X register.");
    insert(mnem(
               "dfetchx",    // Mnemonic name.
               2,           //Length
               "\x33\x00",  // Opcode byte.
               "\xff\x00"  //Mask
               ))
        ->help("Direct fetch from RAM addressed by the X register.")
        ->example("dfetchx #0xff")
        ->imm("\x00\xff"); //$xx
    insert(mnem(
               "ifetchy",  // Mnemonic name.
               "\x34"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the Y register.");
    insert(mnem(
               "ifetchppy",  // Mnemonic name.
               "\x35"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the pre-incremented Y register.");
    insert(mnem(
               "ifetchypp",  // Mnemonic name.
               "\x36"  // Opcode byte.
               ))
        ->help("Indirect fetch from RAM addressed by the post-incremented Y register.");
    insert(mnem(
               "dfetchy",  // Mnemonic name.
               2,        //Length
               "\x37\x00",  // Opcode.
               "\xff\x00"   // Mask.
               ))
        ->help("Direct fetch from RAM addressed by the Y register.")
        ->example("dfetchy #0xff")
        ->imm("\x00\xff"); //$xx
    insert(mnem(
               "istorex",  // Mnemonic name.
               "\x38"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addressed by the X register.");

    //Page 28
    insert(mnem(
               "istoreppx",  // Mnemonic name.
               "\x39"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addrssed by the pre-incremented X register.");
    insert(mnem(
               "istorexmm",  // Mnemonic name.
               "\x3a"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addressed by the post-decremented X register.");
    insert(mnem(
               "dstorex",  // Mnemonic name.
               2,
               "\x3b\x00",  // Opcode byte.
               "\xff\x00"   // Mask
               ))
        ->help("Direct store into RAM addressed by the X register.")
        ->example("dstorex #0xff")
        ->imm("\x00\xff");
    insert(mnem(
               "istorey",  // Mnemonic name.
               "\x3c"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addressed by the Y register.");
    insert(mnem(
               "istoreppy",  // Mnemonic name.
               "\x3d"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addressed by the pre-incremented Y register.");
    insert(mnem(
               "istoreymm",  // Mnemonic name.
               "\x3e"  // Opcode byte.
               ))
        ->help("Indirect store into RAM addressed by the post-decremented Y register.");
    insert(mnem(
               "dstorey",  // Mnemonic name.
               2,
               "\x3f\x00",  // Opcode byte.
               "\xff\x00"   // Mask
               ))
        ->help("Direct store into RAM addressed by the Y register.")
        ->example("dstorey #0xff")
        ->imm("\x00\xff"); // $xx
    insert(mnem(
               "fetch",  // Mnemonic name.
               "\x70"  // Opcode byte.
               ))
        ->help("Fetch the current Expression Stack Pointer.")
        ->example("fetch esp")
        ->regname("esp");
    insert(mnem(
               "fetch",  // Mnemonic name.
               "\x71"  // Opcode byte.
               ))
        ->help("Fetch the current Return Stack Pointer.")
        ->example("fetch rsp")
        ->regname("rsp");
    insert(mnem(
               "fetch",  // Mnemonic name.
               "\x72"  // Opcode byte.
               ))
        ->help("Fetch the current X register contents.")
        ->example("fetch x")
        ->regname("x");
    insert(mnem(
               "fetch",  // Mnemonic name.
               "\x73"  // Opcode byte.
               ))
        ->help("Fetch the current Y register contents.")
        ->example("fetch y")
        ->regname("y");
    insert(mnem(
               "mov",  // Mnemonic name.
               "\x74"  // Opcode byte.
               ))
        ->help("Move the address into the Expression Stack Pointer.")
        ->example("mov esp, adr")
        ->regname("esp")
        ->regname("adr");
    insert(mnem(
               "mov",  // Mnemonic name.
               "\x75"  // Opcode byte.
               ))
        ->help("Move the address into the Return Stack Pointer.")
        ->example("mov rsp, adr")
        ->regname("rsp")
        ->regname("adr");
    insert(mnem(
               "mov",  // Mnemonic name.
               "\x76"  // Opcode byte.
               ))
        ->help("Move the address into the X register.")
        ->example("mov x, adr")
        ->regname("x")
        ->regname("adr");
    insert(mnem(
               "mov",  // Mnemonic name.
               "\x77"  // Opcode byte.
               ))
        ->help("Move the address into the Y register.")
        ->example("mov y, adr")
        ->regname("y")
        ->regname("adr");
    insert(mnem(
               "mov",  // Mnemonic name.
               2,      //Length
               "\x78\x00",  // Opcode byte.
               "\xff\x00"  // Mask
               ))
        ->help("Set the Expression Stack Pointer")
        ->example("mov esp, #0xff")
        ->regname("esp")
        ->imm("\x00\xff");
    insert(mnem(
               "mov",  // Mnemonic name.
               2,      //Length
               "\x79\x00",  // Opcode byte.
               "\xff\x00"  // Mask
               ))
        ->help("Set the Return Stack Pointer direct.")
        ->example("mov rsp, #0xff")
        ->regname("rsp")
        ->imm("\x00\xff");
    insert(mnem(
               "mov",  // Mnemonic name.
               2,      //Length
               "\x7a\x00",  // Opcode byte.
               "\xff\x00"  // Mask
               ))
        ->help("Set the RAM address register X direct.")
        ->example("mov x, #0xff")
        ->regname("x")
        ->imm("\x00\xff");
    insert(mnem(
               "mov",  // Mnemonic name.
               2,      //Length
               "\x7b\x00",  // Opcode byte.
               "\xff\x00"  // Mask
               ))
        ->help("Set the RAM address register Y direct.")
        ->example("mov y, #0xff")
        ->regname("y")
        ->imm("\x00\xff");


    // Page 29.
    insert(mnem(
               "nop",  // Mnemonic name.
               1,
               "\x7c",  // Opcode byte.  0x7C is the legal nop, others are illegal nops.
               "\xfc"   //Mask
               ))
        ->help("No operation or illegal instruction.")
        ->example("nop")
        ->dontcare("\x03"); //We don't care about these bits, but we still have to define them.
    insert(mnem(
               "call",  // Mnemonic name.
               2,      //Length
               "\x40\x00",  // Opcode byte.
               "\xf0\x00"  // Mask
               ))
        ->help("Unconditional long call.")
        ->example("call @0x0fff")
        ->abs("\x0f\xff");
    insert(mnem(
               "bra",  // Mnemonic name.
               2,      //Length
               "\x50\x00",  // Opcode byte.
               "\xf0\x00"  // Mask
               ))
        ->help("Conditional long branch.")
        ->example("bra @0x0fff")
        ->abs("\x0f\xff");
    insert(mnem(
               "lit",  // Mnemonic name.
               1,      //Length
               "\x60",  // Opcode byte.
               "\xf0"  // Mask
               ))
        ->help("Push literal onto TOS.")
        ->example("lit #0xf")
        ->imm("\x0f");  // n
    insert(mnem(
               "sbra",  // Mnemonic name.
               1,      //Length
               "\x80",  // Opcode byte.
               "\xC0"  // Mask
               ))
        ->help("FIXME: Conditional short branch in page.")
        ->example("sbra #0x3f")
        ->imm("\x3f"); //FIXME: Need to calculate in-page addressing.
    insert(mnem(
               "scall",  // Mnemonic name.
               1,      //Length
               "\xc0",  // Opcode byte.
               "\xc0"  // Mask
               ))
        ->help("Conditional short call.")
        ->example("scall #0x3f")
        ->imm("\x3f"); //FIXME: Need to calculate in-page addressing.

}
