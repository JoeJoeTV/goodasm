#include "gagrader8051jmptable.h"

#include "gainstruction.h"
#include "gamnemonic.h"
#include "goodasm.h"

#include <QDebug>

GAGrader8051JmpTable::GAGrader8051JmpTable() {
    name="8051jmptable";
    stable=true;
}


// Is it real?
uint64_t GAGrader8051JmpTable::isValid(GoodASM *goodasm){
    assert(goodasm);

    //Needed to set the threshold.
    isCompatible(goodasm->lang);

    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    uint64_t invalid=0, valid=0;

    /* This looks for a lot of LJMP instructions in a
     * row, which is indicative of a jump table in 8051.
     *
     * We probably need other tricks for programs without
     * jump tables.
     */
    foreach(auto ins, goodasm->instructions){
        if(ins.data[0]==(char) 0x02) // LJMP instruction.
            valid++;
        else
            valid=0;

        if(valid>7){
            return 1;
        }
    }

    //No jump table, so not identified by this rule.
    return 0;
}

// Is this grader compatible?
bool GAGrader8051JmpTable::isCompatible(GALanguage *lang){
    //Only works for 8051.
    return lang->name=="8051";
}
