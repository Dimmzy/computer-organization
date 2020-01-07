	.file	"switch.c"
	.text
	.globl	switch1
	.type	switch1, @function
switch1:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp) // p1
	movq	%rsi, -32(%rbp) // p2
	movq	%rdx, -40(%rbp) // action
	movq	$0, -8(%rbp) // nills rbp
	movq	-40(%rbp), %rax // move action to %rax
	subq	$21, %rax // Subtract 21 from "action"
	cmpq	$10, %rax // Compare 10 to "action" (largest case)
	ja	.L2 // Jump if above (action > 10)
	leaq	0(,%rax,4), %rdx
	leaq	.L4(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L4(%rip), %rdx
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L4:
	.long	.L9-.L4
	.long	.L2-.L4
	.long	.L8-.L4
	.long	.L2-.L4
	.long	.L7-.L4
	.long	.L2-.L4
	.long	.L6-.L4
	.long	.L2-.L4
	.long	.L5-.L4
	.long	.L5-.L4
	.long	.L3-.L4
	.text
.L6:
	movq	-24(%rbp), %rax // p1 to rax
	movq	(%rax), %rdx // p1.val to rdx
	movq	-32(%rbp), %rax // p2.val to rax
	movq	%rdx, (%rax) 
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	jmp	.L10
.L7:
	movq	-24(%rbp), %rax
	movq	(%rax), %rdx
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	addq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	jmp	.L10
.L5:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	leaq	-59(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	subq	%rax, %rdx
	movq	-32(%rbp), %rax
	movq	%rdx, (%rax)
	jmp	.L10
.L8:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	leaq	60(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
.L9:
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	imulq	%rax, %rax
	movq	%rax, -8(%rbp)
	jmp	.L10
.L3:
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movl	%eax, %ecx
	sarq	%cl, -8(%rbp)
	jmp	.L10
.L2:			// Default
	movq	$12, -8(%rbp)
	salq	$3, -8(%rbp)
.L10:
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	switch1, .-switch1
	.ident	"GCC: (GNU) 9.2.0"
	.section	.note.GNU-stack,"",@progbits
