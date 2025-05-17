#include "galangarm7tdmi.h"

#include "gamnemonic.h"
#include "gainstruction.h"


//Just to keep things shorter.
#define armmnem new GAMnemonicARM7TDMI
#define reg(x) insert(new GAParameterARM7TDMIReg((x)))


GALangARM7TDMI::GALangARM7TDMI() {
    endian=LITTLE;
    name="arm7tdmi";
    maxbytes=4;
    minbytes=4;
    align=4;

    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        "r0"<<"r1"<<"r2"<<"r3"<<
        "r4"<<"r5"<<"r6"<<"r7"<<
        "r8"<<"r9"<<"r10"<<"r11"<<
        "r12"<<
        "sp"<<"lr"<<"pc"
        "r13"<<"r14"<<"r15"
        ;


    insert(armmnem("bx", 4, "\x10\xff\x2f\x01", "\xf0\xff\xff\x0f"))
        ->help("Branch and Exchange")
        ->example("bx r0")
        ->reg("\x0f\x00\x00\x00");
}


//Simple constructor.
GAMnemonicARM7TDMI::GAMnemonicARM7TDMI(QString mnemonic,
                                       uint32_t length, //Always 4.
                                       const char *opcode,

                                       //Optional arguments default to null.
                                       const char *opcodemask,
                                       const char *invertmask)
    //Derived constructor.
    : GAMnemonic(mnemonic, length, opcode, opcodemask, invertmask)
{
    assert(length==4);
    dontcare("\x00\x00\x00\xf0");
}
//Does the Mnemonic match bytes?  If so, decode bytes to an instruction.
int GAMnemonicARM7TDMI::match(GAInstruction &ins, uint64_t adr, uint32_t &len,
                      const char *bytes){
    assert(length==4); //No zero byte mnemonics.

    //Don't match on a misaligned byte.
    if(adr%lang->align != 0)
        return 0;

    //This fails out if any byte does not equal the opcode, modulo masking.
    for(int i=0; i<length; i++){
        if((bytes[i]&opcodemask[i])!=opcode[i])
            return 0; //No match.
    }

    //Here we have a match, but we need to form a valid instruction.
    ins.verb=name;
    ins.type=GAInstruction::MNEMONIC;
    ins.len=len=this->length;
    ins.helpstr=this->helpstr;

    //Don't forget ARM's conditional flag.
    conditionCode=0x0f&(bytes[3]>>4);
    assert(conditionCode<16);
    conditionString=conditions[conditionCode];
    if(conditionCode!=0xe)
        ins.verb=name+conditionString;

    ins.params="";  //Resets the decoding.
    for(int i=0; i<params.count(); i++){
        ins.params+=params[i]->decode(lang, adr, bytes, len);
        if(i+1<params.count()) ins.params+=", ";
    }

    //Set the back pointer.
    ins.mnem=this;

    //Set the data as a QByteArray with the right length.
    QByteArray d;
    for(int i=0; i<len; i++)
        d.append(bytes[i]);
    ins.data=d;
    assert(ins.len=ins.data.length());

    return 1;
}

//Does the Mnemonic match source?  If so, encode it to bytes.
int GAMnemonicARM7TDMI::match(GAInstruction &ins, uint64_t adr,
                      QString verb, QList<GAParserOperand> ops){
    //Names and parameter count must match.
    if(ops.count()!=params.count()) return 0;
    if(!verb.startsWith(this->name)) return 0;
    if(ins.len!=0 && ins.len<length)
        return 0;

    //Condition code must be legal.
    uint8_t code=0xff;
    if(verb==this->name) code=0xe;
    for(int i=0; code==0xff && i<16; i++){
        if(verb==(this->name+conditions[i]))
            code=i;
    }
    if(code>16) return 0;
    conditionCode=code;
    conditionString=conditions[code];

    //Check all parameters.
    int i=0;
    foreach (auto param, params) {
        GAParserOperand op=ops[i];
        //Use mnemonic length because instruction length is not yet set!
        bool match=param->match(&op, length);
        if(!match) return 0;
        i++;
    }

    //FIXME: We're in trouble if the match is a mistake.
    //Registers might be mistaken for values.

    ins.len=length;
    ins.type=ins.MNEMONIC;
    ins.verb=verb;

    QByteArray bytes;
    QString opstring="";
    //First apply opcode.
    for(i=0; i<length; i++)
        bytes.append(opcode[i]);
    //And the condition code.
    bytes[3]|=conditionCode<<4;
    //Then apply parameters
    i=0;
    foreach (auto param, params) {
        GAParserOperand op=ops[i++];
        param->encode(lang, adr, bytes, op, ins.len);

        if(i>1) opstring+=", ";
        opstring+=op.render();
    }

    //Finally, set the bytes.
    ins.data=bytes;
    assert(bytes.length()==ins.len);
    ins.params=opstring;
    //Set the back pointer.
    ins.mnem=this;
    //Disassemble for listings.
    //match(ins, adr, length, ins.data.constData()); //Breaks stuff.
    return 1;
}

GAParameterARM7TDMIReg::GAParameterARM7TDMIReg(const char* mask){
    setMask(mask);
}
int GAParameterARM7TDMIReg::match(GAParserOperand *op, int len){
    //No prefixes or suffixes on ARM registers.
    if(prefix!="" || suffix!="")
        return 0;

    for(int i=0; i<19; i++)
        if(op->value==regnames[i])
            return 1;
    return 0;
}

QString GAParameterARM7TDMIReg::decode(GALanguage *lang, uint64_t adr,
                                       const char *bytes, int inslen){
    uint8_t regnum=rawdecode(lang, adr, bytes, inslen);
    if(regnum<16)
        return regnames[regnum];
    return "";
}
void GAParameterARM7TDMIReg::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen
            ){
    uint8_t rn=0xff;
    //Grab the register name.
    for(int i=0; i<19 && rn==0xff; i++){
        if(regnames[i]==op.value)
            rn=i;
    }
    //Register must've been in the list.
    assert(rn!=0xff);
    //Tail end of the list is for synonyms.
    if(rn>=16) rn-=3;
    rawencode(lang, adr, bytes, op, inslen, rn);

    return;
}
