#include "gagrader.h"
#include "goodasm.h"
#include "galanguage.h"
#include "gamnemonic.h"
#include "gainstruction.h"

GAGrader::GAGrader() {}

// Which lang the most likely?
QString GAGrader::mostValid(GoodASM *goodasm){
    uint64_t bestscore=0;
    QString bestlang="";
    foreach(auto l, goodasm->languages){
        goodasm->setLanguage(l->name);
        uint64_t score=isValid(goodasm);
        if(score>bestscore){  // Update the best score.
            bestscore=score;
            bestlang=l->name;
        }
    }

    return bestlang;
}
