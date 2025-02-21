


all: simple.txt grade demo error
	cd nanochess && make clean all

clean:
	rm -f simple simple.txt demo
	cd nanochess && make clean

simple: simple.s
	nasm simple.s
simple.txt: simple
	goodasm -dabA --6502 simple | tee simple.txt
grade:
	goodasm --6502 --grade basic.bin
	goodasm --6502 --grade kernal.bin
error:
# Checking for errors on particular line numbers.
	goodasm error.asm | grep "error2.asm:12"
	goodasm error.asm | grep "error.asm:45"

demo: demo.asm
	goodasm --6502 demo.asm -o demo
#	unidasm demo -arch m6502
rundemo: demo
	stella demo



