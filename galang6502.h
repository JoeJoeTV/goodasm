#ifndef GALANGS6502_H
#define GALANGS6502_H

/* This represents the 6502 machine language.  Illegal instructions
 * are not yet supported, but the standard ones ought to be complete
 * by now.
 *
 * See tests/6502/nanochess for some Atari examples from
 * Programming Games for the Atari 2600 by Oscar Toledo Jr.
 */

#include "galanguage.h"

class GALang6502 : public GALanguage
{
public:
    GALang6502();
};

#endif // GALANGS6502_H
