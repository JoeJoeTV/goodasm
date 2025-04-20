#include "galangz8.h"
#include "gamnemonic.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

/* These types are made to match the abreviations on page 148
 * of the Z8 CPU User Manual, table 37.
 */

//CC  -- Condition Code.
//r   -- 4-bit working register
#define z8r(x) insert(new GAParameterZ8Reg4((x), false))
//R   -- 8-bit register
#define z8R(x) insert(new GAParameterZ8Reg8((x), false))
//RR  -- Working register, even in the range 00 to FE.
//ir  -- indirect 4-bit working register.
#define z8ir(x) insert(new GAParameterZ8Reg4((x), true))
//IR  -- indirect 8-bit register.
#define z8IR(x) insert(new GAParameterZ8Reg8((x), true))
//irr -- Indirect 4-bit register pair.
//IRR -- Indirect 8-bit register pair.
//X   -- Indexed
//DA  -- Direct address, 8-bit
//RA  -- Relative addressed, -128 to +127.
//IM  -- Immediate data.


GALangZ8::GALangZ8() {
    endian=LITTLE;  // Higher bits come in earlier bytes.
    name="z8";
    maxbytes=4;

    //page 158
    insert(mnem("add", 2, "\x02\x00", "\xff\x00"))
        ->help("dst=dst+src")
        ->example("add r1, r2")
        ->z8r("\x00\xf0")  //dst
        ->z8r("\x00\x0f"); //src
    insert(mnem("add", 2, "\x03\x00\x00", "\xff\x00\x00"))
        ->help("dst=dst+mem[src]")
        ->example("add r1, @r2")
        ->z8r("\x00\xf0")   //dst
        ->z8ir("\x00\x0f"); //src
    insert(mnem("add", 3, "\x04\x00\x00", "\xff\x00\x00"))
        ->help("dst=dst+src")
        ->example("add r16, r26")
        ->z8R("\x00\x00\xff")  //dst
        ->z8R("\x00\xff\x00"); //src
    insert(mnem("add", 3, "\x05\x00\x00", "\xff\x00\x00"))
        ->help("dst=dst+mem[src]")
        ->example("add r16, @r16")
        ->z8R("\x00\x00\xff")  //dst
        ->z8IR("\x00\xff\x00"); //src
    insert(mnem("add", 3, "\x06\x00\x00", "\xff\x00\x00"))
        ->help("dst=dst+imm")
        ->example("add r16, #0xff")
        ->z8R("\x00\xff\x00")  //dst
        ->imm("\x00\x00\xff"); //src
    insert(mnem("add", 3, "\x07\x00\x00", "\xff\x00\x00"))
        ->help("dst=mem[dst]+imm")
        ->example("add @r16, #0xff")
        ->z8IR("\x00\xff\x00")  //dst
        ->imm("\x00\x00\xff"); //src
}




GAParameterZ8Reg4::GAParameterZ8Reg4(const char* mask, bool indirect){
    if(indirect) prefix="@";
    setMask(mask);
}
int GAParameterZ8Reg4::match(GAParserOperand *op, int len){
    if(op->prefix!=prefix)
        return 0;
    if(!op->value.startsWith("r"))
        return 0;

    QString v=op->value.mid(1, op->value.length()-1);
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
                        +"r"+QString::number(r)
                        +suffix;
    return rendering;
}
void GAParameterZ8Reg4::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){

    QString v=op.value.mid(1, op.value.length()-1);
    bool okay=false;
    uint64_t val=v.toInt(&okay);
    assert(okay);

    rawencode(lang,adr,bytes,op,inslen,val);
}



GAParameterZ8Reg8::GAParameterZ8Reg8(const char* mask, bool indirect){
    if(indirect) prefix="@";
    setMask(mask);
}
int GAParameterZ8Reg8::match(GAParserOperand *op, int len){
    if(op->prefix!=prefix)
        return 0;
    if(!op->value.startsWith("r"))
        return 0;

    return 1;
}

QString GAParameterZ8Reg8::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    //Registers begin with r1.
    uint64_t r=rawdecode(lang,adr,bytes,inslen);
    assert(r<256); //Reg count on this architecture.

    QString rendering=prefix
                        +"r"+QString::number(r)
                        +suffix;
    return rendering;
}
void GAParameterZ8Reg8::encode(GALanguage *lang,
                               uint64_t adr, QByteArray &bytes,
                               GAParserOperand op,
                               int inslen
                               ){

    QString v=op.value.mid(1, op.value.length()-1);
    bool okay=false;
    uint64_t val=v.toInt(&okay);
    assert(okay);

    rawencode(lang,adr,bytes,op,inslen,val);
}
