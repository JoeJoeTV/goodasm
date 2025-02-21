#ifndef GALANG6805_H
#define GALANG6805_H

/* This 6805 definition comes from Motorola's HC05 MC68HC05P9
 * Technical Data book.  The four pages are in Table 3-12.
 *
 * https://github.com/travisgoodspeed/goodasm/issues/73
 */

#include "galanguage.h"

class GALang6805 : public GALanguage
{
public:
    GALang6805();
};

#endif // GALANG6805_H
