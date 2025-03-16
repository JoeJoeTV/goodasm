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
class GALanguage;

class GAGraderGrade;

class GAGrader {
public:
    GAGrader();
    QString name="nameless";

    //Definitely override these.
    virtual uint64_t isValid(GoodASM *goodasm)=0;    // Is it real?
    virtual bool isCompatible(GALanguage *lang)=0;   // Is this grader compatible?

    //Optionally override these.
    virtual QString mostValid(GoodASM *goodasm);     // Which lang the most likely?
    virtual GAGraderGrade score(GoodASM *goodasm);   // Score the result in the current language.
    virtual QVector<GAGraderGrade> scores(GoodASM *goodasm); //Score all languages.
};

class GAGraderGrade {
public:
    GAGraderGrade(GAGrader *grader, GALanguage *lang, int64_t score);
    GAGrader *grader=0;
    GALanguage *lang=0;
    int64_t score=0;
};



#endif // GAGRADER_H
