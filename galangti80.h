#ifndef GALANGTI8__H
#define GALANGTI8__H

#include "galanguage.h"


/* Module for the TI80 graphing calculator, which uses
 * the Toshiba T6M53.  This instruction set was reverse
 * engineered by Randy.
 *
 * Randy's docs,
 * https://www.ticalc.org/archives/files/fileinfo/442/44236.html
 */

class GALangTI80 : public GALanguage
{
public:
    GALangTI80();
};

#endif // GALANGTI8__H
