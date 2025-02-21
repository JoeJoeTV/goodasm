#ifndef GALISTING_H
#define GALISTING_H

#include <QString>
class GoodASM;
class GAInstruction;

/* Assemblers generally produce "listings" as a text output.
 * These might be a disassembly, or they might be assembly
 * annotated with specific addresses instead of abstract labels.
 *
 * We're intentionally flexible here, because our users will
 * likely want listings that can be directly imported into
 * their development languages.  For example, we might render
 * to valid C, Rust or Golang code while retaining comments.
 *
 * Export to object formats will be abstracted separately.
 * Don't hurt yourself by trying to make this output ELF or
 * COFF files!
 */

class GAListing
{
public:
    GAListing();
    QString name="";

    //Formats a line of source code.
    virtual QString formatSource(QString label, QString code,
                                 QString comment, QString comment2);
    //Returns the working name of the current module without suffixes.
    virtual QString moduleName(GoodASM *goodasm);

    //We might make this virtual in the future.
    QString bitstring(uint8_t b);

    //You must overide these or inherit them from another example.
    virtual QString render(GoodASM *goodasm)=0;
    virtual QString render(GAInstruction *ins)=0;
    virtual QString renderSymbols(GoodASM *goodasm)=0;

    //Called between runs, to clear column measurements.
    void clearStops();


//private:
    //These offsets will grow and shrink as needed.
    int label_len=0;
    int code_len=0;
    int comment_len=0;
};


#endif // GALISTING_H
