#include<QDebug>

#include "galanguage.h"
#include "gainstruction.h"
#include "goodasm.h"
#include "gamnemonic.h"
#include "gaparser.h"

GALanguage::~GALanguage(){
    foreach (auto m, mnemonics) {
        delete m;
    }
}

//Raw encode function, used by parameters but not opcodes.
void GALanguage::rawencode(uint64_t adr, QByteArray &bytes,
                           GAParserOperand op,
                           int inslen,
                           GAParameter *param,
                           int64_t val
                           ){
    uint8_t buf[GAMAXLEN];

    /* For big endian, we count bytes down from the last
     * byte while populating with the least bits.  Little
     * endian is the reverse, counting forward from the first
     * byte.
     */
    if(endian==GALanguage::BIG)
        for(int i=inslen-1; i>=0; i--){
            int mb=param->mask[i];
            buf[i]=0;
            for(int j=0; j<8; j++){
                if(mb & (1<<j)){
                    if(val&1)
                        buf[i]|=(1<<j);
                    val>>=1;
                }
            }
        }
    else //Little endian.
        for(int i=0; i<inslen; i++){
            int mb=param->mask[i];
            buf[i]=0;
            for(int j=0; j<8; j++){
                if(mb & (1<<j)){
                    if(val&1)
                        buf[i]|=(1<<j);
                    val>>=1;
                }
            }
        }

    for(int i=0; i<bytes.length() && i<inslen; i++){
        //Invert bits of just this parameter from the invert mask.
        buf[i]^=(param->invertmask[i]&param->mask[i]);
        //Apply those to the output.
        bytes[i]=bytes[i]|buf[i];
    }
}

//Raw decode function, used by parameters but not by opcodes.
uint64_t GALanguage::rawdecode(GAParameter *param, uint64_t adr,
                                const char *bytes, int inslen){
    int64_t val=0;  // Value to return.

    /* For little endian, we count bytes down from the last
     * byte while populating with the least bits.  Big
     * endian is the reverse, counting forward from the first
     * byte.
     */
    if(endian==GALanguage::LITTLE)
        for(int i=inslen-1; i>=0; i--){
            int mb=param->mask[i];
            for(int j=7; j>=0; j--){
                if(mb & (1<<j)){
                    val<<=1;
                    if((bytes[i]^param->invertmask[i])&(1<<j)){
                        val|=1;
                    }
                }
            }
        }
    else //Big endian.
        for(int i=0; i<inslen; i++){
            int mb=param->mask[i];
            for(int j=7; j>=0; j--){
                if(mb & (1<<j)){
                    val<<=1;
                    if((bytes[i]^param->invertmask[i])&(1<<j)){
                        val|=1;
                    }
                }
            }
        }

    if(param->isSigned){    //Sign extend if necessary.
        int count=param->rawbitcount(inslen);  //Count of bits after decoding.
        if(val&(1<<(count-1))){           //If the highest bit is set,
            val^=(1<<count)-1;            //  first flip the used bits.
            val^=-1;                      //   then flip all bits.
        }
    }
    return val;
}


uint64_t GALanguage::find_reg_by_name(QString name){
    /* By default, every valid register name needs
     * to begin with the letter R and then an integer
     * number.  r0 and r1 on 8051, for example.
     *
     * Override these functions if you need to do something
     * more complex.
     */

    assert(name.length()>=2);
    assert(name[0]==QString("r"));

    QString sub=name.mid(1,name.length()-1);
    bool okay;
    int regnum=sub.toInt(&okay);
    if(!okay)
        qDebug()<<"Error parsing register name: "<<sub;
    return regnum;
}
uint64_t GALanguage::find_bit_by_name(QString name){
    /* Bit names come two ways:
     * bit1, bit2, bit3, etc
     * 1, 2, 4, 8.
     *
     * Override these functions if you need to do something
     * more complex.
     */

    if(name.length()>=4 && name.startsWith("bit")){
        QString sub=name.mid(3,name.length()-3);
        bool okay;
        int bitnum=sub.toInt(&okay);
        if(!okay)
            qDebug()<<"Error parsing bitfield name: "<<sub;
        return bitnum;
    }else{
        bool okay;
        int i=name.toInt(&okay);
        if(!okay)
            qDebug()<<"Error parsing bitfield mask "<<name;
        switch(i){
        case 1: return 0;
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        case 0x10: return 4;
        case 0x20: return 5;
        case 0x40: return 6;
        case 0x80: return 7;
        }
        qDebug()<<"Bitfield mask is out of range: "<<name;
        return 0;
    }
}
QString GALanguage::find_regname(uint64_t num){
    QString name="";
    name.setNum(num);
    return "r"+name;
}

void GALanguage::setGoodASM(GoodASM* gasm){
    assert(gasm);
    this->goodasm=gasm;
}

//Disassemble an instruction from an address.
GAInstruction GALanguage::dis(uint64_t adr){
    assert(goodasm);

    /* By default, we just output db directives.
     * You can add your own implementation as you subclass this.
     */
    uint32_t len; //throwaway, passed by reference.
    GAInstruction ins=decode(adr, len);
    ins.adr=adr;
    return ins;
}

//Returns a GAInstruction from a verb and operands.
GAInstruction GALanguage::match(QString verb, QList<GAParserOperand> ops,
                                uint64_t adr){
    assert(goodasm);

    GAInstruction ins(goodasm);
    GAMnemonic *matched=0;
    int count=0;
    QString dups="";

    if(goodasm->verbose)
        qDebug()<<"Matching"<<verb<<"with"<<ops.count()<<"parameters.";
    foreach (GAMnemonic *m, mnemonics) {
        if(m->match(ins, adr, verb, ops)){
            if(!matched){
                matched=m;
            }
            dups+=m->examplestr+";";
            count++;
        }
    }

    //Handy to highlight collisions if we aren't separating them right.
    if(goodasm->verbose && count>1){
        qDebug()<<"Duplicates:"<<dups;
    }

    if(matched){
        matched->match(ins, adr, verb, ops);
        return ins;
    }

    //Empty instruction if we don't know what it is.
    //qDebug()<<"ERROR: unmatched instruction on line"<<goodasm->line;
    goodasm->error("Unmatched instruction.");
    return GAInstruction(goodasm);
}


//Insert a mnemonic into the language.
GAMnemonic* GALanguage::insert(GAMnemonic* mnemonic){
    mnemonic->lang=this;
    mnemonics.append(mnemonic);
    assert(name!="nameless");
    return mnemonic;
}

//Convert a native address to a byte address.
uint64_t GALanguage::byteAdr(uint64_t adr){
    return adr*epsilon;
}
//Convert a byte address to a native address.
uint64_t GALanguage::nativeAdr(uint64_t adr){
    return adr/epsilon;
}


//Cheat sheet of mnemonic names.
QString GALanguage::cheatSheet(){
    QString cheat="";

    GAMnemonic* mnem=0;
    for(QList<GAMnemonic*>::iterator i = mnemonics.begin(),
         end = mnemonics.end();
         i != end;
         ++i){
        mnem=((GAMnemonic*)*i);

        if(mnem->examplestr.length()>0){
            cheat+=goodasm->formatSource("", mnem->examplestr, "; "+mnem->helpstr);
        }else{
            cheat+=goodasm->formatSource(mnem->name, "", "MISSING");
        }
    }

    return cheat;
}

//Matches a known mnemonic to decode, or db for default.
GAInstruction GALanguage::decode(uint64_t adr, char *bytes, uint32_t &len){
    //Default decoding is a DB directive of a single byte.
    GAInstruction ins(goodasm, bytes[0]);
    //Count matches, panicking if there's a collision.
    int count=0;

    //Duplicate matches
    QString dupes="";
    QString lastexample="";

    GAMnemonic *mnem=0, *bestmnem=0;
    for(QList<GAMnemonic*>::iterator i = mnemonics.begin(),
         end = mnemonics.end();
         i != end;
         ++i){
        mnem=((GAMnemonic*)*i);

        //We want the match with the highest priority.
        if(mnem->match(ins, adr, len, bytes)){
            if(bestmnem && bestmnem->priority>mnem->priority){
                //We already have a better match, so do nothing.
            }else if(bestmnem && bestmnem->priority==mnem->priority){
                //Priority is steady; we have a collision!
                if(!count)
                    dupes+=bestmnem->name+" ";
                dupes+=mnem->name+" ";
                count++;
                lastexample=mnem->examplestr;
            }else{
                //We have a new best match.
                bestmnem=mnem;
                //And the old collisions are no longer a problem.
                count=0;
                dupes="";
            }
        }
    }

    if(count>=1){
        goodasm->duplicates++;
        qDebug()<<"Colliding matches: "<<dupes;
        qDebug()<<"1: "<<bestmnem->examplestr;
        qDebug()<<"2: "<<lastexample;
    }

    //Reapply the best match.
    if(bestmnem)
        bestmnem->match(ins,adr,len,bytes);

    return ins;
}

//Matches a known mnemonic to decode, or db for default.
GAInstruction GALanguage::decode(uint64_t adr, uint32_t &len){
    //Here we populate a few bytes to decode.
    assert(goodasm->lang->maxbytes<=GAMAXLEN);
    char bytes[GAMAXLEN];
    for(int i=0; i<GAMAXLEN; i++)
        bytes[i]=goodasm->byteAt(adr+i);
    return decode(adr, bytes, len);
}
