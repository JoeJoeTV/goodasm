#include "galistingyarax.h"
#include "goodasm.h"
#include "gainstruction.h"

GAListingYaraX::GAListingYaraX() {
    name="yarax";
}


//Symbols not supported for Yara rules.
QString GAListingYaraX::renderSymbols(GoodASM *goodasm){
    return "";
}


QString GAListingYaraX::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(0);
    QString modulename=moduleName(goodasm);
    QString src="// Rendered by GoodASM\n"
                  +renderSymbols(goodasm)+
                  "rule "+modulename+" {\n"+
                  "  strings:\n"+
                  "  $"+modulename+" = {\n"
        ;

    //Assembly comes from source, must match data.
    foreach(auto ins, goodasm->instructions){
        src+=render(&ins);
    }


    src=src+
          "\n"+
          "  }\n"+
          "condition:\n"+
          "  $"+modulename+"\n"+
          "}\n";
    return src;
}

QString GAListingYaraX::render(GAInstruction *instruction){
    // label, instruction, comments
    QString label="";
    QString ins="";
    GoodASM *gasm=instruction->gasm;


    switch(instruction->type){
    case GAInstruction::DIRECTIVE:
        if(instruction->override!="")
            return instruction->override+"\n";
        else
            return "// Unknown directive.\n";
        break;
    case GAInstruction::COMMENT:
        return "//"+instruction->comment+"\n";
    case GAInstruction::NONE:
        return "// No instruction type.\n";
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

        for(int i=0; i<instruction->data.length(); i++){
            ins+=QString::asprintf("%02x ", instruction->data[i] & 0xFF);
        }


        //Finally the instruction and parameters.
        ins+=instruction->override;

        return gasm->formatSource("", ins, "// "+label+instruction->comment);
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
        for(int i=0; i<gasm->lang->maxbytes && i<instruction->len; i++){
            ins+=QString::asprintf("%02x ",
                                     instruction->data[i] & 0xFF);
        }

        //Finally the instruction and parameters.
        return formatSource("",
                            ins,
                            "// "+label+""+instruction->verb+" "+instruction->params,
                            instruction->comment);
        break;
    }

    return "// unimplemented\n";
}
