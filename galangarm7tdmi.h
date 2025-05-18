#ifndef GALANGARM7TDMI_H
#define GALANGARM7TDMI_H

#include "galanguage.h"
#include "gaparameter.h"
#include "gamnemonic.h"

/* ARM7TDMI is the 32-bit instruction set casually
 * known as ARM32.
 *
 * Unique features:
 * Variable length Thumb or Thumb2 instruction set when PC is odd.
 * 4-bit condition code for each instruction, most significant nybble.
 * Little and Big endian modes.
 *
 * Identification:
 * Most significant nybble is most often E to always execute an extraction.
 * Most significant nybble is never F, which is undefined.
 * Many instructions are undefined.
 *
 *
 * We subclass GAMnemonic to handle condition codes, which apply to all
 * instructions on this architecture.  GAParameterARM7TDMIReg represents
 * a register, preferring to use sp/lr/pc to r13/r14/15,
 * but accepting either.
 */


//Main language class for ARM7TDMI.
class GALangARM7TDMI : public GALanguage {
public:
    GALangARM7TDMI();

    //MVN and MOV ignore Op1.
    QString dataopcodes[16]={
        "and", "eor", "sub", "rsb",
        "add", "adc", "sbc", "rsc",
        "tst", "teq", "cmp", "cmn",
        "orr", "mov", "bic", "mvn"
    };
    QString datahelp[16]={
        "AND",
        "XOR",
        "Subtract",
        "Reversed Subtract",
        "Add",
        "Add w/ Carry",
        "Subtract w/ Carry",
        "Reverse Subtract w/ Carry",
        "AND, but result is not written.",
        "EOR, but result is not written.",
        "SUB, but result is not written.",
        "ADD, but result is not written.",
        "OR",
        "Move",   //Ignores Op1!
        "Bit Clear",
        "Not"     //Ignores Op1!
    };

};


/* ARM7TDMI instructions have an optional condition code,
 * which comes after the verb name.  We have to subclass GAMnemonic
 * to implement it.
 */
class GAMnemonicARM7TDMI : public GAMnemonic {
public:
    //Simple constructor.
    GAMnemonicARM7TDMI(QString mnemonic,
                       uint32_t length, //Always 4.
                       const char *opcode,

                       //Optional arguments default to null.
                       const char *opcodemask=0,
                       const char *invertmask=0);


    //Does the Mnemonic match bytes?  If so, decode it.
    int match(GAInstruction &ins, uint64_t adr, uint32_t &len,
              const char *bytes) override;
    //Does the Mnemonic match parameters?  If so, encode it.
    int match(GAInstruction &ins, uint64_t adr, QString verb,
              QList<GAParserOperand> ops) override;

    //Used for arm condition codes.
    QString conditionString="";
    uint8_t conditionCode=0xE;
    QString conditions[16]{
        "eq", "ne", "cs", "cc",
        "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt",
        "gt", "le", "al", "illegal"
    };
};

//Represents an ARM register.
class GAParameterARM7TDMIReg : public GAParameter {
public:
    GAParameterARM7TDMIReg(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    const QString regnames[19]={
        "r0", "r1", "r2", "r3",
        "r4", "r5", "r6", "r7",
        "r8", "r9", "r10", "r11",
        "r12",
        "sp", //r13
        "lr", //r14
        "pc", //r15
        "r13", "r14", "r15"
    };
};

/* Represents a shifted (non-immediate) alteration of Op2 of a data operation.
 * This does not include the op2 source register.  That is a separate field.
 * See section 4.5.2.
 */
class GAParameterARM7TDMIShift : public GAParameter {
public:
    GAParameterARM7TDMIShift(const char* mask="\xf0\x0f\x00\x00");
    int match(GAParserOperand *op, int len) override;
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
private:
    uint8_t shiftregistermode=0;  //Register or amount?
    uint8_t shifttype=0xff;       //Index in set below.
    uint8_t shiftregister=0xff;   //Which register?
    uint8_t shiftamount=0xff;     //5-bit amount.
    const QString names[4]={
        "lsl", "lsr", "asr", "ror"
    };
    const QString regnames[19]={
        "r0", "r1", "r2", "r3",
        "r4", "r5", "r6", "r7",
        "r8", "r9", "r10", "r11",
        "r12",
        "sp", //r13
        "lr", //r14
        "pc", //r15
        "r13", "r14", "r15"
    };
};


/* Represents a rotated immediate from an ARM7TDMI data processing instruction.
 * The low 8 bits represent the immediate value, and the upper 4 bits are the
 * rotation.
 *
 * The rotation is in multiples of two bits.  Is it applied by rotating the base
 * to the right.
 */
class GAParameterARM7TDMIImmediate : public GAParameter
{
public:
    GAParameterARM7TDMIImmediate(const char* mask="\xff\x0f\x00\x00");
    int match(GAParserOperand *op, int len) override;
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;

    uint32_t dec(uint64_t base, uint32_t rotate);
    bool enc(uint32_t val, uint64_t *base, uint32_t *rotate);
};

#endif // GALANGARM7TDMI_H
