all: hello

hello: hello.asm
	goodasm  hello.asm  -o hello
runhello: hello
	mgba-qt -l 255 hello
clean:
	rm -f hello hello.sav

