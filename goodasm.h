#ifndef GOODASM_H
#define GOODASM_H

/* This is the main class, which repesents an assembler and disassembler
 * for all supported languages.  It holds a symbol array and convenience
 * functions for loading code or binaries.
 */

#include <QString>
#include <QByteArray>
#include <QList>
#include <QStack>

#include "gagrader.h"
#include "galanguage.h"
#include "gainstruction.h"
#include "gasymboltable.h"

class GAListing;

class GoodASM {
public:
    GoodASM(GALanguage *language=0);
    GoodASM(QString language);
    ~GoodASM();

    bool setLanguage(GALanguage *language);
    bool setLanguage(QString language);
    void setListing(QString style);
    void setListing(GAListing *style);
    void setGrader(QString grader);
    void setGrader(GAGrader *grader);
    QString opcodeTable();                    //Opcode table for active language.

    GASymbolTable symbols;

    QString source();              //Text input or disassembly.
    QString formatSource(QString label, QString code,
                         QString comment, QString comment2="");
    QByteArray code();             //Binary input or assembly.
    QString hexdump();             //Code in hex.

    void load(QByteArray bytes);        //Load a binary for disassembly.
    void loadDamage(QByteArray bytes);  //Load a binary for disassembly.

    //Disassemble bytes and append them to the working project.
    void loadFragment(QString label, QByteArray bytes, QString comment);
    void load(QString source);      //Load source for assembly.
    void loadFile(QString file);    //Loads a filename for assembly.
    void loadBinFile(QString file); //Loads a filename for disassembly.
    void loadDamageFile(QString file); //Loads a filename for disassembly.
    void clear(bool symbols=false); //Clears all data or instructions.


    QVector<GAGraderGrade> identify(); //Identifies potential languages for a binary.
    QVector<GAGraderGrade> grade();    //Grades in the current language.



    void append(GAInstruction ins); //Insert the next instruction.

    uint8_t byteAt(uint64_t adr);   //Grab a byte from an address.
    uint8_t damageAt(uint64_t adr); //Grab a damage mask from an address.
    GAInstruction at(uint64_t adr); //Grab an instruction from an address.
    uint64_t address();             //Calculated current working address.
    uint64_t baseaddress=0;         //Base address of the image.

    QString addr2line(int64_t adr=-1); //Returns filename and line number.
    void pushFilename(QString name);   //Pushes filename and line number.
    QString popFilename();             //Pops them back.

    //Error handling.
    void error(QString message);       //Logs an error at current file and line.
    QVector<QString> errors;           //Vector of error messages from latest run.
    int printErrors();                 //Prints all errors collected since the last run.
    void clearErrors();                //Clear errors between rows.

    //Self tests for sanity and consistency.
    bool selftest_all();        // All the self testing.
    bool selftest_examples();   // Do the examples line up?
    bool selftest_collisions(); // Are there collisions?
    bool selftest_fuzz();       // Do disassemblies ever violate assertions?
    bool selftest_overlap();    // Does a parameter's mask overlap with the opcode?
    bool selftest_length();     // Are the mask strings null-terminated?

    //Should these be private?
    GALanguage *lang=0;
    GAListing *listing=0;
    GAGrader *grader=0;
    int listbytes=-1;    //Number of bytes to show in listing.
    int listadr=1;       //Include the address in the listing.
    int autocomment=0;   //Autogen comments when none is available.
    int verbose=0;       //Useful for printf debugging.
    bool listbits=false; //Shows bits instead of bytes.
    bool listdbits=false;//Shows damaged bits.
    int line=0;          //Line number.
    QString filename=""; //Filename.
    QStack<int> filenamelines;
    QStack<QString> filenames;
    int duplicates=0;    //Count of instructions that match both ways.

    //New complication array, for REPLXX.
    QVector<QString> completions(QString line);

    QVector<QString> languageNames();

    //Formerly private.
    QByteArray bytes;                    //Byte representation of program.
    QByteArray damage;                   //Byte representation of damage.  No damage if empty.
    QList<GAInstruction> instructions;   //Source listing of the program.
    enum {NONE, ASSEMBLY, DISASSEMBLY} type=NONE;


    //Available languages and listing drivers.
    QVector<GALanguage*> languages;
    QVector<GAListing*> listings;
    QVector<GAGrader*> graders;

private:
    uint64_t workingadr=0;  //Working address for in-progress parsing.


};


#endif // GOODASM_H
