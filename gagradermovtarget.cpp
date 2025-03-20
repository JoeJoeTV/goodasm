#include <QDebug>

#include "gainstruction.h"
#include "gamnemonic.h"
#include "goodasm.h"
#include "gagradermovtarget.h"

GAGraderMovTarget::GAGraderMovTarget() {
    name="movtarget";
    //stable=1;
}

// Is it real?
uint64_t GAGraderMovTarget::isValid(GoodASM *goodasm){
    assert(goodasm);
    QString lname=goodasm->lang->name;

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
            foreach(QString p, P){
                //qDebug()<<"p="<<p;
                if(p.startsWith("@0x")){
                    bool okay=false;
                    auto q=p.remove(",");
                    uint64_t adr=GAParser::str2uint(q.right(q.length()-1), &okay);
                    //FIXME: This is the wrong way to handle address boundaries.
                    //qDebug()<<"Adr is "<<adr<<"for"<<ins.params;
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
        if(targets[i]>0)
            thresholdcount++;
    }

    uint32_t highcount=0;    // Globals near end of memory.
    uint32_t lowcount=0;     // Globals in low memory.
    uint32_t midcount=0;     // Globals outside that range.
    uint32_t highest=0x0, lowest=0xffff; //Range.
    for(uint64_t i=0x0; i<0x10000; i++){
        if(targets[i]>10){
            if(i>0xfe00) highcount++;
            else if(i<0x0200) lowcount++;
            else midcount++;

            if(i>highest) highest=i;
            if(i<lowest) lowest=i;

            //if(goodasm->verbose) qDebug()<<"adr: "<<i;
        }
    }
    qDebug()<<lname<<"Highest "<<highest<<"Lowest"<<lowest;
    uint32_t range=highest-lowest;

    if(goodasm->verbose){
        qDebug()<<lname
                 <<"high"<<highcount
                 <<"low"<<lowcount
                 <<"mid"<<midcount;
    }

    if((lname=="z80")
               && (midcount>lowcount+highcount)
               //&& range<12000
               ){
        //Z80 instruction push this to the low range.
        qDebug()<<"Z80 Range is "<<range;
        return 1;
    }

    //Not a match if we get this far.
    return 0;
}

// Is this grader compatible?
bool GAGraderMovTarget::isCompatible(GALanguage *lang){
    assert(lang);
    QString name=lang->name;

    //Known compatible chips.
    if(name=="z80")
        return true;

    //Maybe this works everywhere?
    return false;
}
