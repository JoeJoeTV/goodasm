#ifndef GALANGSH83_H
#define GALANGSH83_H

/* This represents the H83 machine language.
 */

#include "galanguage.h"
#include "gaparameter.h"

// This class represents the H83.
class GALangH83 : public GALanguage
{
public:
    GALangH83();
};

// H83 Constant Operand.
class GAParameterH83Const : public GAParameter {
public:
    GAParameterH83Const(unsigned int value);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

    unsigned int const_value;
    QString prefix = "#";
    QString suffix = "";
};

// H83 8-Bit Register Access Decoding.
class GAParameterH83Reg8 : public GAParameter {
public:
    GAParameterH83Reg8(const char* mask, bool indirect = false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;
private:
    QString regnames[16] = {
        "r0h", "r1h", "r2h", "r3h", "r4h", "r5h", "r6h", "r7h",
        "r0l", "r1l", "r2l", "r3l", "r4l", "r5l", "r6l", "r7l",
    };
};

// H83 16-Bit Register Access Decoding.
class GAParameterH83Reg16 : public GAParameter {
public:
    GAParameterH83Reg16(const char* mask, bool indirect = false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;
private:
    QString regnames[16] = {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7",
    };
};

// H83 32-Bit Register Access Decoding.
class GAParameterH83Reg32 : public GAParameter {
public:
    GAParameterH83Reg32(const char* mask, bool indirect = false, bool postinc = false, bool predec = false);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char* bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;
private:
    QString regnames[8] = {
        "er0", "er1", "er2", "er3", "er4", "er5", "er6", "er7",
    };
};

#endif // GALANGSH83_H
