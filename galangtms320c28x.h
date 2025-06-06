#ifndef GALANGTMS320C28X_H
#define GALANGTMS320C28X_H

#include <QMap>
#include <QStringList>

#include "galanguage.h"
#include "gamnemonic.h"

/* This implements the TMS320C28x DSP series by Texas Instruments.

 The architecture includes two compability modes for migrating older designs:
 C2xLP source-compatible mode and C27x object-compatible mode. At reset, the
 C28x CPU is in C27x object-compatible mode but a "c28obj" instruction is
 typically called early on to switch to full C28x device mode.
 
 Current development efforts for this target are limited to C28x device mode.
 */

class GALangTMS320C28x : public GALanguage
{
  public:
    GALangTMS320C28x();
};

// TMS320C28x 32-bit register access decoding.
class GAParameterTMS320C28xReg32 : public GAParameter
{
  public:
    GAParameterTMS320C28xReg32(const char *mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

  private:
    QMap<uint8_t, QString> regnames = {
        {0b0'000, "xar0"},
        {0b0'001, "xar1"},
        {0b0'010, "xar2"},
        {0b0'011, "xar3"},
        {0b0'100, "xar4"},
        {0b0'101, "xar5"},
        {0b0'110, "xar6"},
        {0b0'111, "xar7"},
        {0b1'001, "acc"},
        {0b1'011, "p"},
        {0b1'100, "xt"},
    };
};

// TMS320C28x 16-bit register access decoding.
class GAParameterTMS320C28xReg16 : public GAParameter
{
  public:
    GAParameterTMS320C28xReg16(const char *mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

  private:
    QMap<uint8_t, QString> regnames = {
        {0b0'000, "ar0"},
        {0b0'001, "ar1"},
        {0b0'010, "ar2"},
        {0b0'011, "ar3"},
        {0b0'100, "ar4"},
        {0b0'101, "ar5"},
        {0b0'110, "ar6"},
        {0b0'111, "ar7"},
        {0b1'000, "ah"},
        {0b1'001, "al"},
        {0b1'010, "ph"},
        {0b1'011, "pl"},
        {0b1'100, "th"},
        {0b1'101, "sp"},
    };
};

// TMS320C28x 22-bit immediate.
class GAParameterTMS320C28xImm22 : public GAParameter
{
  public:
    GAParameterTMS320C28xImm22(const char *mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

    QString prefix = "#";
    QString suffix = "";
};

// TMS320C28x 16-bit SP-relative address.
class GAParameterTMS320C28xRel16 : public GAParameter
{
  public:
    GAParameterTMS320C28xRel16(const char *mask);

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

    QString prefix = "";
    QString suffix = "";
};

// TMS320C28x constant operand.
class GAParameterTMS320C28xConst : public GAParameter
{
  public:
    GAParameterTMS320C28xConst(unsigned int value);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

    unsigned int const_value;
    QString prefix = "#";
    QString suffix = "";
};

// TMS320C28x 3-bit Product Mode shift bits.
class GAParameterTMS320C28xPM : public GAParameter
{
  public:
    GAParameterTMS320C28xPM(const char *mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

  private:
    QMap<uint8_t, QString> shifts = {
        {0b000, "+1"},
        {0b001, "0"},
        {0b010, "-1"},
        {0b011, "-2"},
        {0b100, "-3"},
        {0b101, "-4"},
        {0b110, "-5"},
        {0b111, "-6"},
    };
};

// TMS320C28x 8-bit Status mode bits.
class GAParameterTMS320C28xStatus : public GAParameter
{
  public:
    GAParameterTMS320C28xStatus(const char *mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

  private:
    QMap<uint8_t, QString> status = {
        {0x01, "sxm"},
        {0x02, "ovm"},
        {0x04, "tc"},
        {0x08, "c"},
        {0x10, "intm"},
        {0x20, "dbgm"},
        {0x40, "page0"},
        {0x80, "vmap"},
    };
};

// TMS320C28x 8-bit Condition code bits.
class GAParameterTMS320C28xCondition : public GAParameter
{
  public:
    GAParameterTMS320C28xCondition(const char *mask, bool reduced);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang, uint64_t adr, QByteArray &bytes, GAParserOperand op, int inslen) override;

  private:
    QStringList conds = {
        "neq", "eq", "gt", "geq", "lt", "leq", "hi", "his", "lo", "los", "nov", "ov", "ntc", "tc", "nbio", "unc"};
    QStringList conds_alt = {
        "neq", "eq", "gt", "geq", "lt", "leq", "hi", "c", "nc", "los", "nov", "ov", "ntc", "tc", "nbio", "unc"};
    QStringList conds_reduced = {
        "eq", "neq", "tc", "ntc"};

    bool reduced;
};

#endif // GALANGTMS320C28X_H
