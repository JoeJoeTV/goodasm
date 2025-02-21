#ifndef GALANGTLCS47_H
#define GALANGTLCS47_H

/* This tries to implement the Toshiba TLCS47 architecture.
 * I'm still trying to get a full dump of a TLCS47 chip, so
 * please expect some instructions to be wrong.
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALangTLCS47 : public GALanguage
{
public:
    GALangTLCS47();
};


//Represents an 10-bit address offset on 8051
class GAParameterTLCS47Addr6 : public GAParameter {
public:
    /* This for page-local addressing such as the "BSS A" instruction,
     * where the low six bits of the program counter are replaced with
     * those from this instruction.
     */
    GAParameterTLCS47Addr6(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr,
                   const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GALANGTLCS47_H
