all: test

test: ffred.txt
	cat ffred.txt | grep 07df | grep 7d7 | grep jmp

ffred.txt:
	goodasm -abd --ucom43 ffred.bin >ffred.txt
clean:
	rm -f ffred.txt
