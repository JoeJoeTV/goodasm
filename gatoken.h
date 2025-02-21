#ifndef GATOKEN_H
#define GATOKEN_H

/* The tokenizer goes one word a time, categorizing each string
 * as one of these types.  Some are as short as a byte, but others
 * like a symbol or a quote are longer strings.
 *
 * Tokenization is performed in galexer.c.
 */

#include <QString>

enum GATokenType {
    symbol = 0,
    lparen,      //Begin group.
    rparen,      //End group.
    quote,       //Double-quoted string.
    plus,
    minus,
    at,          //Absolute
    octalthorpe, //Immediate
    percent,     //Port
    slash,       //Inversion
    dot,
    colon,
    comma,       //Separates elements in a group.
    newline,     //End of every line.
    comment,     //Remainder of line doesn't count.
    endoffile    //Nothing left.
};


class GAToken
{
public:
    GAToken();
    GATokenType type;
    QString literal;
};

#endif // GATOKEN_H
