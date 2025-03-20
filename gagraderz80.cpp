#include "gagraderz80.h"

#include <QDebug>

#include "goodasm.h"
#include "gamnemonic.h"

GAGraderZ80::GAGraderZ80() {
    name="z80";
    stable=1;
}

// Is it real?
uint64_t GAGraderZ80::isValid(GoodASM *goodasm){
    //Needed to set the threshold.
    isCompatible(goodasm->lang);

    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    uint64_t invalid=0, valid=0;

    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        if(ins.data[0]==(char) 0xdd)
            switch(ins.type){
            case GAInstruction::DATA:
                invalid++;
                break;
            case GAInstruction::MNEMONIC:
                valid++;
                break;
            default:
                qDebug()<<"Type="<<ins.type;
                break;
            }
    }

    qreal ratio=((qreal)valid)/((qreal)(valid+invalid));
    if(goodasm->verbose){
        qDebug()<<"Z80 DD "<<invalid<<"/"<<(valid);
        qDebug()<<"Z80 DD Ratio:     "<<ratio;
    }

    return ratio>0.90;
}

// Is this grader compatible?
bool GAGraderZ80::isCompatible(GALanguage *lang){
    GALanguage *l=lang;

    if(lang->name=="z80") return true;

    //Doesn't work on anything else.
    return false;
}
