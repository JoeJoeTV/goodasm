#include <QDebug>

#include "gagraderjumptargets.h"
#include "gainstruction.h"
#include "gamnemonic.h"
#include "goodasm.h"


GAGraderJumpTargets::GAGraderJumpTargets() {
    name="jumptargets";
}


// Is it real?
uint64_t GAGraderJumpTargets::isValid(GoodASM *goodasm){
    assert(goodasm);

    //Needed to set the threshold.
    isCompatible(goodasm->lang);

    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    uint64_t valid=0, invalid=0;

    //Zeroed array of targets to count;
    uint32_t targets[0x10000];
    memset((void*) targets, 0, 0x40000);

    //Populates targets[] with a count of references to each location.
    foreach(auto ins, goodasm->instructions){
        auto P=ins.params.split(" ");
        switch(ins.type){
        case GAInstruction::MNEMONIC:
            //FIXME: Adjust GAInstruction to include parameters.
            foreach(auto p, P){
                //qDebug()<<"p="<<p;
                if(p.startsWith("0x")){
                    bool okay=false;
                    uint64_t adr=GAParser::str2uint(p, &okay);
                    //FIXME: This is the wrong way to handle address boundaries.
                    if(okay) targets[adr&0xFFFF]++;
                }
            }
            break;
        default:
            break;
        }
    }

    uint64_t maxcount=0;
    uint64_t thresholdcount=0;
    for(int i=0x0; i<0x10000; i++){
        if(targets[i]>maxcount)
            maxcount=targets[i];
        if(targets[i]>threshold){
            thresholdcount++;
        }
    }

    if(thresholdcount<10) return 0;

    return maxcount;
}

// Is this grader compatible?
bool GAGraderJumpTargets::isCompatible(GALanguage *lang){
    assert(lang);
    QString name=lang->name;

    //4-bit chips aren't compatible with this trick.
    if(name=="ucom43" || name=="tlcs47" || name=="chip8" || name=="pic16c5x")
        return false;

    //8051 also triggers false positives a lot.  Not sure why.
    if(name=="8051") return false;

    //Maybe this works everywhere?
    return true;
}
