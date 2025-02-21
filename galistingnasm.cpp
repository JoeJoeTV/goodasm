#include "galistingnasm.h"
#include "goodasm.h"
#include "gainstruction.h"

GAListingNasm::GAListingNasm() {
    name="nasm";
}

QString GAListingNasm::renderSymbols(GoodASM *goodasm){
    return ";; No symbol table yet.";
}

QString GAListingNasm::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(0);
    QString src="";

    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        src+=render(&ins);
    }
    return src;
}

QString GAListingNasm::render(GAInstruction *instruction){
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
        //Address comes first, if we show it.
        if(gasm->listadr)
            label=QString::asprintf("%04x: ",
                    (int) gasm->lang->nativeAdr(instruction->adr));

        //Then the data bytes.  We always print these when they aren't
        //a known instruction.
        assert(instruction->len==instruction->data.length());

        ins="";

            ins="db ";
            for(int i=0; i<instruction->data.length(); i++){
                ins+=QString::asprintf("0x%02x, ", instruction->data[i] & 0xFF);
            }


        //Finally the instruction and parameters.
        ins+=instruction->override;

        //if(gasm->autocomment && instruction->comment.length()==0)
        //    instruction->comment="; illegal";

        return gasm->formatSource("", ins, "; "+label+instruction->comment);
        break;
    case GAInstruction::MNEMONIC:
        //Bad things happen if the length and the data disagree.
        assert(instruction->len==instruction->data.length());
        assert(instruction->len);

        //Address comes first, if we show it.
        if(gasm->listadr)
            label+=QString::asprintf("%04x: ",
                            (int) gasm->lang->nativeAdr(instruction->adr));

        //Then the data bytes, if we show them.
        ins="db ";
        for(int i=0; i<gasm->lang->maxbytes && i<instruction->len; i++){
            ins+=QString::asprintf("0x%02x, ",
                            instruction->data[i] & 0xFF);
        }

        //Finally the instruction and parameters.
        return formatSource("",
                            ins,
                            "; "+label+""+instruction->verb+" "+instruction->params,
                                  instruction->comment);
        break;
    }

    return ";;; unimplemented\n";
}
