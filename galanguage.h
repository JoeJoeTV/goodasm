#ifndef GALANGUAGE_H
#define GALANGUAGE_H

#include <QSet>
class GAParserOperand;

#include<stdint.h>

class GAInstruction;
class GAMnemonic;
class GoodASM;
class GAParameter;

class GALanguage {
public:
    ~GALanguage();
    void setGoodASM(GoodASM* gasm);
    //Disassemble an instruction from an address.
    GAInstruction dis(uint64_t adr);
    //Insert a mnemonic into the language.
    GAMnemonic* insert(GAMnemonic* mnemonic);
    //Convert a native address to a byte address.
    uint64_t byteAdr(uint64_t adr);
    //Convert a byte address to a native address.
    uint64_t nativeAdr(uint64_t adr);

    //Override these if your platform registers have cool nicknames.
    virtual uint64_t find_reg_by_name(QString name);
    virtual QString find_regname(uint64_t num);
    virtual uint64_t find_bit_by_name(QString name);


    //Raw encode function, used by parameters but not opcodes.
    virtual void rawencode(uint64_t adr, QByteArray &bytes,
                   GAParserOperand op,
                   int inslen,
                   GAParameter *param,
                   int64_t val
                   );
    //Raw decode function, used by parameters but not by opcodes.
    virtual uint64_t rawdecode(GAParameter *param, uint64_t adr,
                               const char *bytes, int inslen);


    //Should these be private?
    QString name="nameless";
    enum {LITTLE, BIG} endian=LITTLE;
    QList<GAMnemonic*> mnemonics; //Set of encodings.
    uint8_t wordsize=1;           //Bytes in a word.
    uint8_t epsilon=1;            //Bytes per address.
    uint8_t align=1;              //Alignment in bytes.
    uint8_t maxbytes=2;           //Longest instruction.
    uint8_t minbytes=1;           //Shortest instruction.
    QStringList regnames;         //Register names are illegal as variables.

    //Options for swapping on load/save.
    bool swapwordonload=false;  //Swaps bytes of wordsize when loading.  PIC16C5x.

    QString cheatSheet(); //Cheat sheet of mnemonic names.

    qreal threshold=0.9;

    //Returns a GAInstruction from a verb and operands.
    GAInstruction match(QString verb, QList<GAParserOperand> ops, uint64_t adr);
    GoodASM* goodasm=0;


    //Matches a known mnemonic to decode, or db for default.
    GAInstruction decode(uint64_t adr, uint32_t &len);            //From project.
    GAInstruction decode(uint64_t adr, char *buf, uint32_t &len); //From another buffer.

};

#endif // GALANGUAGE_H
