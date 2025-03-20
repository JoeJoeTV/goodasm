#ifndef GAGRADERZ80_H
#define GAGRADERZ80_H

/* This grader identifies Z80 code by looking for opcodes prefixed
 * by DD.  Those opcodes do not appear in SM83, where DD is an illegal
 * opcode that halts the GameBoy.
 *
 * For now, our threshold is 95% of the DD opcodes need to be real.
 * We can adjust this if false positives pop up somewhere.
 *
 * Kind thanks to Olivier Poncet for the suggestion. --Travis
 */

#include "gagrader.h"

class GAGraderZ80 : public GAGrader
{
public:
    GAGraderZ80();
    uint64_t isValid(GoodASM *goodasm) override;    // Is it real?
    bool isCompatible(GALanguage *lang) override;   // Is this grader compatible?
};

#endif // GAGRADERZ80_H
