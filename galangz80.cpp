#include "gamnemonic.h"
#include "galangz80.h"

#include "QDebug"

//Just to keep things shorter.
#define mnem new GAMnemonic
//8-bit register.  &HL is handled elsewhere by overloads.
#define z80reg8(x) insert(new GAParameterZ80Reg8((x)))
//16-bit register for most instructions.
#define z80reg16(x) insert(new GAParameterZ80Reg16((x),false))
//16-bit register for pp, rr instructions
QString reg16pp[4]={"bc", "de", "ix", "sp"};
QString reg16rr[4]={"bc", "de", "iy", "sp"};
QString reg16ss[4]={"bc", "de", "hl", "sp"};
#define z80reg16pp(x) insert(new GAParameterZ80Reg16((x), reg16pp))
#define z80reg16rr(x) insert(new GAParameterZ80Reg16((x), reg16rr))
#define z80reg16ss(x) insert(new GAParameterZ80Reg16((x), reg16ss))
//16-bit register for push and pop, with AF in place of SP.
#define z80reg16stk(x) insert(new GAParameterZ80Reg16((x),true))
#define z80cond(x) insert(new GAParameterZ80Cond((x)))
#define z80rsthandler(x) insert(new GAParameterZ80ResetHandler((x)))

GALangZ80::GALangZ80() {
    GAMnemonic* m;
    endian=LITTLE;  // Higher bits come in earlier bytes.
    name="z80";
    maxbytes=4;

    //Register names are illegal as symbol names.
    regnames.clear();
    regnames<<
        // 8-bit
        "b" << "c" << "d" << "e" <<
        "f" << "h" << "l" << "a" <<

        // 16-bit
        "hl" << "ix" << "iy" <<
        "bc" << "de" << "sp" <<
        "af" <<

        // Special
        "i" << "r" <<

        // Shadow registers
        "af'" <<

        // Condtion Codes, kinda like regs
        "nz" << "z" << "nc" << "c" <<
        "po" << "pe" << "p" <<"m"
        ;

    //ld r, r'
    //This overlaps with ld r, @hl.  Priority sorts that out.
    insert(mnem("ld", 1, "\x40", "\xc0"))
        ->help("Copies one register's value into another.")
        ->example("ld b, b")
        ->z80reg8("\x38")
        ->z80reg8("\x07");

    //ld r, n
    insert(mnem("ld", 2, "\x06\x00", "\xc7\x00"))
        ->help("Loads a register with an immediate.")
        ->example("ld b, #0xff")
        ->z80reg8("\x38\x00")
        ->imm("\x00\xff");

    //ld r, @hl
    //This overlaps with ld r,r.  Priority sorts it.
    insert(mnem("ld", 1, "\x46", "\xc7"))
        ->help("reg8=mem[HL]")
        ->example("ld b, @hl")
        ->prioritize()          //Resolves conflict with ld r,r'.
        ->z80reg8("\x38")
        ->regnameind("hl");

    //LD r, (IX+d)
    insert(mnem("ld", 3, "\xdd\x46\x00", "\xff\xc7\x00"))
        ->help("reg8=mem[IX+d]")
        ->example("ld b, (ix, -16)")
        ->z80reg8("\x00\x38\x00")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff");

    //LD r, (IY+d)
    insert(mnem("ld", 3, "\xfd\x46\x00", "\xff\xc7\x00"))
        ->help("reg8=mem[IY+d]")
        ->example("ld b, (iy, -16)")
        ->z80reg8("\x00\x38\x00")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff");

    //LD (HL), r
    insert(mnem("ld", 1, "\x70", "\xf8"))
        ->help("mem[HL]=reg8")
        ->example("ld @hl, b")
        ->prioritize()
        ->regnameind("hl")
        ->z80reg8("\x07");

    // Page 81

    //LD (IX+d), r
    GAParameterGroup *g=
        insert(mnem("ld", 3, "\xdd\x70\x00", "\xff\xf8\x00"))
                              ->help("mem[IX+d]=reg8")
                              ->example("ld (ix, -16), b")
                              ->prioritize();
    g->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    g->z80reg8("\x00\x07\x00");

    //LD (IY+d), r
    g=insert(mnem("ld", 3, "\xfd\x70\x00", "\xff\xf8\x00"))
            ->help("mem[IY+d]=reg8")
            ->example("ld (iy, -16), b")
            ->prioritize();
    g->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));
    g->z80reg8("\x00\x07\x00");

    //LD (HL), n
    insert(mnem("ld", 2, "\x36\x00", "\xff\x00"))
        ->help("mem[HL]=imm8")
        ->example("ld @hl, #0xff")
        ->prioritize()
        ->regnameind("hl")
        ->imm("\x00\xff");


    //LD (IX+d), n
    g=insert(mnem("ld", 4, "\xdd\x36\x00\x00", "\xff\xff\x00\x00"))
            ->help("mem[IX+d]=imm8")
            ->example("ld (ix, -16), #0xff");
    g->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff\x00"));
    g->imm("\x00\x00\x00\xff");


    // Page 87

    //LD (IY+d), n
    g=insert(mnem("ld", 4, "\xfd\x36\x00\x00", "\xff\xff\x00\x00"))
            ->help("mem[IY+d]=imm8")
            ->example("ld (iy, -16), #0xff");
    g->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff\x00"));
    g->imm("\x00\x00\x00\xff");

    //LD A, (BC)
    insert(mnem("ld", 1, "\x0a", "\xff"))
        ->help("A=mem[BC]")
        ->example("ld a, @bc")
        ->regname("a")
        ->regnameind("bc");

    //LD A, (DE)
    insert(mnem("ld", 1, "\x1a", "\xff"))
        ->help("A=mem[DE]")
        ->example("ld a, @de")
        ->regname("a")
        ->regnameind("de");

    //LD A, (nn)
    insert(mnem("ld", 3, "\x3a\x00\x00", "\xff\x00\x00"))
        ->help("A=mem[abs]")
        ->example("ld a, @0xffff")
        ->regname("a")
        ->abs("\x00\xff\xff");

    //LD (BC), A
    insert(mnem("ld", 1, "\x02", "\xff"))
        ->help("mem[BC]=A")
        ->example("ld @bc, a")
        ->regnameind("bc")
        ->regname("a");

    //LD (DE), A
    insert(mnem("ld", 1, "\x12", "\xff"))
        ->help("mem[DE]=A")
        ->example("ld @de, a")
        ->regnameind("de")
        ->regname("a");

    //LD (nn), A
    insert(mnem("ld", 3, "\x32\x00\x00", "\xff\x00\x00"))
        ->help("mem[abs]=A")
        ->example("ld @0xffff, a")
        ->abs("\x00\xff\xff")
        ->regname("a");

    //LD A, I
    insert(mnem("ld", 2, "\xed\x57", "\xff\xff"))
        ->help("A=I")
        ->example("ld a, i")
        ->regname("a")
        ->regname("i");
    //LD A, R
    insert(mnem("ld", 2, "\xed\x5f", "\xff\xff"))
        ->help("A=R")
        ->example("ld a, r")
        ->regname("a")
        ->regname("r");


    //LD I, A
    insert(mnem("ld", 2, "\xed\x47", "\xff\xff"))
        ->help("I=A")
        ->example("ld i, a")
        ->regname("i")
        ->regname("a");
    //LD R, A
    insert(mnem("ld", 2, "\xed\x4f", "\xff\xff"))
        ->help("R=A")
        ->example("ld r, a")
        ->regname("r")
        ->regname("a");


    //Page 98: 16-bit Load Group

    //LD dd, nn
    insert(mnem("ld", 3, "\x01\x00\x00", "\xcf\x00\x00"))
        ->help("reg16=imm16")
        ->example("ld bc, #0xffff")
        ->z80reg16("\x30\x00\x00")
        ->imm("\x00\xff\xff");
    //LD IX, nn
    insert(mnem("ld", 4, "\xdd\x21\x00\x00", "\xff\xff\x00\x00"))
        ->help("IX=imm16")
        ->example("ld ix, #0xffff")
        ->regname("ix")
        ->imm("\x00\x00\xff\xff");
    //LD IY, nn
    insert(mnem("ld", 4, "\xfd\x21\x00\x00", "\xff\xff\x00\x00"))
        ->help("IY=imm16")
        ->example("ld iy, #0xffff")
        ->regname("iy")
        ->imm("\x00\x00\xff\xff");


    //LD HL, (nn)
    insert(mnem("ld", 3, "\x2a\x00\x00", "\xff\x00\x00"))
        ->help("HL=mem[abs16]")
        ->example("ld hl, @0xffff")
        ->regname("hl")
        ->abs("\x00\xff\xff");
    //LD dd, (nn)
    insert(mnem("ld", 4, "\xed\x01\x00\x00", "\xff\xcf\x00\x00"))
        ->help("reg16=mem[abs16]")
        ->example("ld bc, @0xffff")
        ->z80reg16("\x00\x30\x00\x00")
        ->abs("\x00\x00\xff\xff");
    //LD IX, (nn)
    insert(mnem("ld", 4, "\xdd\x2a\x00\x00", "\xff\xff\x00\x00"))
        ->help("IX=mem[abs16]")
        ->example("ld ix, @0xffff")
        ->regname("ix")
        ->abs("\x00\x00\xff\xff");
    //LD IY, (nn)
    insert(mnem("ld", 4, "\xfd\x2a\x00\x00", "\xff\xff\x00\x00"))
        ->help("IY=mem[abs16]")
        ->example("ld iy, @0xffff")
        ->regname("iy")
        ->abs("\x00\x00\xff\xff");
    //LD (nn), HL
    insert(mnem("ld", 3, "\x22\x00\x00", "\xff\x00\x00"))
        ->help("mem[imm16]=HL")
        ->example("ld @0xffff, hl")
        ->abs("\x00\xff\xff")
        ->regname("hl");

    //LD (nn), dd
    insert(mnem("ld", 4, "\xed\x43\x00\x00", "\xff\xcf\x00\x00"))
        ->help("reg16=mem[abs16]")
        ->example("ld @0xffff, bc")
        ->abs("\x00\x00\xff\xff")
        ->z80reg16("\x00\x30\x00\x00");

    //LD (nn), IX
    insert(mnem("ld", 4, "\xdd\x22\x00\x00", "\xff\xff\x00\x00"))
        ->help("IX=mem[abs16]")
        ->example("ld @0xffff, ix")
        ->abs("\x00\x00\xff\xff")
        ->regname("ix");
    //LD (nn), IY
    insert(mnem("ld", 4, "\xfd\x22\x00\x00", "\xff\xff\x00\x00"))
        ->help("IY=mem[abs16]")
        ->example("ld @0xffff, iy")
        ->abs("\x00\x00\xff\xff")
        ->regname("iy");
    //LD SP, HL
    insert(mnem("ld", 1, "\xf9", "\xff"))
        ->help("SP=HL")
        ->example("ld sp, hl")
        ->regname("sp")
        ->regname("hl");
    //LD SP, IX
    insert(mnem("ld", 2, "\xdd\xf9", "\xff\xff"))
        ->help("SP=IX")
        ->example("ld sp, ix")
        ->regname("sp")
        ->regname("ix");
    //LD SP, IY
    insert(mnem("ld", 2, "\xfd\xf9", "\xff\xff"))
        ->help("SP=IY")
        ->example("ld sp, iy")
        ->regname("sp")
        ->regname("iy");

    //Page 115

    //PUSH qq
    insert(mnem("push", 1, "\xc5", "\xcf"))
        ->help("PUSH reg16")
        ->example("push af")
        ->z80reg16stk("\x30");
    //PUSH IX
    insert(mnem("push", 2, "\xdd\xe5", "\xff\xff"))
        ->help("PUSH IX")
        ->example("push ix")
        ->regname("ix");
    //PUSH IY
    insert(mnem("push", 2, "\xfd\xe5", "\xff\xff"))
        ->help("PUSH IY")
        ->example("push iy")
        ->regname("iy");

    //POP qq
    insert(mnem("pop", 1, "\xc1", "\xcf"))
        ->help("POP reg16")
        ->example("pop af")
        ->z80reg16stk("\x30");
    //POP IX
    insert(mnem("pop", 2, "\xdd\xe1", "\xff\xff"))
        ->help("POP IX")
        ->example("pop ix")
        ->regname("ix");
    //POP IY
    insert(mnem("pop", 2, "\xfd\xe1", "\xff\xff"))
        ->help("POP IY")
        ->example("pop iy")
        ->regname("iy");


    //Page 123: Exchange, Block Transfer and Search Group

    //EX DE, HL
    insert(mnem("ex", 1, "\xeb", "\xff"))
        ->help("EX DE, HL")
        ->example("ex de, hl")
        ->regname("de")
        ->regname("hl");

    //EX AF, AF'
    insert(mnem("ex", 1, "\x08", "\xff"))
        ->help("EX AF, AF'")
        ->example("ex af, af'")
        ->regname("af")
        ->regname("af'");


    //EXX
    insert(mnem("exx", 1, "\xd9", "\xff"))
        ->help("Exchange BC, DE and HL for BC', DE' and HL'.")
        ->example("exx");

    //EX (SP), HL
    insert(mnem("ex", 1, "\xe3", "\xff"))
        ->help("Exchange HL with mem[SP].")
        ->example("ex @sp, hl")
        ->regnameind("sp")
        ->regname("hl");
    //EX (SP), IX
    insert(mnem("ex", 2, "\xdd\xe3", "\xff\xff"))
        ->help("Exchange IX with mem[SP].")
        ->example("ex @sp, ix")
        ->regnameind("sp")
        ->regname("ix");
    //EX (SP), IY
    insert(mnem("ex", 2, "\xfd\xe3", "\xff\xff"))
        ->help("Exchange IY with mem[SP].")
        ->example("ex @sp, iy")
        ->regnameind("sp")
        ->regname("iy");

    //LDI
    insert(mnem("ldi", 2, "\xed\xa0", "\xff\xff"))
        ->help("@DE=@HL, DE++, HL++, BC--")
        ->example("ldi");
    //LDIR
    insert(mnem("ldir", 2, "\xed\xb0", "\xff\xff"))
        ->help("Repeat {@DE=@HL, DE++, HL++, BC--} while BC!=0.")
        ->example("ldir");
    //LDD
    insert(mnem("ldd", 2, "\xed\xa8", "\xff\xff"))
        ->help("@DE=@HL, DE--, HL--, BC--.")
        ->example("ldd");
    //LDDR
    insert(mnem("lddr", 2, "\xed\xb8", "\xff\xff"))
        ->help("Repeat {@DE=@HL, DE--, HL--, BC--} while BC!=0.")
        ->example("lddr");

    //CPI
    insert(mnem("cpi", 2, "\xed\xa1", "\xff\xff"))
        ->help("A-@HL, HL++, BC--")
        ->example("cpi");
    //CPIR
    insert(mnem("cpir", 2, "\xed\xb1", "\xff\xff"))
        ->help("Repeat {A-@HL, HL++, BC--} while BC>0.")
        ->example("cpir");

    //CPD
    insert(mnem("cpd", 2, "\xed\xa9", "\xff\xff"))
        ->help("A-@HL, HL--, BC--")
        ->example("cpd");
    //CPDR
    insert(mnem("cpdr", 2, "\xed\xb9", "\xff\xff"))
        ->help("Repeat {A-@HL, HL--, BC--} while BC>0.")
        ->example("cpdr");

    //ADD A, r
    insert(mnem("add", 1, "\x80", "\xf8"))
        ->help("A+=reg8")
        ->example("add a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //ADD A, n
    insert(mnem("add", 2, "\xc6\x00", "\xff\x00"))
        ->help("A+=imm8")
        ->example("add a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //ADD A, (HL)
    insert(mnem("add", 1, "\x86", "\xff"))
        ->help("A+=mem[HL]")
        ->example("add a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //ADD A, (IX+d)
    insert(mnem("add", 3, "\xdd\x86\x00", "\xff\xff\x00"))
        ->help("A+=mem[IY+d]")
        ->example("add a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //ADD A, (IY+d)
    insert(mnem("add", 3, "\xfd\x86\x00", "\xff\xff\x00"))
        ->help("A+=mem[IY+d]")
        ->example("add a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    //Page 151:


    //ADC A, r
    insert(mnem("adc", 1, "\x88", "\xf8"))
        ->help("A+=reg8")
        ->example("adc a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //ADC A, n
    insert(mnem("adc", 2, "\xce\x00", "\xff\x00"))
        ->help("A+=imm8")
        ->example("adc a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //ADC A, (HL)
    insert(mnem("adc", 1, "\x8e", "\xff"))
        ->help("A+=mem[HL]")
        ->example("adc a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //ADC A, (IX+d)
    insert(mnem("adc", 3, "\xdd\x8e\x00", "\xff\xff\x00"))
        ->help("A+=mem[IY+d]")
        ->example("adc a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //ADC A, (IY+d)
    insert(mnem("adc", 3, "\xfd\x8e\x00", "\xff\xff\x00"))
        ->help("A+=mem[IY+d]")
        ->example("adc a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));



    //SUB A, r
    insert(mnem("sub", 1, "\x90", "\xf8"))
        ->help("A-=reg8")
        ->example("sub a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //SUB A, n
    insert(mnem("sub", 2, "\xd6\x00", "\xff\x00"))
        ->help("A-=imm8")
        ->example("sub a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //SUB A, (HL)
    insert(mnem("sub", 1, "\x96", "\xff"))
        ->help("A-=mem[HL]")
        ->example("sub a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //SUB A, (IX+d)
    insert(mnem("sub", 3, "\xdd\x96\x00", "\xff\xff\x00"))
        ->help("A-=mem[IY+d]")
        ->example("sub a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //SUB A, (IY+d)
    insert(mnem("sub", 3, "\xfd\x96\x00", "\xff\xff\x00"))
        ->help("A-=mem[IY+d]")
        ->example("sub a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    //SBC A, r
    insert(mnem("sbc", 1, "\x98", "\xf8"))
        ->help("A-=reg8")
        ->example("sbc a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //SBC A, n
    insert(mnem("sbc", 2, "\xde\x00", "\xff\x00"))
        ->help("A-=imm8")
        ->example("sbc a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //SBC A, (HL)
    insert(mnem("sbc", 1, "\x9e", "\xff"))
        ->help("A-=mem[HL]")
        ->example("sbc a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //SBC A, (IX+d)
    insert(mnem("sbc", 3, "\xdd\x9e\x00", "\xff\xff\x00"))
        ->help("A-=mem[IY+d]")
        ->example("sbc a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //SBC A, (IY+d)
    insert(mnem("sbc", 3, "\xfd\x9e\x00", "\xff\xff\x00"))
        ->help("A-=mem[IY+d]")
        ->example("sbc a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    //AND A, r
    insert(mnem("and", 1, "\xa0", "\xf8"))
        ->help("A&=reg8")
        ->example("and a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //AND A, n
    insert(mnem("and", 2, "\xe6\x00", "\xff\x00"))
        ->help("A&=imm8")
        ->example("and a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //AND A, (HL)
    insert(mnem("and", 1, "\xa6", "\xff"))
        ->help("A&=mem[HL]")
        ->example("and a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //AND A, (IX+d)
    insert(mnem("and", 3, "\xdd\xa6\x00", "\xff\xff\x00"))
        ->help("A&=mem[IY+d]")
        ->example("and a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //AND A, (IY+d)
    insert(mnem("and", 3, "\xfd\xa6\x00", "\xff\xff\x00"))
        ->help("A&=mem[IY+d]")
        ->example("and a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    //OR A, r
    insert(mnem("or", 1, "\xb0", "\xf8"))
        ->help("A|=reg8")
        ->example("or a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //OR A, n
    insert(mnem("or", 2, "\xf6\x00", "\xff\x00"))
        ->help("A|=imm8")
        ->example("or a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //OR A, (HL)
    insert(mnem("or", 1, "\xb6", "\xff"))
        ->help("A|=mem[HL]")
        ->example("or a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //OR A, (IX+d)
    insert(mnem("or", 3, "\xdd\xb6\x00", "\xff\xff\x00"))
        ->help("A|=mem[IY+d]")
        ->example("or a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //OR A, (IY+d)
    insert(mnem("or", 3, "\xfd\xb6\x00", "\xff\xff\x00"))
        ->help("A|=mem[IY+d]")
        ->example("or a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));




    //XOR A, r
    insert(mnem("xor", 1, "\xa8", "\xf8"))
        ->help("A^=reg8")
        ->example("xor a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //XOR A, n
    insert(mnem("xor", 2, "\xee\x00", "\xff\x00"))
        ->help("A^=imm8")
        ->example("xor a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //XOR A, (HL)
    insert(mnem("xor", 1, "\xae", "\xff"))
        ->help("A^=mem[HL]")
        ->example("xor a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //XOR A, (IX+d)
    insert(mnem("xor", 3, "\xdd\xae\x00", "\xff\xff\x00"))
        ->help("A^=mem[IY+d]")
        ->example("xor a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //XOR A, (IY+d)
    insert(mnem("xor", 3, "\xfd\xae\x00", "\xff\xff\x00"))
        ->help("A^=mem[IY+d]")
        ->example("xor a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));



    //CP A, r
    insert(mnem("cp", 1, "\xb8", "\xf8"))
        ->help("A-reg8")
        ->example("cp a, b")
        ->regname("a")
        ->z80reg8("\x07");
    //CP A, n
    insert(mnem("cp", 2, "\xfe\x00", "\xff\x00"))
        ->help("A-imm8")
        ->example("cp a, #0xff")
        ->regname("a")
        ->imm("\x00\xff");
    //CP A, (HL)
    insert(mnem("cp", 1, "\xbe", "\xff"))
        ->help("A-mem[HL]")
        ->example("cp a, @hl")
        ->prioritize()
        ->regname("a")
        ->regnameind("hl");
    //CP A, (IX+d)
    insert(mnem("cp", 3, "\xdd\xbe\x00", "\xff\xff\x00"))
        ->help("A-mem[IY+d]")
        ->example("cp a, (ix, -16)")
        ->regname("a")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //CP A, (IY+d)
    insert(mnem("cp", 3, "\xfd\xbe\x00", "\xff\xff\x00"))
        ->help("A-mem[IY+d]")
        ->example("cp a, (iy, -16)")
        ->regname("a")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    // Page 165

    //INC r
    insert(mnem("inc", 1, "\x04", "\xC7"))
        ->help("reg8++")
        ->example("inc b")
        ->z80reg8("\x38");
    //INC (HL)
    insert(mnem("inc", 1, "\x34", "\xff"))
        ->help("mem[HL]++")
        ->example("inc @hl")
        ->prioritize()
        ->regnameind("hl");
    //INC (IX+d)
    insert(mnem("inc", 3, "\xdd\x34\x00", "\xff\xff\x00"))
        ->help("mem[IY+d]++")
        ->example("inc (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //INC(IY+d)
    insert(mnem("inc", 3, "\xfd\x34\x00", "\xff\xff\x00"))
        ->help("mem[IY+d]++")
        ->example("inc (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));


    //DEC r
    insert(mnem("dec", 1, "\x05", "\xC7"))
        ->help("reg8++")
        ->example("dec b")
        ->z80reg8("\x38");
    //DEC (HL)
    insert(mnem("dec", 1, "\x35", "\xff"))
        ->help("mem[HL]++")
        ->example("dec @hl")
        ->prioritize()
        ->regnameind("hl");
    //DEC (IX+d)
    insert(mnem("dec", 3, "\xdd\x35\x00", "\xff\xff\x00"))
        ->help("mem[IY+d]++")
        ->example("dec (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm(("\x00\x00\xff"));
    //DEC(IY+d)
    insert(mnem("dec", 3, "\xfd\x35\x00", "\xff\xff\x00"))
        ->help("mem[IY+d]++")
        ->example("dec (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm(("\x00\x00\xff"));

    // Page 172: General Purpose Arithmetic and CPU Control Groups

    // DAA
    insert(mnem("daa", 1, "\x27", "\xff"))
        ->help("Adjusts Accumulator for Carry.")
        ->example("daa a")
        ->regname("a");
    // CPL
    insert(mnem("cpl", 1, "\x2f", "\xff"))
        ->help("A=~A.  One's complement.")
        ->example("cpl a")
        ->regname("a");
    // NEG
    insert(mnem("neg", 2, "\xed\x44", "\xff\xff"))
        ->help("A=0-A.  Two's complement.")
        ->example("neg a")
        ->regname("a");
    // CCF
    insert(mnem("ccf", 1, "\x3f", "\xff"))
        ->help("Invert CY, the Carry Flag.")
        ->example("ccf");
    // SCF
    insert(mnem("scf", 1, "\x37", "\xff"))
        ->help("Set CY, the Carry Flag.")
        ->example("scf");
    // NOP
    insert(mnem("nop", 1, "\x00", "\xff"))
        ->help("No Operation.")
        ->example("nop");
    // HALT
    insert(mnem("halt", 1, "\x76", "\xff"))
        ->help("Stop CPU until interrupt or reset.")
        ->prioritize(2)
        ->example("halt");
    // DI
    insert(mnem("di", 1, "\xf3", "\xff"))
        ->help("Disable Interrupts.")
        ->example("di");
    // EI
    insert(mnem("ei", 1, "\xfb", "\xff"))
        ->help("Enable Interrupts.")
        ->example("ei");


    // IM0
    insert(mnem("im0", 2, "\xed\x46", "\xff\xff"))
        ->help("Set Interupt Mode 0.")
        ->example("im0");
    // IM1
    insert(mnem("im1", 2, "\xed\x56", "\xff\xff"))
        ->help("Set Interupt Mode 1.")
        ->example("im1");
    // IM2
    insert(mnem("im2", 2, "\xed\x5e", "\xff\xff"))
        ->help("Set Interupt Mode 2.")
        ->example("im2");


    // Page 187: 16-bit Arithmetic Group

    // ADD HL, ss
    insert(mnem("add", 1, "\x09", "\xcf"))
        ->help("HL+=reg16")
        ->example("add hl, bc")
        ->regname("hl")
        ->z80reg16("\x30");
    insert(mnem("adc", 2, "\xed\x4a", "\xff\xcf"))
        ->help("HL+=reg16+CY")
        ->example("adc hl, bc")
        ->regname("hl")
        ->z80reg16("\x00\x30");
    insert(mnem("sbc", 2, "\xed\x42", "\xff\xcf"))
        ->help("HL+=reg16+CY")
        ->example("sbc hl, bc")
        ->regname("hl")
        ->z80reg16("\x00\x30");
    insert(mnem("add", 2, "\xdd\x09", "\xff\xcf"))
        ->help("IX+=reg16+CY")
        ->example("add ix, sp")
        ->regname("ix")
        ->z80reg16pp("\x00\x30");
    insert(mnem("add", 2, "\xfd\x09", "\xff\xcf"))
        ->help("IY+=reg16+CY")
        ->example("add iy, sp")
        ->regname("iy")
        ->z80reg16rr("\x00\x30");
    insert(mnem("inc", 1, "\x03", "\xcf"))
        ->help("reg16++")
        ->example("inc sp")
        ->z80reg16ss("\x30");
    insert(mnem("inc", 2, "\xdd\x23", "\xff\xff"))
        ->help("IX++")
        ->example("inc ix")
        ->regname("ix");
    insert(mnem("inc", 2, "\xfd\x23", "\xff\xff"))
        ->help("IY++")
        ->example("inc iy")
        ->regname("iy");

    insert(mnem("dec", 1, "\x0b", "\xcf"))
        ->help("reg16++")
        ->example("dec sp")
        ->z80reg16ss("\x30");
    insert(mnem("dec", 2, "\xdd\x2b", "\xff\xff"))
        ->help("IX--")
        ->example("dec ix")
        ->regname("ix");
    insert(mnem("dec", 2, "\xfd\x2b", "\xff\xff"))
        ->help("IY--")
        ->example("dec iy")
        ->regname("iy");


    // Page 204: Rotate and Shift

    // RLCA
    insert(mnem("rlca", 1, "\x07", "\xff"))
        ->help("Rotate Left Carry Accumulator")
        ->example("rlca");
    // RLA
    insert(mnem("rla", 1, "\x17", "\xff"))
        ->help("Rotate Left Accumulator")
        ->example("rla");
    // RRCA
    insert(mnem("rrca", 1, "\x0f", "\xff"))
        ->help("Rotate Right Carry Accumulator")
        ->example("rrca");
    // RRA
    insert(mnem("rra", 1, "\x1f", "\xff"))
        ->help("Rotate Right Accumulator")
        ->example("rra");


    // RLC r
    insert(mnem("rlc", 2, "\xcb\x00", "\xff\xf8"))
        ->help("Rotate Left Carry r8")
        ->example("rlc b")
        ->z80reg8("\x00\x07");
    // RLC (HL)
    insert(mnem("rlc", 2, "\xcb\x06", "\xff\xff"))
        ->help("Rotate Left Carry HL")
        ->example("rlc @hl")
        ->prioritize()
        ->regnameind("hl");
    // RLC (IX, d)
    insert(mnem("rlc", 4, "\xdd\xcb\x00\x06", "\xff\xff\x00\xff"))
        ->help("Rotate Left Carry HL")
        ->example("rlc (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // RLC (IY, d)
    insert(mnem("rlc", 4, "\xfd\xcb\x00\x06", "\xff\xff\x00\xff"))
        ->help("Rotate Left Carry HL")
        ->example("rlc (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    // RL r
    insert(mnem("rl", 2, "\xcb\x10", "\xff\xf8"))
        ->help("Rotate Left r8")
        ->example("rl b")
        ->z80reg8("\x00\x07");
    // RL (HL)
    insert(mnem("rl", 2, "\xcb\x16", "\xff\xff"))
        ->help("Rotate Left HL")
        ->example("rl @hl")
        ->prioritize()
        ->regnameind("hl");
    // RL (IX, d)
    insert(mnem("rl", 4, "\xdd\xcb\x00\x16", "\xff\xff\x00\xff"))
        ->help("Rotate Left HL")
        ->example("rl (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // RL (IY, d)
    insert(mnem("rl", 4, "\xfd\xcb\x00\x16", "\xff\xff\x00\xff"))
        ->help("Rotate Left HL")
        ->example("rl (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    // RRC r
    insert(mnem("rrc", 2, "\xcb\x08", "\xff\xf8"))
        ->help("Rotate Right Carry r8")
        ->example("rrc b")
        ->z80reg8("\x00\x07");
    // RRC (HL)
    insert(mnem("rrc", 2, "\xcb\x0e", "\xff\xff"))
        ->help("Rotate Right Carry HL")
        ->example("rrc @hl")
        ->prioritize()
        ->regnameind("hl");
    // RRC (IX, d)
    insert(mnem("rrc", 4, "\xdd\xcb\x00\x0e", "\xff\xff\x00\xff"))
        ->help("Rotate Right Carry HL")
        ->example("rrc (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // RRC (IY, d)
    insert(mnem("rrc", 4, "\xfd\xcb\x00\x0e", "\xff\xff\x00\xff"))
        ->help("Rotate Right Carry HL")
        ->example("rrc (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    // RR r
    /* NOTE WELL: This looks like a dupe of "RRC r8" in UM0080011,
     * but that's apparently a typo.  Thanks to Bob Mahar for spotting
     * the difference. */
    insert(mnem("rr", 2, "\xcb\x18", "\xff\xf8"))
        ->help("Rotate Right r8")
        ->example("rr b")
        ->z80reg8("\x00\x07");

    // RR (HL)
    insert(mnem("rr", 2, "\xcb\x1e", "\xff\xff"))
        ->help("Rotate Right HL")
        ->example("rr @hl")
        ->prioritize()
        ->regnameind("hl");
    // RR (IX, d)
    insert(mnem("rr", 4, "\xdd\xcb\x00\x1e", "\xff\xff\x00\xff"))
        ->help("Rotate Right HL")
        ->example("rr (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // RR (IY, d)
    insert(mnem("rr", 4, "\xfd\xcb\x00\x1e", "\xff\xff\x00\xff"))
        ->help("Rotate Right HL")
        ->example("rr (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    // SLA r
    insert(mnem("sla", 2, "\xcb\x20", "\xff\xf8"))
        ->help("Shift Left Arithmetic r8")
        ->example("sla b")
        ->z80reg8("\x00\x07");
    // SLA (HL)
    insert(mnem("sla", 2, "\xcb\x26", "\xff\xff"))
        ->help("Shift Left Arithmetic HL")
        ->example("sla @hl")
        ->prioritize()
        ->regnameind("hl");
    // SLA (IX, d)
    insert(mnem("sla", 4, "\xdd\xcb\x00\x26", "\xff\xff\x00\xff"))
        ->help("Shift Left Arithmetic HL")
        ->example("sla (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // SLA (IY, d)
    insert(mnem("sla", 4, "\xfd\xcb\x00\x26", "\xff\xff\x00\xff"))
        ->help("Shift Left Arithmetic HL")
        ->example("sla (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");



    // SRA r
    insert(mnem("sra", 2, "\xcb\x28", "\xff\xf8"))
        ->help("Shift Right Arithmetic r8")
        ->example("sra b")
        ->z80reg8("\x00\x07");
    // SRA (HL)
    insert(mnem("sra", 2, "\xcb\x2e", "\xff\xff"))
        ->help("Shift Right Arithmetic HL")
        ->example("sra @hl")
        ->prioritize()
        ->regnameind("hl");
    // SRA (IX, d)
    insert(mnem("sra", 4, "\xdd\xcb\x00\x2e", "\xff\xff\x00\xff"))
        ->help("Shift Right Arithmetic HL")
        ->example("sra (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // SRA (IY, d)
    insert(mnem("sra", 4, "\xfd\xcb\x00\x2e", "\xff\xff\x00\xff"))
        ->help("Shift Right Arithmetic HL")
        ->example("sra (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");



    // SRL r
    insert(mnem("srl", 2, "\xcb\x38", "\xff\xf8"))
        ->help("Shift Right Logical r8")
        ->example("srl b")
        ->z80reg8("\x00\x07");
    // SRL (HL)
    insert(mnem("srl", 2, "\xcb\x3e", "\xff\xff"))
        ->help("Shift Right Logical HL")
        ->example("srl @hl")
        ->prioritize()
        ->regnameind("hl");
    // SRL (IX, d)
    insert(mnem("srl", 4, "\xdd\xcb\x00\x3e", "\xff\xff\x00\xff"))
        ->help("Shift Right Logical HL")
        ->example("srl (ix, -16)")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    // SRL (IY, d)
    insert(mnem("srl", 4, "\xfd\xcb\x00\x3e", "\xff\xff\x00\xff"))
        ->help("Shift Right Logical HL")
        ->example("srl (iy, -16)")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");

    //Page 238

    // RLD
    insert(mnem("rld", 2, "\xed\x6f", "\xff\xff"))
        ->help("FIXME")
        ->example("rld");
    // RRD
    insert(mnem("rrd", 2, "\xed\x67", "\xff\xff"))
        ->help("FIXME")
        ->example("rrd");


    // Page 242: Bit Set, Reset and Test Group

    insert(mnem("bit", 2, "\xcb\x40", "\xff\xc0"))
        ->help("Bit Immediate Test")
        ->example("bit bit0, b")
        ->bit3("\x00\x38")
        ->z80reg8("\x00\x07");
    //BIT b, (HL)
    insert(mnem("bit", 2, "\xcb\x46", "\xff\xc7"))
        ->help("Bit Immediate Test")
        ->example("bit bit0, @hl")
        ->prioritize() // Otherwise conflicts with previous instruction.
        ->bit3("\x00\x38")
        ->regnameind("hl");
    //BIT b, (IX+d)
    insert(mnem("bit", 4, "\xdd\xcb\x00\x46", "\xff\xff\x00\xc7"))
        ->help("Bit Immediate Test")
        ->example("bit bit0, (ix, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    //BIT b, (IY+d)
    insert(mnem("bit", 4, "\xfd\xcb\x00\x46", "\xff\xff\x00\xc7"))
        ->help("Bit Immediate Test")
        ->example("bit bit0, (iy, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    //SET b, r
    insert(mnem("set", 2, "\xcb\xc0", "\xff\xc0"))
        ->help("Set Bit")
        ->example("set bit0, b")
        ->bit3("\x00\x38")
        ->z80reg8("\x00\x07");
    //SET b, (HL)
    insert(mnem("set", 2, "\xcb\xc6", "\xff\xc7"))
        ->help("Set Bit")
        ->example("set bit0, @hl")
        ->prioritize() // Otherwise conflicts with previous instruction.
        ->bit3("\x00\x38")
        ->regnameind("hl");
    //SET b, (IX+d)
    insert(mnem("set", 4, "\xdd\xcb\x00\xc6", "\xff\xff\x00\xc7"))
        ->help("Set Bit")
        ->example("set bit0, (ix, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    //SET b, (IY+d)
    insert(mnem("set", 4, "\xfd\xcb\x00\xc6", "\xff\xff\x00\xc7"))
        ->help("Set Bit")
        ->example("set bit0, (iy, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    //RES b, r
    insert(mnem("res", 2, "\xcb\x80", "\xff\xc0"))
        ->help("Clear Bit")
        ->example("res bit0, b")
        ->bit3("\x00\x38")
        ->z80reg8("\x00\x07");
    //RES b, (HL)
    insert(mnem("res", 2, "\xcb\x86", "\xff\xc7"))
        ->help("Clear Bit")
        ->example("res bit0, @hl")
        ->prioritize() // Otherwise conflicts with previous instruction.
        ->bit3("\x00\x38")
        ->regnameind("hl");
    //RES b, (IX+d)
    insert(mnem("res", 4, "\xdd\xcb\x00\x86", "\xff\xff\x00\xc7"))
        ->help("Clear Bit")
        ->example("res bit0, (ix, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("ix")
        ->simm("\x00\x00\xff\x00");
    //RES b, (IY+d)
    insert(mnem("res", 4, "\xfd\xcb\x00\x86", "\xff\xff\x00\xc7"))
        ->help("Clear Bit")
        ->example("res bit0, (iy, -16)")
        ->bit3("\x00\x00\x00\x38")
        ->group('(')
        ->regname("iy")
        ->simm("\x00\x00\xff\x00");


    //Page 261: Jump Group
    // JP adr16
    insert(mnem("jp", 3, "\xc3\x00\x00", "\xff\x00\x00"))
        ->help("Jump")
        ->example("jp 0xffff")
        ->adr("\x00\xff\xff");
    // JP cond, adr16
    insert(mnem("jp", 3, "\xc2\x00\x00", "\xc7\x00\x00"))
        ->help("Jump")
        ->example("jp nz, 0xffff")
        ->z80cond("\x38\x00\x00")
        ->adr("\x00\xff\xff");
    // JR e
    insert(mnem("jr", 2, "\x18\x00", "\xff\x00"))
        ->help("Jump")
        ->example("self: jr self")
        ->rel("\x00\xff");
    // JR C, e
    insert(mnem("jr", 2, "\x38\x00", "\xff\x00"))
        ->help("Jump")
        ->example("self: jr c, self")
        ->regname("c")
        ->rel("\x00\xff");
    // JR NC, e
    insert(mnem("jr", 2, "\x30\x00", "\xff\x00"))
        ->help("Jump")
        ->example("self: jr nc, self")
        ->regname("nc")
        ->rel("\x00\xff");
    // JR Z, e
    insert(mnem("jr", 2, "\x28\x00", "\xff\x00"))
        ->help("Jump")
        ->example("self: jr z, self")
        ->regname("z")
        ->rel("\x00\xff");
    // JR NZ, e
    insert(mnem("jr", 2, "\x20\x00", "\xff\x00"))
        ->help("Jump")
        ->example("self: jr nz, self")
        ->regname("nz")
        ->rel("\x00\xff");
    // JP (HL)
    insert(mnem("jp", 1, "\xe9", "\xff"))
        ->help("Jump")
        ->example("jp @hl")
        ->regnameind("hl");
    // JP (IX)
    insert(mnem("jp", 2, "\xdd\xe9", "\xff\xff"))
        ->help("Jump")
        ->example("jp @ix")
        ->regnameind("ix");
    // JP (IY)
    insert(mnem("jp", 2, "\xfd\xe9", "\xff\xff"))
        ->help("Jump")
        ->example("jp @iy")
        ->regnameind("iy");
    // DJNZ e
    insert(mnem("djnz", 2, "\x10\x00", "\xff\x00"))
        ->help("Jump if B--!=0.")
        ->example("self: djnz self")
        ->rel("\x00\xff");

    // Page 280: Call and Return Group

    // CALL nn
    insert(mnem("call", 3, "\xcd\x00\x00", "\xff\x00\x00"))
        ->help("Call")
        ->example("call 0xffff")
        ->adr("\x00\xff\xff");
    // CALL cc, nn
    insert(mnem("call", 3, "\xc4\x00\x00", "\xc7\x00\x00"))
        ->help("Conditional Call")
        ->example("call nz, 0xffff")
        ->z80cond("\x38\x00\x00")
        ->adr("\x00\xff\xff");
    // RET
    insert(mnem("ret", 1, "\xc9", "\xff"))
        ->help("Return from Call")
        ->example("ret");
    // RET cc
    insert(mnem("ret", 1, "\xc0", "\xc7"))
        ->help("Conditionally Return from Call")
        ->example("ret nz")
        ->z80cond("\x38\x00\x00");
    // RETI
    insert(mnem("reti", 2, "\xed\x4d", "\xff\xff"))
        ->help("Return from Interrupt")
        ->example("reti");
    // RETN
    insert(mnem("retn", 2, "\xed\x45", "\xff\xff"))
        ->help("Return from NMI")
        ->example("retn");
    // RST p
    insert(mnem("rst", 1, "\xc7", "\xc7"))
        ->help("Call Zero-Page Reset Handler")
        ->example("rst 0x38")
        ->z80rsthandler("\x38");

    // Page 294: Input and Output Group

    // IN A, (n)
    insert(mnem("in", 2, "\xdb\x00", "\xff\x00"))
        ->help("Input from I/O Port")
        ->example("in a, %0xff")
        ->regname("a")
        ->port("\x00\xff");
    // IN r, (C)
    insert(mnem("in", 2, "\xed\x40", "\xff\xc7"))
        ->help("Input byte from I/O Port in C register.")
        ->example("in b, @c")
        ->z80reg8("\x00\x38")
        ->regnameind("c");
    // INI
    insert(mnem("ini", 2, "\xed\xa2", "\xff\xff"))
        ->help("@hl=@c, b--, hl++")
        ->example("ini");
    // INIR
    insert(mnem("inir", 2, "\xed\xb2", "\xff\xff"))
        ->help("while(b) {@hl=@c, b--, hl++}")
        ->example("inir");
    // IND
    insert(mnem("ind", 2, "\xed\xaa", "\xff\xff"))
        ->help("@hl=@c, b--, hl--")
        ->example("ind");
    // INDR
    insert(mnem("indr", 2, "\xed\xba", "\xff\xff"))
        ->help("while(b) {@hl=@c, b--, hl--}")
        ->example("indr");
    // OUT (n), A
    insert(mnem("out", 2, "\xd3\x00", "\xff\x00"))
        ->help("Output to I/O Port")
        ->example("out %0xff, a")
        ->port("\x00\xff")
        ->regname("a");

    //OUT (C), r
    insert(mnem("out", 2, "\xed\x41", "\xff\xc7"))
        ->help("Output to I/O Port")
        ->example("out b, a")
        ->z80reg8("\x00\x38")  //FIXME: Indirection?
        ->regname("a");
    //OUTI
    insert(mnem("outi", 2, "\xed\xa3", "\xff\xff"))
        ->help("@hl=@c, b--, hl++")
        ->example("outi");
    //OTIR
    insert(mnem("outir", 2, "\xed\xb3", "\xff\xff"))
        ->help("while(b) {@hl=@c, b--, hl++}")
        ->example("outir");
    //OUTD
    insert(mnem("outd", 2, "\xed\xab", "\xff\xff"))
        ->help("@hl=@c, b--, hl--")
        ->example("outd");
    //OTDR
    insert(mnem("otdr", 2, "\xed\xbb", "\xff\xff"))
        ->help("while(b) {@hl=@c, b--, hl--}")
        ->example("otdr");

}



/* Z80 has rather unique register names.  We define support here,
 * and use the preprocessor so that they needn't be added to the
 * GAParameterGroup class. */
GAParameterZ80Reg8::GAParameterZ80Reg8(const char* mask){
    setMask(mask);
}
int GAParameterZ80Reg8::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(op->value.length()!=1)
        return 0; //Wrong size.

    char c=op->value[0].toLatin1();


    switch(c){
        //FIXME: The Z80 Register Order might be B, C, D, E, H, L, [HL], A
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    //case 'f': //Illegal
    case 'h':
    case 'l': // Usually overloaded?
    case 'a':
        return 1;  //Legal register name.
    default:
        return 0;  //Not an 8-bit register name.
    }
}

QString GAParameterZ80Reg8::decode(GALanguage *lang,
                                    uint64_t adr, const char *bytes,
                                    int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    if(p>=8)
        qDebug()<<"p="<<p;
    assert(p<8); //Reg count on this architecture.

    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameterZ80Reg8::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen){
    uint64_t val;

    QString v=op.value;
    for(int i=0; i<8; i++)
        if(v==regnames[i]) val=i;

    rawencode(lang,adr,bytes,op,inslen,val);
}


/* These are the same, except for 16-bit register pairs.  Like SM83,
 * the pairs are usally as follows, but there are variants.
 * BC 00
 * DE 01
 * HL 10
 * SP 11  (sometimes AF for stack operations)
 */


GAParameterZ80Reg16::GAParameterZ80Reg16(const char* mask, bool stk){
    setMask(mask);
    if(stk)
        regnames[3]="af";
}
GAParameterZ80Reg16::GAParameterZ80Reg16(const char* mask, QString regnames[4]){
    setMask(mask);
    for(int i=0; i<4; i++)
        this->regnames[i]=regnames[i];
}
int GAParameterZ80Reg16::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.
    if(op->value.length()!=2)
        return 0; //Wrong size.

    //This set is right for r16, wrong for r16stk, r16mem.
    for(int i=0; i<4; i++)
        if(op->value==regnames[i]) return 1;
    return 0;
}

QString GAParameterZ80Reg16::decode(GALanguage *lang,
                                     uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<4); //Reg count on this architecture.
    QString rendering=prefix
                        +regnames[p]
                        +suffix;
    return rendering;
}
void GAParameterZ80Reg16::encode(GALanguage *lang,
                                  uint64_t adr, QByteArray &bytes,
                                  GAParserOperand op,
                                  int inslen){
    uint64_t val;
    for(int i=0; i<4; i++){
        if(op.value==regnames[i]) val=i;
    }

    rawencode(lang,adr,bytes,op,inslen,val);
}




GAParameterZ80Cond::GAParameterZ80Cond(const char* mask){
    setMask(mask);
}
int GAParameterZ80Cond::match(GAParserOperand *op, int len){
    if(op->prefix!=this->prefix)
        return 0; //Wrong type.

    //Is the condition in the list?
    for(int i=0; i<8; i++)
        if(op->value==condnames[i]) return 1;
    return 0;
}

QString GAParameterZ80Cond::decode(GALanguage *lang,
                                    uint64_t adr, const char *bytes,
                                    int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen);
    assert(p<8); //Reg count on this architecture.
    QString rendering=prefix
                        +condnames[p]
                        +suffix;
    return rendering;
}
void GAParameterZ80Cond::encode(GALanguage *lang,
                                 uint64_t adr, QByteArray &bytes,
                                 GAParserOperand op,
                                 int inslen){
    uint64_t val;
    for(int i=0; i<8; i++){
        if(op.value==condnames[i]) val=i;
    }

    rawencode(lang,adr,bytes,op,inslen,val);
}




/* Restart handler addresses, which are oddly
 * like addresses except unshifted.
 */

GAParameterZ80ResetHandler::GAParameterZ80ResetHandler(const char* mask){
    setMask(mask);
}
int GAParameterZ80ResetHandler::match(GAParserOperand *op, int len){
    int64_t val=op->uint64(false); //False on a match.

    //Reject values that don't fit.
    if(val&(0xc7)){
        qDebug()<<"Not a valid Z80 restart target address: "<<op->value;
        return 0;
    }

    return 1;
}
QString GAParameterZ80ResetHandler::decode(GALanguage *lang, uint64_t adr, const char *bytes, int inslen){
    uint64_t p=rawdecode(lang,adr,bytes,inslen)
                 <<3;   //Shift to expand it.
    return QString::asprintf("0x%02x",(unsigned int) p);
}
void GAParameterZ80ResetHandler::encode(GALanguage *lang,
                                         uint64_t adr, QByteArray &bytes,
                                         GAParserOperand op,
                                         int inslen
                                         ){
    int64_t val=op.uint64(true)
                  >>3;   //Shift to shrink it.
    rawencode(lang,adr,bytes,op,inslen,val);
}
