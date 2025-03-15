#ifndef GAGRADERVALIDOPS_H
#define GAGRADERVALIDOPS_H

#include "gagrader.h"

/* This grader looks at which percentage of instructions contain a valid opcode.
 * It's a very simple test to write, but it is absolutely useless on architectures
 * like 8051 that define every opcode.
 */

class GAGraderValidOps : public GAGrader
{
public:
    GAGraderValidOps();

    bool isValid(GoodASM *goodasm) override;        // Is it real?
    bool isCompatible(GoodASM *goodasm) override;   // Is this grader compatible?
private:
    qreal threshold=100.0;

};

#endif // GAGRADERVALIDOPS_H
