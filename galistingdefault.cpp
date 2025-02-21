#include "gainstruction.h"
#include "galistingdefault.h"
#include "goodasm.h"

GAListingDefault::GAListingDefault() {
    name="default";
}

QString GAListingDefault::renderSymbols(GoodASM *goodasm){
    return ";; No symbol table yet.";
}

//Render the whole program.
QString GAListingDefault::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    QString src="";


    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        src+=render(&ins);
    }

    return src;
}

//Render just one instruction.
QString GAListingDefault::render(GAInstruction *instruction){
    // label, instruction, comments
    QString label="";
    QString ins="";
    GoodASM *gasm=instruction->gasm;

    switch(instruction->type){
    case GAInstruction::DIRECTIVE:
        if(instruction->override!="")
            return instruction->override;
        else
            return ";;; Unknown directive.\n";
        break;
    case GAInstruction::COMMENT:
        return ";"+instruction->comment+"\n";
    case GAInstruction::NONE:
        return ";;; No instruction type.\n";
        break;
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
        return gasm->formatSource(label, ins, instruction->comment);
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

        return gasm->formatSource(label, ins, instruction->comment);
        break;
    }

    return ";;; unimplemented\n";
}
