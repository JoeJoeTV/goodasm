#ifndef GALANGSCPU2_H
#define GALANGSCPU2_H

/* This implements a modified version of the Sharp SM85 core used for the Saar CPU 2 (name not final).
 * See the SM38 core for more details.
 *
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALangSCPU2 : public GALanguage
{
public:
    GALangSCPU2();
};

//Represents an 8-bit register by SCPU2 name.
class GAParameterSCPU2Reg8 : public GAParameter {
public:
    GAParameterSCPU2Reg8(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    QString regnames[8]={
        "b", "c", "d", "e",
        "f", "h", "l", "a"};
};

//Represents a 16-bit register pair by SCPU2 name.
class GAParameterSCPU2Reg16 : public GAParameter {
public:
    GAParameterSCPU2Reg16(const char* mask, bool stk);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    QString regnames[4]={"bc", "de", "hl", "sp"};
};

//Represents a condition flag on SCPU2
class GAParameterSCPU2Cond : public GAParameter {
public:
    GAParameterSCPU2Cond(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    /* NotZero, Zero, NotCarry, Carry.
     * There is no condition for Always.
     */
    QString condnames[4]={"nz", "z", "nc", "c"};
};

//Represents a reset handler on SCPU2
class GAParameterSCPU2ResetHandler : public GAParameter {
public:
    /* These are basically 3 bits representing a handler address
     * early in the first page.
     * ff should decode to "rst 38"
     */
    GAParameterSCPU2ResetHandler(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GALANGSCPU2_H
