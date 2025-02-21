#ifndef GALISTINGYARAX_H
#define GALISTINGYARAX_H

#include "galisting.h"


/* Yara-X is a pattern matching framework for malware researcher.
 * It efficiently scans large collections of files for matches
 * to brief hexadecimal strings, and this module produces such
 * matching strings.
 *
 * Later on, we'll need to come up with a syntax to describe
 * which pieces should and should not be blinded.  For now,
 * you will need to do such blinding manually.
 */


class GAListingYaraX : public GAListing
{
public:
    GAListingYaraX();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;
    //Render just one instruction.
    QString render(GAInstruction *ins) override;
    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;
};

#endif // GALISTINGYARAX_H
