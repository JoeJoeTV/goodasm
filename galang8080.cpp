#include "gamnemonic.h"
#include "galang8080.h"

//Just to keep things shorter.
#define mnem new GAMnemonic
//8-bit register.  &HL is handled elsewhere by overloads.
#define reg8(x) insert(new GAParameter8080Reg8((x)))
//16-bit register pairs in STAX and LDAX.
#define rp1(x) insert(new GAParameter8080RP1((x)))
//16-bit register pairs with Flags+A.
#define rp2fa(x) insert(new GAParameter8080RP2((x), false))
//16-bit register pairs with SP.
#define rp2sp(x) insert(new GAParameter8080RP2((x), true))
//Restart handler address.
#define exp(x) insert(new GAParameter8080EXP((x)))


GALang8080::GALang8080() {
    GAMnemonic* m;
    endian=LITTLE;  // Lower bits come in earlier bytes.
    name="8080";
    maxbytes=3;

    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        // 8-bit
        "b"<<"c"<<"d"<<"e"<<"h"<<"l"<<"a"<<
        // 16-bit register pairs
        "bc"<<"de"<<"hl"<<"sp"
        ;


    //Single Register Instructions

    insert(mnem("cmc", 1, "\x3f", "\xff"))
        ->help("Complement Carry")
        ->example("cmc");
    insert(mnem("ctc", 1, "\x37", "\xff"))
        ->help("Set Carry")
        ->example("ctc");
    insert(mnem("inr", 1, "\x04", "\xc7"))
        ->help("Increment Register or Memory")
        ->example("inr b")
        ->reg8("\x38");
    insert(mnem("dcr", 1, "\x05", "\xc7"))
        ->help("Decrement Register or Memory")
        ->example("dcr b")
        ->reg8("\x38");
    insert(mnem("cma", 1, "\x2f", "\xff"))
        ->help("Complement Accumulator")
        ->example("cma");
    insert(mnem("daa", 1, "\x27", "\xff"))
        ->help("Decimal Adjust Accumulator")
        ->example("daa");

    //Nop Instructions, page 16

    insert(mnem("nop", 1, "\x00", "\xff"))
        ->help("No Operation")
        ->example("nop");


    //Data Transfer Instructions, page 16
    insert(mnem("mov", 1, "\x40", "\xc0"))
        ->help("Move Destination, Source")
        ->example("mov b, @hl")
        ->reg8("\x38")   //dst
        ->reg8("\x07");  //src

    insert(mnem("stax", 1, "\x02", "\xef"))
        ->help("Store Accumulator")
        ->example("stax @de")
        ->rp1("\x10");
    insert(mnem("ldax", 1, "\x0a", "\xef"))
        ->help("Load Accumulator")
        ->example("ldax @de")
        ->rp1("\x10");

    //Register or Memory to Accumulator Instructions, Page 17
    insert(mnem("add", 1, "\x80", "\xf8"))
        ->help("Add Reg or Mem to Accumulator")
        ->example("add b")
        ->reg8("\x07");
    insert(mnem("adc", 1, "\x88", "\xf8"))
        ->help("Add Reg or Mem to Accumulator w/ Carry")
        ->example("adc b")
        ->reg8("\x07");
    insert(mnem("sub", 1, "\x90", "\xf8"))
        ->help("Sub Reg or Mem from Accumulator")
        ->example("sub b")
        ->reg8("\x07");
    insert(mnem("sbb", 1, "\x98", "\xf8"))
        ->help("Sub Reg or Mem from Accumulator w/ Borrow")
        ->example("sbb b")
        ->reg8("\x07");
    insert(mnem("ana", 1, "\xa0", "\xf8"))
        ->help("Logical AND Reg/Mem with Accumulator")
        ->example("ana b")
        ->reg8("\x07");
    insert(mnem("xra", 1, "\xa8", "\xf8"))
        ->help("Logical XOR Reg/Mem with Accumulator")
        ->example("xra b")
        ->reg8("\x07");
    insert(mnem("ora", 1, "\xb0", "\xf8"))
        ->help("Logical OR Reg/Mem with Accumulator")
        ->example("ora b")
        ->reg8("\x07");
    insert(mnem("cmp", 1, "\xb8", "\xf8"))
        ->help("Compare Reg/Mem with Accumulator")
        ->example("cmp b")
        ->reg8("\x07");

    // Rotate Accumulator Instructions, page 21
    // NB, this convention is confusing as hell!
    // RLC does *NOT* carry, but RAL does.

    insert(mnem("rlc", 1, "\x07", "\xff"))
        ->help("Rotate Accumulator Left")
        ->example("rlc");
    insert(mnem("rrc", 1, "\x0f", "\xff"))
        ->help("Rotate Accumulator Right")
        ->example("rrc");
    insert(mnem("ral", 1, "\x17", "\xff"))
        ->help("Rotate Accumulator Left w/ Carry")
        ->example("ral");
    insert(mnem("rar", 1, "\x1f", "\xff"))
        ->help("Rotate Accumulator Right w/ Carry")
        ->example("rar");

    // Register pair instructions, page 22.
    insert(mnem("push", 1, "\xc5", "\xcf"))
        ->help("Push two bytes to stack.")
        ->example("push fa")
        ->rp2fa("\x30");
    insert(mnem("pop", 1, "\xc1", "\xcf"))
        ->help("Pop two bytes from stack.")
        ->example("pop fa")
        ->rp2fa("\x30");
    insert(mnem("dad", 1, "\x09", "\xcf"))
        ->help("Adds a 2-byte register into HL.")
        ->example("dad hl, sp")
        ->regname("hl")  //TODO Make this field optional?
        ->rp2sp("\x30");
    insert(mnem("inx", 1, "\x03", "\xcf"))
        ->help("Increment a register pair.")
        ->example("inx sp")
        ->rp2sp("\x30");
    insert(mnem("dcx", 1, "\x0b", "\xcf"))
        ->help("Decrement a register pair.")
        ->example("dcx sp")
        ->rp2sp("\x30");
    insert(mnem("xchg", 1, "\xeb", "\xff"))
        ->help("Swaps HL with DE.")
        ->example("xchg hl, de")
        ->regname("hl")   //FIXME: Optional when matching?  Reverse order.
        ->regname("de");
    insert(mnem("xchg", 1, "\xe3", "\xff"))
        ->help("Swaps HL with @SP.  XTHL in spec.")
        ->example("xchg hl, @sp")
        ->regname("hl")   //FIXME: Optional when matching?  Reverse order.
        ->regnameind("sp");
    insert(mnem("sphl", 1, "\xf9", "\xff"))
        ->help("Loads SP with value of HL.")
        ->example("sphl");


    //Immediate Instructions, page 25.
    insert(mnem("lxi", 3, "\x01\x00\x00", "\xcf\x00\x00"))
        ->help("Load 16-bit immediate.")
        ->example("lxi sp, #0xffff")
        ->rp2sp("\x30\x00\x00")
        ->imm("\x00\xff\xff");
    insert(mnem("mvi", 2, "\x06\x00", "\xc7\x00"))
        ->help("Move immediate data.")
        ->example("mvi a, #0xff")
        ->reg8("\x38\x00")
        ->imm("\x00\xff");
    insert(mnem("adi", 2, "\xc6\x00", "\xff\x00"))
        ->help("Add immediate to accumulator.")
        ->example("adi a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("aci", 2, "\xce\x00", "\xff\x00"))
        ->help("Add immediate to accumulator w/ carry.")
        ->example("aci a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("sui", 2, "\xd6\x00", "\xff\x00"))
        ->help("Subtract immediate from accumulator.")
        ->example("sui a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("sbi", 2, "\xde\x00", "\xff\x00"))
        ->help("Subtract immediate from accumulator w/ borrow.")
        ->example("sbi a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("ani", 2, "\xe6\x00", "\xff\x00"))
        ->help("AND immediate with accumulator.")
        ->example("ani a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("xri", 2, "\xee\x00", "\xff\x00"))
        ->help("XOR immediate with accumulator.")
        ->example("xri a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("ori", 2, "\xf6\x00", "\xff\x00"))
        ->help("OR immediate with accumulator.")
        ->example("ori a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    insert(mnem("cpi", 2, "\xfe\x00", "\xff\x00"))
        ->help("Compare immediate with accumulator.")
        ->example("cpi a, #0xff")
        ->regname("a") //FIXME: Optional?
        ->imm("\x00\xff");


    // Direct addressing instructions, page 30.
    insert(mnem("sta", 3, "\x32\x00\x00", "\xff\x00\x00"))
        ->help("Store accumulator direct.")
        ->example("sta a, @0xffff")
        ->regname("a")
        ->abs("\x00\xff\xff");
    insert(mnem("lda", 3, "\x3a\x00\x00", "\xff\x00\x00"))
        ->help("Load accumulator direct.")
        ->example("lda @0xffff, a")
        ->abs("\x00\xff\xff")
        ->regname("a");
    insert(mnem("shld", 3, "\x22\x00\x00", "\xff\x00\x00"))
        ->help("Store HL direct.")
        ->example("shld @0xffff, hl")
        ->abs("\x00\xff\xff")
        ->regname("hl");
    insert(mnem("lhld", 3, "\x2a\x00\x00", "\xff\x00\x00"))
        ->help("Load HL direct.")
        ->example("lhld hl, @0xffff")
        ->regname("hl")
        ->abs("\x00\xff\xff");

    // Jump instructions, page 31.
    insert(mnem("pchl", 1, "\xe9", "\xff"))
        ->help("Set PC to HL.")
        ->example("pchl")
        //->regname("pc")   //FIXME: Support for optional fields.
        //->regname("hl")
        ;

    insert(mnem("jmp", 3, "\xc3\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional jump.")
        ->example("jmp 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jc", 3, "\xda\x00\x00", "\xff\x00\x00"))
        ->help("Jump if carry.")
        ->example("jc 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jnc", 3, "\xd2\x00\x00", "\xff\x00\x00"))
        ->help("Jump if not carry.")
        ->example("jnc 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jz", 3, "\xca\x00\x00", "\xff\x00\x00"))
        ->help("Jump if zero.")
        ->example("jz 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jnz", 3, "\xc2\x00\x00", "\xff\x00\x00"))
        ->help("Jump if not zero.")
        ->example("jnz 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jm", 3, "\xfa\x00\x00", "\xff\x00\x00"))
        ->help("Jump if minus.")
        ->example("jm 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jp", 3, "\xf2\x00\x00", "\xff\x00\x00"))
        ->help("Jump if positive.")
        ->example("jp 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jpe", 3, "\xea\x00\x00", "\xff\x00\x00"))
        ->help("Jump if parity even.")
        ->example("jpe 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("jpo", 3, "\xe2\x00\x00", "\xff\x00\x00"))
        ->help("Jump if parity odd.")
        ->example("jpo 0xffff")
        ->adr("\x00\xff\xff");

    // Call subroutine instructions, page 34.

    insert(mnem("call", 3, "\xcd\x00\x00", "\xff\x00\x00"))
        ->help("Unconditional call to subroutine.")
        ->example("call 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cc", 3, "\xdc\x00\x00", "\xff\x00\x00"))
        ->help("Call if carry.")
        ->example("cc 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cnc", 3, "\xd4\x00\x00", "\xff\x00\x00"))
        ->help("Call if no carry.")
        ->example("cnc 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cz", 3, "\xcc\x00\x00", "\xff\x00\x00"))
        ->help("Call if zero.")
        ->example("cz 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cnz", 3, "\xc4\x00\x00", "\xff\x00\x00"))
        ->help("Call if not zero.")
        ->example("cnz 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cm", 3, "\xfc\x00\x00", "\xff\x00\x00"))
        ->help("Call if minux.")
        ->example("cm 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cp", 3, "\xf4\x00\x00", "\xff\x00\x00"))
        ->help("Call if plus.")
        ->example("cp 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cpe", 3, "\xec\x00\x00", "\xff\x00\x00"))
        ->help("Call if parity even.")
        ->example("cpe 0xffff")
        ->adr("\x00\xff\xff");
    insert(mnem("cpo", 3, "\xe4\x00\x00", "\xff\x00\x00"))
        ->help("Call if parity odd.")
        ->example("cpo 0xffff")
        ->adr("\x00\xff\xff");


    //Return from Subroutine Instructions, page 35.
    insert(mnem("ret", 1, "\xc9", "\xff"))
        ->help("Return from subroutine.")
        ->example("ret");
    insert(mnem("rc", 1, "\xd8", "\xff"))
        ->help("Return if carry.")
        ->example("rc");
    insert(mnem("rnc", 1, "\xd0", "\xff"))
        ->help("Return if no carry.")
        ->example("rnc");
    insert(mnem("rz", 1, "\xc8", "\xff"))
        ->help("Return if zero.")
        ->example("rz");
    insert(mnem("rnz", 1, "\xc0", "\xff"))
        ->help("Return if not zero.")
        ->example("rnz");
    insert(mnem("rm", 1, "\xf8", "\xff"))
        ->help("Return if minus.")
        ->example("rm");
    insert(mnem("rp", 1, "\xf0", "\xff"))
        ->help("Return if plus.")
        ->example("rp");
    insert(mnem("rpe", 1, "\xe8", "\xff"))
        ->help("Return if parity even.")
        ->example("rpe");
    insert(mnem("rpo", 1, "\xe0", "\xff"))
        ->help("Return if parity odd.")
        ->example("rpo");


    //Restart instruction, page 37.
    insert(mnem("rst", 1, "\xc7", "\xc7"))
        ->help("Restart to handler address.")
        ->example("rst 0x8")
        ->exp("\x38");

    //Interrupt flip-flop instructions, page 38
    insert(mnem("ei", 1, "\xfb", "\xff"))
        ->help("Enable Interrupts")
        ->example("ei");
    insert(mnem("di", 1, "\xf3", "\xff"))
        ->help("Disable Interrupts")
        ->example("di");

    //Input/Output Instructions
    insert(mnem("in", 2, "\xdb\x00", "\xff\x00"))
        ->help("Input to Accumulator")
        ->example("in %0")
        ->port("\x00\xff");
    insert(mnem("out", 2, "\xd3\x00", "\xff\x00"))
        ->help("Input from Accumulator")
        ->example("out %0x10")
        ->port("\x00\xff");


    //This is prioritized over "mov @hl, @hl".
    insert(mnem("halt", 1, "\x76", "\xff"))
        ->help("Halt Until Interrupted")
        ->example("halt")
        ->prioritize();

    //FIXME: Remove this.
    maxbytes=5;
    insert(mnem("fake", 5, "\x10\x00\x00\x00\x00", "\xff\x00\x00\x00\x00"))
        ->help("Halt Until Interrupted")
        ->example("fake #0x10")
        ->imm("\x00\xff\xff\xff\xff");
}


/* Like Z80, 8080 has odd register names.  We define support here,
 * and use the preprocessor so that they needn't be added to the
 * GAParameterGroup class.
 *
 * This represents a 3-bit register number, all of which are
 * 8-bit registers except M, which is an absolute reference
 * to mem[HL].  We prefer the "@hl" convention to "m", but
 * encode either way.
 */
GAParameter8080Reg8::GAParameter8080Reg8(const char* mask){
    setMask(mask);
}
int GAParameter8080Reg8::match(GAParserOperand *op, int len){
    if(op->prefix=="@" && op->value=="hl")
        return 1;

    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(op->value.length()!=1)
        return 0; //Wrong size.

    char c=op->value[0].toLatin1();

    switch(c){
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'h':
    case 'l':
    case 'a':
    case 'm': // Special case, meaning @HL
        return 1;  //Legal register name.
    default:
        return 0;  //Not an 8-bit register name.
    }
}

QString GAParameter8080Reg8::decode(GALanguage *lang,
                                   uint64_t adr, const char *bytes,
                                   int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<8); //Reg count on this architecture.

    if(p==6){
        return "@hl";
    }

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameter8080Reg8::encode(GALanguage *lang,
                                uint64_t adr, QByteArray &bytes,
                                GAParserOperand op,
                                int inslen){
    uint64_t val=0;

    if(op.value=="hl" || op.value=="m")
        //8080 convention is M, but our preferred convention is @HL.
        val=6;
    else
        for(int i=0; i<8; i++)
            if(op.value==regnames[i]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}



/* 8080 also has 16-bit register pairs.  The simpliest we call RP1,
 * and it's simply @BC if 0 or @DE if 1.  These are used in STAX and LDAX
 * instructions.
 */

GAParameter8080RP1::GAParameter8080RP1(const char* mask){
    prefix="@";
    suffix="";
    setMask(mask);
}
int GAParameter8080RP1::match(GAParserOperand *op, int len){
    if(op->prefix=="@" && (op->value=="bc" || op->value=="de"))
        return 1;

    if(op->prefix=="" && (op->value=="b" || op->value=="d")){
        //FIXME: Warn when doing weird backward compatibilities like this.
        return 1;
    }

    return 0;
}
QString GAParameter8080RP1::decode(GALanguage *lang,
                                   uint64_t adr, const char *bytes,
                                   int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<2); //Reg count.

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameter8080RP1::encode(GALanguage *lang,
                                uint64_t adr, QByteArray &bytes,
                                GAParserOperand op,
                                int inslen){
    uint64_t val=0;

    //Must match whole name, or abbreviation of first letter.
    for(int i=0; i<2; i++)
        if(op.value==regnames[i] || op.value[0]==regnames[i][0]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}


/* 8080 also has 16-bit register pairs.  This one is called RP2,
 * and it either ends with the stack pointer or with the flags
 * and accumulator.
 */
GAParameter8080RP2::GAParameter8080RP2(const char* mask, bool stack){
    prefix="";
    suffix="";
    setMask(mask);

    //Final entry is either SP or Flags+A.
    if(stack)
        regnames[3]=stdregnames[3]="sp";
}
int GAParameter8080RP2::match(GAParserOperand *op, int len){
    //Not compatible.
    if(op->prefix!=prefix || op->suffix!=suffix)
        return 0;

    //Compatible if we match either the regname or the standard regname.
    for(int i=0; i<4; i++)
        if(regnames[i]==op->value || stdregnames[i]==op->value)
            return 1;

    return 0;
}
QString GAParameter8080RP2::decode(GALanguage *lang,
                                   uint64_t adr, const char *bytes,
                                   int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<4); //Reg count.

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameter8080RP2::encode(GALanguage *lang,
                                uint64_t adr, QByteArray &bytes,
                                GAParserOperand op,
                                int inslen){
    uint64_t val=0;

    for(int i=0; i<2; i++)
        if(op.value==regnames[i] || op.value[0]==regnames[i][0]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}



/* 8080 has restart instructions that take a three-bit pointer
 * to an offset handler address.
 */
GAParameter8080EXP::GAParameter8080EXP(const char* mask){
    assert(mask[0]==0x38);
    setMask(mask);
}
int GAParameter8080EXP::match(GAParserOperand *op, int len){
    assert(len==1);
    if(op->prefix!="" || op->suffix!="") return 0;

    //Reject values that don't fit.
    int64_t val=op->uint64(false); //False on a match.
    if(val&(0xc7))
        return 0;

    //We're a match if we get this far.
    return 1;
}

QString GAParameter8080EXP::decode(GALanguage *lang, uint64_t adr,
                                   const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<8); //Limits.
    p<<=3;
    return QString::asprintf("0x%04lx", (unsigned long) p);
}
void GAParameter8080EXP::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){
    rawencode(lang, adr, bytes, op, inslen,
              op.int64(true)>>3);
}


