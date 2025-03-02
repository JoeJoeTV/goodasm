#ifndef GALISTINGGA_H
#define GALISTINGGA_H

#include "galisting.h"

/* This generates a listing for in the form of
 * GoodASM-buildable source code, including directives
 * to allow assembly.  Comments are also brought along.
 *
 * You might use this to reformat your code, or to
 * convert a disassembly listing from another format.
 */

class GAListingGA : public GAListing
{
public:
    GAListingGA();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;

    //Render just one instruction.
    QString render(GAInstruction *ins) override;

    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;
};

#endif // GALISTINGGA_H
