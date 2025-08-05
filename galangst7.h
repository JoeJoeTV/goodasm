#ifndef GALANGST7_H
#define GALANGST7_H


//#include "galang6805.h"
#include "galanguage.h"

class GALangST7 : public GALanguage //public GALang6805
{
public:
    GALangST7();

private:
    //15 opcode arithmetic instruction, destination A.
    void buildArithmetic15(uint8_t opcode,
                           QString name,
                           QString help,
                           bool mode6805=false);
    //11 opcode arithmetic instruction.  (not A)
    void buildArithmetic11(uint8_t opcode,
                           QString name,
                           QString help,
                           bool mode6805=false);
};

#endif // GALANGST7_H
