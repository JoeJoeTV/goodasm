#include "galang8086.h"
#include "gamnemonic.h"

//Just to keep things shorter.
#define mnem new GAMnemonic

GALang8086::GALang8086() {
    endian=LITTLE;
    name="8086";
    maxbytes=6;

    //Register names.
    regnames.clear();
    regnames<<"x"<<"y"<<"a"
             <<"al"<<"ah"<<"bl"<<"bh"<<"cl"<<"ch"
             <<"ax"<<"bx"<<"cx";



    //Instructions begin on page 105 with AAA, macros on page 326.

    /* CodeMacroDB AAA
     *   DB 37H
     * EndM
     */
    insert(mnem("aaa", 1, "\x37", "\xff"))
        ->help("ASCII Adjust for Addition in AL, AH.")
        ->example("aaa");
    /* CodeMacro AAD
     *   DW 0AD5H       // Little endian!
     * EndM
     */
    insert(mnem("aad", 2, "\xd5\xa0", "\xff\xff"))
        ->help("ASCII Adjust for Division in AL, HL.")
        ->example("aad");
    insert(mnem("aam", 2, "\xd4\xa0", "\xff\xff"))
        ->help("ASCII Adjust for Multiplication in AL, HL.")
        ->example("aam");
    insert(mnem("aas", 1, "\x3f", "\xff"))
        ->help("ASCII Adjust for Subtraction in AL, HL.")
        ->example("aas");

    // Page 109, ADC Add w/ Carry

    /* CodeMacro Adc dst:Eb,src:Db
     *   Segfix dst
     *   DB 80H
     *   ModRM 2, dst
     *   DB src
     * EndM
     */

    /* CodeMacro Adc dst:Ew,src:Db
     *   Segfix dst
     *   DB 81H
     *   ModRM 2,dst
     *   DW src
     * EndM
     */

    /* CodeMacro Adc dst~Ew,src:Db(-128,127)
     *   Segfix dst
     *   DB 83H
     *   ModRM 2,dst
     *   DB src
     * EndM
     */

    /* CodeMacro Adc dst:Ew,src:Dw
     *   Segfix dst
     *   DB 81H
     *   ModRM 2,dst
     *   DW src
     * EndM
     */

    /* CodeMacro Adc dst:Ab,src:Db
     *   DB 14H
     *   DB src
     * EndM
     */
    insert(mnem("adc", 2, "\x14\x00", "\xff\x00"))
        ->help("Add with carry to accumulator.")
        ->example("adc al, #5")
        ->regname("al")
        ->imm("\x00\xff");

    /* CodeMacro Adc dst:Aw,src:Db
     *   DB 15H
     *   DW src
     * EndM
     */
    //Skipping this because the next one works.

    /* CodeMacro Adc dst:Aw,src:Dw
     *   DB 15H
     *   DW src
     * EndM
     */
    insert(mnem("adc", 3, "\x15\x00\x00", "\xff\x00\x00"))
        ->help("Add with carry to accumulator.")
        ->example("adc ax, #0x400")
        ->regname("ax")
        ->imm("\x00\xff\xff");

    /* CodeMacro Adc dst:Eb,src:Rb
     *   Segfix dst
     *   DB 10H
     *   ModRM src,dst
     * EndM
     */

    /* CodeMacro Adc dst:Ew,src:Rw
     *   Segfix dst
     *   DB 11H
     *   ModRM src,dst
     * EndM
     */

    /* CodeMacro Adc dst:Rb,src:Eb
     *   Segfix src
     *   DB 12H
     *   ModRM dst,src
     * EndM
     */

    /* CodeMacro Adc dst:Rw,src:Ew
     *   Segfix src
     *   DB 13H
     *   ModRM dst,src
     * EndM
     */

}
