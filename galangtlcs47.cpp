#include <QDebug>

#include "galangtlcs47.h"
#include "gamnemonic.h"
#include "goodasm.h"

//Just to keep things shorter.
#define mnem new GAMnemonic
//6 bit address field, for an in-page branch.
#define addr6(x) insert(new GAParameterTLCS47Addr6((x)))



/* Documentation begins on PDF page 333 of docs/1982_Toshiba_Microprocessor_Data_Book.pdf.
 * Other TLCS chips use different instruction formats, and are not supported by this
 * module.
 */



GALangTLCS47::GALangTLCS47() {
    name="tlcs47";
    endian=BIG;

    //Register names must be set or they will be confused for symbols.
    regnames.clear();
    regnames<<"a"<<"h"<<"l"<<"hl"<<"cf"<<"dc"<<"eir"<<"gf"<<"il"<<"zf";

    //Move Instructions
    //PDF page 333.

    /* This is a very simple instruction, comprised of a text mnemonic
     * and a single byte opcode.  The mask is implied as "\xff" and
     * there are no parameters.
     *
     * The help line afterward is handy for the cheat sheet.
     */
    insert(mnem("ld","\x0c"))
        ->help("A=mem[HL]")
        ->example("ld a, @hl")
        ->regname("a")
        ->regname("hl","@");

    /* This one is a little more complex.  The instruction is two bytes,
     * with the first (3c) being masked as the opcode.  insert()
     * returns a pointer to the GAMnemonic, so we then add a parameter
     * as a mask of the second byte.
     */
    insert(mnem("ld",2,"\x3c\x00","\xff\x00"))
        ->help("A=mem4[abs8]")
        ->example("ld a, @0x35")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("ld",2,"\x28\x00","\xff\x00"))
        ->help("HL=mem8[abs8]")
        ->example("ld hl, @0x35")
        ->regname("hl")
        ->abs("\x00\xff");   //Low two bits don't matter?
    insert(mnem("ld",1,"\x40","\xf0"))
        ->help("A=imm4")
        ->example("ld a, #0xf")
        ->regname("a")
        ->imm("\x0f");
    //PDF page 334
    insert(mnem("ld",1,"\xc0","\xf0"))
        ->help("H=imm4")
        ->example("ld h, #0xf")
        ->regname("h")
        ->imm("\x0f");
    insert(mnem("ld",1,"\xe0","\xf0"))
        ->help("L=imm4")
        ->example("ld l, #0xf")
        ->regname("l")
        ->imm("\x0f");
    insert(mnem("ldl","\x33"))
        ->help("Loads lower nybble of read from data table at DC "
               "into the accumulator.")
        ->example("ldl a, @dc")
        ->regname("a")
        ->regname("dc", "@");
    insert(mnem("ldh","\x32"))
        ->help("Loads higher nybble of read from data table at DC "
               "into the accumulator.")
        ->example("ldh a, @dc+")
        ->regname("a")
        ->regname("dc", "@", "+"); //Prefix and suffix?
    insert(mnem("st","\x0f"))
        ->help("mem[HL]=A")
        ->example("st a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("st","\x1a"))
        ->help("mem[HL]=A, L++")
        ->example("st a, @hl+")
        ->regname("a")
        ->regname("hl","@","+");
    insert(mnem("st","\x1b"))
        ->help("mem[HL]=A, L--")
        ->example("st a, @hl-")
        ->regname("a")
        ->regname("hl","@","-");
    insert(mnem("st",2,"\x3f\x00","\xff\x00"))
        ->help("mem4[imm8]=A")
        ->example("st a, @0x35")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("st",1,"\xf0","\xf0"))
        ->help("mem4[HL]=imm4, L++")
        ->example("st #0xf, @hl+")
        ->imm("\x0f")
        ->regname("hl","@","+");

    //PDF page 335

    /* Previous instructions have had a single parameter to them.
     * This instruction takes two parameters, and we insert them
     * in the order that we wish them to appear in assembly
     * language.  The machine language order is determined by
     * the masks, and need not match the assembly.
     */
    insert(mnem("st",2,"\x2d\x00","\xff\x00"))
        ->help("mem[abs4]=imm4.  Zero page.")
        ->example("st #0xf, @0xf")
        ->imm("\x00\xf0") //#K
        ->abs("\x00\x0f"); //Y
    insert(mnem("mov","\x10"))
        ->help("H=A")
        ->example("mov h, a")
        ->regname("h")
        ->regname("a");
    insert(mnem("mov","\x11"))
        ->help("L=A")
        ->example("mov l, a")
        ->regname("l")
        ->regname("a");
    insert(mnem("xch","\x30"))
        ->help("Exchange A and H.")
        ->example("xch a, h")
        ->regname("a")
        ->regname("h");
    insert(mnem("xch","\x31"))
        ->help("Exchange A and L.")
        ->example("xch a, l")
        ->regname("a")
        ->regname("l");
    insert(mnem("xch","\x13"))
        ->help("Exchange A for EIR (interrupt enable register).")
        ->example("xch a, eir")
        ->regname("a")
        ->regname("eir");
    insert(mnem("xch","\x0d"))
        ->help("Exchange A with mem[HL]")
        ->example("xch a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("xch",2,"\x3d\x00","\xff\x00"))
        ->help("Exchanges A for mem[abs8].")
        ->example("xch a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("xch",2,"\x29\x00","\xff\x00"))
        ->help("Exchanges HL for mem[abs8].")
        ->example("xch hl, @0x35")
        ->regname("hl")
        ->abs("\x00\xff");
    //Compare Instructions
    //PDF page 336
    insert(mnem("cmpr","\x16"))
        ->help("Compare A to mem[HL].")
        ->example("cmpr a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("cmpr",2,"\x3e\x00","\xff\x00"))
        ->help("Compare A to mem[abs8].")
        ->example("cmpr a, @0xff")
        ->regname("a")
        ->abs("\x00\xff");
    insert(mnem("cmpr",1,"\xd0","\xf0"))
        ->help("Compare A to imm4.")
        ->example("cmpr a, #0xf")
        ->regname("a")
        ->imm("\x0f");
    insert(mnem("cmpr",2,"\x38\xd0","\xff\xf0"))
        ->help("Compare H to imm4.")
        ->example("cmpr h, #0xf")
        ->regname("h")
        ->imm("\x00\x0f");
    insert(mnem("cmpr",2,"\x38\x90","\xff\xf0"))
        ->help("Compare L to imm4.")
        ->example("cmpr l, #0xf")
        ->regname("l")
        ->imm("\x00\x0f");
    insert(mnem("cmpr",2,"\x2e\x00","\xff\x00"))
        ->help("Compare mem[abs4] to imm4.  Zero page.")
        ->example("cmpr @0xF, #0xF")
        ->abs("\x00\x0f") // Y
        ->imm("\x00\xf0"); // K

    //Arithmetic Instructions
    insert(mnem("inc","\x08"))
        ->help("A++")
        ->example("inc a")
        ->regname("a");
    insert(mnem("inc","\x18"))
        ->help("L++")
        ->example("inc l")
        ->regname("l");
    //PDF page 337
    insert(mnem("inc","\x0a"))
        ->help("mem[HL]++")
        ->example("inc @hl")
        ->regname("hl","@");
    insert(mnem("dec","\x09"))
        ->help("A--")
        ->example("dec a")
        ->regname("a");
    insert(mnem("dec","\x19"))
        ->help("L--")
        ->example("dec l")
        ->regname("l");
    insert(mnem("dec","\x0b"))
        ->help("mem[HL]--")
        ->example("dec @hl")
        ->regname("hl","@");
    insert(mnem("addc","\x15"))
        ->help("Add w/ Carry mem[HL] into A.")
        ->example("addc a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("add","\x17"))
        ->help("A=A+mem[HL].")
        ->example("add a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("add",2,"\x38\x00","\xff\xf0"))
        ->help("A=A+imm4")
        ->example("add a, #0xf")
        ->regname("a")
        ->imm("\x00\x0f");
    insert(mnem("add",2,"\x38\xc0","\xff\xf0"))
        ->help("Adds imm4 into H.  Use 1 or F for inc/dec.")
        ->example("add h, #0xf")
        ->regname("h")
        ->imm("\x00\x0f");
    //PDF page 338
    insert(mnem("add",2,"\x38\x80","\xff\xf0"))
        ->help("Adds imm4 into L.  Use 1 or F for inc/dec.")
        ->example("add l, #0x7")
        ->regname("l")
        ->imm("\x00\x0f");
    insert(mnem("add",2,"\x38\x40","\xff\xf0"))
        ->help("mem[HL]+=imm4.")
        ->example("add @hl, #0xf")
        ->regname("hl", "@")
        ->imm("\x00\x0f");
    insert(mnem("add",2,"\x2f\x00","\xff\x00"))  //These were in the wrong order.
        ->help("mem[abs4]+=imm4.  Zero page.")
        ->example("add @0xf, #0xf")
        ->abs("\x00\x0f") // Y comes second in machine language, first in asm.
        ->imm("\x00\xf0"); // K# comes first in machine language, second in asm.
    insert(mnem("subrc","\x14"))
        ->help("A=mem[HL]-A w/ carry.")
        ->example("subrc a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("subr",2,"\x38\x10","\xff\xf0"))
        ->help("A=mem[HL]-A.")
        ->example("subr a, #0xf")
        ->regname("a")
        ->imm("\x00\x0f");
    //PDF page 339
    insert(mnem("subr",2,"\x38\x50","\xff\xf0"))
        ->help("mem[HL]-=imm4.")
        ->example("subr @hl, #0xf")
        ->regname("hl","@")
        ->imm("\x00\x0f");
    //Logical Instructions
    insert(mnem("rolc","\x05"))
        ->help("Rotates A and Carry flag left .")
        ->example("rolc a")
        ->regname("a");
    insert(mnem("rorc","\x07"))
        ->help("Rotate A and Carry flag right.")
        ->example("rorc a")
        ->regname("a");
    insert(mnem("and","\x1e"))
        ->help("A = A & mem[HL].")
        ->example("and a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("and",2,"\x38\x30","\xff\xf0"))
        ->help("A = A & imm4.")
        ->example("and a, #0xf")
        ->regname("a")
        ->imm("\x00\x0f");
    insert(mnem("and",2,"\x38\x70","\xff\xf0"))
        ->help("mem[HL] = mem[HL] & imm4.")
        ->example("and @hl, #0xf")
        ->regname("hl","@")
        ->imm("\x00\x0f");
    //PDF page 340
    insert(mnem("or","\x1d"))
        ->help("A = A | mem[HL].")
        ->example("or a, @hl")
        ->regname("a")
        ->regname("hl","@");
    insert(mnem("or",2,"\x38\x20","\xff\xf0"))
        ->help("A = A | imm4.")
        ->example("or a, #0xf")
        ->regname("a")
        ->imm("\x00\x0f");
    insert(mnem("or",2,"\x38\x60","\xff\xf0"))
        ->help("mem[HL] = mem[HL] | imm4.")
        ->example("or @hl, #0xf")
        ->regname("hl","@")
        ->imm("\x00\x0f");
    insert(mnem("xor","\x1f"))
        ->help("A = A ^ mem[HL]")
        ->example("xor a, @hl")
        ->regname("a")
        ->regname("hl","@");
    //Bit Manipulation Instructions
    insert(mnem("test","\x06"))
        ->help("Tests the carry flag.")
        ->example("test cf")
        ->regname("cf");
    insert(mnem("test",1,"\x5c","\xfc"))
        ->help("Tests a bit of the accuulator.")
        ->example("test a, bit4")
        ->regname("a")
        ->bit3("\x03");
    insert(mnem("test",1,"\x58","\xfc"))
        ->help("Tests a bit of mem[HL].")
        ->example("test @hl, bit4")
        ->regname("hl","@")
        ->bit3("\x03");
    //PDF page 341
    insert(mnem("test",2,"\x39\x80","\xff\xc0"))
        ->help("Tests a bit of mem[abs4].")
        ->example("test @0xf, bit4")
        ->abs("\x00\x0f") //Y first
        ->bit3("\x00\x30"); //then B
    insert(mnem("test",2,"\x3b\x80","\xff\xc0"))
        ->help("Tests bits on an input port.")
        ->example("test %0xf, bit4")
        ->port("\x00\x0f") //Y first
        ->bit3("\x00\x30"); //then B
    insert(mnem("test","\x37"))
        ->help("Tests I/O pin from low bits of L.")
        ->example("test @l")
        ->regname("l","@");
    insert(mnem("testp","\x04"))
        ->help("Tests Carry flag and sets flag.")
        ->example("testp cf")
        ->regname("cf");
    insert(mnem("testp","\x0e"))
        ->help("Test the zero flag.")
        ->example("testp zf")
        ->regname("zf");
    insert(mnem("testp","\x01"))
        ->help("Test the general flag.")
        ->example("testp gf")
        ->regname("gf");
    insert(mnem("testp",2,"\x39\xc0","\xff\xc0"))
        ->help("Tests a bit of mem[abs4].  Zero page.")
        ->example("testp @0xf, bit4")
        ->abs("\x00\x0f") //Y first
        ->bit3("\x00\x30"); //then B.

    //PDF page 342

    insert(mnem("testp",2,"\x3b\xc0","\xff\xc0"))
        ->help("Test a bit in a port.")
        ->example("testp %0xf, bit4")
        ->port("\x00\x0f") //P first
        ->bit3("\x00\x30"); //then B
    insert(mnem("set","\x03"))
        ->help("Set the general flag.")
        ->example("set gf")
        ->regname("gf");
    insert(mnem("set",1,"\x50","\xfc"))
        ->help("Set a bit in HL.")
        ->example("set @hl, bit4")
        ->regname("hl","@")
        ->bit3("\x03");
    insert(mnem("set",2,"\x39\x00","\xff\xc0"))
        ->help("Sets a bit of mem[abs4].Zero page.")
        ->example("set @0xf, bit4")
        ->abs("\x00\x0f") //Y first
        ->bit3("\x00\x30"); //then B.
    insert(mnem("set",2,"\x3b\x00","\xff\xc0"))
        ->help("Sets a bit of an IO port.")
        ->example("set %0xf, bit4")
        ->port("\x00\x0f")  //%p first
        ->bit3("\x00\x30"); //then B
    insert(mnem("set","\x34"))
        ->help("Sets a bit in I/O port from low bits of L.")
        ->example("set @l")
        ->regname("l", "@");
    insert(mnem("clr","\x02"))
        ->help("Clears the general flag.")
        ->example("clr gf")
        ->regname("gf");
    insert(mnem("clr",1,"\x54","\xfc"))
        ->help("Clears a bit in mem[HL].")
        ->example("clr @hl, bit4")
        ->regname("hl","@")
        ->bit3("\x03");


    //PDF page 343
    insert(mnem("clr",2,"\x39\x40","\xff\xc0"))
        ->help("Clears bit of mem[abs4].  Zero page.")
        ->example("clr @0xf, bit4")
        ->abs("\x00\x0f") //Y first
        ->bit3("\x00\x30"); //then B
    insert(mnem("clr",2,"\x3b\x40","\xff\xc0"))
        ->help("Clears bit of I/O port.")
        ->example("clr %0xf, bit4")
        ->port("\x00\x0f") //%p first
        ->bit3("\x00\x30"); //then B
    insert(mnem("clr","\x35"))
        ->help("Clears I/O from low bits of L.")
        ->example("clr @l")
        ->regname("l","@");

    insert(mnem("clr",2,"\x36\xc0","\xff\xc0"))
        ->help("Clears interrupt latches.")
        ->example("clr il, #0x3f")
        ->regname("il")
        ->imm("\x00\x3f");
    insert(mnem("eiclr",2,"\x36\x40","\xff\xc0"))
        ->help("Sets interrupt latches.")
        ->example("eiclr il, #0x3f")
        ->regname("il")
        ->imm("\x00\x3f");
    insert(mnem("diclr",2,"\x36\x80","\xff\xc0"))
        ->help("Resets interrupt latches.")
        ->example("diclr il, #0x3f")
        ->regname("il")
        ->imm("\x00\x3f");

    //Input/Output Instructions
    insert(mnem("in",2,"\x3a\x20","\xff\xf0"))
        ->help("A=port4.")
        ->example("in %0xf, a")
        ->port("\x00\x0f")
        ->regname("a");
    //PDF Page 344
    insert(mnem("in",2,"\x3a\x60","\xff\xf0"))
        ->help("mem[HL]=port4.")
        ->example("in %0xf, @hl")
        ->port("\x00\x0f")
        ->regname("hl","@");

    /* FIXME: This is where things get weird.  These two instructions
     * have *disjoint* masks and an *inverted* bit.  The disjoint mask is
     * no longer a problem, but we don't support inverted bits yet.
     *
     * The same thing happens in Thumb2, where always-set bits from Thumb1 were
     * defined as inverted and disjoint for backward compatibility.
     */
    insert(mnem("out",2,"\x3a\x80","\xff\xd0","\x00\x20")) //FIXME
        ->help("Output accumulator to a port.")
        ->example("out a, %0x1f")
        ->regname("a")
        ->port("\x00\x2f");
    insert(mnem("out",2,"\x3a\xc0","\xff\xd0","\x00\x20")) //FIXME
        ->help("Output mem[HL] to a port.")
        ->example("out @hl, %0x1f")
        ->regname("hl","@")
        ->port("\x00\x2f");

    insert(mnem("out",2,"\x2c\x00","\xff\x00"))
        ->help("Output imm4 to a port.")
        ->example("out #0xf, %0xf")
        ->imm("\x00\xf0")   //#k
        ->port("\x00\x0f");  //%p
    insert(mnem("outb","\x12"))
        ->help("Outputs memory at (FE0+HL) to P2,P1 ports.")
        ->example("outb @hl") // FIXME: Include offset in example?
        ->regname("hl","@");

    //Branch Subroutine Instructions
    insert(mnem("bs",2,"\x60\x00","\xf0\x00"))
        ->help("Branches if SF is set, otherwise sets SF.")
        ->example("bs 0xfff")
        ->adr("\x0f\xff");

    //PDF Page 345
    insert(mnem("bss",1,"\x80","\xc0"))
        ->help("Branches within page is SF is set, otherwise sets SF.")
        ->example("loop: bss loop")
        ->addr6("\x3f");
    // Or within next page if at very end of a page.
    insert(mnem("call",2,"\x20\x00","\xf8\x00"))
        ->help("Calls a subroutine, pushing return pointer to stack.")
        ->example("call 0x7ff")
        ->adr("\x07\xff");
    insert(mnem("calls",1,"\x70","\xf0"))
        ->help("Short form CALL.")
        ->example("calls 0xF")
        ->adr("\x0f");
    insert(mnem("ret","\x2a"))
        ->help("Return")
        ->example("ret");
    insert(mnem("reti","\x2b"))
        ->help("Return from interrrupt.")
        ->example("reti");

    insert(mnem("nop","\x00"))
        ->help("No Operation.")
        ->example("nop");

}



GAParameterTLCS47Addr6::GAParameterTLCS47Addr6(const char* mask){
    setMask(mask);
}
int GAParameterTLCS47Addr6::match(GAParserOperand *op, int len){
    int64_t val=op->uint64(false); //False on a match.

    if(op->prefix!=prefix || op->suffix!=suffix)
        return 0;

    //Reject values that don't fit.
    if(((val)&0xFFC0) != ((1+op->goodasm->address())&0xffc0)){
        op->goodasm->error("TLCS47 ADDR6 is out of range.");
        return 0;
    }

    return 1;
}

QString GAParameterTLCS47Addr6::decode(GALanguage *lang, uint64_t adr,
                                       const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    p+=1;    // Add 1 for PC advance.
    p&=0x3f; //Mask off high bits.
    p|=(adr+1)&0xffc0; // Mask in the relevant PC bits.


    return QString::asprintf("0x%02x",(unsigned int) p);
}
void GAParameterTLCS47Addr6::encode(GALanguage *lang,
                                   uint64_t adr, QByteArray &bytes,
                                   GAParserOperand op,
                                   int inslen
                                   ){
    int64_t val=op.uint64(true);
    val-=1;       // PC+1 will be applied when the CPU decodes the instruction.
    val&=~0xffc0; // Mask off upper bits, which are not encoded.

    rawencode(lang,adr,bytes,op,inslen,val);
}
