#include<QDebug>

#include "galistingmarkdown.h"
#include "goodasm.h"

GAListingMarkdown::GAListingMarkdown() {
    name="markdown";
}

QString GAListingMarkdown::renderSymbols(GoodASM *goodasm){
    return ";; No symbol table yet.";
}

//Formats a source line for printing.
QString GAListingMarkdown::formatSource(QString label, QString code,
                                        QString comment, QString comment2){
    /* Every source line beings with a potential label, then some code
     * and finally a comment.  This fuction is largely just concatenating
     * with padding, but we cannot use QString::arg() because of its
     * trouble with format string injection when presented with I/O addresses.
     */

    QString s="| "+label;
    if(s.length()>label_len-2) label_len=s.length()+2;
    while(s.length()<label_len) s+=" ";
    s+="| "+code;
    if(s.length()>code_len-2) code_len=s.length()+2;
    while(s.length()<code_len) s+=" ";
    s+="| "+comment;
    if(s.length()>comment_len-2) comment_len=s.length()+2;
    while(s.length()<comment_len) s+=" ";
    s+="| "+comment2;

    s+="|\n";

    return s;
}


//Render the whole program.
QString GAListingMarkdown::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    QString src=
        formatSource("label","code","c1","c2")
        +formatSource(":--",":--",":--",":--");


    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        src+=render(&ins);
    }

    return src;
}

//Render just one instruction.
QString GAListingMarkdown::render(GAInstruction *instruction){
    // label, instruction, comments
    QString label="";
    QString ins="";
    GoodASM *gasm=instruction->gasm;

    switch(instruction->type){
    case GAInstruction::NONE:
        //return ";;; No instruction type.\n";
        break;
    case GAInstruction::DIRECTIVE:
    case GAInstruction::COMMENT:
    case GAInstruction::DATA:
        //Address comes first.
        if(instruction->label!="")      //Explicit label.
            label+=instruction->label+":";
        else if(gasm->listadr)          //Implied address label.
            label+=QString::asprintf("%04x:",(int) gasm->lang->nativeAdr(instruction->adr));

        //Then the data bytes.  We always print these when they aren't
        //a known instruction.
        for(int i=0; i<instruction->data.length(); i++){
            if(gasm->listbits){
                //List bits, not bytes.
                ins+=bitstring(instruction->data[i])+" ";
            }else if(gasm->listbytes){
                //List bytes, just for visibility.
                ins+=QString::asprintf("%02x ", instruction->data[i] & 0xFF);
            }else if(!gasm->listbytes){
                ins+=QString::asprintf("%02x ", instruction->data[i] & 0xFF);
            }
        }

        //Finally the instruction and parameters.
        ins+=instruction->override;
        if(gasm->autocomment && instruction->comment.length()==0)
            instruction->comment="; illegal";
        return formatSource(label, ins, instruction->comment, "");
        break;
    case GAInstruction::MNEMONIC:
        //Bad things happen if the length and the data disagree.
        assert(instruction->len=instruction->data.length());

        //Address comes first.
        if(instruction->label!="")      //Explicit label.
            label+=instruction->label+":";
        else if(gasm->listadr)          //Implied address label.
            label+=QString::asprintf("%04x:",(int) gasm->lang->nativeAdr(instruction->adr));

        //Then the data bytes, if we show them.
        if(gasm->listbytes){
            for(int i=0; i<gasm->lang->maxbytes; i++){
                if(gasm->listbits){
                    ins+=bitstring(instruction->data[i])+" ";
                }else{
                    if(i<instruction->len)
                        ins+=QString::asprintf("%02x ",instruction->data[i] & 0xFF);
                    else
                        ins+="   ";
                }
            }
            ins+="  ";
        }

        //Finally the instruction and parameters.
        ins+=instruction->verb+" "+instruction->params;
        if(gasm->autocomment && instruction->comment.length()==0)
            instruction->comment="; "+instruction->helpstr;

        return formatSource(label, ins, instruction->comment, "");
        break;
    }

    return ";;; unimplemented\n";
}
