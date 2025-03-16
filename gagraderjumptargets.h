#ifndef GAGRADERJUMPTARGETS_H
#define GAGRADERJUMPTARGETS_H

#include "gagrader.h"

/* This grader charts the most popular targets of relative and
 * absolute branches, then counts the more popular ones to see
 * if common functions are called.  This should reliably identify
 * larger ROM images from things like smart cards, but it is
 * less reliable for smaller images like the GameBoy boot ROM.
 */

class GAGraderJumpTargets : public GAGrader
{
public:
    GAGraderJumpTargets();

    uint64_t isValid(GoodASM *goodasm) override;        // Is it real?
    bool isCompatible(GoodASM *goodasm) override;   // Is this grader compatible?

private:
    int threshold=5;
};

#endif // GAGRADERJUMPTARGETS_H
