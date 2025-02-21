all: test

test: bins
	md5sum -c md5.txt
%.bin: %.asm
	goodasm $< -o $@

bins: demo1 demo2 demo3


demo1: demo1_4.bin demo1_8.bin
demo2: demo2_1.bin demo2_2.bin demo2_3.bin demo2_4.bin demo2_5.bin
demo3: demo3_1.bin demo3_2.bin
rundemo: demo3
	stella demo3_2.bin


clean:
	rm -f *.bin

