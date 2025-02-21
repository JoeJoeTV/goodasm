#ifndef GALISTINGC_H
#define GALISTINGC_H

#include "galisting.h"

class GAListingC : public GAListing
{
public:
    GAListingC();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;

    //Render just one instruction.
    QString render(GAInstruction *ins) override;

    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;

};

#endif // GALISTINGC_H
