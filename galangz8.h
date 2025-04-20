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
    GAParameterZ8Reg4(const char* mask, bool indirect=false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

/* This represents an 8-bit register or a 4-bit register.  For a 4-bit register,
 * the high nybble is set to E and the 4-bit naming convention is used.
 */
class GAParameterZ8Reg8 : public GAParameter {
public:
    GAParameterZ8Reg8(const char* mask, bool indirect=false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GALANGZ8_H
