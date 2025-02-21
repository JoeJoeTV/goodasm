#ifndef GALISTINGMARKDOWN_H
#define GALISTINGMARKDOWN_H

#include "galisting.h"

/* This listing plugin produces Markdown pages for display on
 * websites or QtQuick.
 */

class GAListingMarkdown : public GAListing
{
public:
    GAListingMarkdown();
    //Formats a line of source code.
    QString formatSource(QString label, QString code,
                         QString comment, QString comment2) override;
    //Render the whole program.
    QString render(GoodASM *goodasm) override;
    //Render just one instruction.
    QString render(GAInstruction *ins) override;
    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;

};

#endif // GALISTINGMARKDOWN_H
