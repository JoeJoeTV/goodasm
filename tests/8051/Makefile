


all: simple.txt grade

clean:
	rm -f simple simple.txt grade match
simple: simple.s
	nasm simple.s
simple.txt: simple
	goodasm -dabA --8051 simple | tee simple.txt
match: match.asm
	goodasm --8051 match.asm -o match
grade: match
	goodasm -i match | grep 8051

