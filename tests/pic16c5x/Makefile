all: unidasm.asm gasm.asm test

test: gasm.asm
	cat gasm.asm | grep 03fd | grep goto | grep 0x0043
	cat gasm.asm | grep 03f9 | grep call | grep 002e

unidasm.asm: *.bin
	unidasm -arch pic16c5x basicstamp1-pic16c56.bin >unidasm.asm
gasm.asm: *.bin
	goodasm --pic16c5x basicstamp1-pic16c56.bin -dab >gasm.asm

clean:
	rm -f *.asm
