#include "gagradergameboy.h"

#include "gainstruction.h"
#include "gamnemonic.h"
#include "goodasm.h"

GAGraderGameBoy::GAGraderGameBoy() {
    name="gameboy";
}


// Is it real?
uint64_t GAGraderGameBoy::isValid(GoodASM *goodasm){
    /* This checks to see if the image contains the first sixteen bytes
     * of the Nintendo logo, which the BootROM contains and all cartridges
     * are required to contain.
     */

    QByteArray logo;
    logo.append("\xCE\xED\x66\x66\xCC\x0D\x00\x0B\x03\x73\x00\x83\x00\x0C\x00\x0D", 16);
    return goodasm->bytes.contains(logo);
}

// Is this grader compatible?
bool GAGraderGameBoy::isCompatible(GALanguage *lang){
    //Only works for SM83.
    return lang->name=="sm83";
}
