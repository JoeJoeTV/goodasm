#ifndef GALISTINGGO_H
#define GALISTINGGO_H

/* This listing driver produces a .go file with a symbol table
 * as an associative array of strings to numbers and an array
 * of the code bytes.
 *
 * See NipperTool for an example:
 * git clone https://github.com/travisgoodspeed/nippertool
 */

#include "galisting.h"

class GAListingGo : public GAListing
{
public:
    GAListingGo();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;
    //Render just one instruction.
    QString render(GAInstruction *ins) override;
    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;
};

#endif // GALISTINGGO_H
