.code64
.text

.global asm_add
.type asm_add, @function

asm_add:
	push %rbp			# save old frame pointer
	mov %rsp, %rbp 		# set frame pointer to stack pointer
						# without local vars, %esp is not adjusted

	#mov %rdi, %rax
	#mov %rsi, %rcx
	#add %rcx, %rax 		# C assumes return value to be in %eax

	mov 24(%rbp), %rax
	add 16(%rbp), %rax


	mov %rbp, %rsp  	# re-adjust stack pointer
	pop %rbp			# pop old frame pointer back into %ebp
	ret					# pop the return address and go to it
