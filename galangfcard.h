#ifndef GALANGFCARD_H
#define GALANGFCARD_H

/* DirecTV's F-Card is a Motorola 68HC05 with its opcodes scrambled
 * and one bit flipped.  This is performed on the input to the
 * instruction fetch latch, so it applies to the opcode bytes but not
 * to the parameter bytes.
 *
 * From tv-crypt in 1995, Chris Gerlinsky found this explanation:
 * "Only the op-codes are scrambled, not the operands. The bit order
 * (MSB to LSB) is 76453012. Then the opcode is xored with 10000000b"
 *
 * We'll implement this by inheriting the standard 6805 instructions,
 * then performing the flips and reordering in the constructor after
 * calling the patent's 6805 constructor.
 *
 * We also override the rawencode and rawdecode methods, which is necessary
 * for those few opcodes that contain a bitfield parameter.
 */

#include "galang6805.h"

class GALangFCard : public GALang6805
{
public:
    GALangFCard();

    //Raw encode function, used by parameters but not opcodes.
    void rawencode(uint64_t adr, QByteArray &bytes,
                   GAParserOperand op,
                   int inslen,
                   GAParameter *param,
                   int64_t vals
                   ) override;
    //Raw decode function, used by parameters but not by opcodes.
    uint64_t rawdecode(GAParameter *param, uint64_t adr,
                       const char *bytes, int inslen) override;

private:
    //Helpfully also an unscrambling function!
    uint8_t scramble(uint8_t b);
};

#endif // GALANGFCARD_H
