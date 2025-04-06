#include <QList>
#include <QDebug>

#include "galanguage.h"
#include "gaparameter.h"

GAParameter::GAParameter(const char* mask){
    setMask(mask);

}


void GAParameter::setMask(const char* mask){
    memcpy(this->mask, mask, GAMAXLEN);
}

GAParameter::~GAParameter(){
    for(int i=0; i<params.size(); i++){
        delete params[i];
    }
}

//Does a parsed parameter match this parameter?
int GAParameter::match(GAParserOperand *op, int len){
    /* This implementation is for numeric parameters.
     * We need to be careful to return failure if
     * the parameter cannot fit, so as to allow overloaded
     * opcodes like "ldx @u8" and "ldx @u16" on 6502.
     */

    if(len==0){
        qDebug()<<"Len="<<len<<"and"<<op->value;
    }

    //Prefix must match.
    if(op->prefix!=prefix
        //Odd exception for negative immediates.
        && ( op->prefix!=prefix+"-" && !isSigned)
        )
        return 0;

    //Suffix must match.
    if(op->suffix!=suffix)
        return 0;


    /* This is a little tricky, in that the value needs to fit in
     * range but we don't necessarily know the value of the symbol
     * yet on this pass.
     *
     * Supposing the value is absolute, we might get away with guessing
     * zero for unknown symbols on the first pass, then repeating
     * passes until the hash of all symbols remains the same for two passes.
     */

    if(!isSigned){
        int64_t max=
            (((uint64_t) 1)<<rawbitcount(len));
        uint64_t val=op->uint64(false)-offset;
        //Signed value illegal in unsigned parameter.
        if(val<0) return 0;
        if(val >= max) return 0;
    }else{
        int64_t val=op->int64(false)-offset;
        //Positive values must fit range.
        if(val >= ~(((int64_t) -1)& ~((((int64_t) 1)<<(rawbitcount(len)-1))))) return 0;
        //Negative values must also fit range.
        if(val<0 && (val| ~(((int64_t) 1)<<(rawbitcount(len)-1)))!=-1) return 0;
    }

    //We're good if we get here.
    return 1;
}

int GAParameterRelative::match(GAParserOperand *op, int len){
    //FIXME: We should do some sort of a match here.
    //https://github.com/travisgoodspeed/goodasm/issues/47
    //It's lazy to just return success.
    if(op->prefix=="-" && this->prefix=="")
        return 1;  //Negative value, but a match.
    if(op->prefix!=this->prefix)
        return 0;
    return 1;
}

int GAParameterAddress::match(GAParserOperand *op, int len){
    //FIXME: We should do some sort of a match here.
    //https://github.com/travisgoodspeed/goodasm/issues/47
    //It's lazy to just return success.
    if(op->prefix!=this->prefix) return 0;

    //Check that value fits in the field.
    if(op->uint64(false)>=(1<<rawbitcount(len)))
        return 0;
    return 1;
}

/* Numbered registers are a little tricky to match, in that
 * they need to be a number in the instruction but they might
 * also be a name in the decoding. */
int GAParameterRegNum::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(!op->value.startsWith("r"))
        return 0; //Not a register number.
    return 1; //Success!
}

/* Numbered bit, used in bit setting and clearing instructions. */
int GAParameterBitIndex::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Bits rarely have prefixes.
    //FIXME: Check the bit range.
    return 1; //Success!
}



//Absolute or immediate.
QString GAParameterRegNum::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) {
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    QString rendering=prefix
                        +QString::asprintf("r%d",(unsigned int) p)
                        +suffix;
    return rendering;
}


//Encode a parser parameter to bytes, which are OR'ed into target.
void GAParameterRegNum::encode(GALanguage *lang, uint64_t adr,
                         QByteArray &bytes,
                         GAParserOperand op,
                         int inslen){
    uint64_t val=lang->find_reg_by_name(op.value);
    rawencode(lang,adr,bytes,op,inslen,val);
}


QString GAParameterBitIndex::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    QString rendering=prefix
                        +QString::asprintf("bit%d",(unsigned int) p)
                        +suffix;
    return rendering;
}
void GAParameterBitIndex::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen
                                 ){
    uint64_t val=lang->find_bit_by_name(op.value);
    rawencode(lang,adr,bytes,op,inslen,val);
}


//Group value.
int GAParameterGroup::match(GAParserOperand *op, int len){
    assert(len>0);
    if(op->children.count()==this->params.count()){
        int i=0;
        foreach (auto param, this->params){
            if(!param->match(&op->children[i], len))
                return 0;
            i++;
        }
        return 1;
    }
    return 0;
}


//Explicit register name, must exactly match.
int GAParameterRegName::match(GAParserOperand *op, int len){
    assert(len>0);
    return
        op->value==name
           && op->prefix==prefix
           && op->suffix==suffix;
    ;
}


//Absolute or immediate.
QString GAParameter::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) {
    //Right value for positives.
    int64_t p=rawdecode(lang,adr,bytes,inslen);



    p+=offset;
    QString rendering;
    if(!isSigned){
        /* Most parameters are unsigned, and the trick there is just to keep
         * the rendering short enough that the zeroes don't feel weird.
         */
        rendering=prefix
                    +QString::asprintf("0x%04x",(unsigned int) p)
                    +suffix;
    }else{
        /* Signed parameters are a little harder.  Here, we need to make sure
         * that the decoded string encodes back as an encoded string.
         */
        rendering=prefix
                    +(p<0?"-":"")
                    +QString::asprintf("0x%04x",
                                        p>=0?(unsigned int) p:(unsigned int) -p)
                    +suffix;
    }
    return rendering;
}

//How many bits are in the mask?
uint64_t GAParameter::rawbitcount(int inslen){
    int count=0;
    for(int i=0; i<inslen; i++){
        for(int j=0; j<8; j++){
            if(mask[i] & (1<<j))
                count++;
        }
    }
    return count;
}

//Raw decode function, intentionally not virtual.
uint64_t GAParameter::rawdecode(GALanguage *lang, uint64_t adr,
                               const char *bytes, int inslen){
    return lang->rawdecode(this, adr, bytes, inslen);
}

//Raw encode function, also intentionally not virtual.
void GAParameter::rawencode(GALanguage *lang,
                            uint64_t adr, QByteArray &bytes,
                            GAParserOperand op,
                            int inslen,
                            int64_t val
                       ){
    //If you need to override this, do it in GALanguage.
    lang->rawencode(adr, bytes, op, inslen, this, val);
}


//Encode a parser parameter to bytes, which are OR'ed into target.
void GAParameter::encode(GALanguage *lang, uint64_t adr,
                         QByteArray &bytes,
                         GAParserOperand op,
                         int inslen){
    rawencode(lang, adr, bytes, op, inslen,
              op.int64(true)-offset);
}

//Encode a parser parameter to bytes, which are OR'ed into target.
void GAParameterGroup::encode(GALanguage *lang,
            uint64_t adr, QByteArray &bytes,
            GAParserOperand op,
            int inslen){
    assert(inslen>0);

    if(op.children.count()==this->params.count()){
        int i=0;
        foreach (auto param, this->params){
            //FIXME: bytes.length() is not set yet.
            assert(param->match(&op.children[i], inslen));
            param->encode(lang,adr,bytes,op.children[i], inslen);
            i++;
        }
    }

}

//PC-relative
QString GAParameterRelative::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes, inslen);

    //Need to adjust the relative size if we have more than one byte.
    assert(rawbitcount(inslen)==8);

    //FIXME: Hideous hack that only works on 6502 and 6805.
    uint64_t tadr;
    //qDebug()<<"Decoding at address"<<adr<<"parameter"<<p;
    if(p>128){
        tadr=adr-256+p+offset;
    }else{
        tadr=adr+p+offset;
    }


    QString rendering=prefix
                        +QString::asprintf("0x%04x",(unsigned int) tadr)
                        +suffix;
    return rendering;
}

//Encode a parser parameter to bytes, which are OR'ed into target.
void GAParameterRelative::encode(GALanguage *lang, uint64_t adr,
                         QByteArray &bytes,
                         GAParserOperand op,
                         int inslen){
    uint8_t buf[10];
    for(int i=0; i<lang->maxbytes && i<inslen; i++)
        buf[i]=0;
    int64_t val=op.int64(true);

    //qDebug()<<"Encoding relative jump to "<<val<<"from "<<adr;
    val-=adr;
    val-=offset;

    rawencode(lang, adr, bytes, op, inslen, val);

}

//Symbol address.
QString GAParameterAddress::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);

    QString rendering=prefix
                        +QString::asprintf("0x%04x",(unsigned int) p)
                        +suffix;
    return rendering;
}

//High part of address is fixed, low part provided in the instruction.
QString GAParameterPageAddress::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);

    //FIXME: Apply the page here.

    QString rendering=prefix
                        +QString::asprintf("0x%04x",(unsigned int) p)
                        +suffix;
    return rendering;
}

QString GAParameterGroup::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    QString rendering=QString()+groupstart;
    for(auto i=params.constBegin(); i!=params.constEnd(); i++){
        rendering+=(*i)->decode(lang, adr, bytes, inslen);
        if(i+1!=params.constEnd())
            rendering+=", ";
    }
    rendering+=groupend;

    //qDebug()<<"Rendered"<<rendering;
    return rendering;
}

GAParameter::GAParameter(){
    //this->mask="\x00\x00\x00";
    setMask("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
}

//Immediate values are direct numbers.
GAParameterImmediate::GAParameterImmediate(const char* mask, uint64_t offset){
    setMask(mask);
    this->offset=offset;
    prefix="#";
}


//Absolute values are addresses.
GAParameterAbsolute::GAParameterAbsolute(const char* mask, uint64_t offset){
    setMask(mask);
    this->offset=offset;
    prefix="@";
}
//Ports are an extra memory space.
GAParameterPort::GAParameterPort(const char* mask){
    setMask(mask);
    prefix="%";
}
//PC-relative addressing with some offset.
GAParameterRelative::GAParameterRelative(const char* mask, int offset){
    setMask(mask);
    this->offset=offset;
    prefix="";
}
//An address itself, rather than the value at that address.
GAParameterAddress::GAParameterAddress(const char* mask){
    setMask(mask);
    prefix="";
}
//Offset into the current page.
GAParameterPageAddress::GAParameterPageAddress(const char* mask, uint32_t pagemask){
    setMask(mask);
    this->pagemask=pagemask;
    prefix="";
}
//Register numbers are counted from a bitfield.
GAParameterRegNum::GAParameterRegNum(const char* mask){
    setMask(mask);
}
//3 bits representing a bit choice.
GAParameterBitIndex::GAParameterBitIndex(const char* mask){
    setMask(mask);
}


GAParameterGroup::GAParameterGroup(){

}

GAParameterGroup::GAParameterGroup(const char symbol){
    groupstart=symbol;
    switch(symbol){
    case '(':
        groupend=')';
        break;
    case '[':
        groupend=']';
        break;
    }

    //Crash here if the group end is unsupported.
    assert(groupend!='_');
}


GAParameterRegName::GAParameterRegName(const QString name){
    this->name=name;
}


QString GAParameterRegName::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    //A register name must be a register name.
    if(!lang->regnames.contains(name)){
        qDebug()<<"Names missing"<<name;
    }
    assert(lang->regnames.contains(name));


    return prefix+name+suffix;
}







//Insert parameters into the group.
GAParameterGroup* GAParameterGroup::insert(GAParameter* p){
    memcpy(p->opcodemask, opcodemask, GAMAXLEN);
    memcpy(p->invertmask, invertmask, GAMAXLEN);


    params.append(p);
    for(int i=0; i<GAMAXLEN; i++){
        mask[i]|=p->mask[i];
    }
    return this;
}

//Insert parameters into the group.
GAParameterGroup* GAParameterGroup::operator<<(GAParameter* p){
    insert(p);
    return this;
}



//Registers an immediate argument, given a mask of the original size.
GAParameterGroup* GAParameterGroup::imm(const char *mask, uint64_t offset){
    //Prefixed by #
    insert(new GAParameterImmediate(mask, offset));
    return this;
}
//Same but the immediate is signed.
GAParameterGroup* GAParameterGroup::simm(const char *mask, uint64_t offset){
    //Prefixed by #
    GAParameterImmediate *p= new GAParameterImmediate(mask, offset);
    p->isSigned=true;
    insert(p);
    return this;
}

//Same for an absolute argument, prefixed by @.
GAParameterGroup* GAParameterGroup::abs(const char *mask, uint64_t offset){
    insert(new GAParameterAbsolute(mask, offset));
    return this;
}

//Adds a port number by just its mask, preceeded by %.
GAParameterGroup* GAParameterGroup::port(const char *mask){
    insert(new GAParameterPort(mask));
    return this;
}


//PC-relative address.
GAParameterGroup* GAParameterGroup::rel(const char *mask, int offset){
    insert(new GAParameterRelative(mask, offset));
    return this;
}
//Absolute address is the same as relative from zero.
GAParameterGroup* GAParameterGroup::adr(const char *mask){
    insert(new GAParameterAddress(mask));
    return this;
}
// Page-offset address, common on 4-bit chips.
GAParameterGroup* GAParameterGroup::pageadr(const char *mask, uint32_t pagemask){
    //qDebug()<<"FIXME: Faking a page address. https://github.com/travisgoodspeed/goodasm/issues/55";
    insert(new GAParameterPageAddress(mask, pagemask));
    return this;
}

//Absolute indexed by X.
GAParameterGroup* GAParameterGroup::absx(const char *mask){
    abs(mask);
    insert(new GAParameterRegName("x"));
    return this;
}
//Absolute indexed by Y.
GAParameterGroup* GAParameterGroup::absy(const char *mask){
    abs(mask);
    insert(new GAParameterRegName("y"));
    return this;
}

//Explicit register by name.
GAParameterGroup* GAParameterGroup::regname(const QString name,
                                            const QString prefix,
                                            const QString suffix){
    GAParameterRegName *reg=new GAParameterRegName(name);
    reg->prefix=prefix;
    reg->suffix=suffix;
    insert(reg);
    return this;
}
//Explicit register by name, the indirected.
GAParameterGroup* GAParameterGroup::regnameind(const QString name){
    GAParameterRegName *reg=new GAParameterRegName(name);
    reg->prefix="@";
    reg->suffix="";
    insert(reg);
    return this;
}


//Numbered register with a mask, indirect access.
GAParameterGroup* GAParameterGroup::regind(const char *mask){
    GAParameterRegNum *reg=new GAParameterRegNum(mask);
    reg->prefix="@";
    insert(reg);

    return this;
}
//Numbered register with a mask, direct access.
GAParameterGroup* GAParameterGroup::regdir(const char *mask){
    insert(new GAParameterRegNum(mask));
    return this;
}

//Group of parameters.
GAParameterGroup* GAParameterGroup::group(const char g){
    GAParameterGroup *pg=new GAParameterGroup(g);
    insert(pg);
    return pg;
}

//Bit index.
GAParameterGroup* GAParameterGroup::bit3(const char *mask){
    insert(new GAParameterBitIndex(mask));
    return this;
}
