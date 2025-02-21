#ifndef GALANGCHIP8_H
#define GALANGCHIP8_H

/* This chip8 language definition comes from Cowgod's
 * Chip-8 Technical Reference.  Many others seem to define
 * psuedo-C for the opcodes, without also defining
 * assembly like Cowgod does.
 *
 * There are many variants of this thing, and I'm not
 * at all convinced that this list is complete.
 *
 * Some quirks:
 * 1. We use assembly, not psuedo-C, and the latter seems to be standard.
 * 2. Registers begin with "r", not "v".
 * 3. We do not yet support Super8 or other extensions.
 */

#include "galanguage.h"

class GALangChip8 : public GALanguage
{
public:
    GALangChip8();
};

#endif // GALANGCHIP8_H
