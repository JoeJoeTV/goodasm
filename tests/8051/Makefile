


all: simple.txt 

clean:
	rm -f simple simple.txt

simple: simple.s
	nasm simple.s
simple.txt: simple
	goodasm -dabA --8051 simple | tee simple.txt
grade:
	goodasm --6502 --grade basic.bin
	goodasm --6502 --grade kernal.bin

