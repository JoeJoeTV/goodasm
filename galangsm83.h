#ifndef GALANGSM83_H
#define GALANGSM83_H

/* This implements the Sharp SM85 core of the Nintendo GameBoy.
 * It's a lot like Z80 or 8080, but not enough to be compatible.
 * The two-byte opcodes are not yet supported, and sometimes
 * we get confused about which 16-bit register pair to use.
 *
 * Some quirks of this architecture:
 *
 * CPU registers: https://rylev.github.io/DMG-01/public/book/cpu/registers.html
 *
 * SM83 has eight 8-bit registers declared by letter (a b c d e f h l) and four 16-bit
 * registers, (af bc de hl).  The 16-bit registers are made from pairs of the 8-bit registers,
 * so AF stores the upper byte in A and the lower by in F.  Some instructions specifically
 * refer to the register by name, others by the index number of its type.
 *
 * The F register is a little weird, with the lower half zeroed and the upper half as flags.
 *
 * Remember that despite the mostly regular layout, this is a CISC chip whose opcodes
 * do not exactly fit the regular pattern.
 *
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALangSM83 : public GALanguage
{
public:
    GALangSM83();
};

//Represents an 8-bit register by SM83 name.
class GAParameterSM83Reg8 : public GAParameter {
public:
    GAParameterSM83Reg8(const char* mask);
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

//Represents a 16-bit register pair by SM83 name.
class GAParameterSM83Reg16 : public GAParameter {
public:
    GAParameterSM83Reg16(const char* mask, bool stk);
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

//Represents a condition flag on SM83
class GAParameterSM83Cond : public GAParameter {
public:
    GAParameterSM83Cond(const char* mask);
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

//Represents a reset handler on SM83
class GAParameterSM83ResetHandler : public GAParameter {
public:
    /* These are basically 3 bits representing a handler address
     * early in the first page.
     * ff should decode to "rst 38"
     */
    GAParameterSM83ResetHandler(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GALANGSM83_H
