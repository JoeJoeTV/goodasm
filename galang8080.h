#ifndef GALANG8080_H
#define GALANG8080_H

#include "galanguage.h"
#include "gaparameter.h"

class GALang8080 : public GALanguage
{
public:
    GALang8080();
};

//Represents an 8-bit register by 8080 name.
class GAParameter8080Reg8 : public GAParameter {
public:
    GAParameter8080Reg8(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    //M is handled separately, but included for spacing.
    QString regnames[8]={"b", "c", "d", "e",
                         "h", "l", "@hl", "a"};
};

//Represents 1-bit register pair, @bc or @de.
class GAParameter8080RP1 : public GAParameter {
public:
    GAParameter8080RP1(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    //M is handled separately, but included for spacing.
    QString regnames[2]={"bc", "de"};
};

//Represents 2-bit register pair.
class GAParameter8080RP2 : public GAParameter {
public:
    GAParameter8080RP2(const char* mask, bool sp);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    //Final entry is either Flags+A or SP.
    QString regnames[4]={"bc", "de", "hl", "fa"};   //Our register names.
    QString stdregnames[4]={"b", "d", "h", "psw"};  //Standard register names.
};

//Represents 3-bit
class GAParameter8080EXP : public GAParameter {
public:
    GAParameter8080EXP(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GALANG8080_H
