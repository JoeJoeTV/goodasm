#ifndef GAPARSER_H
#define GAPARSER_H

/* GAParser's job is to translate a string of assembly directives
 * into parsed mnemonics.  Expect some pretty extreme refactoring
 * as the first few languages are supported, but it's an explicit
 * goal that we *not* extend the GAParser class except in extremely
 * strange cases.
 */

class GALexer;
class GAToken;
class GALanguage;
class GoodASM;
class GASymbol;

#include <QVector>
#include "gatoken.h"

/* The parser is responsible for iterating over input token produced by the
 * lexer and using them to create instructions, which produce the assembled
 * bytes of the application.
 *
 * Labels and directives are also created by the parser, and it is never
 * quite clear whether it is running as a batch or interactively
 * with GNU Readline.
 */


class GAParserOperand {
public:
    GAParserOperand(GoodASM* goodasm);
    GAParserOperand(GoodASM* goodasm, QString prefix, QString value, QString suffix);
    QString prefix;
    QString value;
    QString suffix;
    int64_t int64(bool create);
    uint64_t uint64(bool create);
    uint64_t fromhex();
    GASymbol *symbol(bool create);
    GoodASM* goodasm=0;
    QVector<GAParserOperand> children;
    QString render(); //Renders back to string.
};


class GAParser{
public:
    GAParser(GALexer *lexer, GoodASM *goodasm);

    //These work on the tokens.
    bool checkToken(GATokenType type);
    bool checkPeek(GATokenType type);
    QString match(GATokenType type);
    void nextToken();

    //These react to the meaning of the tokens.
    void parse();
    void label();
    void directive();
    void instruction();
    void precomment();
    GAParserOperand operand();
    GAParserOperand operandgroup();

    int line=0;

    //Convenience functions.
    static uint64_t str2uint(QString value, bool *okay);
    static uint64_t hex2uint(QString value, bool *okay);
    static int64_t str2int(QString value, bool *okay);
private:
    GALexer* l=0;
    GoodASM* goodasm=0;
    GAToken* token=0;     //Current token.
    GAToken* peek=0;      //Next token in line.
    QString labelname=""; //Last label, if not yet rendered.
};

#endif // GAPARSER_H
