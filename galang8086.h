#ifndef GALANG8086_H
#define GALANG8086_H

#include "galanguage.h"
#include "gamnemonic.h"

/* This is the barest sketch of an X86 module.  X86 is complicated
 * by prefix bytes (segment override, repeat) and the ModRM byte,
 * which sits in the middle of many instructions and has variable-length
 * parameters between it and the later parameters.
 *
 * Low priority, as X86 folks have Nasm and Yasm.
 */

/* This implements the original 8086 instruction set,
 * which is also used for the 8088.  It does not suport
 * any of the later extensions, and it sure as hell can't
 * do 32-bit code.
 *
 * ASM86 Language Reference Manual
 * https://opencourses.ionio.gr/modules/document/file.php/DAVA150/8086%20instruction%20set.pdf
 *
 * Page 326 describes "code macros", defining all instructions.  Direct
 * translation would be nice.
 */

class GALang8086 : public GALanguage
{
public:
    GALang8086();
};

/* X86 registers are defined by size, either 16-bit if W=1 or
 * 8-bit if W=0.  When no W bit is available, the mode is 16-bit.
 *
 * 16-bit registers are AX, CX, DX, BX, SP, BP, SI, DI.
 * 8-bit registers are AL, CL, DL, BL, AH, CH, DH, BH.
 */


/* X86 modes, registers and memory are defined by the "ModRM byte"
 * defined on pages 85 and 86 of the doc.  A macro producing it is
 * described on page 314.
 *
 * Mod Reg RM
 *  .. ... ...
 *
 * https://wiki.osdev.org/X86-64_Instruction_Encoding#ModR.2FM_and_SIB_bytes
 *
 * Of particular frustration is that the ModRM field has variable
 * length, depending on addressing mode!  Everywhere else in GoodASM,
 * we know the instruction's length at encoding, but here we don't, so
 * the convention is to define the remaining bytes afterward and to never
 * really know the instruction length until we use it.
 */


/* X86 supports up to four prefix instruction bytes.  Usually the
 * segment override comes from the gramar of the instruction
 * and the repeat or lock prefixes set in the instruction line
 * before the verb.
 *
 * Segment Override Prefix
 * Repeat Prefix (for strings)
 * Lock Prefix
 */



/* X86 Segment Override Prefixes are explained on page 87.
 * It sits before an instruction to set the segment register
 * of the instruction that follows it.
 * 001reg110 where REG is ES, CS, SS, or DS.
 */

#endif // GALANG8086_H
