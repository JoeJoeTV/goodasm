#include "galistingga.h"
#include "goodasm.h"

GAListingGA::GAListingGA() {
    name="goodasm";
}

QString GAListingGA::renderSymbols(GoodASM *goodasm){
    return ";; No symbol table yet.";
}

QString GAListingGA::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(0);
    QString src=".lang "+goodasm->lang->name+"\n";
    if(goodasm->baseaddress!=0)
        src+=QString::asprintf(".org 0x%04x\n", (unsigned int) goodasm->baseaddress);

    //FIXME: Track address, include .org directive


    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        src+=render(&ins);
    }
    return src;
}

QString GAListingGA::render(GAInstruction *instruction){
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

        if(instruction->label.length()>0)
            label=instruction->label+":";
        ins=".dh ";
        for(int i=0; i<instruction->data.length(); i++){
            ins+=QString::asprintf("%02x ", instruction->data[i] & 0xFF);
        }

        return formatSource(label, ins, instruction->comment, "");
        break;
    case GAInstruction::MNEMONIC:
        //Bad things happen if the length and the data disagree.
        assert(instruction->len==instruction->data.length());
        assert(instruction->len);

        //Then the data bytes, if we show them.
        if(instruction->label.length()>0)
            label=instruction->label+":";
        ins=instruction->verb+" "+instruction->params;

        //Finally the instruction and parameters.
        return formatSource(label,
                            ins,
                            instruction->comment,
                            "");
        break;
    }

    return ";;; unimplemented\n";
}
