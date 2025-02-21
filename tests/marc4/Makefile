

all: test


test: sample
	goodasm --marc4 -dba sample

clean:
	rm -f sample *~ *#


sample: sample.s
	nasm sample.s
