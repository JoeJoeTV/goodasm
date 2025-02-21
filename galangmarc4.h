#ifndef GALANGMARC4_H
#define GALANGMARC4_H

/* This is an early attempt at support for Atmel's MARC4
 * architecture.  Some types of addressing are not yet supported.
 *
 * Here's the only other MARC4 disassembler that I know of:
 * https://github.com/AdamLaurie/marc4dasm
 */

#include "galanguage.h"

class GALangMARC4 : public GALanguage
{
public:
    GALangMARC4();
};

#endif // GALANGMARC4_H
