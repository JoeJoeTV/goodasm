all:
	goodasm  missingcomments.asm -Lab | grep "one point"
	goodasm  missingcomments.asm -Gab | grep "one point"
	goodasm  missingcomments.asm -Yab | grep "one point"
	goodasm  missingcomments.asm -Cab | grep "one point"
	goodasm badquotes.asm # Just shouldn't crash or hang.
	goodasm -Ha --base 0xde11 dataandlabels.asm | grep de10 | grep "   cc de 15 9d cc de"
	goodasm -Lab disassemble.asm  | grep nop
#	goodasm disassemble.asm  -Lab | grep 39 | grep data

clean:

