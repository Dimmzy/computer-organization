.section text
.globl   kefel
kefel:  movl	%edi,%eax
		lea		(%eax,%eax,4), %eax
		ret