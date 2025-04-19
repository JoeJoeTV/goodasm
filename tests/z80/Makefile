

all: grade
	goodasm zsh-hello.asm -o zsh-hello.bin
	md5sum -c md5.txt
clean:
	rm -f *.bin
grade:
# Alien Breed 4 for the TI 86 from ticalc.org
	goodasm -i ab4.86p | grep z80
