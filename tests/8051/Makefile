


all: simple.txt damage

clean:
	rm -f simple simple.txt grade match damage.bin
simple: simple.s
	nasm simple.s
simple.txt: simple
	goodasm -dabA --8051 simple | tee simple.txt
damage.bin:
	dd if=/dev/zero of=damage.bin count=1 bs=1
damage: simple damage.bin
	goodasm -daD simple --8051 --damage damage.bin
match: match.asm
	goodasm --8051 match.asm -o match

## This is currently broken because the image is too short.
grade: match
	goodasm -i match | grep 8051

