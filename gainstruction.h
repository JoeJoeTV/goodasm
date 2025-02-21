#ifndef GAINSTRUCTION_H
#define GAINSTRUCTION_H

#include<QString>

#include<stdint.h>

/* This represents one instruction decoded from a binary or built
 * from source code.  It's how the program is walked to produce a
 * listing.
 */

class GoodASM;
class GALanguage;
class GAMnemonic;
class GAParser;

class GAInstruction {
    friend class GAMnemonic;
    friend class GoodASM;
    friend class GAParser;
public:
    GAInstruction(GoodASM *gasm);
    GAInstruction(GoodASM *gasm, QString str);
    GAInstruction(GoodASM *gasm, uint8_t byte);     //Single byte.
    GAInstruction(GoodASM *gasm, QByteArray bytes); //Array of data bytes.


    QString source();        //Return source code disassembly.
    QString sourceNasm();    //Return source code disassembly in NASM format.
    QByteArray code();       //Return binary machine code.

    uint32_t len=-1;         //Length in bytes.
    uint64_t adr=-1;         //Start address.
    QString label="";        //pre-instruction label.
    QString verb="unset";    //Mnemonic string.
    GAMnemonic *mnem=0;      //Mnemonic pointer from the language parsing.
    QString params="unset";  //Parameter strings.
    QString helpstr="";      //Help string.

    //Next instruction.
    GAInstruction next();

    enum {NONE, MNEMONIC, DIRECTIVE, DATA, COMMENT} type=NONE;

    QString comment="";  //Comment after bytes.
    QString override=""; //Overrides string.
    GoodASM* gasm=0;     //Pointer to the local context.
    QByteArray data;     //Bytes of the instruction, or data bytes.
private:
    GALanguage* lang=0;
    QString bitstring(uint8_t b);
};

#endif // GAINSTRUCTION_H
