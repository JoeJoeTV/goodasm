#ifndef GALISTINGNASM_H
#define GALISTINGNASM_H

#include "galisting.h"

/* This produces a NASM script that generates the code
 * in question, while keeping the assembly and comments
 * in the loop.
 *
 * Useful for sharing projects with folks who do not use
 * this assembler.
 */

class GAListingNasm : public GAListing
{
public:
    GAListingNasm();

    //Render the whole program.
    QString render(GoodASM *goodasm) override;

    //Render just one instruction.
    QString render(GAInstruction *ins) override;

    //Render the symbol table.
    QString renderSymbols(GoodASM *goodasm) override;
};

#endif // GALISTINGNASM_H
