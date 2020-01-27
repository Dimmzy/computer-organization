.section		.text
.globl		kefel
		 kefel: 		movl		%edi, %eax
		movl		%edi, %ebx
		shl $7, %eax
		shl $16, %ebx
		subl %ebx, %eax
		subl %edi, %eax
		movl %edi, %ebx
		shl $20, %ebx
		subl %ebx, %eax
		ret
