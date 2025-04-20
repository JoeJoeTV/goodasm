#ifndef GALANGZ8_H
#define GALANGZ8_H

/* This implements Zilog's Z8 instruction set, not to be confused
 * with Z80.
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALangZ8 : public GALanguage
{
public:
    GALangZ8();
};

// Represents an 4-bit register by Z8 name, such as r15.
class GAParameterZ8Reg4 : public GAParameter {
public:
    GAParameterZ8Reg4(const char* mask, bool indirect=false, bool pair=false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;

    bool pair=false;
};

/* This represents an 8-bit register or a 4-bit register.  For a 4-bit register,
 * the high nybble is set to E and the 4-bit naming convention is used.
 */
class GAParameterZ8Reg8 : public GAParameter {
public:
    GAParameterZ8Reg8(const char* mask, bool indirect=false, bool pair=false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
    bool pair=false;
};

// Represents a Z8 condition code nybble.
class GAParameterZ8CC : public GAParameter {
public:
    GAParameterZ8CC(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
    QString names[16]={
        "f",   // 0: False
        "lt",  // 1: Less Than
        "le",  // 2: Less than or Equal to
        "ule", // 3: Unsigned <=
        "ov",  // 4: Overflow
        "mi",  // 5: Minus
        "eq",  // 6: Equal / Z
        "c",   // 7: Carry / ULT
        "t",   // 8: True / (empty)
        "ge",  // 9: Greater or Equal
        "gt",  // A: Greater Than
        "ugt", // B: Unsigned Greater Than
        "nov", // C: No Overflow
        "pl",  // D: Plus
        "ne",  // E: Not Equal / NZ
        "nc",  // F: No Carry / UGE
    };
    /* Condition codes are fickel.  This set is also accepted for
     * encoding, but we never decode to these names.  They are
     * equal when unambiguous.
     */
    QString othernames[16]={
        "f",   // 0: False
        "lt",  // 1: Less Than
        "le",  // 2: Less than or Equal to
        "ule", // 3: Unsigned <=
        "ov",  // 4: Overflow
        "mi",  // 5: Minus
        "z",   // 6: Equal / Z
        "ult", // 7: Carry / ULT
        "t",   // 8: True / (empty)
        "ge",  // 9: Greater or Equal
        "gt",  // A: Greater Than
        "ugt", // B: Unsigned Greater Than
        "nov", // C: No Overflow
        "pl",  // D: Plus
        "nz",  // E: Not Equal / NZ
        "uge", // F: No Carry / UGE
    };
};

#endif // GALANGZ8_H
