all: guesses.txt hello.txt fibonacci.txt test

clean:
	rm -f guesses guesses.txt hello.txt fibonacci.txt *~

guesses: guesses.s
	nasm guesses.s
guesses.txt: guesses
	goodasm --s2000 -dba guesses >guesses.txt
fibonacci.txt: fibonacci.bin
	goodasm --s2000 -dba fibonacci.bin >fibonacci.txt
hello.txt: hello.bin
	goodasm --s2000 -dba hello.bin >hello.txt


test: hello.txt
	cat hello.txt | grep 03d7 | grep jmp
	cat hello.txt | grep 03df | grep 0x0011 | grep jmp
