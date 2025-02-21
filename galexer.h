#ifndef GALEXER_H
#define GALEXER_H

/* GALexer's job is to break apart an input string into tokens,
 * which are defined in gatoken.h.  Its job is not to parse
 * those tokens into instructions; that is done by GAParser.
 *
 * We'll largely be stuck with this class once we need to maintain
 * backward compatibility, so the goal is to make it support
 * a standard grammar for all languages we will ever support
 * from the very beginning.  We'll fuck that up, of course,
 * but it's nice to have goals.
 *
 * nextToken() returns a pointer that is in a fixed buffer, so
 * the token lifetime is limited and it need *NOT* be freed or
 * deleted.
 */

#include <QString>
#include "gatoken.h"

#define TOKENCOUNT 16


class GALexer
{
public:
    QString src;
    GALexer(QString source);
    GAToken* nextToken();
    bool isEOF();
    int pos=0;
    int line=1;
    int nextline=1;

private:
    QChar currChar;
    void nextChar();
    QChar peekChar();
    bool isPunct();
    bool isAlpha();
    bool isNumber();
    bool isNewline();
    bool isSpace();
    void skipSpaces();

    int tokenindex=0;
    GAToken tokens[TOKENCOUNT];
};

#endif // GALEXER_H
