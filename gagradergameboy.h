#ifndef GAGRADERGAMEBOY_H
#define GAGRADERGAMEBOY_H

#include "gagrader.h"

/* This identifies GameBoy ROMs and cartridges
 * by looking for the first sixteen bytes of the Nintendo
 * logo.
 */

class GAGraderGameBoy : public GAGrader
{
public:
    GAGraderGameBoy();

    uint64_t isValid(GoodASM *goodasm) override;    // Is it real?
    bool isCompatible(GALanguage *lang) override;   // Is this grader compatible?
};

#endif // GAGRADERGAMEBOY_H
