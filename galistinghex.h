#ifndef GALISTINGHEX_H
#define GALISTINGHEX_H

#include "galisting.h"

class GAListingHex : public GAListing
{
public:
    GAListingHex();

    //You must overide these or inherit them from another example.
    QString render(GoodASM *goodasm) override;
    QString render(GAInstruction *ins) override;
    QString renderSymbols(GoodASM *goodasm) override;

};

#endif // GALISTINGHEX_H
