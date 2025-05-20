#include "galangarm7tdmi.h"

#include "gamnemonic.h"
#include "gainstruction.h"

#include<QDebug>


//Just to keep things shorter.
#define armmnem new GAMnemonicARM7TDMI
#define reg(x) insert(new GAParameterARM7TDMIReg((x))) // Destination or Op1.
#define rel(x) insert(new GAParameterRelative((x), 8, 4))
//Op2 register and shift.
#define op2reg(x) insert(new GAParameterARM7TDMIReg((x)))->insert(new GAParameterARM7TDMIShift())
#define armimm insert(new GAParameterARM7TDMIImmediate())


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
        "sp"<<"lr"<<"pc"<<
        //Short names for SP, LR, PC.
        "r13"<<"r14"<<"r15"<<

        //Non-general purpose registers.
        "cpsr"<<"spsr"<<
        "cpsr_flg"<<"spsr_flg"
        ;

    //Define these as Little Endian.

    //Section 4.3
    insert(armmnem("bx", 4, "\x10\xff\x2f\x01", "\xf0\xff\xff\x0f"))
        ->help("Branch and Exchange")
        ->example("bx r0")
        ->prioritize()   //Prevents conflict with TEQ data processing instruction.
        ->reg("\x0f\x00\x00\x00");

    //Section 4.4
    insert(armmnem("b", 4, "\x00\x00\x00\x0a", "\x00\x00\x00\x0f"))
        ->help("Branch")
        ->example("loop: b loop")
        ->rel("\xff\xff\xff\x00");
    insert(armmnem("bl", 4, "\x00\x00\x00\x0b", "\x00\x00\x00\x0f"))
        ->help("Branch w/ Link")
        ->example("loop: bl loop")
        ->rel("\xff\xff\xff\x00");

    //Section 4.5, data processing.
    /* These exist in a few forms:
     * Bit 25 (I) chooses between Operand 2 being a shifted register or an immediate.
     * Bit 20 (S) chooses whether condition codes are written, implied by S after condition code.
     * Sub-opcode chooses the data processing operation.
     */
    int I=0,  //Immediate Bit
        S=0;  //S Bit, preventing writeback of result.
    for(int i=0; i<16; i++)
        for(S=0; S<2; S++)
            for(I=-1; I<2; I++)
    {
        // .... 00I.  ...S ....  ....   .... .... ....
        // cond    opcode   Op1  dest   ----op2-------

        QString opname=dataopcodes[i]+(S?"s":"");
        if(i>=8 && i<=11){
            opname=dataopcodes[i];
            if(!S) continue;
        }

        char word[]="\x00\x00\x00\x00";
        //Set the opcode.
        word[3]|=(i&0x8)?1:0;
        word[2]|=(i&0x7)<<5;
        //Set the option bits.
        word[2]|=(S==1?0x10:0);   // S-suffix on operand?
        word[3]|=(I==1?0x02:0);   // Immediate mode?

        char mask[]="\x00\x00\xf0\x0f";
        if(I==-1){
            mask[0]|=0xf0;
            mask[1]|=0x0f;
        }

        QString example=opname+" r0, ";

        auto m=insert(armmnem(opname, 4, word, mask));
        m->help(datahelp[i])
            ->reg("\x00\xf0\x00\x00");  //Destination register.

        //Operand 1 is ommitted for both MOV and MVN.
        if(i!=0xd && i!=0xf){
            m->reg("\x00\x00\x0f\x00"); //Operand 1.
            example+="r2, ";
        }else{
            m->dontcare("\x00\x00\x0f\x00");  //Unused Op1.
        }

        //Operand 2.
        if(I==-1){
            //Shifted register mode, without a shift.
            m->reg("\x0f\x00\x00\x00");
            //m->dontcare("\x00\x0f\x00\x00"); //Intentionally skipped.
            m->prioritize();
            example+="r3";
        }else if(I==0){
            //Shifted Register mode.
            m->op2reg("\x0f\x00\x00\x00");
            example+="r3, lsl #1";
        }else{
            //Shifted Immediate Mode.
            //m->imm("\xff\x00\x00\x00");
            //m->dontcare("\x00\x0f\x00\x00");  //FIXME: Missing rotation parameter!
            m->armimm;
            example+="#0xff";
        }

        m->example(example);
    }


    //Section 4.6: PSR Transfer (MRS, MSR)
    //R15 is illegal for these.
    for(int ps=0; ps<2; ps++){
        char word[]="\x00\x00\x0f\x01";
        if(ps) word[2]|=0x40;
        insert(armmnem("mrs", 4, word, "\xff\x0f\xff\x0f"))
            ->help("Transfer PSR contents to a Register")
            ->example(ps?"mrs r0, spsr":"mrs r0, cpsr")
            ->reg("\x00\xf0\x00\x00")
            ->regname(ps?"spsr":"cpsr");
    }
    for(int pd=0; pd<2; pd++){
        char word[]="\x00\xf0\x14\x01";
        if(pd) word[2]|=0x40;
        insert(armmnem("msr", 4, word, "\xf0\xff\xff\x0f"))
            ->help("Transfer from Register to PSR")
            ->example(pd?"msr spsr, r0":"msr cpsr, r0")
            ->regname(pd?"spsr":"cpsr")
            ->reg("\x0f\x00\x00\x00");
    }


    for(int pd=0; pd<2; pd++)   //CPSR or SPSR
        for(int I=0; I<2; I++) //Unshifted reg, shited reg, rotated immediate.
    {
        char word[]="\x00\xf0\x28\x01";
        char mask[]="\x00\xf0\xff\x0f";
        if(pd) word[2]|=0x40;
        if(I){
            word[3]|=0x02;
        }else{ //Mandatory zeroes when source is a register.
            mask[0]|=0xf0;
            mask[1]|=0x0f;
        }

        assert(mask[2]&0x40);
        assert(mask[3]&0x02);

        auto m=insert(armmnem("msr", 4, word, mask))
                     ->help("Transfer from Register to PSR Flags");
        QString example="msr ";
        example+=(pd?"spsr_flg, ":"cpsr_flg, ");
        example+=(I?"#0xf0000000":"r0");

        m->example(example)
            ->regname(pd?"spsr_flg":"cpsr_flg");
        if(I)
            m->armimm;
        else
            m->reg("\x0f\x00\x00\x00");
    }

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



GAParameterARM7TDMIShift::GAParameterARM7TDMIShift(const char* mask){
    setMask(mask);
}
int GAParameterARM7TDMIShift::match(GAParserOperand *op, int len){
    //No prefixes on ARM registers.
    if(prefix!="")
        return 0;

    //Optional suffix contains the shifting size.
    QString name=op->value;
    QString shift=op->suffix; //FIXME: Check that this shift is valid.

    if(!op->suffix.startsWith(" #") && ! op->suffix.startsWith(" r"))
        return 0;

    for(int i=0; i<4; i++)
        if(op->value==names[i])
            return 1;
    return 0;
}
QString GAParameterARM7TDMIShift::decode(GALanguage *lang, uint64_t adr,
                                         const char *bytes, int inslen){
    uint8_t d=rawdecode(lang, adr, bytes, inslen);
    shiftregistermode=d&1;
    shifttype=(d>>1)&3;
    shiftregister=(d>>4)&0xf;
    shiftamount=(d>>3)&0x1f;

    if(shiftregistermode){   // Shift Register
        return names[shifttype]+" "+regnames[shiftregister];
    }else{                   // Shift Amount
        //Special case: What should be LSR #0 is actually LSR #32.
        if(shifttype==1 && shiftamount==0)
            shiftamount=32;
        return names[shifttype]+" "+QString::asprintf("#%d", shiftamount);
    }
    return "";
}
void GAParameterARM7TDMIShift::encode(GALanguage *lang,
                                      uint64_t adr, QByteArray &bytes,
                                      GAParserOperand op,
                                      int inslen
                                      ){
    //First we mark our settings.
    if(op.suffix.startsWith(" #")){
        shiftregistermode=0;
        QString num=op.suffix.last(op.suffix.length()-2);
        bool okay=false;
        shiftamount=GAParser::str2uint(num, &okay);
        if(!okay)
            qDebug()<<"Failed to encode shift: "<<num;
    }else if(op.suffix.startsWith(" r")){
        shiftregistermode=1;
        for(int i=0; i<16; i++)
            if(op.suffix==" "+regnames[i]){
                shiftregistermode=1;
                shiftregister=i;
            }
    }
    shifttype=0xff;
    for(int i=0; i<4 && shifttype>4; i++)
        if(op.value==names[i])
            shifttype=i;

    //Second, we write them back to the instruction bits.
    uint8_t rn=0;
    rn|=(shifttype<<1);
    if(shiftregistermode){
        //Register mode.
        rn|=1;
        rn|=shiftregister<<4;
        assert(shiftregister<16);
    }else{
        //Immediate shift.
        if(shiftamount==0x20 && shifttype==1)
            shiftamount=0;
        rn|=(shiftamount<<3);
        assert(shiftamount<0x20);
    }

    rawencode(lang, adr, bytes, op, inslen, rn);
    return;
}




GAParameterARM7TDMIImmediate::GAParameterARM7TDMIImmediate(const char* mask){
    setMask(mask);
}
int GAParameterARM7TDMIImmediate::match(GAParserOperand *op, int len){
    if(op->prefix!="#")
        return 0;

    return 1;
}
QString GAParameterARM7TDMIImmediate::decode(GALanguage *lang, uint64_t adr,
                                             const char *bytes, int inslen){
    uint32_t d=rawdecode(lang, adr, bytes, inslen);
    uint32_t val=dec(d&0xff, (d&0xf00)>>8);

    return QString::asprintf("#0x%lx", (unsigned long) val);
}
void GAParameterARM7TDMIImmediate::encode(GALanguage *lang,
                                          uint64_t adr, QByteArray &bytes,
                                          GAParserOperand op,
                                          int inslen){
    uint32_t rn=0;
    uint64_t base=0;
    uint32_t rotate=0;


    bool b=enc(op.uint64(false), &base, &rotate);
    if(b){
        rn|=base&0xff;
        rn|=(rotate<<8)&0xf00;
    }

    //Rotation steps two at a time.
    rawencode(lang, adr, bytes, op, inslen, rn);
}

/* This takes a base and a rotation, producing the decoded uint32_t. */
uint32_t GAParameterARM7TDMIImmediate::dec(uint64_t base, uint32_t rotate){
    base|=(base<<32);
    base>>=rotate*2;
    return base&0xFFFFFFFF;
}


bool GAParameterARM7TDMIImmediate::enc(uint32_t val, uint64_t *base, uint32_t *rotate){
    uint64_t b=(((uint64_t) val)<<32)|val;
    for(int i=0; i<16; i++){
        uint64_t newbase=
            ((b<<i*2)&0xffffffff)
                           | (b>>(32-i*2));
        newbase&=0xff;

        if(dec(newbase, i)==val){
            *base=newbase;
            *rotate=i;
            return true;
        }
    }
    qDebug()<<"";
    return false;
}




