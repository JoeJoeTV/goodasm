#include <QDebug>
#include <QFile>
#include <iostream>

#include "gaparser.h"
#include "gainstruction.h"
#include "galexer.h"

#include "goodasm.h"

#define ENDLINE while(token->type!=GATokenType::newline && token->type!=GATokenType::endoffile) nextToken();




GAParserOperand::GAParserOperand(GoodASM* goodasm, QString prefix, QString value, QString suffix){
    this->goodasm=goodasm;
    this->prefix=prefix;
    this->value=value;
    this->suffix=suffix;
}
GAParserOperand::GAParserOperand(GoodASM* goodasm){
    this->prefix="noprefix";
    this->value="novalue";
    this->suffix="novalue";
    this->goodasm=goodasm;
}

/* This renders a group of operators, hopefully reforming
 * the string that made them.  This is useful in program listings.
 */
QString GAParserOperand::render(){
    if(children.empty())
        return prefix+value+suffix;
        //return "_"+prefix+"."+value+"."+suffix+"_";

    //Because we have parameters, we have to list them individually.
    QString sum="(";  //FIXME: Other group types.
    int count=0;
    foreach(auto op, children){
        if(count++)
            sum+=", ";
        sum+=op.render();
    }
    sum+=")";
    return sum;
}


uint64_t GAParser::hex2uint(QString value, bool *okay){
    return ("0x"+value).toInt(okay, 16);
}

uint64_t GAParser::str2uint(QString value, bool *okay){
    uint64_t val=0;
    QString v=value;
    bool neg=false;

    if(v.startsWith("-")){
        qDebug()<<"Uh oh,"<<value<<"is negative in str2uint.";
    }

    //First start with a literal in base 16 or 10.
    if(value.startsWith("0x"))
        val=value.toInt(okay, 16);
    else if(value.startsWith("0b"))
        val=value.mid(2).toInt(okay, 2);  //FIXME: Crop first two symbols first.
    else
        val=value.toInt(okay, 10);
    return val; //Caller better check okay.
}

int64_t GAParser::str2int(QString value, bool *okay){
    int64_t val=0;
    if(value.startsWith("-"))
        return -str2uint(value.right(value.length()-1), okay);
    else
        return str2uint(value, okay);
}

int64_t GAParserOperand::int64(bool create){
    bool okay;
    int64_t val;

    val=GAParser::str2int(
        (prefix.contains("-")?"-":"")
        +value, &okay);

    if(okay) return val;

    //If not, maybe it's a symbol?
    GASymbol *s=symbol(create);
    if(s){
        //qDebug()<<"Resolved symbol"<<s->name<<"to"<<s->absval;
        return s->absval;
    }

    return -1;
}

uint64_t GAParserOperand::fromhex(){
    bool okay;
    //First try without hex.
    uint64_t val=GAParser::hex2uint(value, &okay);
    if(okay) return val;

    //Then with.
    val=GAParser::str2uint(value, &okay);
    if(okay) return val;

    return -1;
}

uint64_t GAParserOperand::uint64(bool create){
    bool okay;
    uint64_t val=GAParser::str2uint(value, &okay);
    if(okay) return val;

    //If not, maybe it's a symbol?
    GASymbol *s=symbol(create);
    if(s){
        return s->absval;
    }

    //qDebug()<<"Error parsing operand"<<value;
    //Completely normal when operand is a register name.
    return -1;
}


GASymbol *GAParserOperand::symbol(bool create){
    assert(goodasm);

    //Null when the symbol is a register name.
    if(goodasm->lang->regnames.contains(this->value))
        return 0;
    //Null when the symbol looks like a number.
    if(this->value[0].isNumber()) return 0;

    GASymbol *sym=goodasm->symbols.findSymbol(this->value, create);
    return sym; //Null when undefined.
}

GAParser::GAParser(GALexer *lexer, GoodASM *goodasm){
    l=lexer;
    this->goodasm=goodasm;

    //Both the token and the peek.
    nextToken();
    nextToken();
}

/* These functions operate on the tokens, optionally
 * peeking ahead by one token to know what's coming up.
 */

bool GAParser::checkToken(GATokenType type){
    return token->type==type;
}
bool GAParser::checkPeek(GATokenType type){
    return peek->type==type;
}
QString GAParser::match(GATokenType type){
    QString toret=token->literal;

    //TODO Make some sort of a parsing error instead of a crash.
    if(token->type!=type){
        goodasm->error("Token not expected.");
    }
    nextToken();

    return toret;
}
void GAParser::nextToken(){
    token=peek;
    peek=l->nextToken();

    if(token && goodasm->verbose){
        qDebug()<<"Token: "<<token->literal<<"type"<<token->type;
    }
}


/* These functions do the parsing. */
void GAParser::parse(){
    while(token->type!=GATokenType::endoffile){
        //Labels come at beginning, can precede code.
        if(checkPeek(GATokenType::colon))
            label();

        //Code comes in the middle.
        if(checkToken(GATokenType::dot))
            directive();
        else if(checkToken(GATokenType::symbol))
            instruction();
        else if(checkToken(GATokenType::comment))
            precomment(); //Skip comment.
        else if(!checkToken(GATokenType::newline))
            qDebug()<<"Unexpected middle token"<<token->literal;

        //Comments at the end.
        if(checkToken(GATokenType::comment))
            nextToken(); //FIXME -- internally map the comment.

        //And finally a new line.
        match(GATokenType::newline);
    }
}
void GAParser::label(){
    goodasm->symbols.setSymbol(token->literal, goodasm->address());
    labelname=token->literal;  //Save the symbol name for next instruction.
    match(GATokenType::symbol);
    nextToken();
}
void GAParser::directive(){
    nextToken(); //Preceding dot.
    QString directive=match(GATokenType::symbol);

    //FIXME: These should be extendable and parsed like instructions.
    if(directive=="db"){  // Data bytes.
        QByteArray bytes;
        QString comment;
        while(token->type!=GATokenType::newline && token->type!=GATokenType::endoffile){
            if(token->type==GATokenType::symbol){
                GAParserOperand op(goodasm, "", token->literal, "");
                uint8_t byte=op.uint64(true);
                bytes.append(byte);
            }else if(token->type==GATokenType::quote){
                bytes.append(token->literal.toUtf8());
            }else if(token->type==GATokenType::comment){
                comment=token->literal;
            }
            nextToken();
        }
        GAInstruction ins(goodasm, bytes);
        ins.comment=comment;
        if(labelname.length()>0){       //Consume label if it's waiting.
            ins.label=labelname;
            labelname="";
        }
        goodasm->append(ins);
    }else if(directive=="ib"){  // Instruction bytes.
        QByteArray bytes;
        QString comment;
        while(token->type!=GATokenType::newline && token->type!=GATokenType::endoffile){
            if(token->type==GATokenType::symbol){
                GAParserOperand op(goodasm, "", token->literal, "");
                uint8_t byte=op.fromhex();
                bytes.append(byte);
            }else if(token->type==GATokenType::comment){
                comment=token->literal;
            }
            nextToken();
        }
        goodasm->loadFragment(labelname, bytes, comment);
        labelname="";
    }else if(directive=="word"){
        QByteArray bytes;
        QString comment;
        while(token->type!=GATokenType::newline){
            if(token->type==GATokenType::symbol){
                GAParserOperand op(goodasm, "", token->literal, "");
                uint16_t word=op.uint64(true);
                if(goodasm->lang->endian==GALanguage::LITTLE){
                    bytes.append(word & 0xff);
                    bytes.append((word>>8) & 0xff);
                }else{
                    bytes.append((word>>8) & 0xff);
                    bytes.append(word & 0xff);
                }

            }else if(token->type==GATokenType::comment){
                comment=token->literal;
            }
            nextToken();
        }
        GAInstruction ins(goodasm, bytes);
        ins.comment=comment;
        if(labelname.length()>0){       //Consume label if it's waiting.
            ins.label=labelname;
            labelname="";
        }
        goodasm->append(ins);
    }else if(directive=="lang"){
        checkToken(GATokenType::symbol);
        goodasm->setLanguage(token->literal);
        nextToken();
    }else if(directive=="equ" || directive=="def"){
        checkToken(GATokenType::symbol);
        QString name=token->literal;
        nextToken();
        QString value=token->literal;
        nextToken();
        goodasm->symbols.setSymbol(name, value);
    }else if(directive=="cheat"){
        //FIXME: Add these as comments.
        std::cout<<goodasm->lang->cheatSheet().toStdString();
    }else if(directive=="symbols"){
        //FIXME: Comments, not cout.
        std::cout<<goodasm->symbols.exportTable().toStdString();
    }else if(directive=="org"){
        GAParserOperand op(goodasm, "", token->literal, "");
        uint64_t adr=op.uint64(true); //FIXME: Shouldn't be a symbol.
        if(goodasm->baseaddress==0 && goodasm->address()==0){
            // First call sets base address.
            goodasm->baseaddress=adr;
        }else{
            // Second call inserts padding.
            QByteArray bytes;
            int64_t count=adr-goodasm->address();
            //assert(count>0);
            //qDebug()<<"Adding "<<count<<"Bytes.";
            if(count<0){
                qDebug()<<"ERROR: .org directive would move backward"<<count<<"bytes.";
                //FIXME: Mark an error?
            }else{
                for(uint64_t i=0; i<count; i++)
                    bytes.append(0xff); //FIXME: Make configurable.
            }
            GAInstruction ins(goodasm, bytes);
            goodasm->append(ins);
        }
        nextToken(); //Skip the address.
    }else if(directive=="include"){
        QString filename=token->literal;
        goodasm->loadFile(filename);
        while(token->type!=GATokenType::newline)
            nextToken();
    }else if(directive=="incbin"){
        QString filename=token->literal;
        QFile input(filename);
        if(input.open(QFile::ReadOnly)){
            QByteArray bytes=input.readAll();
            goodasm->append(GAInstruction(goodasm,bytes));
        }else{
            qDebug()<<"Unable to import:"<<filename;
        }
        ENDLINE;
    }else{
        goodasm->error("Unknown directive: "+directive);

        //TODO Match the directives instead of skipping to the end.
        ENDLINE;
    }
}
void GAParser::instruction(){
    assert(goodasm);
    assert(goodasm->lang);

    goodasm->line=line=l->line;
    QString verb=match(GATokenType::symbol);  //Consume the verb.
    QList<GAParserOperand> ops;

    if(!checkToken(GATokenType::newline) && !checkToken(GATokenType::comment)){
        ops.append(operand());
        while(checkToken(GATokenType::comma)){
            nextToken();
            ops.append(operand());
        }
    }

    GAInstruction ins=goodasm->lang->match(verb,ops, goodasm->address());
    if(checkToken(GATokenType::comment)){
        ins.comment=token->literal;
        nextToken();
    }

    if(labelname.length()>0){       //Consume label if it's waiting.
        ins.label=labelname;
        labelname="";
    }
    goodasm->append(ins);
}

/* This marks a comment on its own line, that has no bytes attached.
 */
void GAParser::precomment(){
    assert(goodasm);
    assert(goodasm->lang);

    goodasm->line=line=l->line;

    QByteArray empty;
    GAInstruction ins(goodasm, empty);

    if(checkToken(GATokenType::comment)){
        ins.comment=token->literal;
        ins.type=GAInstruction::COMMENT;
        nextToken();
    }
    goodasm->append(ins);
}

GAParserOperand GAParser::operandgroup(){
    /* This returns an operand with a group of objects.  If the
     * first token is a grouping symbol, that's the grouping type.
     * If the first toekn is an operand, then the group has no type,
     * which is appropriate for the outermost group of operands.
     */
    GAParserOperand op(goodasm);

    //If there's a grouping symbol, we wrap the group in it.
    if(checkToken(GATokenType::lparen)){
        op.prefix="(";
        nextToken();
    }

    op.children.append(operand());
    //Just parameters here.
    while(!checkToken(GATokenType::newline)
           && !checkToken(GATokenType::endoffile)
           && !checkToken(GATokenType::newline)
           && !checkToken(GATokenType::comment)
           ){
        if(checkToken(GATokenType::comma)){
            nextToken();
            assert(!checkToken(GATokenType::comma));
            op.children.append(operand());
        }else if(checkToken(GATokenType::rparen)){
            nextToken();
            return op;
        }else{
            //Parsing error here, so keep going and error out with th ebest.

            nextToken();
        }
    }

    qDebug()<<"WARNING: Ending group implicitly on line"<<goodasm->line;

    //Return is like any other operator.
    return op;
}

GAParserOperand GAParser::operand(){
    QString prefix=""; //Prefix before the parameter.
    QString suffix=""; //Suffix after the parameter.

    //Optional prefix comes before the operand.
    if( checkToken(GATokenType::plus) ){
        prefix="+";
        nextToken();
    }else if( checkToken(GATokenType::minus) ){
        prefix="-";
        nextToken();
    }else if( checkToken(GATokenType::at) ){            //Absolute
        prefix="@";
        nextToken();
    }else if( checkToken(GATokenType::octalthorpe) ){   //Immediate.
        prefix="#";
        nextToken();
    }else if( checkToken(GATokenType::percent) ){       //Port
        prefix="%";
        nextToken();
    }else if( checkToken(GATokenType::slash) ){       //Port
        prefix="/";
        nextToken();
    }else if( checkToken(GATokenType::lparen) ){        //New group.
        /* For a group, we parse the whole group and
         * then return it as a GAParserOperandGroup. */
        return operandgroup();
    }else if( checkToken(GATokenType::rparen)) {
        //End group.
        qDebug()<<"Ending group.";
        nextToken();
    }

    //Sometimes we have two prefixes, as in a negative number.
    if( checkToken(GATokenType::minus) ){
        prefix+="-";
        nextToken();
    }

    if( checkToken(GATokenType::symbol)){
        QString value=token->literal;
        nextToken();

        // Sometimes a symbol has a suffix before the next symbol.
        if(token->literal=="+"){
            suffix="+";
            nextToken();
        }else if(token->literal=="-"){
            suffix="-";
            nextToken();
        }


        return GAParserOperand(goodasm, prefix, value, suffix);
    }

    //Empty parameter.
    goodasm->error("No operating type for token: "+token->literal);
    return GAParserOperand(goodasm);
}
