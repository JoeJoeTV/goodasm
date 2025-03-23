#ifndef GAGRADER8051JMPTABLE_H
#define GAGRADER8051JMPTABLE_H

/* This grader looks for jump tables within 8051
 * code, as a way to confirm that the architecture is 8051.
 * This is usually right when it matches, but it won't match
 * code that doesn't use this technique.
 *
 * In the DirecTV H card, jump tables look like this:
 * mov dptr, #0xDEAD   ; Launcher code.
 * jmp (a, dptr)       ; Do the launch.
 * ...
 * 0xdead: ljmp 0xdead ; Jump table entry, first of many.
 * ...
 */


/* For other ideas, check out 8051Enthusiast's repo of 8051 tools.
 * https://github.com/8051Enthusiast/at51
 */

#include "gagrader.h"

class GAGrader8051JmpTable : public GAGrader
{
public:
    GAGrader8051JmpTable();

    uint64_t isValid(GoodASM *goodasm) override;    // Is it real?
    bool isCompatible(GALanguage *lang) override;   // Is this grader compatible?
};

#endif // GAGRADER8051JMPTABLE_H
