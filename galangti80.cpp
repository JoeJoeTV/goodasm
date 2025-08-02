#include "gamnemonic.h"
#include "galangti80.h"

/* Module for the TI80 graphing calculator, which uses
 * the Toshiba T6M53.  This instruction set was reverse
 * engineered by Randy.
 *
 * Randy Compton's docs,
 * https://www.ticalc.org/archives/files/fileinfo/442/44236.html
 */


//Just to keep things shorter.
#define mnem new GAMnemonic

//9-bit address file with an offset of 0x100.
#define ram9(x) insert(new GAParameterTI80Ram9((x)))

GALangTI80::GALangTI80() {
    GAMnemonic* m;
    endian=BIG;   //Code is big endian byte pairs, data registers little endian.
    name="ti80";
    maxbytes=2;

    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        "a"<<
        "sp"<<
        "pc"<<
        "dp"<<
        "i"<<// (102)(101)(100)
        "lr" // (107)(106)(105)(104)
        ;

    /* Instruction orders come from T6M53A.TXT in Randy's documentation
     * of 24 September 2021.
     */



/*

OPCODE BIT FIELD TYPES:
A: ARITHMETIC MODE {BINARY,BCD}
C: COMPARISON (TOGETHER WITH LSB OF M) {=,≠,<,≥}
D: DIRECTION (SWAP ARGUMENTS)
F: FUNCTION
M: MODE {PLAIN,JUMP/CALL,IF NO CARRY: JUMP/CALL,IF CARRY: JUMP/CALL}
S: OPERAND SIZE {B,N}
(OTHERS ARE PLAIN NUMERIC VALUES)

  JUMP/CALL MEANS READ ANOTHER WORD & JUMP TO (MSB CLEAR) OR CALL (MSB SET) THE ADDRESS SPECIFIED IN THE LOWER 15 BITS.
  CALL DOES <SP+>=PC BEFORE JUMPING.
  ADD & SUB KEEP TRACK OF CARRIES WHEN REPEATED.
*/

//0000 000S 0000 01FF {READ,WRITE}{U,D}.S (LOAD (I),[DP{+,-}]/LOAD [DP{+,-}],(I))

//0000 0000 0000 1000 (NOP?) (USES LOW PAGE OF REGISTER FILE)
    insert(mnem("nop1", 2, "\x00\x08", "\xff\xff"))
        ->help("No Operation.")
        ->example("nop1");

//0000 0000 0001 0000 (NOP?) (USES LOW PAGE OF REGISTER FILE)
    insert(mnem("nop2", 2, "\x00\x10", "\xff\xff"))
        ->help("No Operation.")
        ->example("nop2");

//0000 0000 0001 1000 SHR A (SHR A,2 IF COMBINED WITH READU/READD & DP<4000)
    insert(mnem("shr", 2, "\x00\x18", "\xff\xff"))
        ->help("Shift Right Accumulator")
        ->example("shr a")
        ->regname("a");
//0000 0000 0010 0000 SCANKEYS (ACTIVATE COLUMNS IN (112) & STORE ROWS TO (114))
    insert(mnem("scankeys", 2, "\x00\x20", "\xff\xff"))
        ->help("Activate columns in 112, store rows to 114.")
        ->example("scankeys");

//0000 0000 0100 0000 RET (NOP IF SP=0, ELSE PC=<-SP> (0000 IF SP>8))
    insert(mnem("ret", 2, "\x00\x40", "\xff\xff"))
        ->help("NOP IF SP=0, ELSE PC=<-SP> (0000 IF SP>8).")
        ->example("ret");
//0000 0000 1000 0000 JUMP (104) (IF COMBINED WITH RET, THE RET WINS)
    insert(mnem("jmp", 2, "\x00\x80", "\xff\xff"))
        ->help("JUMP (104) (IF COMBINED WITH RET, THE RET WINS).")
        ->example("jmp @lr")
        ->regnameind("lr");
//0000 000S VWXY YZZZ (COMBINATION OF THE ABOVE)
    //How the hell do I add support combos? --Travis
//0000 001S XXXX YYYY LOAD.S (JYX),(I)
//0000 01FS XXXX YYYY {LOAD,XCHG}.S (I),(JYX)
//0000 10MX XXXX XXXX LOAD I,XXX
//000F 110S XXXX XXXX LOAD {(I+),A},XX
//000X 111X XXXX YYYY LOAD (1XX),Y
//0001 DFWS XXXX YYYY {LOAD,XCHG}.S A,(WYX) (USES 1FX=A)
//001F FBWB XXXX YYYY {TOGGLE,CLEAR,SET} (WYX).B
//0011 10WS XXXX YYYY NOT.S (WYX)
//0011 11FS XXXX YYYY {XOR,OR}.S (I),(JYX)
//010X CMMX XXXX YYYY (1XX) C Y
//011F CMMS XXXX XXXX {(I+),A} C XX
//100F AMMS XXXX YYYY {ADD,SUB}.S (I),(JYX)
//1010 FBWB XXXX YYYY IF {CLEAR,SET} (WYX).B: JUMP/CALL
//1011 CMMS XXXX YYYY (I) C (JYX)
//110X AMMX XXXX YYYY ADD (1XX),Y
    //FIXME: Support A and MM variants.
    insert(mnem("add", 2, "\xC0\x00", "\xEE\x00"))
        ->help("ADD (1XX),Y")
        ->example("add 0x110, #0xf")
        ->adr("\x11\xf0", -0x100)  // 10-bit address.  FIXME offset.
        ->imm("\x00\x0f")  // 4-bit immediate.
        ;
//111F AMMS XXXX XXXX ADD {(I),A},XX


}
