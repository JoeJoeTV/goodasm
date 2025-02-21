#ifndef GALANG8051_H
#define GALANG8051_H

/* This is an 8051 driver for GoodASM, implementing all standard instructions
 * in all standard memory modes.
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALang8051 : public GALanguage
{
public:
    GALang8051();
};

//Represents an 11-bit address offset on 8051
class GAParameter8051Addr11 : public GAParameter {
public:
    /* These parameters are sort of like the page-local
     * addressing of other chips.  First you add the length
     * of the active instruction to the PC, then mask off PC
     * to apply the 11 bits provided by this field.
     */
    GAParameter8051Addr11(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr,
                   const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};


//Represents a register by 8051 name.
class GAParameter8051Reg : public GAParameter {
public:
    GAParameter8051Reg(const char* mask, bool indirect=false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    QString regnames[8]={
                           "r0", "r1", "r2", "r3",
                           "r4", "r5", "r6", "r7"};
};

//Represents an 8051 "bit address", used by some bit masking instructions.
class GAParameter8051Bit : public GAParameter {
public:
    GAParameter8051Bit(const char* mask, bool inverted=false);
};


#endif // GALANG8051_H
