all: exploit.bin sample famous99
	md5sum -c md5.txt 

exploit.bin: exploit.asm
	goodasm exploit.asm -o exploit.bin

sample: sample.cor
	dd if=sample.cor of=sample-0000-sram.bin bs=1 count=256
	dd if=sample.cor of=sample-0520-eeprom.bin bs=1 count=3040 skip=1312
	goodasm --base 0x0520 -dLab --fcard sample-0520-eeprom.bin >sample-0520-eeprom.asm
	dd if=sample.cor of=sample-1100-rompage0.bin bs=1 count=3840 skip=4352
#	dd if=sample.cor of=sample-1100-rompage1.bin bs=1 skip=8192  # Cut at 0x2000, too big and includes memory to 1a00.
	dd if=sample.cor of=sample-1100-rompage1.bin bs=1 skip=10496  # Cut at 0x2900, avoid repeat of page 0.
	goodasm --base 0x1100 -dLab --fcard sample-1100-rompage0.bin >sample-1100-rompage0.asm
	goodasm --base 0x1100 -dLab --fcard sample-1100-rompage1.bin >sample-1100-rompage1.asm
	goodasm --base 0x1a00 -dLab --fcard sample-1100-rompage1.bin >sample-1a00-rompage1.asm  # Maybe latter half changes?

# Famous 0x99 loop.
famous99:
	goodasm famous99-1100.asm -o famous99-1100.bin
	goodasm famous99-1a10.asm -o famous99-1a10.bin
	goodasm famous99-1100.asm -Y >famous99-1100.yar
	yr scan famous99-1100.yar .

clean:
	rm -f *.bin sample*.asm *.yar






