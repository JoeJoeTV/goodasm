#include "galexer.h"
#include "gatoken.h"

#include <QDebug>

GALexer::GALexer(QString source){
    src=source+"\n";
    pos=0;
    nextChar();
}

GAToken* GALexer::nextToken(){
    /* We use a fixed array of pre-allocated tokens so
     * that we don't need to free them on a return.
     */
    GAToken *t=&tokens[tokenindex++%TOKENCOUNT];


    line=nextline;

    skipSpaces();
    t->literal=QString(currChar);

    if(isEOF()){
        t->type=GATokenType::endoffile;
        return t;
    }


    switch(currChar.cell()){
    case '\r': case '\n':
        t->type=GATokenType::newline;
        while(isNewline() || isSpace())
            nextChar();
        break;
    case '+':
        t->type=GATokenType::plus;
        nextChar();
        break;
    case '-':
        t->type=GATokenType::minus;
        nextChar();
        break;
    case '@':
        t->type=GATokenType::at;
        nextChar();
        break;
    case '#':
        t->type=GATokenType::octalthorpe;
        nextChar();
        break;
    case '%':
        t->type=GATokenType::percent;
        nextChar();
        break;
    case '/':
        t->type=GATokenType::slash;
        nextChar();
        break;
    case '(':
        t->type=GATokenType::lparen;
        nextChar();
        break;
    case ')':
        t->type=GATokenType::rparen;
        nextChar();
        break;
    case '.':
        t->type=GATokenType::dot;
        nextChar();
        break;
    case ',':
        t->type=GATokenType::comma;
        nextChar();
        break;
    case ':':
        t->type=GATokenType::colon;
        nextChar();
        break;
    case ';':
        t->type=GATokenType::comment;
        nextChar();
        while(!isNewline()){
            t->literal += currChar;
            nextChar();
        }
        break;
    case '"':
        t->type=GATokenType::quote;
        nextChar();
        t->literal="";
        while(currChar!='"' && currChar!='\n' && !isEOF()){
            //Append each character in the quotation.
            //FIXME: Allow the end quote to be escaped.
            t->literal+=currChar;
            nextChar();
        }
        nextChar(); //Consume the ending quote.
        break;
    default:
        /* We've not matched yet, so what's left is an
         * symbol or a number. */

        //FIXME: Add string literal support here.

        nextChar();
        while(isAlpha() || isNumber() || isPunct()){
            t->literal+=currChar;
            nextChar();
        }
        t->type=GATokenType::symbol;
        break;
    }

    return t;
}



void GALexer::nextChar(){
    assert(pos<=src.length());

    if(pos>=src.length()){
        currChar=QChar(u'\n');
    }else{
        currChar=src[pos];
        //So we know the line number.
        if(currChar=='\n')
            nextline++;
    }


    pos++; //Pos always points one character ahead.
}
QChar GALexer::peekChar(){
    if(pos+1>=src.length())
        return u'\n';
    else
        return src[pos]; //Pos is already one ahead.

}
bool GALexer::isPunct(){
    //Replace this if our punctuation needs differ.
    switch(currChar.cell()){
    case '.':
    case '$':
    case '`':
    case '\'':
    case '_':
        return true;
    default:
        return false;
    }
}
bool GALexer::isAlpha(){
    return currChar.isLetter();
}
bool GALexer::isNumber(){
    return currChar.isNumber();
}
bool GALexer::isNewline(){
    //Don't skip past end.
    if(pos>src.length()) return false;

    return currChar=='\n' || currChar=='\r';
}
bool GALexer::isSpace(){
    //Don't skip past end.
    if(pos>src.length()) return false;

    return currChar==' ' || currChar=='\t';
}
void GALexer::skipSpaces(){
    //Don't skip past end.
    if(pos>src.length()) return;

    //Do skip other spaces.
    while(isSpace()) nextChar();
}
bool GALexer::isEOF(){
    return pos>src.length();
}
