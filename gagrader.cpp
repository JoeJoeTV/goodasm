#include "gagrader.h"
#include "goodasm.h"
#include "galanguage.h"
#include "gamnemonic.h"
#include "gainstruction.h"

GAGrader::GAGrader() {}

GAGraderGrade::GAGraderGrade(GAGrader *grader, GALanguage *lang, int64_t score){
    this->grader=grader;
    this->lang=lang;
    this->score=score;
}

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


// Score the result.
GAGraderGrade GAGrader::score(GoodASM *goodasm){
    bool valid=isValid(goodasm);
    int64_t score=valid?100:0;
    GAGraderGrade grade(this, goodasm->lang, score);
    grade.valid=valid;
    return grade;
}

//Static sorting function to put the highest scores first.
static bool above(GAGraderGrade top, GAGraderGrade bottom){
    return (top.score > bottom.score);
}

//Score all languages.
QVector<GAGraderGrade> GAGrader::scores(GoodASM *goodasm){
    QVector<GAGraderGrade> v;

    //Score each compatible language.
    foreach(auto l, goodasm->languages){
        if(isCompatible(l)){
            goodasm->setLanguage(l->name);
            GAGraderGrade grade=score(goodasm);
            if(grade.score>0) v.append(grade);
        }
    }

    //Sort the scores.
    std::sort(v.begin(), v.end(), above);

    return v;
}
