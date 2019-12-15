.section .text
.globl   kefel
kefel:  movl	%edi,%eax
		lea		(%eax,%eax,2), %eax
neg %eax
		ret
