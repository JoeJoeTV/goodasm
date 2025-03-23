#ifndef GAGRADER8051PUSHPOP_H
#define GAGRADER8051PUSHPOP_H

#include "gainstruction.h"
#include "gamnemonic.h"
#include "goodasm.h"

#include <QDebug>

#include "gagrader.h"

/* This grader looks for an 8051 function that pushes
 * things to the stack before popping them in the reverse
 * order.
 *
 * Currently it requires at least two such functions,
 * with at least two items to the stack.
 */

class GAGrader8051PushPop : public GAGrader
{
public:
    GAGrader8051PushPop();

    uint64_t isValid(GoodASM *goodasm) override;    // Is it real?
    bool isCompatible(GALanguage *lang) override;   // Is this grader compatible?
};

#endif // GAGRADER8051PUSHPOP_H
