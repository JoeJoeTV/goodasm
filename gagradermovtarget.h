#ifndef GAGRADERMOVTARGET_H
#define GAGRADERMOVTARGET_H

#include "gagrader.h"

/* On architectures with absolute-addressed variables, it's normal
 * for many moves in a row to populate the same global variable.
 * You might see many moves to @0x94, for example.
 *
 * These samples should mostly be within the RAM region, whatever
 * that might be.
 *
 * This test doesn't work yet, but we might enable it if it does.
 */

class GAGraderMovTarget : public GAGrader
{
public:
    GAGraderMovTarget();
    uint64_t isValid(GoodASM *goodasm) override;    // Is it real?
    bool isCompatible(GALanguage *lang) override;   // Is this grader compatible?
};

#endif // GAGRADERMOVTARGET_H
