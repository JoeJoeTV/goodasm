#ifndef GAGRADER_H
#define GAGRADER_H

#include <QString>

/* A grader's job is to take a disassembly and guess at how likely
 * it is to be real code.  There are many different strategies for
 * this, so we extend a virtual class and keep some notes about
 * which architectures are or are not compatible.
 *
 * https://github.com/travisgoodspeed/goodasm/issues/4
 */

class GoodASM;

class GAGrader
{
public:
    GAGrader();
    QString name="nameless";
    virtual bool isValid(GoodASM *goodasm)=0;        // Is it real?
    virtual bool isCompatible(GoodASM *goodasm)=0;   // Is this grader compatible?
};

#endif // GAGRADER_H
