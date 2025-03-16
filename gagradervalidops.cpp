#include "gagradervalidops.h"

#include <QDebug>

#include "goodasm.h"
#include "gamnemonic.h"

GAGraderValidOps::GAGraderValidOps() {
    name="validops";
}

// Is it real?
uint64_t GAGraderValidOps::isValid(GoodASM *goodasm){
    //Needed to set the threshold.
    isCompatible(goodasm->lang);


    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    uint64_t valid=0, invalid=0;

    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        switch(ins.type){
        case GAInstruction::DATA:
            invalid++;
            break;
        case GAInstruction::MNEMONIC:
            valid++;
            break;
        default:
            break;
        }
    }

    qreal ratio=((qreal)valid)/((qreal)(valid+invalid));
    if(goodasm->verbose){
        qDebug()<<"Ratio:     "<<ratio;
        qDebug()<<"Threshold: "<<threshold;
    }

    return ratio>threshold;
}

// Is this grader compatible?
bool GAGraderValidOps::isCompatible(GALanguage *lang){
    GALanguage *l=lang;

    //How many opcodes are defined?
    int count=0;
    for(unsigned int I=0; I<0x100; I++){
        char i=I;
        int matched=0;
        foreach(auto m, l->mnemonics){
            if((i&m->opcodemask[0])==m->opcode[0])
                matched++;
        }
        if(matched) count++;
    }

    /* The threshold is 90% valid instructions or 5% more valid instructions
     * than would be explained by chance.  On architectures like 8051 where
     * the threshold is >1.0, this technique doens't work anyways.
     */
    threshold=((qreal) count)/256.0;
    threshold+=0.05;
    if(threshold<0.9)
        threshold=0.9;

    //Not enough illegal instructions for this to work.
    if(threshold>1.0) return false;

    //Enough illegal instructions that this might work.
    return true;
}
