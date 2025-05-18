all: test.bin
test: test.bin
	md5sum -c md5.txt 
test.bin: test.asm
	goodasm test.asm -o test.bin -Lab
unidasm: test.bin
	unidasm -arch arm7 test.bin
clean:
	rm -f *.bin 






