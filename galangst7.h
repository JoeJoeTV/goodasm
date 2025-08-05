#ifndef GALANGST7_H
#define GALANGST7_H


//#include "galang6805.h"
#include "galanguage.h"

class GALangST7 : public GALanguage //public GALang6805
{
public:
    bool mode6805=false;
    GALangST7(bool mode6805=false);

private:
    //15 opcode arithmetic instruction, destination A.
    void buildArithmetic15(uint8_t opcode,
                           QString name,
                           QString help);
    //11 opcode arithmetic instruction.  (not A)
    void buildArithmetic11(uint8_t opcode,
                           QString name,
                           QString help);
    //PUSH and POP on ST17.  Not on 6805.
    void buildStackOp(uint8_t opcode,
                      QString name,
                      QString help);
    //CALL and JP.
    void buildCallJump(uint8_t opcode,
                       QString name,
                       QString help);
};

#endif // GALANGST7_H
