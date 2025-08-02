#ifndef GAPARAMETER_H
#define GAPARAMETER_H

/* Thing of a parameter from two sides:
 *
 * From the assembler side, it represents one parameter of the instruction,
 * including a type.  It might be matched as a string (gt, lt, eq flags, for
 * example), or it might be matched as a symbol (RESET, 0xdeadbeef).
 *
 * From the disassembler side, it represents a type that is confined to a
 * particular bitmask.  For example, the bit mask "\x00\xff\xff" might be
 * the big endian 16-bit address that the instruction will jump to.
 *
 * In this project, we define both at the same time.  The GAMnemonic class
 * handles the opcode and mask parts, and we have self tests to ensure that
 * mask bits don't overlap and that all bits are defined.
 */

/* Parameter grammar is based on prefixes:
 *    Naive values, which have no marker.
 * #  Immediate values, meaning exactly that number.
 * %  I/O addresses.
 * @  Absolute values, meaning the number at that address.
 * () Also absolute, but managed by a group instead of a prefix.
 */

#include <QString>
#include <QList>

#include "gaparser.h"

class GAMnemonic;
class GALanguage;
class GAParserOperand;

//Maximum length of an instruction.
#define GAMAXLEN 25

//Basic class.  Constructor doesn't mean much.
class GAParameter{
public:
    GAParameter();
    ~GAParameter();
    GAParameter(const char* mask);
    void setMask(const char* mask);
    QString prefix="", suffix="";
    char mask[GAMAXLEN];
    char opcodemask[GAMAXLEN];
    char invertmask[GAMAXLEN];
    char groupstart='_', groupend='_';


    //Decode bytes to a string.
    virtual QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen);
    //Encode a parser parameter to bytes, which are OR'ed into target.
    virtual void encode(GALanguage *lang,
                        uint64_t adr, QByteArray &bytes,
                        GAParserOperand op,
                        int inslen
                        );
    //Does a parsed parameter match this parameter?
    virtual int match(GAParserOperand *op, int len);

    //Raw decode function, intentionally not virtual.
    int64_t rawdecode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen, bool sign=0);
    //Raw encode function, also intentionally not virtual.
    void rawencode(GALanguage *lang,
                   uint64_t adr, QByteArray &bytes,
                   GAParserOperand op,
                   int inslen,
                   int64_t val
                   );

    //How many bits are in the mask?
    uint64_t rawbitcount(int inslen);

    //Subtracted when encoding to bytes, added when decoding to string.
    int64_t offset=0;      //Not respected by rawdecode and rawencode.
    bool isSigned=false;

    QList<GAParameter*> params; //Set of parameters in a group.  Empty in non-groups.
};


//Simple immediate value.
//ADC #0xnn
class GAParameterImmediate : public GAParameter
{
public:
    GAParameterImmediate(const char* mask, uint64_t offset=0);
};

//ADC @0x25
class GAParameterAbsolute : public GAParameter
{
public:
    GAParameterAbsolute(const char* mask, uint64_t offset=0);
};

//input %0x25
class GAParameterPort : public GAParameter
{
public:
    GAParameterPort(const char* mask);
};

class GAParameterRelative : public GAParameter {
public:
    GAParameterRelative(const char* mask, int offset=0, int multiple=1);
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
    int match(GAParserOperand *op, int len) override;
    uint32_t offset=0;   //Offset from the source address.
    uint32_t multiple=1; //Step size.  Usually 1 for CISC, 4 for RISC.
};

//This is an absolute address.
class GAParameterAddress : public GAParameter {
public:
    GAParameterAddress(const char* mask, int offset=0);
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    int match(GAParserOperand *op, int len) override;
};

//This is an address offset into the current page.
class GAParameterPageAddress : public GAParameter {
public:
    GAParameterPageAddress(const char* mask, uint32_t pagemask);
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    uint32_t pagemask=0;
};


//This represents a group of parameters within a grouping symbol, such as ().
class GAParameterGroup : public GAParameter{
public:
    GAParameterGroup();
    GAParameterGroup(const char symbol);
    //Insert parameters into the group.
    GAParameterGroup* insert(GAParameter* p);
    //Same insertion but as an operator.
    GAParameterGroup* operator<<(GAParameter* p);

    //Decode bytes to a parameter group.
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    //Encode a parser parameter to bytes, which are OR'ed into target.
    void encode(GALanguage *lang,
                        uint64_t adr, QByteArray &bytes,
                        GAParserOperand op,
                        int inslen
                        ) override;


    //Adds an immediate to the definition by just its mask, preceeded by #.
    GAParameterGroup* imm(const char *mask, uint64_t offset=0);   //Unsigned
    GAParameterGroup* simm(const char *mask, uint64_t offset=0);  //Signed

    //Adds an absolute to the definition by just its mask, preceeded by @.
    GAParameterGroup* abs(const char *mask, uint64_t offset=0);
    //Adds a port number by just its mask, preceeded by %.
    GAParameterGroup* port(const char *mask);
    //PC-relative address.
    GAParameterGroup* adr(const char *mask, int offset=0);                 // Absolute address.
    GAParameterGroup* rel(const char *mask, int offset=2, int multiple=1); // Relative address.
    GAParameterGroup* pageadr(const char *mask, uint32_t pagemask);        // Page-offset address.
    //Absolute indexed by X or Y.
    GAParameterGroup* absx(const char *mask);
    GAParameterGroup* absy(const char *mask);
    //Registers
    GAParameterGroup* regname(const QString name,
                              const QString prefix="",
                              const QString suffix=""); //Explicit register name.
    GAParameterGroup* regnameind(const QString name);   //Explicit register name.
    GAParameterGroup* regind(const char *mask);         //Register indirect.
    GAParameterGroup* regdir(const char *mask);         //Register direct.
    GAParameterGroup* bit3(const char *mask);           //3-bit representation of a bit.


    //Group of parameters.
    GAParameterGroup* group(char g);
    int match(GAParserOperand *op, int len) override;

};

//Represents an explicit register name, absolute match.
class GAParameterRegName : public GAParameter{
public:
    GAParameterRegName(const QString name);
    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    QString name="nameless";
    int match(GAParserOperand *op, int len) override;
};

//Represents a register by number, bitfield match.
class GAParameterRegNum : public GAParameter {
public:
    GAParameterRegNum(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

//Represents a bit index by 3 bits.
class GAParameterBitIndex : public GAParameter {
public:
    GAParameterBitIndex(const char* mask);
    int match(GAParserOperand *op, int len) override;

    QString decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen) override;
    void encode(GALanguage *lang,
                uint64_t adr, QByteArray &bytes,
                GAParserOperand op,
                int inslen
                ) override;
};

#endif // GAPARAMETER_H
