

# The jist of this is that if we properly assembly our own versions of
# the Timendus' test suite, we ought to get the same binary.

all: compare


bins: 1-chip8-logo.bin
#2-ibm-logo.bin 3-corax+.bin
#4-flags.bin
#5-quirks.bin
#6-keypad.bin
#7-beep.bin
#8-scrolling.bin


clean:
	rm -f *.bin

compare: bins
	diff 1-chip8-logo.ch8 1-chip8-logo.bin
	md5sum -c md5.txt

#%.ch8: %.asm
#	goodasm $< -o $@
%.bin: %.asm
	goodasm $< -o $@
