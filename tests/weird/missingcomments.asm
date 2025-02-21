;;; At one point, goodasm listings included instructions and their
;;; comments, but not comments without instructions or bytes in them.
;;; This test tells the difference.

	.lang 6805
	
	nop			; Simple NOP, after long comments.
	
