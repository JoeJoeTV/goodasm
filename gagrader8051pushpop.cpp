#include "gagrader8051pushpop.h"

GAGrader8051PushPop::GAGrader8051PushPop() {
    name="8051pushpop";
    stable=true;
}


// Is it real?
uint64_t GAGrader8051PushPop::isValid(GoodASM *goodasm){
    assert(goodasm);

    //Needed to set the threshold.
    isCompatible(goodasm->lang);

    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    uint64_t invalid=0, valid=0;

    /* 1. Functions are between RETs.
     * 2. Looking for a funciton that begins with PUSHes.
     * 3. Same function ends with POPs in reverse order.
     */
    enum {pre,push,pop, off} s=pre;
    QStack<QString> stack;
    int depth=0;  //Maximum depth we've seen since last return.
    int gdepth=0; //Maximum depth we've seen in any function.
    int fcount=0; //Number of functions above threshold.

    foreach(auto ins, goodasm->instructions)
        if(ins.type==GAInstruction::MNEMONIC)
    {
        QString m=ins.mnem->name;
        if(m=="ret" || m=="reti"){
            if(s==pop && depth>1 && stack.count()==0){
                if(depth>gdepth)
                    gdepth=depth;
                fcount++;
            }
            depth=0;
            stack.clear();
            s=pre;  // Ready for next instruction.
        }else if(m=="push"){
            if(s==pre || s==push){
                s=push;  // Pushing.
                stack.push(ins.params[0]);
            }else
                s=off;  //Off the rails.
        }else if(m=="pop"){
            if((s==push || s==pop) && stack.count()>0){
                s=pop;  // Popping.
                QString p=stack.pop();
                if(p==ins.params[0])
                    depth++;
                else
                    s=off;
            }else
                s=off; //Off the rails.
        }

    }

    //Must be at least two functions with at least two globals apiece.
    if(fcount==1) return 0;
    return fcount;
}

// Is this grader compatible?
bool GAGrader8051PushPop::isCompatible(GALanguage *lang){
    //Only works for 8051.
    QString n=lang->name;
    return n=="8051";

    //Also works for Z80 and SM83, but cannot distinguish them.
    //|| n=="z80" || n=="sm83";
}

