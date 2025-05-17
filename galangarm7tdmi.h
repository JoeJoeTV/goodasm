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
    QString regnames[19]={
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

#endif // GALANGARM7TDMI_H
