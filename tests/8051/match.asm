;;; Example 8051 image for testing image recognition tricks.
;;; Part of GoodASM.



	nop
	ret


;;; This looks like a function that calls preserved some registers
;;; before doing its work.
	push @1
	push @2
	push @3
	push @4
	nop
	pop @4
	pop @3
	pop @2
	pop @1
	ret
	
;;; This looks like a call table.
table:	
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ljmp table
	ret
	
