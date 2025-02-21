#ifndef GALANGS2000_H
#define GALANGS2000_H

#include "galanguage.h"

/* This implements most of the S2000 and EMZ1001 architectures,
 * but programs are rare and it hasn't been through much testing.
 *
 * Bit numbers are not yet supported, but oughtn't be difficult to add.
 */

class GALangS2000 : public GALanguage
{
public:
    GALangS2000();
};

#endif // GALANGS2000_H
