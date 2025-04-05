all: TST8080.dis

TST8080.dis: TST8080.COM
	goodasm --base 0x100 -daAb --8080 TST8080.COM >TST8080.dis
clean:
	rm -f TST8080.dis



