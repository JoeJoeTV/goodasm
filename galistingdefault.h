#ifndef GALISTINGDEFAULT_H
#define GALISTINGDEFAULT_H

/* This is the default listing driver, intended to show
 * assembly code back to the user.
 */

#include "galisting.h"

class GAListingDefault : public GAListing
{
public:
    GAListingDefault();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;
    //Render just one instruction.
    QString render(GAInstruction *ins) override;
    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;
};

#endif // GALISTINGDEFAULT_H
