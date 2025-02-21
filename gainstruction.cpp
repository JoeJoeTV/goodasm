#include <QDebug>

#include "galisting.h"
#include "gamnemonic.h"
#include "gainstruction.h"
#include "goodasm.h"

GAInstruction::GAInstruction(GoodASM *gasm) {
    assert(gasm);
    this->gasm=gasm;
}

// FIXME: Support strings.
GAInstruction::GAInstruction(GoodASM *gasm, QString str) {
    assert(gasm);
    this->gasm=gasm;
    qDebug()<<"What do I do with the string?"<<str;
}


// Single byte.
GAInstruction::GAInstruction(GoodASM *gasm, uint8_t byte){
    assert(gasm);
    this->gasm=gasm;
    len=1;
    type=DATA;
    data.append(byte);
}

GAInstruction::GAInstruction(GoodASM *gasm, QByteArray bytes){
    assert(gasm);
    this->gasm=gasm;
    len=bytes.length();
    type=DATA;
    data=bytes;
}



//Return source code disassembly.
QString GAInstruction::source(){
    return gasm->listing->render(this);
}



//Return binary machine code.
QByteArray GAInstruction::code(){
    //assert(len==data.length());
    //len=data.length();
    return data;
}



//Next instruction.
GAInstruction GAInstruction::next(){
    assert(gasm);
    return gasm->at(adr+len);
}
