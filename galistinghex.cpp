#include "galistinghex.h"
#include "gainstruction.h"
#include "goodasm.h"

GAListingHex::GAListingHex() {
    name="hex";
}


QString GAListingHex::renderSymbols(GoodASM *goodasm){
    return ";; No symbol table yet.";
}

//Render the whole program.
QString GAListingHex::render(GoodASM *goodasm){
    GAInstruction ins=goodasm->at(goodasm->baseaddress);
    QString src="";


    QByteArray bytes=goodasm->bytes;
    uint64_t adr=goodasm->baseaddress;

    foreach(auto B, bytes){
        uint8_t b=B;

        if(adr%16==0 || adr==goodasm->baseaddress){
            if(adr!=goodasm->baseaddress)
                src+="\n";

            if(goodasm->listadr)
                src+=QString::asprintf("%04x:  ", (uint16_t) adr & 0x0fff0);

            if(adr==goodasm->baseaddress){
                int i=0;
                while((adr&0xf)!=i++) src+="   ";
                if((adr&0xf)>0x7) src+="  ";
            }
        }


        if(adr%16==7)
            src+="  ";

        src+=QString::asprintf("%02x ", b);
        adr++;
    }

    return src+"\n";
}

//Render just one instruction.
QString GAListingHex::render(GAInstruction *instruction){
    //We don't really do one at a time here.  Crash to make that obvious.
    assert(0);
    return "";
}
