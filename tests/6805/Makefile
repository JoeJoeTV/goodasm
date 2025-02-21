all: nipperpatch nipperpatch.go

nipperpatch: nipperpatch.asm
	goodasm -o nipperpatch nipperpatch.asm -LN
nipperpatch.go: nipperpatch.asm
	goodasm nipperpatch.asm -LGa | tee nipperpatch.go


clean:
	rm -f nipperpatch nipperpatch.go



