#include "galangfcard.h"
#include "gainstruction.h"
#include "gaparameter.h"
#include "gamnemonic.h"

#include <QDebug>

uint8_t GALangFCard::scramble(uint8_t b){
    /* "Only the op-codes are scrambled, not the operands.
     * The bit order (MSB to LSB) is 76453012. Then the
     * opcode is xored with 10000000b"
     * (from tv-crypt mailing list 1995)
     */
    uint8_t n=0;
    n|=(b&0xC0);         // Bit 7 and 6 stay in place.
    n|=(b&0x10)?0x20:0;  // Bit 4 moves to bit 5.
    n|=(b&0x20)?0x10:0;  // Bit 5 moves to bit 4.
    n|=(b&0x08);         // Bit 3 stays in place.
    n|=(b&0x01)?0x04:0;  // Bit 0 moves to bit 2.
    n|=(b&0x02);         // Bit 1 stays in place.
    n|=(b&0x04)?0x01:0;  // Bit 2 moves to bit 0.
    n^=0x80;             // Most significant bit flips.

    return n;
}




GALangFCard::GALangFCard() {
    assert(maxbytes==3);
    name="fcard";

    //Scrambling the opcodes handles most instructions.
    foreach(auto m, mnemonics){
        //Scramble opcode.
        m->opcode[0]=scramble(m->opcode[0]);
        //Scramble opcode mask, but undo MSBit flip.
        m->opcodemask[0]=scramble(m->opcodemask[0])
                           ^0x80;
        //Scramble each parameter mask.
        foreach(auto p, m->params){
            p->mask[0]=scramble(p->mask[0])^0x80;
            //6805 has no groups, so we don't need to recurse.
        }
    }

    //The rest are bit-fields, handled in rawencode/rawdecode.
}




//Raw encode function, used by parameters but not opcodes.
void GALangFCard::rawencode(uint64_t adr, QByteArray &bytes,
               GAParserOperand op,
               int inslen,
               GAParameter *param,
               int64_t val){



    int bits=param->rawbitcount(inslen);

    uint8_t buf[GAMAXLEN];

    /* For big endian, we count bytes down from the last
     * byte while populating with the least bits.  Little
     * endian is the reverse, counting forward from the first
     * byte.
     */
    assert(this->endian==BIG);
    for(int i=inslen-1; i>=0; i--){
        uint8_t mb=param->mask[i];
        if(i==0) mb=scramble(param->mask[i])^0x80;

        buf[i]=0;
        for(int j=0; j<8; j++){
            if(mb & (1<<j)){
                if(val&1)
                    buf[i]|=(1<<j);
                val>>=1;
            }
        }
    }

    //Scramble the data of the first buffer if needed.
    buf[0]=scramble(buf[0])^0x80;
    for(int i=0; i<bytes.length() && i<inslen; i++){
        //Apply those to the output.
        bytes[i]=bytes[i]|buf[i];
    }
}
//Raw decode function, used by parameters but not by opcodes.
int64_t GALangFCard::rawdecode(GAParameter *param, uint64_t adr,
                                const char *bytes, int inslen){
    int64_t val=0;  // Value to return.

    int bits=param->rawbitcount(inslen);

    assert(this->endian==BIG);
    for(int i=0; i<inslen; i++){
        //Unscramble only the first byte of the mask.
        uint8_t mb=param->mask[i];
        if(i==0) mb=scramble(param->mask[i])^0x80;
        //Same for data.
        uint8_t db=bytes[i];
        if(i==0) db=scramble(bytes[i]);

        for(int j=7; j>=0; j--){
            if(mb & (1<<j)){
                val<<=1;
                if((1<<j) & (db)){
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
