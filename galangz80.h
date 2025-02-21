#ifndef GALANGZ80_H
#define GALANGZ80_H

/* This implements the Zilog Z80 instruction set without extensions.
 * It's a little like Intel 8080 or Sharp SM83, but not compatible with
 * either.
 */

#include "galanguage.h"
#include "gaparameter.h"

class GALangZ80 : public GALanguage
{
public:
    GALangZ80();
};

//Represents an 8-bit register by Z80 name.
class GAParameterZ80Reg8 : public GAParameter {
public:
    GAParameterZ80Reg8(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    //F is illegal, but included for spacing.
    QString regnames[8]={
                           "b", "c", "d", "e",
                           "f", "h", "l", "a"};
};

//Represents a 16-bit register pair by Z80 name.
class GAParameterZ80Reg16 : public GAParameter {
public:
    GAParameterZ80Reg16(const char* mask, bool stk);

    GAParameterZ80Reg16(const char* mask, QString regnames[4]);
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


//Represents a 3-bit condition flag on Z80.
class GAParameterZ80Cond : public GAParameter {
public:
    GAParameterZ80Cond(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    /* NotZero, Zero, NotCarry, Carry.
     * Parityodd, ParityEven, Positive, Negative
     */
    QString condnames[8]={"nz", "z", "nc", "c",
                          "po", "pe", "p", "m"
                          };
};


//Represents a reset handler on SM83
class GAParameterZ80ResetHandler : public GAParameter {
public:
    /* These are basically 3 bits representing a handler address
     * early in the first page.
     * 111 should decode to "rst 38"
     */
    GAParameterZ80ResetHandler(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};


#endif // GALANGZ80_H
