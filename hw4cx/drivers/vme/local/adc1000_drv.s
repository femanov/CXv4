	.file	"adc1000_drv.c"
	.text
.Ltext0:
	.type	check_snprintf, @function
check_snprintf:
.LFB14:
	.file 1 "../../../../4cx/src/../exports/include/misc_macros.h"
	.loc 1 105 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$240, %rsp
	movq	%rdi, -216(%rbp)
	movq	%rsi, -224(%rbp)
	movq	%rcx, -152(%rbp)
	movq	%r8, -144(%rbp)
	movq	%r9, -136(%rbp)
	testb	%al, %al
	je	.L2
	.loc 1 105 0
	movaps	%xmm0, -128(%rbp)
	movaps	%xmm1, -112(%rbp)
	movaps	%xmm2, -96(%rbp)
	movaps	%xmm3, -80(%rbp)
	movaps	%xmm4, -64(%rbp)
	movaps	%xmm5, -48(%rbp)
	movaps	%xmm6, -32(%rbp)
	movaps	%xmm7, -16(%rbp)
.L2:
	movq	%rdx, -232(%rbp)
	.loc 1 109 0
	movl	$24, -208(%rbp)
	movl	$48, -204(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -200(%rbp)
	leaq	-176(%rbp), %rax
	movq	%rax, -192(%rbp)
	.loc 1 110 0
	leaq	-208(%rbp), %rcx
	movq	-232(%rbp), %rdx
	movq	-224(%rbp), %rsi
	movq	-216(%rbp), %rax
	movq	%rax, %rdi
	call	vsnprintf@PLT
	movl	%eax, -180(%rbp)
	.loc 1 113 0
	cmpl	$0, -180(%rbp)
	js	.L3
	.loc 1 113 0 is_stmt 0 discriminator 2
	movl	-180(%rbp), %eax
	movl	%eax, %eax
	movq	-224(%rbp), %rdx
	subq	$1, %rdx
	cmpq	%rdx, %rax
	jbe	.L4
.L3:
	.loc 1 113 0 discriminator 1
	movl	$1, %eax
	jmp	.L5
.L4:
	.loc 1 113 0 discriminator 3
	movl	$0, %eax
.L5:
	.loc 1 114 0 is_stmt 1 discriminator 4
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	check_snprintf, .-check_snprintf
	.data
	.align 32
	.type	pll_presets_table, @object
	.size	pll_presets_table, 36
pll_presets_table:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	50372610
	.long	265985
	.long	2
	.long	33587204
	.long	805506817
	.align 32
	.type	chinfo, @object
	.size	chinfo, 1000
chinfo:
	.long	1
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.zero	32
	.long	7
	.long	0
	.long	2
	.long	0
	.long	2
	.long	0
	.long	2
	.long	0
	.long	2
	.long	0
	.long	3
	.long	0
	.zero	16
	.long	4
	.long	0
	.long	2
	.long	0
	.long	2
	.long	0
	.long	4
	.long	0
	.long	4
	.long	0
	.long	4
	.long	0
	.long	4
	.long	0
	.long	4
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	4
	.long	0
	.long	4
	.long	0
	.zero	72
	.long	3
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	3
	.long	0
	.zero	8
	.long	2
	.long	0
	.zero	8
	.long	5
	.long	-1
	.long	5
	.long	-1
	.long	6
	.long	0
	.long	5
	.long	20
	.long	5
	.long	21
	.long	5
	.long	22
	.long	5
	.long	23
	.zero	8
	.long	5
	.long	24
	.long	0
	.long	25
	.long	0
	.long	26
	.long	0
	.long	27
	.long	5
	.long	-1
	.long	5
	.long	-1
	.long	5
	.long	-1
	.long	5
	.long	-1
	.zero	16
	.long	5
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	5
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	5
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	0
	.long	-1
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	6
	.long	0
	.section	.rodata
.LC0:
	.string	"int"
.LC1:
	.string	"front"
.LC2:
	.string	"back"
	.section	.data.rel.local,"aw",@progbits
	.align 32
	.type	adcNNN_timing_lkp, @object
	.size	adcNNN_timing_lkp, 64
adcNNN_timing_lkp:
	.quad	.LC0
	.long	0
	.zero	4
	.quad	.LC1
	.long	1
	.zero	4
	.quad	.LC2
	.long	2
	.zero	4
	.quad	0
	.long	0
	.zero	4
	.section	.rodata
.LC3:
	.string	"disable"
.LC4:
	.string	"back+sync"
	.section	.data.rel.local
	.align 32
	.type	adcNNN_trig_type_lkp, @object
	.size	adcNNN_trig_type_lkp, 96
adcNNN_trig_type_lkp:
	.quad	.LC3
	.long	0
	.zero	4
	.quad	.LC0
	.long	1
	.zero	4
	.quad	.LC1
	.long	2
	.zero	4
	.quad	.LC2
	.long	3
	.zero	4
	.quad	.LC4
	.long	4
	.zero	4
	.quad	0
	.long	0
	.zero	4
	.section	.rodata
.LC5:
	.string	"0.5"
.LC6:
	.string	"1"
.LC7:
	.string	"2"
.LC8:
	.string	"4"
	.section	.data.rel.local
	.align 32
	.type	adcNNN_range_lkp, @object
	.size	adcNNN_range_lkp, 80
adcNNN_range_lkp:
	.quad	.LC5
	.long	0
	.zero	4
	.quad	.LC6
	.long	1
	.zero	4
	.quad	.LC7
	.long	2
	.zero	4
	.quad	.LC8
	.long	3
	.zero	4
	.quad	0
	.long	0
	.zero	4
	.section	.rodata
.LC9:
	.string	"internal"
.LC10:
	.string	"vepp5_rfmeas"
	.section	.data.rel.local
	.align 32
	.type	adcNNN_pll_preset_lkp, @object
	.size	adcNNN_pll_preset_lkp, 48
adcNNN_pll_preset_lkp:
	.quad	.LC9
	.long	1
	.zero	4
	.quad	.LC10
	.long	2
	.zero	4
	.quad	0
	.long	0
	.zero	4
	.section	.rodata
.LC11:
	.string	"ptsofs"
.LC12:
	.string	"numpts"
.LC13:
	.string	"timing"
.LC14:
	.string	"frqdiv"
.LC15:
	.string	"trigger"
.LC16:
	.string	"trig_n"
.LC17:
	.string	"range"
.LC18:
	.string	"pll_preset"
.LC19:
	.string	"run_mode"
	.section	.data.rel,"aw",@progbits
	.align 32
	.type	adc1000_params, @object
	.size	adc1000_params, 560
adc1000_params:
	.quad	.LC11
	.long	50
	.long	872
	.quad	4
	.long	0
	.long	0
	.long	-1
	.long	0
	.long	49998
	.zero	12
	.quad	.LC12
	.long	50
	.long	876
	.quad	4
	.long	0
	.long	0
	.long	-1
	.long	1
	.long	50000
	.zero	12
	.quad	.LC13
	.long	52
	.long	880
	.quad	4
	.long	0
	.long	0
	.long	-1
	.zero	4
	.quad	adcNNN_timing_lkp
	.zero	8
	.quad	.LC14
	.long	50
	.long	884
	.quad	4
	.long	0
	.long	0
	.long	-1
	.long	0
	.long	65535
	.zero	12
	.quad	.LC15
	.long	52
	.long	904
	.quad	4
	.long	0
	.long	0
	.long	-1
	.zero	4
	.quad	adcNNN_trig_type_lkp
	.zero	8
	.quad	.LC16
	.long	50
	.long	908
	.quad	4
	.long	0
	.long	0
	.long	-1
	.long	0
	.long	7
	.zero	12
	.quad	.LC17
	.long	52
	.long	888
	.quad	4
	.long	0
	.long	0
	.long	-1
	.zero	4
	.quad	adcNNN_range_lkp
	.zero	8
	.quad	.LC18
	.long	52
	.long	948
	.quad	4
	.long	0
	.long	0
	.long	0
	.zero	4
	.quad	adcNNN_pll_preset_lkp
	.zero	8
	.quad	.LC19
	.long	52
	.long	864
	.quad	4
	.long	0
	.long	0
	.long	-1
	.zero	4
	.quad	pzframe_drv_run_mode_lkp
	.zero	8
	.quad	0
	.long	0
	.long	0
	.quad	0
	.long	0
	.long	0
	.zero	24
	.text
	.type	Return1Param, @function
Return1Param:
.LFB21:
	.file 2 "../src/adc4x250_meat.h"
	.loc 2 309 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	.loc 2 310 0
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	leaq	196(%rdx), %rcx
	movl	-16(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$56, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-8(%rbp), %rax
	movl	208(%rax), %eax
	movl	-12(%rbp), %esi
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	.loc 2 311 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	Return1Param, .-Return1Param
	.type	ReturnDevInfo, @function
ReturnDevInfo:
.LFB22:
	.loc 2 314 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 2 319 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-4(%rbp), %rdx
	movl	$0, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 320 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-8(%rbp), %rdx
	movl	$4, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 321 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-12(%rbp), %rdx
	movl	$8, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 323 0
	movl	-8(%rbp), %eax
	movzbl	%al, %edx
	movq	-24(%rbp), %rax
	movl	%edx, 228(%rax)
	.loc 2 325 0
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	$40, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 327 0
	movl	-8(%rbp), %eax
	movzbl	%al, %edx
	movq	-24(%rbp), %rax
	movl	$41, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 328 0
	movl	-8(%rbp), %eax
	shrl	$8, %eax
	movzbl	%al, %edx
	movq	-24(%rbp), %rax
	movl	$42, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 329 0
	movl	-8(%rbp), %eax
	shrl	$16, %eax
	movzbl	%al, %edx
	movq	-24(%rbp), %rax
	movl	$43, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 330 0
	movl	-8(%rbp), %eax
	shrl	$24, %eax
	andl	$63, %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	$44, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 331 0
	movl	-8(%rbp), %eax
	shrl	$30, %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	$45, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 333 0
	movl	-12(%rbp), %eax
	movzwl	%ax, %edx
	movq	-24(%rbp), %rax
	movl	$46, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 334 0
	movl	-12(%rbp), %eax
	shrl	$16, %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	$47, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 335 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	ReturnDevInfo, .-ReturnDevInfo
	.type	ReturnClbInfo, @function
ReturnClbInfo:
.LFB23:
	.loc 2 338 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 2 343 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-8(%rbp), %rdx
	movl	$256, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 344 0
	movl	$0, -4(%rbp)
	jmp	.L10
.L11:
	.loc 2 347 0 discriminator 2
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	$14, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	.loc 2 346 0 discriminator 2
	andl	$1, %eax
	movl	%eax, %edx
	movl	-4(%rbp), %eax
	leal	82(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 349 0 discriminator 2
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	.loc 2 350 0 discriminator 2
	movl	-4(%rbp), %edx
	subl	$-128, %edx
	sall	$2, %edx
	.loc 2 349 0 discriminator 2
	movl	%edx, %esi
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-12(%rbp), %rdx
	movl	%ecx, %edi
	call	*%rax
	.loc 2 353 0 discriminator 2
	movl	-12(%rbp), %edx
	movl	-4(%rbp), %eax
	leal	86(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 355 0 discriminator 2
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	.loc 2 356 0 discriminator 2
	movl	-4(%rbp), %edx
	addl	$132, %edx
	sall	$2, %edx
	.loc 2 355 0 discriminator 2
	movl	%edx, %esi
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-12(%rbp), %rdx
	movl	%ecx, %edi
	call	*%rax
	.loc 2 359 0 discriminator 2
	movl	-12(%rbp), %edx
	movl	-4(%rbp), %eax
	leal	90(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 361 0 discriminator 2
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	.loc 2 362 0 discriminator 2
	movl	-4(%rbp), %edx
	addl	$136, %edx
	sall	$2, %edx
	.loc 2 361 0 discriminator 2
	movl	%edx, %esi
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-12(%rbp), %rdx
	movl	%ecx, %edi
	call	*%rax
	.loc 2 365 0 discriminator 2
	movl	-12(%rbp), %edx
	movl	-4(%rbp), %eax
	leal	94(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 344 0 discriminator 2
	addl	$1, -4(%rbp)
.L10:
	.loc 2 344 0 is_stmt 0 discriminator 1
	cmpl	$3, -4(%rbp)
	jle	.L11
	.loc 2 367 0 is_stmt 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	ReturnClbInfo, .-ReturnClbInfo
	.type	ValidateParam, @function
ValidateParam:
.LFB24:
	.loc 2 370 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	.loc 2 371 0
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	.loc 2 375 0
	cmpl	$22, -28(%rbp)
	jne	.L13
	.loc 2 377 0
	cmpl	$0, -32(%rbp)
	je	.L15
	.loc 2 377 0 is_stmt 0 discriminator 1
	cmpl	$1, -32(%rbp)
	je	.L15
	.loc 2 378 0 is_stmt 1
	cmpl	$2, -32(%rbp)
	je	.L15
	.loc 2 379 0
	movl	$0, -32(%rbp)
	jmp	.L15
.L13:
	.loc 2 381 0
	cmpl	$23, -28(%rbp)
	jne	.L16
	.loc 2 383 0
	movq	-8(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$7, %eax
	jg	.L17
	.loc 2 383 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
	jmp	.L15
.L17:
	.loc 2 385 0 is_stmt 1
	movq	-8(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$998, %eax
	jg	.L19
	.loc 2 385 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
	jmp	.L15
.L19:
	.loc 2 387 0 is_stmt 1
	cmpl	$0, -32(%rbp)
	jns	.L20
	.loc 2 387 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
	jmp	.L15
.L20:
	.loc 2 388 0 is_stmt 1
	cmpl	$65535, -32(%rbp)
	jle	.L15
	.loc 2 388 0 is_stmt 0 discriminator 1
	movl	$65535, -32(%rbp)
	jmp	.L15
.L16:
	.loc 2 390 0 is_stmt 1
	cmpl	$23, -28(%rbp)
	jle	.L21
	.loc 2 390 0 is_stmt 0 discriminator 1
	cmpl	$27, -28(%rbp)
	jg	.L21
	.loc 2 392 0 is_stmt 1
	cmpl	$0, -32(%rbp)
	jns	.L22
	.loc 2 392 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
.L22:
	.loc 2 393 0 is_stmt 1
	cmpl	$3, -32(%rbp)
	jle	.L23
	.loc 2 393 0 is_stmt 0 discriminator 1
	movl	$3, -32(%rbp)
	jmp	.L15
.L23:
	.loc 2 393 0 discriminator 2
	jmp	.L15
.L21:
	.loc 2 395 0 is_stmt 1
	cmpl	$28, -28(%rbp)
	jne	.L24
	.loc 2 397 0
	cmpl	$0, -32(%rbp)
	je	.L15
	.loc 2 397 0 is_stmt 0 discriminator 1
	cmpl	$1, -32(%rbp)
	je	.L15
	.loc 2 398 0 is_stmt 1
	cmpl	$2, -32(%rbp)
	je	.L15
	.loc 2 399 0
	cmpl	$3, -32(%rbp)
	je	.L15
	.loc 2 400 0
	cmpl	$4, -32(%rbp)
	je	.L15
	.loc 2 401 0
	movl	$0, -32(%rbp)
	jmp	.L15
.L24:
	.loc 2 403 0
	cmpl	$29, -28(%rbp)
	jne	.L26
	.loc 2 405 0
	cmpl	$0, -32(%rbp)
	jns	.L27
	.loc 2 405 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
.L27:
	.loc 2 406 0 is_stmt 1
	cmpl	$7, -32(%rbp)
	jle	.L15
	.loc 2 407 0
	movl	$7, -32(%rbp)
	jmp	.L15
.L26:
	.loc 2 409 0
	cmpl	$20, -28(%rbp)
	jne	.L29
	.loc 2 411 0
	andl	$-2, -32(%rbp)
	.loc 2 412 0
	cmpl	$0, -32(%rbp)
	jns	.L30
	.loc 2 412 0 is_stmt 0 discriminator 1
	movl	$0, -32(%rbp)
.L30:
	.loc 2 413 0 is_stmt 1
	cmpl	$49998, -32(%rbp)
	jle	.L15
	.loc 2 413 0 is_stmt 0 discriminator 1
	movl	$49998, -32(%rbp)
	jmp	.L15
.L29:
	.loc 2 415 0 is_stmt 1
	cmpl	$21, -28(%rbp)
	jne	.L15
	.loc 2 417 0
	cmpl	$1, -32(%rbp)
	jg	.L32
	.loc 2 417 0 is_stmt 0 discriminator 1
	movl	$2, -32(%rbp)
.L32:
	.loc 2 418 0 is_stmt 1
	cmpl	$50000, -32(%rbp)
	jle	.L15
	.loc 2 418 0 is_stmt 0 discriminator 1
	movl	$50000, -32(%rbp)
.L15:
	.loc 2 421 0 is_stmt 1
	movl	-32(%rbp), %eax
	.loc 2 422 0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE24:
	.size	ValidateParam, .-ValidateParam
	.section	.rodata
.LC20:
	.string	"[%d] %s %d: %d 0x%08x 0x%08x\n"
	.text
	.type	ActivatePLLPreset, @function
ActivatePLLPreset:
.LFB25:
	.loc 2 425 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	.loc 2 426 0
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	8(%rax,%rdx), %esi
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	4(%rax,%rdx), %ecx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	(%rax,%rdx), %r8d
	movq	-8(%rbp), %rax
	movl	208(%rax), %edx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	-12(%rbp), %edi
	movl	%esi, 8(%rsp)
	movl	%ecx, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	leaq	__FUNCTION__.5952(%rip), %rcx
	leaq	.LC20(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 427 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	(%rax,%rdx), %edx
	movq	-8(%rbp), %rax
	movl	212(%rax), %eax
	movl	$300, %esi
	movl	%eax, %edi
	call	*%rcx
	.loc 2 428 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	4(%rax,%rdx), %edx
	movq	-8(%rbp), %rax
	movl	212(%rax), %eax
	movl	$304, %esi
	movl	%eax, %edi
	call	*%rcx
	.loc 2 429 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	8(%rax,%rdx), %edx
	movq	-8(%rbp), %rax
	movl	212(%rax), %eax
	movl	$308, %esi
	movl	%eax, %edi
	call	*%rcx
	.loc 2 430 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-8(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$1, %edx
	movl	$296, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 431 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE25:
	.size	ActivatePLLPreset, .-ActivatePLLPreset
	.type	Init1Param, @function
Init1Param:
.LFB26:
	.loc 2 442 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	.loc 2 443 0
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %eax
	testl	%eax, %eax
	jns	.L36
	.loc 2 443 0 is_stmt 0 discriminator 1
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	leaq	196(%rdx), %rcx
	movl	-16(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
.L36:
	.loc 2 444 0 is_stmt 1
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	ValidateParam
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$196, %rcx
	movl	%eax, 8(%rdx,%rcx,4)
	.loc 2 445 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE26:
	.size	Init1Param, .-Init1Param
	.section	.rodata
	.align 8
.LC21:
	.string	"WARNING: irq_n=%d, but ADC4x250 supports only =5"
.LC22:
	.string	"%s *** %s[%d] ***\n"
.LC23:
	.string	"\t\tTIMER=%08x r=%d\n"
	.align 8
.LC24:
	.string	"a32rd32(ADC4X250_R_TIMER): %d; probably a missing/unconfigured device, terminating"
.LC25:
	.string	"\t\tTRIG_SOURCE=%08x\n"
	.align 8
.LC26:
	.string	"\t\t[ADC250_CHAN_TRIG_TYPE]=%d [ADC250_CHAN_TRIG_INPUT]=%d\n"
	.text
	.type	InitParams, @function
InitParams:
.LFB27:
	.loc 2 447 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	.loc 2 448 0
	movq	-56(%rbp), %rax
	movq	%rax, -32(%rbp)
	.loc 2 461 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 462 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$20, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 465 0
	movq	-32(%rbp), %rax
	movl	216(%rax), %eax
	cmpl	$5, %eax
	je	.L38
	.loc 2 466 0
	movq	-32(%rbp), %rax
	movl	216(%rax), %edx
	movq	-32(%rbp), %rax
	movl	208(%rax), %eax
	movl	%edx, %ecx
	leaq	.LC21(%rip), %rdx
	movl	$0, %esi
	movl	%eax, %edi
	movl	$0, %eax
	call	DoDriverLog@PLT
.L38:
	.loc 2 467 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	216(%rdx), %edx
	movq	-32(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$28, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 468 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	220(%rdx), %edx
	movq	-32(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$24, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 469 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$7, %edx
	movl	$16, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 474 0
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	ReturnDevInfo
	.loc 2 475 0
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	ReturnClbInfo
	.loc 2 478 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$264, %esi
	movl	%ecx, %edi
	call	*%rax
	movl	%eax, -36(%rbp)
	.loc 2 479 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %ebx
	call	strcurtime_msc@PLT
	movq	%rax, %rdx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	%ebx, %r8d
	leaq	__FUNCTION__.5973(%rip), %rcx
	leaq	.LC22(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 480 0
	movl	-40(%rbp), %edx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %ecx
	leaq	.LC23(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 481 0
	cmpl	$0, -36(%rbp)
	jns	.L39
	.loc 2 483 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %eax
	movl	-36(%rbp), %edx
	movl	%edx, %ecx
	leaq	.LC24(%rip), %rdx
	movl	$0, %esi
	movl	%eax, %edi
	movl	$0, %eax
	call	DoDriverLog@PLT
	.loc 2 484 0
	movl	$-1, %eax
	jmp	.L54
.L39:
	.loc 2 486 0
	movl	-40(%rbp), %eax
	cmpl	$1, %eax
	ja	.L41
	.loc 2 486 0 is_stmt 0 discriminator 1
	movl	$1024, -40(%rbp)
.L41:
	.loc 2 487 0 is_stmt 1
	movl	-40(%rbp), %eax
	cmpl	$50000, %eax
	jbe	.L42
	.loc 2 487 0 is_stmt 0 discriminator 1
	movl	$50000, -40(%rbp)
.L42:
	.loc 2 488 0 is_stmt 1
	movl	-40(%rbp), %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$21, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 489 0
	movq	-32(%rbp), %rax
	movl	$0, %edx
	movl	$20, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 491 0
	movq	-32(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$7, %eax
	jg	.L43
	.loc 2 491 0 is_stmt 0 discriminator 1
	movl	$0, -40(%rbp)
	jmp	.L44
.L43:
	.loc 2 493 0 is_stmt 1
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$268, %esi
	movl	%ecx, %edi
	call	*%rax
.L44:
	.loc 2 494 0
	movl	-40(%rbp), %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$23, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 496 0
	movq	-32(%rbp), %rax
	movl	948(%rax), %eax
	movl	%eax, -20(%rbp)
	.loc 2 497 0
	cmpl	$0, -20(%rbp)
	jle	.L45
	.loc 2 497 0 is_stmt 0 discriminator 1
	cmpl	$2, -20(%rbp)
	jg	.L45
	.loc 2 499 0 is_stmt 1
	movl	-20(%rbp), %edx
	movq	-32(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	ActivatePLLPreset
	.loc 2 500 0
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	(%rax,%rdx), %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$22, %esi
	movq	%rax, %rdi
	call	Init1Param
	jmp	.L46
.L45:
	.loc 2 504 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$300, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 505 0
	movl	-40(%rbp), %eax
	andl	$3, %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$22, %esi
	movq	%rax, %rdi
	call	Init1Param
.L46:
	.loc 2 509 0
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$276, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 510 0
	movl	-40(%rbp), %edx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	leaq	.LC25(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 511 0
	movl	-40(%rbp), %eax
	andl	$7, %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$28, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 515 0
	movl	-40(%rbp), %eax
	shrl	$3, %eax
	.loc 2 514 0
	andl	$7, %eax
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movl	$29, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 517 0
	movq	-32(%rbp), %rax
	movl	908(%rax), %ecx
	movq	-32(%rbp), %rax
	movl	904(%rax), %edx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	leaq	.LC26(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 519 0
	movl	$0, -20(%rbp)
	jmp	.L47
.L48:
	.loc 2 521 0 discriminator 2
	movq	-32(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	.loc 2 522 0 discriminator 2
	movl	-20(%rbp), %edx
	addl	$78, %edx
	sall	$2, %edx
	.loc 2 521 0 discriminator 2
	movl	%edx, %esi
	movq	-32(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	%ecx, %edi
	call	*%rax
	.loc 2 524 0 discriminator 2
	movl	-40(%rbp), %eax
	andl	$3, %eax
	movl	%eax, %edx
	movl	-20(%rbp), %eax
	leal	24(%rax), %ecx
	movq	-32(%rbp), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Init1Param
	.loc 2 519 0 discriminator 2
	addl	$1, -20(%rbp)
.L47:
	.loc 2 519 0 is_stmt 0 discriminator 1
	cmpl	$0, -20(%rbp)
	jle	.L48
	.loc 2 529 0 is_stmt 1
	movq	-32(%rbp), %rax
	movl	$-9, %edx
	movl	$54, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 530 0
	movl	$0, -20(%rbp)
	jmp	.L49
.L50:
	.loc 2 532 0 discriminator 2
	movl	-20(%rbp), %eax
	leal	116(%rax), %ecx
	movq	-32(%rbp), %rax
	movl	$-32767, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 533 0 discriminator 2
	movl	-20(%rbp), %eax
	leal	120(%rax), %ecx
	movq	-32(%rbp), %rax
	movl	$32767, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 530 0 discriminator 2
	addl	$1, -20(%rbp)
.L49:
	.loc 2 530 0 is_stmt 0 discriminator 1
	cmpl	$0, -20(%rbp)
	jle	.L50
	.loc 2 535 0 is_stmt 1
	movq	-32(%rbp), %rax
	movl	$1, %edx
	movl	$124, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 537 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %ecx
	movabsq	$4661225614328463360, %rax
	xorpd	%xmm1, %xmm1
	movq	%rax, -64(%rbp)
	movsd	-64(%rbp), %xmm0
	movl	$1, %edx
	movl	$0, %esi
	movl	%ecx, %edi
	call	SetChanRDs@PLT
	.loc 2 542 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %ecx
	movabsq	$4661225614328463360, %rax
	xorpd	%xmm1, %xmm1
	movq	%rax, -64(%rbp)
	movsd	-64(%rbp), %xmm0
	movl	$16, %edx
	movl	$100, %esi
	movl	%ecx, %edi
	call	SetChanRDs@PLT
	.loc 2 545 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %ecx
	movabsq	$4652007308841189376, %rax
	xorpd	%xmm1, %xmm1
	movq	%rax, -64(%rbp)
	movsd	-64(%rbp), %xmm0
	movl	$1, %edx
	movl	$50, %esi
	movl	%ecx, %edi
	call	SetChanRDs@PLT
	.loc 2 548 0
	movq	-32(%rbp), %rax
	movl	208(%rax), %eax
	movq	-32(%rbp), %rsi
	movl	$0, %r8d
	leaq	adcNNN_hbt(%rip), %rcx
	movl	$100000, %edx
	movl	%eax, %edi
	call	sl_enq_tout_after@PLT
	.loc 2 552 0
	movl	$0, -20(%rbp)
	jmp	.L51
.L53:
	.loc 2 553 0
	movl	-20(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$2, %eax
	jne	.L52
	.loc 2 554 0 discriminator 1
	movq	-32(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %eax
	.loc 2 553 0 discriminator 1
	testl	%eax, %eax
	js	.L52
	.loc 2 555 0
	movq	-32(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-32(%rbp), %rax
	movl	-20(%rbp), %esi
	movl	$2, %ecx
	movq	%rax, %rdi
	call	pzframe_drv_rw_p@PLT
.L52:
	.loc 2 552 0
	addl	$1, -20(%rbp)
.L51:
	.loc 2 552 0 is_stmt 0 discriminator 1
	cmpl	$124, -20(%rbp)
	jle	.L53
	.loc 2 557 0 is_stmt 1
	movl	$1, %eax
.L54:
	.loc 2 558 0
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE27:
	.size	InitParams, .-InitParams
	.type	StopDevice, @function
StopDevice:
.LFB28:
	.loc 2 560 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 2 561 0
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	.loc 2 563 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-8(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$0, %edx
	movl	$24, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 564 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-8(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$0, %edx
	movl	$16, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 565 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE28:
	.size	StopDevice, .-StopDevice
	.type	StartMeasurements, @function
StartMeasurements:
.LFB29:
	.loc 2 568 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	.loc 2 569 0
	movq	-40(%rbp), %rax
	movq	%rax, -16(%rbp)
	.loc 2 575 0
	movq	-16(%rbp), %rax
	leaq	792(%rax), %rcx
	movq	-16(%rbp), %rax
	addq	$232, %rax
	movl	$560, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 2 578 0
	movq	-16(%rbp), %rax
	movl	312(%rax), %edx
	movq	-16(%rbp), %rax
	movl	316(%rax), %eax
	movl	$50000, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	cmpl	%eax, %edx
	jle	.L64
	.loc 2 579 0
	movq	-16(%rbp), %rax
	movl	316(%rax), %eax
	movl	$50000, %edx
	subl	%eax, %edx
	movq	-16(%rbp), %rax
	movl	$20, %esi
	movq	%rax, %rdi
	call	Return1Param
.L64:
	.loc 2 586 0
	movq	-16(%rbp), %rax
	leaq	232(%rax), %rcx
	movq	-16(%rbp), %rax
	addq	$1352, %rax
	movl	$560, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 2 589 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 590 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-20(%rbp), %rdx
	movl	$20, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 592 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 593 0
	movq	-16(%rbp), %rdx
	movl	312(%rdx), %ecx
	.loc 2 594 0
	movq	-16(%rbp), %rdx
	movl	316(%rdx), %edx
	.loc 2 593 0
	addl	%ecx, %edx
	.loc 2 592 0
	movq	-16(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$264, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 597 0
	movq	-16(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$7, %eax
	jle	.L58
	.loc 2 598 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 599 0
	movq	-16(%rbp), %rdx
	movl	324(%rdx), %edx
	.loc 2 598 0
	movq	-16(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$268, %esi
	movl	%ecx, %edi
	call	*%rax
.L58:
	.loc 2 600 0
	movl	$0, -4(%rbp)
	jmp	.L59
.L60:
	.loc 2 602 0 discriminator 2
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 604 0 discriminator 2
	movl	-4(%rbp), %edx
	leal	24(%rdx), %ecx
	movq	-16(%rbp), %rdx
	movslq	%ecx, %rcx
	addq	$56, %rcx
	movl	8(%rdx,%rcx,4), %edx
	.loc 2 603 0 discriminator 2
	movl	-4(%rbp), %ecx
	addl	$78, %ecx
	sall	$2, %ecx
	.loc 2 602 0 discriminator 2
	movl	%ecx, %esi
	movq	-16(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	%ecx, %edi
	call	*%rax
	.loc 2 600 0 discriminator 2
	addl	$1, -4(%rbp)
.L59:
	.loc 2 600 0 is_stmt 0 discriminator 1
	cmpl	$0, -4(%rbp)
	jle	.L60
	.loc 2 606 0 is_stmt 1
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 607 0
	movq	-16(%rbp), %rdx
	movl	344(%rdx), %edx
	movl	%edx, %ecx
	andl	$7, %ecx
	.loc 2 609 0
	movq	-16(%rbp), %rdx
	movl	348(%rdx), %edx
	andl	$7, %edx
	sall	$3, %edx
	.loc 2 608 0
	orl	%ecx, %edx
	.loc 2 606 0
	movq	-16(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$276, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 613 0
	movq	-16(%rbp), %rax
	movl	288(%rax), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L61
	.loc 2 615 0
	movq	-16(%rbp), %rax
	movl	$4, 101984(%rax)
	.loc 2 616 0
	movq	-16(%rbp), %rax
	movl	$0, %edx
	movl	$14, %esi
	movq	%rax, %rdi
	call	Return1Param
	jmp	.L62
.L61:
	.loc 2 619 0
	movq	-16(%rbp), %rax
	movl	$1, 101984(%rax)
.L62:
	.loc 2 621 0
	movq	-16(%rbp), %rax
	movl	101984(%rax), %eax
	cmpl	$4, %eax
	setne	%al
	movzbl	%al, %edx
	movq	-16(%rbp), %rax
	movl	%edx, 101916(%rax)
	.loc 2 622 0
	movq	-16(%rbp), %rax
	movl	101984(%rax), %eax
	cmpl	$4, %eax
	sete	%al
	movzbl	%al, %edx
	movq	-16(%rbp), %rax
	movl	%edx, 101920(%rax)
	.loc 2 630 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	101984(%rdx), %edx
	movq	-16(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 632 0
	movl	$0, %eax
	.loc 2 633 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE29:
	.size	StartMeasurements, .-StartMeasurements
	.type	TrggrMeasurements, @function
TrggrMeasurements:
.LFB30:
	.loc 2 636 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 2 637 0
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	.loc 2 640 0
	movq	-8(%rbp), %rax
	movl	101984(%rax), %eax
	cmpl	$4, %eax
	jne	.L66
	.loc 2 640 0 is_stmt 0 discriminator 1
	movl	$0, %eax
	jmp	.L67
.L66:
	.loc 2 643 0 is_stmt 1
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 646 0
	movq	-8(%rbp), %rdx
	movl	348(%rdx), %edx
	sall	$3, %edx
	.loc 2 645 0
	orl	$1, %edx
	.loc 2 643 0
	movq	-8(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$276, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 650 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-8(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 652 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-8(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$1, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 655 0
	movq	-8(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	.loc 2 656 0
	movq	-8(%rbp), %rdx
	movl	344(%rdx), %edx
	movl	%edx, %ecx
	andl	$7, %ecx
	.loc 2 658 0
	movq	-8(%rbp), %rdx
	movl	348(%rdx), %edx
	andl	$7, %edx
	sall	$3, %edx
	.loc 2 657 0
	orl	%ecx, %edx
	.loc 2 655 0
	movq	-8(%rbp), %rcx
	movl	212(%rcx), %ecx
	movl	$276, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 660 0
	movl	$0, %eax
.L67:
	.loc 2 661 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE30:
	.size	TrggrMeasurements, .-TrggrMeasurements
	.type	AbortMeasurements, @function
AbortMeasurements:
.LFB31:
	.loc 2 664 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	.loc 2 665 0
	movq	-40(%rbp), %rax
	movq	%rax, -16(%rbp)
	.loc 2 669 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 670 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-20(%rbp), %rdx
	movl	$20, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 672 0
	movq	-16(%rbp), %rax
	movl	316(%rax), %eax
	testl	%eax, %eax
	je	.L69
	.loc 2 676 0
	movq	-16(%rbp), %rax
	movl	316(%rax), %eax
	.loc 2 673 0
	cltq
	leaq	(%rax,%rax), %rdx
	movq	-16(%rbp), %rax
	addq	$1912, %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	bzero@PLT
.L69:
	.loc 2 678 0
	movl	$100, -4(%rbp)
	jmp	.L70
.L71:
	.loc 2 679 0 discriminator 2
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	$0, 8(%rax,%rdx,4)
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$56, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 678 0 discriminator 2
	addl	$1, -4(%rbp)
.L70:
	.loc 2 678 0 is_stmt 0 discriminator 1
	cmpl	$115, -4(%rbp)
	jle	.L71
	.loc 2 681 0 is_stmt 1
	movl	$1, %eax
	.loc 2 682 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE31:
	.size	AbortMeasurements, .-AbortMeasurements
	.type	ReadMeasurements, @function
ReadMeasurements:
.LFB32:
	.loc 2 685 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$120, %rsp
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -152(%rbp)
	.loc 2 686 0
	movq	-152(%rbp), %rax
	movq	%rax, -80(%rbp)
	.loc 2 704 0
	movq	-80(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-80(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-128(%rbp), %rdx
	movl	$20, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 706 0
	movq	-80(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-80(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
.LBB2:
	.loc 2 718 0
	movq	-80(%rbp), %rax
	addq	$114280, %rax
	movq	%rax, -88(%rbp)
	movq	-80(%rbp), %rax
	addq	$101992, %rax
	movq	%rax, -96(%rbp)
	movq	-80(%rbp), %rax
	addq	$106088, %rax
	movq	%rax, -104(%rbp)
	movq	-80(%rbp), %rax
	addq	$110184, %rax
	movq	%rax, -112(%rbp)
	.loc 2 720 0
	movq	-88(%rbp), %r12
	movq	-96(%rbp), %r13
	movq	-104(%rbp), %r14
	movq	-112(%rbp), %r15
	.loc 2 721 0
	movq	-80(%rbp), %rax
	leaq	1912(%rax), %rbx
	.loc 2 723 0
	movl	$1024, -116(%rbp)
	.loc 2 724 0
	movq	-80(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$12345678, %eax
	jne	.L74
	.loc 2 725 0
	movl	$0, -68(%rbp)
	jmp	.L75
.L76:
	.loc 2 727 0 discriminator 2
	movq	%r12, %rax
	leaq	4(%rax), %r12
	movl	(%rax), %eax
	movl	%eax, %edi
	.loc 2 728 0 discriminator 2
	movq	%r13, %rax
	leaq	4(%rax), %r13
	movl	(%rax), %eax
	movl	%eax, %esi
	.loc 2 729 0 discriminator 2
	movq	%r14, %rax
	leaq	4(%rax), %r14
	movl	(%rax), %eax
	movl	%eax, %ecx
	.loc 2 730 0 discriminator 2
	movq	%r15, %rax
	leaq	4(%rax), %r15
	movl	(%rax), %eax
	movl	%eax, %r8d
	.loc 2 731 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%edi, %edx
	movw	%dx, (%rax)
	.loc 2 732 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%esi, %edx
	movw	%dx, (%rax)
	.loc 2 733 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%ecx, %edx
	movw	%dx, (%rax)
	.loc 2 734 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%r8d, %edx
	movw	%dx, (%rax)
	.loc 2 735 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%edi, %edx
	shrl	$16, %edx
	movw	%dx, (%rax)
	.loc 2 736 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%esi, %edx
	shrl	$16, %edx
	movw	%dx, (%rax)
	.loc 2 737 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%ecx, %edx
	shrl	$16, %edx
	movw	%dx, (%rax)
	.loc 2 738 0 discriminator 2
	movq	%rbx, %rax
	leaq	2(%rax), %rbx
	movl	%r8d, %edx
	shrl	$16, %edx
	movw	%dx, (%rax)
	.loc 2 725 0 discriminator 2
	addl	$1, -68(%rbp)
.L75:
	.loc 2 725 0 is_stmt 0 discriminator 1
	movl	-68(%rbp), %eax
	cmpl	-116(%rbp), %eax
	jl	.L76
.L74:
.LBE2:
	.loc 2 743 0 is_stmt 1
	movq	-80(%rbp), %rax
	movl	316(%rax), %eax
	addl	$1, %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	movl	%eax, -116(%rbp)
	.loc 2 744 0
	movl	$0, -52(%rbp)
	jmp	.L77
.L81:
	.loc 2 746 0
	movq	-80(%rbp), %rax
	movl	316(%rax), %eax
	imull	-52(%rbp), %eax
	cltq
	addq	$952, %rax
	leaq	(%rax,%rax), %rdx
	movq	-80(%rbp), %rax
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, -64(%rbp)
	.loc 2 747 0
	movl	-52(%rbp), %eax
	addl	$2, %eax
	sall	$19, %eax
	movl	%eax, %edx
	.loc 2 748 0
	movq	-80(%rbp), %rax
	movl	312(%rax), %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%ecx, %eax
	sarl	%eax
	.loc 2 747 0
	addl	%edx, %eax
	.loc 2 748 0
	addl	$36, %eax
	sall	$2, %eax
	.loc 2 747 0
	movl	%eax, -120(%rbp)
	.loc 2 752 0
	movq	-80(%rbp), %rax
	movq	200(%rax), %rax
	movq	304(%rax), %rax
	movq	-80(%rbp), %rdx
	movl	212(%rdx), %edi
	movl	-116(%rbp), %ecx
	movq	-64(%rbp), %rdx
	movl	-120(%rbp), %esi
	call	*%rax
	movl	%eax, -68(%rbp)
	.loc 2 756 0
	movq	-80(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$6, %eax
	jg	.L78
	.loc 2 757 0
	movl	-116(%rbp), %eax
	movl	%eax, -68(%rbp)
	jmp	.L79
.L80:
	.loc 2 759 0 discriminator 2
	movq	-64(%rbp), %rax
	movzwl	(%rax), %eax
	movw	%ax, -122(%rbp)
	.loc 2 760 0 discriminator 2
	movq	-64(%rbp), %rax
	movzwl	2(%rax), %edx
	movq	-64(%rbp), %rax
	movw	%dx, (%rax)
	addq	$2, -64(%rbp)
	.loc 2 761 0 discriminator 2
	movq	-64(%rbp), %rax
	movzwl	-122(%rbp), %edx
	movw	%dx, (%rax)
	addq	$2, -64(%rbp)
	.loc 2 757 0 discriminator 2
	subl	$1, -68(%rbp)
.L79:
	.loc 2 757 0 is_stmt 0 discriminator 1
	cmpl	$0, -68(%rbp)
	jg	.L80
.L78:
	.loc 2 744 0 is_stmt 1
	addl	$1, -52(%rbp)
.L77:
	.loc 2 744 0 is_stmt 0 discriminator 1
	cmpl	$0, -52(%rbp)
	jle	.L81
	.loc 2 780 0 is_stmt 1
	movl	$100, -68(%rbp)
	jmp	.L82
.L83:
	.loc 2 781 0 discriminator 2
	movq	-80(%rbp), %rax
	movl	-68(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	$0, 8(%rax,%rdx,4)
	movq	-80(%rbp), %rax
	movl	-68(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-80(%rbp), %rax
	movl	-68(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$56, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 780 0 discriminator 2
	addl	$1, -68(%rbp)
.L82:
	.loc 2 780 0 is_stmt 0 discriminator 1
	cmpl	$115, -68(%rbp)
	jle	.L83
	.loc 2 784 0 is_stmt 1
	movq	-80(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-80(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-132(%rbp), %rdx
	movl	$256, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 785 0
	movl	$0, -68(%rbp)
	jmp	.L84
.L85:
	.loc 2 786 0 discriminator 2
	movl	-68(%rbp), %eax
	leal	64(%rax), %edi
	.loc 2 787 0 discriminator 2
	movl	-68(%rbp), %eax
	addl	$64, %eax
	.loc 2 788 0 discriminator 2
	movl	-132(%rbp), %esi
	movl	-68(%rbp), %edx
	addl	$10, %edx
	movl	%edx, %ecx
	shrl	%cl, %esi
	movl	%esi, %edx
	.loc 2 787 0 discriminator 2
	movl	%edx, %esi
	andl	$1, %esi
	movq	-80(%rbp), %rdx
	movslq	%eax, %rcx
	addq	$196, %rcx
	movl	%esi, 8(%rdx,%rcx,4)
	movq	-80(%rbp), %rdx
	cltq
	addq	$196, %rax
	movl	8(%rdx,%rax,4), %edx
	.loc 2 786 0 discriminator 2
	movq	-80(%rbp), %rax
	movslq	%edi, %rcx
	addq	$56, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 785 0 discriminator 2
	addl	$1, -68(%rbp)
.L84:
	.loc 2 785 0 is_stmt 0 discriminator 1
	cmpl	$3, -68(%rbp)
	jle	.L85
	.loc 2 791 0 is_stmt 1
	movl	$0, %eax
	.loc 2 792 0
	addq	$120, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE32:
	.size	ReadMeasurements, .-ReadMeasurements
	.type	adcNNN_hbt, @function
adcNNN_hbt:
.LFB33:
	.loc 2 797 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -24(%rbp)
	movq	%rcx, -40(%rbp)
	.loc 2 798 0
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
	.loc 2 801 0
	movq	-32(%rbp), %rsi
	movl	-20(%rbp), %eax
	movl	$0, %r8d
	leaq	adcNNN_hbt(%rip), %rcx
	movl	$100000, %edx
	movl	%eax, %edi
	call	sl_enq_tout_after@PLT
	.loc 2 810 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE33:
	.size	adcNNN_hbt, .-adcNNN_hbt
	.type	PrepareRetbufs, @function
PrepareRetbufs:
.LFB34:
	.loc 2 813 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	.loc 2 814 0
	movq	-24(%rbp), %rax
	movq	%rax, -16(%rbp)
	.loc 2 820 0
	movl	$0, -4(%rbp)
	.loc 2 824 0
	movq	-16(%rbp), %rax
	movl	324(%rax), %eax
	leal	1(%rax), %edx
	movq	-16(%rbp), %rax
	movl	%edx, 440(%rax)
	.loc 2 825 0
	movq	-16(%rbp), %rax
	movl	$1, 444(%rax)
	.loc 2 826 0
	movl	$0, -8(%rbp)
	jmp	.L90
.L91:
	.loc 2 828 0 discriminator 2
	movl	-8(%rbp), %eax
	leal	70(%rax), %edx
	movq	-16(%rbp), %rax
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	$1, 8(%rax,%rdx,4)
	.loc 2 829 0 discriminator 2
	movl	-8(%rbp), %eax
	leal	74(%rax), %edx
	movq	-16(%rbp), %rax
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	$-32767, 8(%rax,%rdx,4)
	.loc 2 832 0 discriminator 2
	movl	-8(%rbp), %eax
	leal	78(%rax), %edx
	movq	-16(%rbp), %rax
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	$32767, 8(%rax,%rdx,4)
	.loc 2 826 0 discriminator 2
	addl	$1, -8(%rbp)
.L90:
	.loc 2 826 0 is_stmt 0 discriminator 1
	cmpl	$0, -8(%rbp)
	jle	.L91
	.loc 2 840 0 is_stmt 1
	movq	-16(%rbp), %rax
	movl	300(%rax), %eax
	testl	%eax, %eax
	je	.L92
	.loc 2 841 0
	movl	$100, -8(%rbp)
	jmp	.L93
.L94:
	.loc 2 843 0 discriminator 2
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	leaq	29592(%rdx), %rcx
	movl	-8(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 844 0 discriminator 2
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29732, %rdx
	movl	$10, 8(%rax,%rdx,4)
	.loc 2 845 0 discriminator 2
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29872, %rdx
	movl	$1, 8(%rax,%rdx,4)
	.loc 2 846 0 discriminator 2
	movl	-8(%rbp), %eax
	cltq
	addq	$56, %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	leaq	8(%rax), %rcx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$15006, %rdx
	movq	%rcx, 8(%rax,%rdx,8)
	.loc 2 847 0 discriminator 2
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$30292, %rdx
	movl	$0, 8(%rax,%rdx,4)
	.loc 2 848 0 discriminator 2
	addl	$1, -4(%rbp)
	.loc 2 841 0 discriminator 2
	addl	$1, -8(%rbp)
.L93:
	.loc 2 841 0 is_stmt 0 discriminator 1
	cmpl	$115, -8(%rbp)
	jle	.L94
.L92:
	.loc 2 854 0 is_stmt 1
	movl	$0, -8(%rbp)
	jmp	.L95
.L98:
	.loc 2 855 0
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$5, %eax
	jne	.L96
	.loc 2 857 0
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	4(%rdx,%rax), %eax
	testl	%eax, %eax
	js	.L97
	.loc 2 858 0
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	4(%rdx,%rax), %edx
	movq	-16(%rbp), %rax
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-16(%rbp), %rax
	movl	-8(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$56, %rcx
	movl	%edx, 8(%rax,%rcx,4)
.L97:
	.loc 2 859 0
	movq	-16(%rbp), %rax
	movl	-8(%rbp), %edx
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	-16(%rbp), %rax
	movl	-8(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$196, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 860 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	leaq	29592(%rdx), %rcx
	movl	-8(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 861 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29732, %rdx
	movl	$10, 8(%rax,%rdx,4)
	.loc 2 862 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29872, %rdx
	movl	$1, 8(%rax,%rdx,4)
	.loc 2 863 0
	movl	-8(%rbp), %eax
	cltq
	addq	$56, %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	leaq	8(%rax), %rcx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$15006, %rdx
	movq	%rcx, 8(%rax,%rdx,8)
	.loc 2 864 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$30292, %rdx
	movl	$0, 8(%rax,%rdx,4)
	.loc 2 865 0
	addl	$1, -4(%rbp)
.L96:
	.loc 2 854 0
	addl	$1, -8(%rbp)
.L95:
	.loc 2 854 0 is_stmt 0 discriminator 1
	cmpl	$124, -8(%rbp)
	jle	.L98
	.loc 2 869 0 is_stmt 1
	movq	-16(%rbp), %rax
	movl	101988(%rax), %eax
	testl	%eax, %eax
	je	.L99
	.loc 2 871 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29592, %rdx
	movl	$0, 8(%rax,%rdx,4)
	.loc 2 872 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29732, %rdx
	movl	$9, 8(%rax,%rdx,4)
	.loc 2 873 0
	movq	-16(%rbp), %rax
	movl	316(%rax), %edx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$29872, %rcx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 874 0
	movq	-16(%rbp), %rax
	leaq	1912(%rax), %rcx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$15006, %rdx
	movq	%rcx, 8(%rax,%rdx,8)
	.loc 2 875 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	leaq	30292(%rdx), %rcx
	movl	-28(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 876 0
	addl	$1, -4(%rbp)
.L99:
	.loc 2 891 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29592, %rdx
	movl	$9, 8(%rax,%rdx,4)
	.loc 2 892 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29732, %rdx
	movl	$10, 8(%rax,%rdx,4)
	.loc 2 893 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$29872, %rdx
	movl	$1, 8(%rax,%rdx,4)
	.loc 2 894 0
	movq	-16(%rbp), %rax
	leaq	268(%rax), %rcx
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	addq	$15006, %rdx
	movq	%rcx, 8(%rax,%rdx,8)
	.loc 2 895 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	leaq	30292(%rdx), %rcx
	movl	-28(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	.loc 2 896 0
	addl	$1, -4(%rbp)
	.loc 2 898 0
	movq	-16(%rbp), %rax
	movl	$0, 101988(%rax)
	.loc 2 904 0
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %edx
	movl	%edx, 96(%rax)
	.loc 2 905 0
	movq	-16(%rbp), %rax
	leaq	118376(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, 104(%rax)
	.loc 2 906 0
	movq	-16(%rbp), %rax
	leaq	118936(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, 112(%rax)
	.loc 2 907 0
	movq	-16(%rbp), %rax
	leaq	119496(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, 120(%rax)
	.loc 2 908 0
	movq	-16(%rbp), %rax
	leaq	120056(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, 128(%rax)
	.loc 2 909 0
	movq	-16(%rbp), %rax
	leaq	121176(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, 136(%rax)
	.loc 2 910 0
	movq	-16(%rbp), %rax
	movq	$0, 144(%rax)
	.loc 2 918 0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE34:
	.size	PrepareRetbufs, .-PrepareRetbufs
	.section	.rodata
	.align 8
.LC30:
	.string	"\t[%d] detectPLL: %d: %d 0x%08x 0x%08x\n"
	.text
	.type	adc1000_rw_p, @function
adc1000_rw_p:
.LFB35:
	.loc 2 924 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movl	%edx, -56(%rbp)
	movl	%ecx, -68(%rbp)
	movq	%r8, -80(%rbp)
	movq	%r9, -88(%rbp)
	.loc 2 925 0
	movq	-64(%rbp), %rax
	movq	%rax, -16(%rbp)
	.loc 2 938 0
	movl	$0, -4(%rbp)
	jmp	.L101
.L127:
	.loc 2 940 0
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-80(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	.loc 2 943 0
	cmpl	$51, -20(%rbp)
	jne	.L102
	.loc 2 945 0
	movq	-16(%rbp), %rax
	movl	156(%rax), %edx
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	.loc 2 946 0
	jmp	.L103
.L102:
	.loc 2 950 0
	cmpl	$0, -20(%rbp)
	js	.L104
	.loc 2 950 0 is_stmt 0 discriminator 1
	cmpl	$124, -20(%rbp)
	jg	.L104
	.loc 2 951 0 is_stmt 1 discriminator 1
	movl	-20(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	movl	%eax, -24(%rbp)
	.loc 2 950 0 discriminator 1
	cmpl	$0, -24(%rbp)
	jne	.L105
.L104:
	.loc 2 953 0
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$32, %ecx
	movl	$0, %edx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L105:
	.loc 2 955 0
	cmpl	$1, -24(%rbp)
	jne	.L106
	.loc 2 958 0
	cmpl	$0, -20(%rbp)
	jne	.L107
	.loc 2 959 0
	movq	-16(%rbp), %rax
	movl	$1, 101988(%rax)
.L107:
	.loc 2 966 0
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	pzframe_drv_req_mes@PLT
	jmp	.L103
.L106:
	.loc 2 968 0
	cmpl	$7, -24(%rbp)
	je	.L103
	.loc 2 972 0
	cmpl	$2, -56(%rbp)
	jne	.L108
	.loc 2 974 0
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	jne	.L103
	.loc 2 975 0 discriminator 1
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-88(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	.loc 2 974 0 discriminator 1
	cmpl	$10, %eax
	je	.L109
	.loc 2 975 0
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-88(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	$138, %eax
	jne	.L103
.L109:
	.loc 2 977 0
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	(%rax), %eax
	movl	%eax, -8(%rbp)
	jmp	.L110
.L108:
	.loc 2 981 0
	movl	$-1, -8(%rbp)
.L110:
	.loc 2 983 0
	cmpl	$5, -24(%rbp)
	je	.L111
	.loc 2 983 0 is_stmt 0 discriminator 1
	cmpl	$6, -24(%rbp)
	jne	.L112
.L111:
	.loc 2 986 0 is_stmt 1
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	8(%rax,%rdx,4), %edx
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L112:
	.loc 2 988 0
	cmpl	$4, -24(%rbp)
	jne	.L113
	.loc 2 990 0
	cmpl	$2, -56(%rbp)
	jne	.L114
	.loc 2 991 0
	movq	-16(%rbp), %rax
	movl	-8(%rbp), %edx
	movl	-20(%rbp), %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	ValidateParam
	movq	-16(%rbp), %rdx
	movl	-20(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$196, %rcx
	movl	%eax, 8(%rdx,%rcx,4)
.L114:
	.loc 2 992 0
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L113:
	.loc 2 994 0
	cmpl	$3, -24(%rbp)
	jne	.L115
	.loc 2 996 0
	cmpl	$14, -20(%rbp)
	jne	.L116
	.loc 2 998 0
	cmpl	$2, -56(%rbp)
	jne	.L117
	.loc 2 999 0
	cmpl	$0, -8(%rbp)
	setne	%al
	movzbl	%al, %edx
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$196, %rcx
	movl	%edx, 8(%rax,%rcx,4)
.L117:
	.loc 2 1000 0
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$196, %rdx
	movl	8(%rax,%rdx,4), %edx
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L116:
	.loc 2 1002 0
	cmpl	$48, -20(%rbp)
	jne	.L119
	.loc 2 1004 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-28(%rbp), %rdx
	movl	$256, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1005 0
	movl	-28(%rbp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %edx
	movl	-20(%rbp), %esi
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L119:
	.loc 2 1007 0
	cmpl	$39, -20(%rbp)
	jne	.L103
	.loc 2 1009 0
	cmpl	$2, -56(%rbp)
	jne	.L120
	.loc 2 1009 0 is_stmt 0 discriminator 1
	cmpl	$0, -8(%rbp)
	jle	.L120
	.loc 2 1010 0 is_stmt 1
	cmpl	$2, -8(%rbp)
	jg	.L120
	.loc 2 1012 0
	movl	-8(%rbp), %edx
	movq	-16(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	ActivatePLLPreset
	.loc 2 1013 0
	movq	-16(%rbp), %rax
	movl	228(%rax), %eax
	cmpl	$998, %eax
	jg	.L120
	.loc 2 1015 0
	movl	-8(%rbp), %edx
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	$39, %esi
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	.loc 2 1016 0
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	(%rax,%rdx), %eax
	movl	%eax, %edx
	movq	-16(%rbp), %rax
	movl	$22, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 1017 0
	jmp	.L103
.L120:
	.loc 2 1021 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-32(%rbp), %rdx
	movl	$300, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1022 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-36(%rbp), %rdx
	movl	$304, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1023 0
	movq	-16(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-16(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-40(%rbp), %rdx
	movl	$308, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1025 0
	movl	$2, -8(%rbp)
	jmp	.L121
.L124:
	.loc 2 1026 0
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	(%rax,%rdx), %edx
	movl	-32(%rbp), %eax
	cmpl	%eax, %edx
	jne	.L122
	.loc 2 1027 0 discriminator 1
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	4(%rax,%rdx), %edx
	movl	-36(%rbp), %eax
	.loc 2 1026 0 discriminator 1
	cmpl	%eax, %edx
	jne	.L122
	.loc 2 1028 0
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	pll_presets_table(%rip), %rdx
	movl	8(%rax,%rdx), %edx
	movl	-40(%rbp), %eax
	.loc 2 1027 0
	cmpl	%eax, %edx
	jne	.L122
	.loc 2 1028 0
	jmp	.L123
.L122:
	.loc 2 1025 0
	subl	$1, -8(%rbp)
.L121:
	.loc 2 1025 0 is_stmt 0 discriminator 1
	cmpl	$0, -8(%rbp)
	jg	.L124
.L123:
	.loc 2 1029 0 is_stmt 1
	movl	-40(%rbp), %esi
	movl	-36(%rbp), %r8d
	movl	-32(%rbp), %edi
	movq	-16(%rbp), %rax
	movl	208(%rax), %edx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	-8(%rbp), %ecx
	movl	%esi, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	leaq	.LC30(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 1031 0
	movl	-8(%rbp), %edx
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	$39, %esi
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	.loc 2 1032 0
	movl	-32(%rbp), %eax
	movl	%eax, %edx
	movq	-16(%rbp), %rax
	movl	$22, %esi
	movq	%rax, %rdi
	call	Return1Param
	.loc 2 1033 0
	movl	-36(%rbp), %eax
	movl	%eax, %edx
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	$98, %esi
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	.loc 2 1034 0
	movl	-40(%rbp), %eax
	movl	%eax, %edx
	movl	-52(%rbp), %eax
	movl	$0, %ecx
	movl	$99, %esi
	movl	%eax, %edi
	call	ReturnInt32Datum@PLT
	jmp	.L103
.L115:
	.loc 2 1039 0
	cmpl	$2, -56(%rbp)
	jne	.L125
	.loc 2 1039 0 is_stmt 0 discriminator 1
	movl	-8(%rbp), %eax
	jmp	.L126
.L125:
	.loc 2 1039 0 discriminator 2
	movl	$0, %eax
.L126:
	.loc 2 1039 0 discriminator 3
	movq	-16(%rbp), %rdi
	movl	-56(%rbp), %edx
	movl	-20(%rbp), %esi
	movl	%edx, %ecx
	movl	%eax, %edx
	call	pzframe_drv_rw_p@PLT
.L103:
	.loc 2 938 0 is_stmt 1
	addl	$1, -4(%rbp)
.L101:
	.loc 2 938 0 is_stmt 0 discriminator 1
	movl	-4(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jl	.L127
	.loc 2 1046 0 is_stmt 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE35:
	.size	adc1000_rw_p, .-adc1000_rw_p
	.section	.rodata
.LC31:
	.string	"\r%s devid=%-2d total=%d"
	.text
	.type	adc1000_irq_p, @function
adc1000_irq_p:
.LFB36:
	.loc 2 1087 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movl	%edx, -40(%rbp)
	movl	%ecx, -52(%rbp)
	.loc 2 1088 0
	movq	-48(%rbp), %rax
	movq	%rax, -24(%rbp)
	.loc 2 1094 0
	movq	-24(%rbp), %rax
	movl	220(%rax), %eax
	cmpl	-52(%rbp), %eax
	jne	.L128
	.loc 2 1095 0
	movl	total_irqs.6120(%rip), %eax
	addl	$1, %eax
	movl	%eax, total_irqs.6120(%rip)
	movl	total_irqs.6120(%rip), %ebx
	call	strcurtime_msc@PLT
	movq	%rax, %rdx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	-36(%rbp), %ecx
	movl	%ebx, %r8d
	leaq	.LC31(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 1097 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	160(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	leaq	-28(%rbp), %rdx
	movl	$20, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1099 0
	movq	-24(%rbp), %rax
	movq	200(%rax), %rax
	movq	152(%rax), %rax
	movq	-24(%rbp), %rdx
	movl	212(%rdx), %ecx
	movl	$2, %edx
	movl	$260, %esi
	movl	%ecx, %edi
	call	*%rax
	.loc 2 1103 0
	movq	-24(%rbp), %rax
	movl	101916(%rax), %eax
	testl	%eax, %eax
	jne	.L131
	.loc 2 1103 0 is_stmt 0 discriminator 1
	movq	-24(%rbp), %rax
	movl	101920(%rax), %eax
	testl	%eax, %eax
	je	.L131
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	ReadMeasurements
.L131:
	.loc 2 1104 0 is_stmt 1
	movq	-24(%rbp), %rax
	movl	101916(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	$0, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	pzframe_drv_drdy_p@PLT
.L128:
	.loc 2 1106 0
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE36:
	.size	adc1000_irq_p, .-adc1000_irq_p
	.section	.rodata
	.align 8
.LC32:
	.string	"%s businfo[2]=%08x jumpers=0x%x irq=%d vector=%d\n"
	.text
	.type	adc1000_init_d, @function
adc1000_init_d:
.LFB37:
	.loc 2 1111 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$168, %rsp
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movl	%edx, -72(%rbp)
	movq	%rcx, -88(%rbp)
	movq	%r8, -96(%rbp)
	.loc 2 1112 0
	movq	-80(%rbp), %rax
	movq	%rax, -48(%rbp)
	.loc 2 1118 0
	movq	-88(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -52(%rbp)
	.loc 2 1119 0
	movq	-88(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, -56(%rbp)
	.loc 2 1120 0
	movq	-88(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -60(%rbp)
	.loc 2 1121 0
	movq	-88(%rbp), %rax
	addq	$12, %rax
	movl	(%rax), %eax
	andl	$7, %eax
	movl	%eax, %edx
	movq	-48(%rbp), %rax
	movl	%edx, 216(%rax)
	.loc 2 1122 0
	movq	-88(%rbp), %rax
	addq	$16, %rax
	movl	(%rax), %eax
	movzbl	%al, %edx
	movq	-48(%rbp), %rax
	movl	%edx, 220(%rax)
	.loc 2 1123 0
	movq	-48(%rbp), %rax
	movl	-60(%rbp), %edx
	movl	%edx, 224(%rax)
	.loc 2 1124 0
	movq	-48(%rbp), %rax
	movl	220(%rax), %r12d
	movq	-48(%rbp), %rax
	movl	216(%rax), %r13d
	movq	-88(%rbp), %rax
	addq	$8, %rax
	movl	(%rax), %ebx
	call	strcurtime_msc@PLT
	movq	%rax, %rdx
	movq	stderr@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movl	-60(%rbp), %ecx
	movl	%r12d, (%rsp)
	movl	%r13d, %r9d
	movl	%ecx, %r8d
	movl	%ebx, %ecx
	leaq	.LC32(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	.loc 2 1126 0
	movl	-68(%rbp), %eax
	movl	%eax, %edi
	call	GetLayerVMT@PLT
	movq	-48(%rbp), %rdx
	movq	%rax, 200(%rdx)
	.loc 2 1127 0
	movq	-48(%rbp), %rax
	movl	-68(%rbp), %edx
	movl	%edx, 208(%rax)
	.loc 2 1128 0
	movq	-48(%rbp), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	-48(%rbp), %rdx
	movl	220(%rdx), %r9d
	movq	-48(%rbp), %rdx
	movl	216(%rdx), %r8d
	.loc 2 1130 0
	movl	-60(%rbp), %edx
	sall	$24, %edx
	.loc 2 1128 0
	movl	%edx, %r11d
	movl	-56(%rbp), %ecx
	movl	-52(%rbp), %edx
	movq	-80(%rbp), %rsi
	movl	-68(%rbp), %edi
	movl	$0, 48(%rsp)
	movq	$0, 40(%rsp)
	leaq	adc1000_irq_p(%rip), %r10
	movq	%r10, 32(%rsp)
	movl	%r9d, 24(%rsp)
	movl	%r8d, 16(%rsp)
	movl	$9, 8(%rsp)
	movl	$32, (%rsp)
	movl	$12058624, %r9d
	movl	%r11d, %r8d
	call	*%rax
	movq	-48(%rbp), %rdx
	movl	%eax, 212(%rdx)
	.loc 2 1134 0
	movq	-48(%rbp), %rax
	movl	212(%rax), %eax
	testl	%eax, %eax
	jns	.L133
	.loc 2 1134 0 is_stmt 0 discriminator 1
	movq	-48(%rbp), %rax
	movl	212(%rax), %eax
	jmp	.L134
.L133:
	.loc 2 1136 0 is_stmt 1
	movl	$0, -36(%rbp)
	jmp	.L135
.L139:
	.loc 2 1138 0
	movl	-36(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$6, %eax
	je	.L136
	.loc 2 1139 0 discriminator 1
	movl	-36(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	.loc 2 1138 0 discriminator 1
	cmpl	$5, %eax
	jne	.L137
.L136:
	.loc 2 1140 0
	movq	-48(%rbp), %rax
	movl	208(%rax), %eax
	movl	-36(%rbp), %esi
	movl	$2, %ecx
	movl	$1, %edx
	movl	%eax, %edi
	call	SetChanReturnType@PLT
	jmp	.L138
.L137:
	.loc 2 1141 0
	movl	-36(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	chinfo(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$1, %eax
	jne	.L138
	.loc 2 1142 0
	movq	-48(%rbp), %rax
	movl	208(%rax), %eax
	movl	-36(%rbp), %esi
	movl	$4, %ecx
	movl	$1, %edx
	movl	%eax, %edi
	call	SetChanReturnType@PLT
.L138:
	.loc 2 1136 0
	addl	$1, -36(%rbp)
.L135:
	.loc 2 1136 0 is_stmt 0 discriminator 1
	cmpl	$124, -36(%rbp)
	jle	.L139
	.loc 2 1145 0 is_stmt 1
	movq	-48(%rbp), %rax
	movl	-68(%rbp), %esi
	movq	$0, 88(%rsp)
	movq	$0, 80(%rsp)
	leaq	PrepareRetbufs(%rip), %rdx
	movq	%rdx, 72(%rsp)
	leaq	ReadMeasurements(%rip), %rdx
	movq	%rdx, 64(%rsp)
	leaq	AbortMeasurements(%rip), %rdx
	movq	%rdx, 56(%rsp)
	leaq	TrggrMeasurements(%rip), %rdx
	movq	%rdx, 48(%rsp)
	leaq	StartMeasurements(%rip), %rdx
	movq	%rdx, 40(%rsp)
	movl	$-1, 32(%rsp)
	movl	$-1, 24(%rsp)
	movl	$19, 16(%rsp)
	movl	$18, 8(%rsp)
	movl	$50, (%rsp)
	movl	$11, %r9d
	movl	$13, %r8d
	movl	$12, %ecx
	movl	$10, %edx
	movq	%rax, %rdi
	call	pzframe_drv_init@PLT
	.loc 2 1152 0
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	InitParams
.L134:
	.loc 2 1153 0
	addq	$168, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE37:
	.size	adc1000_init_d, .-adc1000_init_d
	.type	adc1000_term_d, @function
adc1000_term_d:
.LFB38:
	.loc 2 1156 0
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	.loc 2 1157 0
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
	.loc 2 1159 0
	movq	-8(%rbp), %rax
	movl	212(%rax), %eax
	testl	%eax, %eax
	jns	.L141
	.loc 2 1159 0 is_stmt 0 discriminator 1
	jmp	.L140
.L141:
	.loc 2 1161 0 is_stmt 1
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	StopDevice
	.loc 2 1162 0
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	pzframe_drv_term@PLT
.L140:
	.loc 2 1163 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE38:
	.size	adc1000_term_d, .-adc1000_term_d
	.globl	adc1000_driver_modrec
	.section	.rodata
.LC33:
	.string	"adc1000"
.LC34:
	.string	"adc1000 fast-ADC"
.LC35:
	.string	"vme"
	.section	.data.rel.local
	.align 32
	.type	adc1000_driver_modrec, @object
	.size	adc1000_driver_modrec, 136
adc1000_driver_modrec:
	.long	305419896
	.long	110000000
	.quad	.LC33
	.quad	.LC34
	.quad	0
	.quad	0
	.quad	121736
	.quad	adc1000_params
	.long	5
	.long	5
	.quad	.LC35
	.long	10000000
	.zero	4
	.quad	0
	.long	-1
	.zero	4
	.quad	0
	.quad	0
	.quad	adc1000_init_d
	.quad	adc1000_term_d
	.quad	adc1000_rw_p
	.section	.rodata
	.align 16
	.type	__FUNCTION__.5952, @object
	.size	__FUNCTION__.5952, 18
__FUNCTION__.5952:
	.string	"ActivatePLLPreset"
	.type	__FUNCTION__.5973, @object
	.size	__FUNCTION__.5973, 11
__FUNCTION__.5973:
	.string	"InitParams"
	.local	total_irqs.6120
	.comm	total_irqs.6120,4,4
	.text
.Letext0:
	.file 3 "/usr/include/bits/types.h"
	.file 4 "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/include/stddef.h"
	.file 5 "/usr/include/bits/time.h"
	.file 6 "../../../../4cx/src/../exports/include/misc_types.h"
	.file 7 "../../../../4cx/src/../exports/include/cx.h"
	.file 8 "../../../../4cx/src/../exports/include/cx_module.h"
	.file 9 "/usr/include/libio.h"
	.file 10 "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/include/stdarg.h"
	.file 11 "<built-in>"
	.file 12 "/usr/include/stdio.h"
	.file 13 "../../../../4cx/src/../exports/include/misc_sepchars.h"
	.file 14 "../../../../4cx/src/../exports/include/paramstr_parser.h"
	.file 15 "../../../../4cx/src/../exports/include/cxscheduler.h"
	.file 16 "../../../../4cx/src/../exports/include/cxsd_driver.h"
	.file 17 "../../../../4cx/src/../exports/include/pzframe_drv.h"
	.file 18 "../../../include/vme_lyr.h"
	.file 19 "../src/adc4x250_defs.h"
	.file 20 "../../../include/drv_i/adc250_drv_i.h"
	.file 21 "../../../include/drv_i/adc1000_drv_i.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x28e6
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF692
	.byte	0x1
	.long	.LASF693
	.long	.LASF694
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF0
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF1
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF2
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF3
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF6
	.uleb128 0x4
	.long	.LASF7
	.byte	0x3
	.byte	0x8c
	.long	0x5e
	.uleb128 0x4
	.long	.LASF8
	.byte	0x3
	.byte	0x8d
	.long	0x5e
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF9
	.uleb128 0x4
	.long	.LASF10
	.byte	0x3
	.byte	0x94
	.long	0x5e
	.uleb128 0x4
	.long	.LASF11
	.byte	0x3
	.byte	0x96
	.long	0x5e
	.uleb128 0x5
	.byte	0x8
	.uleb128 0x6
	.byte	0x8
	.long	0xa0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF12
	.uleb128 0x4
	.long	.LASF13
	.byte	0x4
	.byte	0xd4
	.long	0x42
	.uleb128 0x7
	.long	.LASF96
	.byte	0x10
	.byte	0x5
	.byte	0x1e
	.long	0xd7
	.uleb128 0x8
	.long	.LASF14
	.byte	0x5
	.byte	0x20
	.long	0x82
	.byte	0
	.uleb128 0x8
	.long	.LASF15
	.byte	0x5
	.byte	0x21
	.long	0x8d
	.byte	0x8
	.byte	0
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF16
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF17
	.uleb128 0x4
	.long	.LASF18
	.byte	0x6
	.byte	0x11
	.long	0x57
	.uleb128 0x4
	.long	.LASF19
	.byte	0x6
	.byte	0x12
	.long	0x3b
	.uleb128 0x4
	.long	.LASF20
	.byte	0x6
	.byte	0x13
	.long	0x50
	.uleb128 0x4
	.long	.LASF21
	.byte	0x6
	.byte	0x14
	.long	0x34
	.uleb128 0x4
	.long	.LASF22
	.byte	0x6
	.byte	0x16
	.long	0x2d
	.uleb128 0x2
	.byte	0x4
	.byte	0x4
	.long	.LASF23
	.uleb128 0x2
	.byte	0x8
	.byte	0x4
	.long	.LASF24
	.uleb128 0x4
	.long	.LASF25
	.byte	0x7
	.byte	0x1e
	.long	0xf0
	.uleb128 0x4
	.long	.LASF26
	.byte	0x7
	.byte	0x1f
	.long	0xf0
	.uleb128 0x9
	.byte	0x10
	.byte	0x7
	.byte	0x21
	.long	0x161
	.uleb128 0xa
	.string	"sec"
	.byte	0x7
	.byte	0x23
	.long	0x5e
	.byte	0
	.uleb128 0x8
	.long	.LASF27
	.byte	0x7
	.byte	0x24
	.long	0x5e
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF28
	.byte	0x7
	.byte	0x25
	.long	0x140
	.uleb128 0xb
	.byte	0x4
	.byte	0x7
	.byte	0x47
	.long	0x1a7
	.uleb128 0xc
	.long	.LASF29
	.sleb128 7
	.uleb128 0xc
	.long	.LASF30
	.sleb128 120
	.uleb128 0xc
	.long	.LASF31
	.sleb128 3
	.uleb128 0xc
	.long	.LASF32
	.sleb128 128
	.uleb128 0xc
	.long	.LASF33
	.sleb128 0
	.uleb128 0xc
	.long	.LASF34
	.sleb128 1
	.uleb128 0xc
	.long	.LASF35
	.sleb128 2
	.uleb128 0xc
	.long	.LASF36
	.sleb128 3
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x7
	.byte	0x62
	.long	0x204
	.uleb128 0xc
	.long	.LASF37
	.sleb128 0
	.uleb128 0xc
	.long	.LASF38
	.sleb128 8
	.uleb128 0xc
	.long	.LASF39
	.sleb128 9
	.uleb128 0xc
	.long	.LASF40
	.sleb128 10
	.uleb128 0xc
	.long	.LASF41
	.sleb128 11
	.uleb128 0xc
	.long	.LASF42
	.sleb128 136
	.uleb128 0xc
	.long	.LASF43
	.sleb128 137
	.uleb128 0xc
	.long	.LASF44
	.sleb128 138
	.uleb128 0xc
	.long	.LASF45
	.sleb128 139
	.uleb128 0xc
	.long	.LASF46
	.sleb128 18
	.uleb128 0xc
	.long	.LASF47
	.sleb128 19
	.uleb128 0xc
	.long	.LASF48
	.sleb128 152
	.uleb128 0xc
	.long	.LASF49
	.sleb128 154
	.byte	0
	.uleb128 0xb
	.byte	0x8
	.byte	0x7
	.byte	0x85
	.long	0x31e
	.uleb128 0xc
	.long	.LASF50
	.sleb128 65535
	.uleb128 0xc
	.long	.LASF51
	.sleb128 4294901760
	.uleb128 0xc
	.long	.LASF52
	.sleb128 1
	.uleb128 0xc
	.long	.LASF53
	.sleb128 2
	.uleb128 0xc
	.long	.LASF54
	.sleb128 4
	.uleb128 0xc
	.long	.LASF55
	.sleb128 8
	.uleb128 0xc
	.long	.LASF56
	.sleb128 16
	.uleb128 0xc
	.long	.LASF57
	.sleb128 32
	.uleb128 0xc
	.long	.LASF58
	.sleb128 1024
	.uleb128 0xc
	.long	.LASF59
	.sleb128 2048
	.uleb128 0xc
	.long	.LASF60
	.sleb128 4096
	.uleb128 0xc
	.long	.LASF61
	.sleb128 8192
	.uleb128 0xc
	.long	.LASF62
	.sleb128 16384
	.uleb128 0xc
	.long	.LASF63
	.sleb128 32768
	.uleb128 0xc
	.long	.LASF64
	.sleb128 63
	.uleb128 0xc
	.long	.LASF65
	.sleb128 65472
	.uleb128 0xc
	.long	.LASF66
	.sleb128 65536
	.uleb128 0xc
	.long	.LASF67
	.sleb128 131072
	.uleb128 0xc
	.long	.LASF68
	.sleb128 262144
	.uleb128 0xc
	.long	.LASF69
	.sleb128 524288
	.uleb128 0xc
	.long	.LASF70
	.sleb128 1048576
	.uleb128 0xc
	.long	.LASF71
	.sleb128 2097152
	.uleb128 0xc
	.long	.LASF72
	.sleb128 4128768
	.uleb128 0xc
	.long	.LASF73
	.sleb128 -2147483648
	.uleb128 0xc
	.long	.LASF74
	.sleb128 1073741824
	.uleb128 0xc
	.long	.LASF75
	.sleb128 536870912
	.uleb128 0xc
	.long	.LASF76
	.sleb128 268435456
	.uleb128 0xc
	.long	.LASF77
	.sleb128 -268435456
	.uleb128 0xc
	.long	.LASF78
	.sleb128 63
	.uleb128 0xc
	.long	.LASF79
	.sleb128 131008
	.uleb128 0xc
	.long	.LASF80
	.sleb128 1310719
	.uleb128 0xc
	.long	.LASF81
	.sleb128 -1073741824
	.uleb128 0xc
	.long	.LASF82
	.sleb128 807403520
	.uleb128 0xc
	.long	.LASF83
	.sleb128 786432
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x7
	.byte	0xd4
	.long	0x339
	.uleb128 0xc
	.long	.LASF84
	.sleb128 1
	.uleb128 0xc
	.long	.LASF85
	.sleb128 2
	.uleb128 0xc
	.long	.LASF86
	.sleb128 1
	.byte	0
	.uleb128 0x4
	.long	.LASF87
	.byte	0x8
	.byte	0xb
	.long	0x344
	.uleb128 0x6
	.byte	0x8
	.long	0x34a
	.uleb128 0xd
	.long	0x57
	.uleb128 0x4
	.long	.LASF88
	.byte	0x8
	.byte	0xc
	.long	0x35a
	.uleb128 0x6
	.byte	0x8
	.long	0x360
	.uleb128 0xe
	.uleb128 0x9
	.byte	0x28
	.byte	0x8
	.byte	0xe
	.long	0x3b2
	.uleb128 0x8
	.long	.LASF89
	.byte	0x8
	.byte	0x10
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF90
	.byte	0x8
	.byte	0x11
	.long	0x57
	.byte	0x4
	.uleb128 0x8
	.long	.LASF91
	.byte	0x8
	.byte	0x12
	.long	0x3b2
	.byte	0x8
	.uleb128 0x8
	.long	.LASF92
	.byte	0x8
	.byte	0x13
	.long	0x3b2
	.byte	0x10
	.uleb128 0x8
	.long	.LASF93
	.byte	0x8
	.byte	0x15
	.long	0x339
	.byte	0x18
	.uleb128 0x8
	.long	.LASF94
	.byte	0x8
	.byte	0x16
	.long	0x34f
	.byte	0x20
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x3b8
	.uleb128 0xf
	.long	0xa0
	.uleb128 0x4
	.long	.LASF95
	.byte	0x8
	.byte	0x17
	.long	0x361
	.uleb128 0x7
	.long	.LASF97
	.byte	0xd8
	.byte	0x9
	.byte	0xf6
	.long	0x549
	.uleb128 0x8
	.long	.LASF98
	.byte	0x9
	.byte	0xf7
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF99
	.byte	0x9
	.byte	0xfc
	.long	0x9a
	.byte	0x8
	.uleb128 0x8
	.long	.LASF100
	.byte	0x9
	.byte	0xfd
	.long	0x9a
	.byte	0x10
	.uleb128 0x8
	.long	.LASF101
	.byte	0x9
	.byte	0xfe
	.long	0x9a
	.byte	0x18
	.uleb128 0x8
	.long	.LASF102
	.byte	0x9
	.byte	0xff
	.long	0x9a
	.byte	0x20
	.uleb128 0x10
	.long	.LASF103
	.byte	0x9
	.value	0x100
	.long	0x9a
	.byte	0x28
	.uleb128 0x10
	.long	.LASF104
	.byte	0x9
	.value	0x101
	.long	0x9a
	.byte	0x30
	.uleb128 0x10
	.long	.LASF105
	.byte	0x9
	.value	0x102
	.long	0x9a
	.byte	0x38
	.uleb128 0x10
	.long	.LASF106
	.byte	0x9
	.value	0x103
	.long	0x9a
	.byte	0x40
	.uleb128 0x10
	.long	.LASF107
	.byte	0x9
	.value	0x105
	.long	0x9a
	.byte	0x48
	.uleb128 0x10
	.long	.LASF108
	.byte	0x9
	.value	0x106
	.long	0x9a
	.byte	0x50
	.uleb128 0x10
	.long	.LASF109
	.byte	0x9
	.value	0x107
	.long	0x9a
	.byte	0x58
	.uleb128 0x10
	.long	.LASF110
	.byte	0x9
	.value	0x109
	.long	0x5d9
	.byte	0x60
	.uleb128 0x10
	.long	.LASF111
	.byte	0x9
	.value	0x10b
	.long	0x5df
	.byte	0x68
	.uleb128 0x10
	.long	.LASF112
	.byte	0x9
	.value	0x10d
	.long	0x57
	.byte	0x70
	.uleb128 0x10
	.long	.LASF113
	.byte	0x9
	.value	0x111
	.long	0x57
	.byte	0x74
	.uleb128 0x10
	.long	.LASF114
	.byte	0x9
	.value	0x113
	.long	0x65
	.byte	0x78
	.uleb128 0x10
	.long	.LASF115
	.byte	0x9
	.value	0x117
	.long	0x34
	.byte	0x80
	.uleb128 0x10
	.long	.LASF116
	.byte	0x9
	.value	0x118
	.long	0x49
	.byte	0x82
	.uleb128 0x10
	.long	.LASF117
	.byte	0x9
	.value	0x119
	.long	0x5e5
	.byte	0x83
	.uleb128 0x10
	.long	.LASF118
	.byte	0x9
	.value	0x11d
	.long	0x5f5
	.byte	0x88
	.uleb128 0x10
	.long	.LASF119
	.byte	0x9
	.value	0x126
	.long	0x70
	.byte	0x90
	.uleb128 0x10
	.long	.LASF120
	.byte	0x9
	.value	0x12f
	.long	0x98
	.byte	0x98
	.uleb128 0x10
	.long	.LASF121
	.byte	0x9
	.value	0x130
	.long	0x98
	.byte	0xa0
	.uleb128 0x10
	.long	.LASF122
	.byte	0x9
	.value	0x131
	.long	0x98
	.byte	0xa8
	.uleb128 0x10
	.long	.LASF123
	.byte	0x9
	.value	0x132
	.long	0x98
	.byte	0xb0
	.uleb128 0x10
	.long	.LASF124
	.byte	0x9
	.value	0x133
	.long	0xa7
	.byte	0xb8
	.uleb128 0x10
	.long	.LASF125
	.byte	0x9
	.value	0x135
	.long	0x57
	.byte	0xc0
	.uleb128 0x10
	.long	.LASF126
	.byte	0x9
	.value	0x137
	.long	0x5fb
	.byte	0xc4
	.byte	0
	.uleb128 0x4
	.long	.LASF127
	.byte	0xa
	.byte	0x28
	.long	0x554
	.uleb128 0x11
	.long	0x564
	.long	0x564
	.uleb128 0x12
	.long	0x7b
	.byte	0
	.byte	0
	.uleb128 0x7
	.long	.LASF128
	.byte	0x18
	.byte	0xb
	.byte	0
	.long	0x5a1
	.uleb128 0x8
	.long	.LASF129
	.byte	0xb
	.byte	0
	.long	0x3b
	.byte	0
	.uleb128 0x8
	.long	.LASF130
	.byte	0xb
	.byte	0
	.long	0x3b
	.byte	0x4
	.uleb128 0x8
	.long	.LASF131
	.byte	0xb
	.byte	0
	.long	0x98
	.byte	0x8
	.uleb128 0x8
	.long	.LASF132
	.byte	0xb
	.byte	0
	.long	0x98
	.byte	0x10
	.byte	0
	.uleb128 0x13
	.long	.LASF695
	.byte	0x9
	.byte	0x9b
	.uleb128 0x7
	.long	.LASF133
	.byte	0x18
	.byte	0x9
	.byte	0xa1
	.long	0x5d9
	.uleb128 0x8
	.long	.LASF134
	.byte	0x9
	.byte	0xa2
	.long	0x5d9
	.byte	0
	.uleb128 0x8
	.long	.LASF135
	.byte	0x9
	.byte	0xa3
	.long	0x5df
	.byte	0x8
	.uleb128 0x8
	.long	.LASF136
	.byte	0x9
	.byte	0xa7
	.long	0x57
	.byte	0x10
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x5a8
	.uleb128 0x6
	.byte	0x8
	.long	0x3c8
	.uleb128 0x11
	.long	0xa0
	.long	0x5f5
	.uleb128 0x12
	.long	0x7b
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x5a1
	.uleb128 0x11
	.long	0xa0
	.long	0x60b
	.uleb128 0x12
	.long	0x7b
	.byte	0x13
	.byte	0
	.uleb128 0x4
	.long	.LASF137
	.byte	0xc
	.byte	0x4f
	.long	0x549
	.uleb128 0x6
	.byte	0x8
	.long	0x57
	.uleb128 0xb
	.byte	0x4
	.byte	0xd
	.byte	0xc
	.long	0x63d
	.uleb128 0xc
	.long	.LASF138
	.sleb128 12
	.uleb128 0xc
	.long	.LASF139
	.sleb128 11
	.uleb128 0xc
	.long	.LASF140
	.sleb128 9
	.uleb128 0xc
	.long	.LASF141
	.sleb128 8
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0xe
	.byte	0x22
	.long	0x697
	.uleb128 0xc
	.long	.LASF142
	.sleb128 0
	.uleb128 0xc
	.long	.LASF143
	.sleb128 1
	.uleb128 0xc
	.long	.LASF144
	.sleb128 2
	.uleb128 0xc
	.long	.LASF145
	.sleb128 10
	.uleb128 0xc
	.long	.LASF146
	.sleb128 11
	.uleb128 0xc
	.long	.LASF147
	.sleb128 12
	.uleb128 0xc
	.long	.LASF148
	.sleb128 50
	.uleb128 0xc
	.long	.LASF149
	.sleb128 51
	.uleb128 0xc
	.long	.LASF150
	.sleb128 52
	.uleb128 0xc
	.long	.LASF151
	.sleb128 53
	.uleb128 0xc
	.long	.LASF152
	.sleb128 70
	.uleb128 0xc
	.long	.LASF153
	.sleb128 80
	.uleb128 0xc
	.long	.LASF154
	.sleb128 81
	.byte	0
	.uleb128 0x4
	.long	.LASF155
	.byte	0xe
	.byte	0x34
	.long	0x63d
	.uleb128 0x9
	.byte	0x10
	.byte	0xe
	.byte	0x36
	.long	0x6c3
	.uleb128 0x8
	.long	.LASF156
	.byte	0xe
	.byte	0x38
	.long	0x3b2
	.byte	0
	.uleb128 0xa
	.string	"val"
	.byte	0xe
	.byte	0x39
	.long	0x57
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF157
	.byte	0xe
	.byte	0x3a
	.long	0x6a2
	.uleb128 0x4
	.long	.LASF158
	.byte	0xe
	.byte	0x3c
	.long	0x6d9
	.uleb128 0x6
	.byte	0x8
	.long	0x6df
	.uleb128 0x14
	.long	0x57
	.long	0x711
	.uleb128 0x15
	.long	0x3b2
	.uleb128 0x15
	.long	0x711
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0xa7
	.uleb128 0x15
	.long	0x3b2
	.uleb128 0x15
	.long	0x3b2
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x717
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x3b2
	.uleb128 0x6
	.byte	0x8
	.long	0x9a
	.uleb128 0x9
	.byte	0x8
	.byte	0xe
	.byte	0x41
	.long	0x732
	.uleb128 0x8
	.long	.LASF159
	.byte	0xe
	.byte	0x43
	.long	0x791
	.byte	0
	.byte	0
	.uleb128 0x7
	.long	.LASF160
	.byte	0x38
	.byte	0xe
	.byte	0x8d
	.long	0x791
	.uleb128 0x8
	.long	.LASF91
	.byte	0xe
	.byte	0x8f
	.long	0x3b2
	.byte	0
	.uleb128 0xa
	.string	"t"
	.byte	0xe
	.byte	0x90
	.long	0x697
	.byte	0x8
	.uleb128 0x8
	.long	.LASF161
	.byte	0xe
	.byte	0x91
	.long	0x57
	.byte	0xc
	.uleb128 0x8
	.long	.LASF162
	.byte	0xe
	.byte	0x92
	.long	0xa7
	.byte	0x10
	.uleb128 0x8
	.long	.LASF163
	.byte	0xe
	.byte	0x93
	.long	0x57
	.byte	0x18
	.uleb128 0x8
	.long	.LASF164
	.byte	0xe
	.byte	0x94
	.long	0x57
	.byte	0x1c
	.uleb128 0xa
	.string	"var"
	.byte	0xe
	.byte	0x95
	.long	0x9b7
	.byte	0x20
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x732
	.uleb128 0x4
	.long	.LASF165
	.byte	0xe
	.byte	0x44
	.long	0x71d
	.uleb128 0x9
	.byte	0x18
	.byte	0xe
	.byte	0x46
	.long	0x7cf
	.uleb128 0x8
	.long	.LASF166
	.byte	0xe
	.byte	0x48
	.long	0x791
	.byte	0
	.uleb128 0x8
	.long	.LASF167
	.byte	0xe
	.byte	0x49
	.long	0x3b2
	.byte	0x8
	.uleb128 0x8
	.long	.LASF168
	.byte	0xe
	.byte	0x4a
	.long	0xa0
	.byte	0x10
	.byte	0
	.uleb128 0x4
	.long	.LASF169
	.byte	0xe
	.byte	0x4b
	.long	0x7a2
	.uleb128 0x9
	.byte	0x10
	.byte	0xe
	.byte	0x4d
	.long	0x7fb
	.uleb128 0x8
	.long	.LASF170
	.byte	0xe
	.byte	0x4f
	.long	0x6ce
	.byte	0
	.uleb128 0x8
	.long	.LASF171
	.byte	0xe
	.byte	0x50
	.long	0x98
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF172
	.byte	0xe
	.byte	0x51
	.long	0x7da
	.uleb128 0x9
	.byte	0xc
	.byte	0xe
	.byte	0x53
	.long	0x833
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x55
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF174
	.byte	0xe
	.byte	0x56
	.long	0x57
	.byte	0x4
	.uleb128 0x8
	.long	.LASF175
	.byte	0xe
	.byte	0x57
	.long	0x57
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF176
	.byte	0xe
	.byte	0x58
	.long	0x806
	.uleb128 0x9
	.byte	0x10
	.byte	0xe
	.byte	0x5a
	.long	0x85f
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x5c
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF177
	.byte	0xe
	.byte	0x5d
	.long	0x717
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF178
	.byte	0xe
	.byte	0x5e
	.long	0x83e
	.uleb128 0x9
	.byte	0x10
	.byte	0xe
	.byte	0x60
	.long	0x88b
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x62
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF159
	.byte	0xe
	.byte	0x63
	.long	0x88b
	.byte	0x8
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x6c3
	.uleb128 0x4
	.long	.LASF179
	.byte	0xe
	.byte	0x64
	.long	0x86a
	.uleb128 0x9
	.byte	0x8
	.byte	0xe
	.byte	0x66
	.long	0x8bd
	.uleb128 0x8
	.long	.LASF180
	.byte	0xe
	.byte	0x68
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF181
	.byte	0xe
	.byte	0x69
	.long	0x57
	.byte	0x4
	.byte	0
	.uleb128 0x4
	.long	.LASF182
	.byte	0xe
	.byte	0x6a
	.long	0x89c
	.uleb128 0x9
	.byte	0x8
	.byte	0xe
	.byte	0x6c
	.long	0x8dd
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x6e
	.long	0x3b2
	.byte	0
	.byte	0
	.uleb128 0x4
	.long	.LASF183
	.byte	0xe
	.byte	0x6f
	.long	0x8c8
	.uleb128 0x9
	.byte	0x8
	.byte	0xe
	.byte	0x71
	.long	0x8fd
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x73
	.long	0x3b2
	.byte	0
	.byte	0
	.uleb128 0x4
	.long	.LASF184
	.byte	0xe
	.byte	0x74
	.long	0x8e8
	.uleb128 0x9
	.byte	0x18
	.byte	0xe
	.byte	0x76
	.long	0x935
	.uleb128 0x8
	.long	.LASF173
	.byte	0xe
	.byte	0x78
	.long	0x123
	.byte	0
	.uleb128 0x8
	.long	.LASF174
	.byte	0xe
	.byte	0x79
	.long	0x123
	.byte	0x8
	.uleb128 0x8
	.long	.LASF175
	.byte	0xe
	.byte	0x7a
	.long	0x123
	.byte	0x10
	.byte	0
	.uleb128 0x4
	.long	.LASF185
	.byte	0xe
	.byte	0x7b
	.long	0x908
	.uleb128 0x16
	.byte	0x18
	.byte	0xe
	.byte	0x7e
	.long	0x9b7
	.uleb128 0x17
	.long	.LASF186
	.byte	0xe
	.byte	0x80
	.long	0x797
	.uleb128 0x17
	.long	.LASF187
	.byte	0xe
	.byte	0x81
	.long	0x7cf
	.uleb128 0x17
	.long	.LASF188
	.byte	0xe
	.byte	0x82
	.long	0x7fb
	.uleb128 0x17
	.long	.LASF189
	.byte	0xe
	.byte	0x83
	.long	0x833
	.uleb128 0x17
	.long	.LASF190
	.byte	0xe
	.byte	0x84
	.long	0x85f
	.uleb128 0x17
	.long	.LASF191
	.byte	0xe
	.byte	0x85
	.long	0x891
	.uleb128 0x17
	.long	.LASF192
	.byte	0xe
	.byte	0x86
	.long	0x8bd
	.uleb128 0x17
	.long	.LASF193
	.byte	0xe
	.byte	0x87
	.long	0x8dd
	.uleb128 0x17
	.long	.LASF194
	.byte	0xe
	.byte	0x88
	.long	0x8fd
	.uleb128 0x17
	.long	.LASF195
	.byte	0xe
	.byte	0x89
	.long	0x935
	.byte	0
	.uleb128 0x4
	.long	.LASF196
	.byte	0xe
	.byte	0x8a
	.long	0x940
	.uleb128 0x4
	.long	.LASF197
	.byte	0xe
	.byte	0x96
	.long	0x732
	.uleb128 0x4
	.long	.LASF198
	.byte	0xf
	.byte	0x17
	.long	0x57
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x1e
	.long	0x9eb
	.uleb128 0xc
	.long	.LASF199
	.sleb128 305419896
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x1f
	.long	0xa09
	.uleb128 0xc
	.long	.LASF200
	.sleb128 11
	.uleb128 0xc
	.long	.LASF201
	.sleb128 0
	.uleb128 0xc
	.long	.LASF202
	.sleb128 110000000
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x37
	.long	0xa24
	.uleb128 0xc
	.long	.LASF203
	.sleb128 -1
	.uleb128 0xc
	.long	.LASF204
	.sleb128 0
	.uleb128 0xc
	.long	.LASF205
	.sleb128 1
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x3e
	.long	0xa39
	.uleb128 0xc
	.long	.LASF206
	.sleb128 1
	.uleb128 0xc
	.long	.LASF207
	.sleb128 2
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x10
	.byte	0x49
	.long	0xa60
	.uleb128 0xc
	.long	.LASF208
	.sleb128 0
	.uleb128 0xc
	.long	.LASF209
	.sleb128 1
	.uleb128 0xc
	.long	.LASF210
	.sleb128 2
	.uleb128 0xc
	.long	.LASF211
	.sleb128 3
	.uleb128 0xc
	.long	.LASF212
	.sleb128 4
	.byte	0
	.uleb128 0x4
	.long	.LASF213
	.byte	0x10
	.byte	0x83
	.long	0xa6b
	.uleb128 0x6
	.byte	0x8
	.long	0xa71
	.uleb128 0x14
	.long	0x57
	.long	0xa94
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x3b2
	.byte	0
	.uleb128 0x4
	.long	.LASF214
	.byte	0x10
	.byte	0x86
	.long	0xa9f
	.uleb128 0x6
	.byte	0x8
	.long	0xaa5
	.uleb128 0x18
	.long	0xab5
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.byte	0
	.uleb128 0x4
	.long	.LASF215
	.byte	0x10
	.byte	0x88
	.long	0xac0
	.uleb128 0x6
	.byte	0x8
	.long	0xac6
	.uleb128 0x18
	.long	0xaf4
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0xaf4
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0xafa
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x135
	.uleb128 0x6
	.byte	0x8
	.long	0x98
	.uleb128 0x9
	.byte	0x10
	.byte	0x10
	.byte	0x8d
	.long	0xb38
	.uleb128 0x8
	.long	.LASF216
	.byte	0x10
	.byte	0x8f
	.long	0x57
	.byte	0
	.uleb128 0xa
	.string	"rw"
	.byte	0x10
	.byte	0x90
	.long	0x57
	.byte	0x4
	.uleb128 0x8
	.long	.LASF217
	.byte	0x10
	.byte	0x91
	.long	0x135
	.byte	0x8
	.uleb128 0x8
	.long	.LASF218
	.byte	0x10
	.byte	0x92
	.long	0x57
	.byte	0xc
	.byte	0
	.uleb128 0x4
	.long	.LASF219
	.byte	0x10
	.byte	0x93
	.long	0xb00
	.uleb128 0x9
	.byte	0x10
	.byte	0x10
	.byte	0x95
	.long	0xb62
	.uleb128 0x8
	.long	.LASF91
	.byte	0x10
	.byte	0x97
	.long	0x3b2
	.byte	0
	.uleb128 0xa
	.string	"n"
	.byte	0x10
	.byte	0x98
	.long	0x57
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF220
	.byte	0x10
	.byte	0x99
	.long	0xb43
	.uleb128 0x9
	.byte	0x10
	.byte	0x10
	.byte	0xb9
	.long	0xb8e
	.uleb128 0x8
	.long	.LASF221
	.byte	0x10
	.byte	0xbb
	.long	0x3b2
	.byte	0
	.uleb128 0x8
	.long	.LASF222
	.byte	0x10
	.byte	0xbc
	.long	0x98
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF223
	.byte	0x10
	.byte	0xbd
	.long	0xb6d
	.uleb128 0x9
	.byte	0x88
	.byte	0x10
	.byte	0xbf
	.long	0xc49
	.uleb128 0xa
	.string	"mr"
	.byte	0x10
	.byte	0xc1
	.long	0x3bd
	.byte	0
	.uleb128 0x8
	.long	.LASF224
	.byte	0x10
	.byte	0xc3
	.long	0xa7
	.byte	0x28
	.uleb128 0x8
	.long	.LASF225
	.byte	0x10
	.byte	0xc4
	.long	0xc49
	.byte	0x30
	.uleb128 0x8
	.long	.LASF226
	.byte	0x10
	.byte	0xc6
	.long	0x57
	.byte	0x38
	.uleb128 0x8
	.long	.LASF227
	.byte	0x10
	.byte	0xc7
	.long	0x57
	.byte	0x3c
	.uleb128 0x8
	.long	.LASF228
	.byte	0x10
	.byte	0xc9
	.long	0x3b2
	.byte	0x40
	.uleb128 0x8
	.long	.LASF229
	.byte	0x10
	.byte	0xca
	.long	0x57
	.byte	0x48
	.uleb128 0x8
	.long	.LASF230
	.byte	0x10
	.byte	0xcc
	.long	0xc4f
	.byte	0x50
	.uleb128 0x8
	.long	.LASF231
	.byte	0x10
	.byte	0xce
	.long	0x57
	.byte	0x58
	.uleb128 0x8
	.long	.LASF232
	.byte	0x10
	.byte	0xcf
	.long	0xc55
	.byte	0x60
	.uleb128 0x8
	.long	.LASF233
	.byte	0x10
	.byte	0xd0
	.long	0xc5b
	.byte	0x68
	.uleb128 0x8
	.long	.LASF234
	.byte	0x10
	.byte	0xd4
	.long	0xa60
	.byte	0x70
	.uleb128 0x8
	.long	.LASF235
	.byte	0x10
	.byte	0xd5
	.long	0xa94
	.byte	0x78
	.uleb128 0x8
	.long	.LASF236
	.byte	0x10
	.byte	0xd6
	.long	0xab5
	.byte	0x80
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x9c2
	.uleb128 0x6
	.byte	0x8
	.long	0xb8e
	.uleb128 0x6
	.byte	0x8
	.long	0xb38
	.uleb128 0x6
	.byte	0x8
	.long	0xb62
	.uleb128 0x4
	.long	.LASF237
	.byte	0x10
	.byte	0xd7
	.long	0xb99
	.uleb128 0xb
	.byte	0x4
	.byte	0x11
	.byte	0x9
	.long	0xca5
	.uleb128 0xc
	.long	.LASF238
	.sleb128 0
	.uleb128 0xc
	.long	.LASF239
	.sleb128 1
	.uleb128 0xc
	.long	.LASF240
	.sleb128 2
	.uleb128 0xc
	.long	.LASF241
	.sleb128 3
	.uleb128 0xc
	.long	.LASF242
	.sleb128 4
	.uleb128 0xc
	.long	.LASF243
	.sleb128 5
	.uleb128 0xc
	.long	.LASF244
	.sleb128 6
	.uleb128 0xc
	.long	.LASF245
	.sleb128 7
	.byte	0
	.uleb128 0x9
	.byte	0x8
	.byte	0x11
	.byte	0x14
	.long	0xcc6
	.uleb128 0x8
	.long	.LASF246
	.byte	0x11
	.byte	0x16
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF247
	.byte	0x11
	.byte	0x17
	.long	0x57
	.byte	0x4
	.byte	0
	.uleb128 0x4
	.long	.LASF248
	.byte	0x11
	.byte	0x18
	.long	0xca5
	.uleb128 0xb
	.byte	0x4
	.byte	0x11
	.byte	0x1c
	.long	0xcf2
	.uleb128 0xc
	.long	.LASF249
	.sleb128 1
	.uleb128 0xc
	.long	.LASF250
	.sleb128 0
	.uleb128 0xc
	.long	.LASF251
	.sleb128 -1
	.uleb128 0xc
	.long	.LASF252
	.sleb128 -1
	.byte	0
	.uleb128 0x4
	.long	.LASF253
	.byte	0x11
	.byte	0x38
	.long	0xcfd
	.uleb128 0x6
	.byte	0x8
	.long	0xd03
	.uleb128 0x14
	.long	0x57
	.long	0xd12
	.uleb128 0x15
	.long	0xd12
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xd18
	.uleb128 0x7
	.long	.LASF254
	.byte	0xc8
	.byte	0x11
	.byte	0x4a
	.long	0xe69
	.uleb128 0x8
	.long	.LASF255
	.byte	0x11
	.byte	0x4c
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF256
	.byte	0x11
	.byte	0x4e
	.long	0x57
	.byte	0x4
	.uleb128 0x8
	.long	.LASF257
	.byte	0x11
	.byte	0x4f
	.long	0x57
	.byte	0x8
	.uleb128 0x8
	.long	.LASF258
	.byte	0x11
	.byte	0x50
	.long	0x57
	.byte	0xc
	.uleb128 0x8
	.long	.LASF259
	.byte	0x11
	.byte	0x51
	.long	0x57
	.byte	0x10
	.uleb128 0x8
	.long	.LASF260
	.byte	0x11
	.byte	0x52
	.long	0x57
	.byte	0x14
	.uleb128 0x8
	.long	.LASF261
	.byte	0x11
	.byte	0x53
	.long	0x57
	.byte	0x18
	.uleb128 0x8
	.long	.LASF262
	.byte	0x11
	.byte	0x54
	.long	0x57
	.byte	0x1c
	.uleb128 0x8
	.long	.LASF263
	.byte	0x11
	.byte	0x55
	.long	0x57
	.byte	0x20
	.uleb128 0x8
	.long	.LASF264
	.byte	0x11
	.byte	0x56
	.long	0x57
	.byte	0x24
	.uleb128 0x8
	.long	.LASF265
	.byte	0x11
	.byte	0x58
	.long	0xcf2
	.byte	0x28
	.uleb128 0x8
	.long	.LASF266
	.byte	0x11
	.byte	0x59
	.long	0xe69
	.byte	0x30
	.uleb128 0x8
	.long	.LASF267
	.byte	0x11
	.byte	0x5a
	.long	0xe74
	.byte	0x38
	.uleb128 0x8
	.long	.LASF268
	.byte	0x11
	.byte	0x5b
	.long	0xe7f
	.byte	0x40
	.uleb128 0x8
	.long	.LASF269
	.byte	0x11
	.byte	0x5c
	.long	0xe9f
	.byte	0x48
	.uleb128 0x8
	.long	.LASF270
	.byte	0x11
	.byte	0x5e
	.long	0x98
	.byte	0x50
	.uleb128 0x8
	.long	.LASF271
	.byte	0x11
	.byte	0x5f
	.long	0x98
	.byte	0x58
	.uleb128 0x8
	.long	.LASF272
	.byte	0x11
	.byte	0x61
	.long	0xf29
	.byte	0x60
	.uleb128 0x8
	.long	.LASF273
	.byte	0x11
	.byte	0x63
	.long	0x57
	.byte	0x98
	.uleb128 0x8
	.long	.LASF274
	.byte	0x11
	.byte	0x64
	.long	0x57
	.byte	0x9c
	.uleb128 0x8
	.long	.LASF275
	.byte	0x11
	.byte	0x65
	.long	0x57
	.byte	0xa0
	.uleb128 0x8
	.long	.LASF276
	.byte	0x11
	.byte	0x66
	.long	0x57
	.byte	0xa4
	.uleb128 0x8
	.long	.LASF277
	.byte	0x11
	.byte	0x67
	.long	0x57
	.byte	0xa8
	.uleb128 0xa
	.string	"tid"
	.byte	0x11
	.byte	0x68
	.long	0x9cd
	.byte	0xac
	.uleb128 0x8
	.long	.LASF278
	.byte	0x11
	.byte	0x69
	.long	0xb2
	.byte	0xb0
	.uleb128 0x8
	.long	.LASF279
	.byte	0x11
	.byte	0x6a
	.long	0xe5
	.byte	0xc0
	.uleb128 0x8
	.long	.LASF280
	.byte	0x11
	.byte	0x6b
	.long	0xe5
	.byte	0xc4
	.byte	0
	.uleb128 0x4
	.long	.LASF281
	.byte	0x11
	.byte	0x39
	.long	0xcfd
	.uleb128 0x4
	.long	.LASF282
	.byte	0x11
	.byte	0x3a
	.long	0xcfd
	.uleb128 0x4
	.long	.LASF283
	.byte	0x11
	.byte	0x3b
	.long	0xe8a
	.uleb128 0x6
	.byte	0x8
	.long	0xe90
	.uleb128 0x14
	.long	0x12a
	.long	0xe9f
	.uleb128 0x15
	.long	0xd12
	.byte	0
	.uleb128 0x4
	.long	.LASF284
	.byte	0x11
	.byte	0x3c
	.long	0xeaa
	.uleb128 0x6
	.byte	0x8
	.long	0xeb0
	.uleb128 0x18
	.long	0xec0
	.uleb128 0x15
	.long	0xd12
	.uleb128 0x15
	.long	0x12a
	.byte	0
	.uleb128 0x9
	.byte	0x38
	.byte	0x11
	.byte	0x3f
	.long	0xf1d
	.uleb128 0x8
	.long	.LASF216
	.byte	0x11
	.byte	0x41
	.long	0x57
	.byte	0
	.uleb128 0x8
	.long	.LASF285
	.byte	0x11
	.byte	0x42
	.long	0x616
	.byte	0x8
	.uleb128 0x8
	.long	.LASF286
	.byte	0x11
	.byte	0x43
	.long	0xaf4
	.byte	0x10
	.uleb128 0x8
	.long	.LASF287
	.byte	0x11
	.byte	0x44
	.long	0x616
	.byte	0x18
	.uleb128 0x8
	.long	.LASF288
	.byte	0x11
	.byte	0x45
	.long	0xafa
	.byte	0x20
	.uleb128 0x8
	.long	.LASF289
	.byte	0x11
	.byte	0x46
	.long	0xf1d
	.byte	0x28
	.uleb128 0x8
	.long	.LASF290
	.byte	0x11
	.byte	0x47
	.long	0xf23
	.byte	0x30
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x12a
	.uleb128 0x6
	.byte	0x8
	.long	0x161
	.uleb128 0x4
	.long	.LASF291
	.byte	0x11
	.byte	0x48
	.long	0xec0
	.uleb128 0x4
	.long	.LASF292
	.byte	0x11
	.byte	0x6c
	.long	0xd18
	.uleb128 0xb
	.byte	0x4
	.byte	0x12
	.byte	0xc
	.long	0xf5d
	.uleb128 0xc
	.long	.LASF293
	.sleb128 1
	.uleb128 0xc
	.long	.LASF294
	.sleb128 0
	.uleb128 0xc
	.long	.LASF295
	.sleb128 10000000
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x12
	.byte	0x15
	.long	0xf6c
	.uleb128 0xc
	.long	.LASF296
	.sleb128 0
	.byte	0
	.uleb128 0x4
	.long	.LASF297
	.byte	0x12
	.byte	0x1c
	.long	0xf77
	.uleb128 0x6
	.byte	0x8
	.long	0xf7d
	.uleb128 0x18
	.long	0xf97
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.byte	0
	.uleb128 0x4
	.long	.LASF298
	.byte	0x12
	.byte	0x1f
	.long	0xfa2
	.uleb128 0x6
	.byte	0x8
	.long	0xfa8
	.uleb128 0x18
	.long	0xfc2
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.byte	0
	.uleb128 0x4
	.long	.LASF299
	.byte	0x12
	.byte	0x25
	.long	0xfcd
	.uleb128 0x6
	.byte	0x8
	.long	0xfd3
	.uleb128 0x14
	.long	0x57
	.long	0x101e
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x98
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf6c
	.uleb128 0x15
	.long	0xf97
	.uleb128 0x15
	.long	0x57
	.byte	0
	.uleb128 0x4
	.long	.LASF300
	.byte	0x12
	.byte	0x2c
	.long	0x1029
	.uleb128 0x6
	.byte	0x8
	.long	0x102f
	.uleb128 0x14
	.long	0x57
	.long	0x1066
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x1066
	.uleb128 0x15
	.long	0x1066
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x616
	.uleb128 0x15
	.long	0x616
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xf0
	.uleb128 0x4
	.long	.LASF301
	.byte	0x12
	.byte	0x32
	.long	0x1077
	.uleb128 0x6
	.byte	0x8
	.long	0x107d
	.uleb128 0x14
	.long	0x57
	.long	0x1097
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0x3b2
	.uleb128 0x15
	.long	0x57
	.uleb128 0x19
	.byte	0
	.uleb128 0x4
	.long	.LASF302
	.byte	0x12
	.byte	0x35
	.long	0x10a2
	.uleb128 0x6
	.byte	0x8
	.long	0x10a8
	.uleb128 0x14
	.long	0x57
	.long	0x10c1
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x111
	.byte	0
	.uleb128 0x4
	.long	.LASF303
	.byte	0x12
	.byte	0x36
	.long	0x10cc
	.uleb128 0x6
	.byte	0x8
	.long	0x10d2
	.uleb128 0x14
	.long	0x57
	.long	0x10eb
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x10eb
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x111
	.uleb128 0x4
	.long	.LASF304
	.byte	0x12
	.byte	0x37
	.long	0x10fc
	.uleb128 0x6
	.byte	0x8
	.long	0x1102
	.uleb128 0x14
	.long	0x57
	.long	0x111b
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x106
	.byte	0
	.uleb128 0x4
	.long	.LASF305
	.byte	0x12
	.byte	0x38
	.long	0x1126
	.uleb128 0x6
	.byte	0x8
	.long	0x112c
	.uleb128 0x14
	.long	0x57
	.long	0x1145
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x1145
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x106
	.uleb128 0x4
	.long	.LASF306
	.byte	0x12
	.byte	0x39
	.long	0x1156
	.uleb128 0x6
	.byte	0x8
	.long	0x115c
	.uleb128 0x14
	.long	0x57
	.long	0x1175
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0xf0
	.byte	0
	.uleb128 0x4
	.long	.LASF307
	.byte	0x12
	.byte	0x3a
	.long	0x1180
	.uleb128 0x6
	.byte	0x8
	.long	0x1186
	.uleb128 0x14
	.long	0x57
	.long	0x119f
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x1066
	.byte	0
	.uleb128 0x4
	.long	.LASF308
	.byte	0x12
	.byte	0x3c
	.long	0x11aa
	.uleb128 0x6
	.byte	0x8
	.long	0x11b0
	.uleb128 0x14
	.long	0x57
	.long	0x11ce
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x10eb
	.uleb128 0x15
	.long	0x57
	.byte	0
	.uleb128 0x4
	.long	.LASF309
	.byte	0x12
	.byte	0x3d
	.long	0x11aa
	.uleb128 0x4
	.long	.LASF310
	.byte	0x12
	.byte	0x3e
	.long	0x11e4
	.uleb128 0x6
	.byte	0x8
	.long	0x11ea
	.uleb128 0x14
	.long	0x57
	.long	0x1208
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x1145
	.uleb128 0x15
	.long	0x57
	.byte	0
	.uleb128 0x4
	.long	.LASF311
	.byte	0x12
	.byte	0x3f
	.long	0x11e4
	.uleb128 0x4
	.long	.LASF312
	.byte	0x12
	.byte	0x40
	.long	0x121e
	.uleb128 0x6
	.byte	0x8
	.long	0x1224
	.uleb128 0x14
	.long	0x57
	.long	0x1242
	.uleb128 0x15
	.long	0x57
	.uleb128 0x15
	.long	0xf0
	.uleb128 0x15
	.long	0x1066
	.uleb128 0x15
	.long	0x57
	.byte	0
	.uleb128 0x4
	.long	.LASF313
	.byte	0x12
	.byte	0x41
	.long	0x121e
	.uleb128 0x1a
	.value	0x138
	.byte	0x12
	.byte	0x44
	.long	0x1432
	.uleb128 0xa
	.string	"add"
	.byte	0x12
	.byte	0x46
	.long	0xfc2
	.byte	0
	.uleb128 0x8
	.long	.LASF314
	.byte	0x12
	.byte	0x48
	.long	0x101e
	.byte	0x8
	.uleb128 0x8
	.long	.LASF315
	.byte	0x12
	.byte	0x4a
	.long	0x106c
	.byte	0x10
	.uleb128 0x8
	.long	.LASF316
	.byte	0x12
	.byte	0x51
	.long	0x1097
	.byte	0x18
	.uleb128 0x8
	.long	.LASF317
	.byte	0x12
	.byte	0x52
	.long	0x10c1
	.byte	0x20
	.uleb128 0x8
	.long	.LASF318
	.byte	0x12
	.byte	0x53
	.long	0x10f1
	.byte	0x28
	.uleb128 0x8
	.long	.LASF319
	.byte	0x12
	.byte	0x54
	.long	0x111b
	.byte	0x30
	.uleb128 0x8
	.long	.LASF320
	.byte	0x12
	.byte	0x55
	.long	0x114b
	.byte	0x38
	.uleb128 0x8
	.long	.LASF321
	.byte	0x12
	.byte	0x56
	.long	0x1175
	.byte	0x40
	.uleb128 0x8
	.long	.LASF322
	.byte	0x12
	.byte	0x59
	.long	0x1097
	.byte	0x48
	.uleb128 0x8
	.long	.LASF323
	.byte	0x12
	.byte	0x5a
	.long	0x10c1
	.byte	0x50
	.uleb128 0x8
	.long	.LASF324
	.byte	0x12
	.byte	0x5b
	.long	0x10f1
	.byte	0x58
	.uleb128 0x8
	.long	.LASF325
	.byte	0x12
	.byte	0x5c
	.long	0x111b
	.byte	0x60
	.uleb128 0x8
	.long	.LASF326
	.byte	0x12
	.byte	0x5d
	.long	0x114b
	.byte	0x68
	.uleb128 0x8
	.long	.LASF327
	.byte	0x12
	.byte	0x5e
	.long	0x1175
	.byte	0x70
	.uleb128 0x8
	.long	.LASF328
	.byte	0x12
	.byte	0x61
	.long	0x1097
	.byte	0x78
	.uleb128 0x8
	.long	.LASF329
	.byte	0x12
	.byte	0x62
	.long	0x10c1
	.byte	0x80
	.uleb128 0x8
	.long	.LASF330
	.byte	0x12
	.byte	0x63
	.long	0x10f1
	.byte	0x88
	.uleb128 0x8
	.long	.LASF331
	.byte	0x12
	.byte	0x64
	.long	0x111b
	.byte	0x90
	.uleb128 0x8
	.long	.LASF332
	.byte	0x12
	.byte	0x65
	.long	0x114b
	.byte	0x98
	.uleb128 0x8
	.long	.LASF333
	.byte	0x12
	.byte	0x66
	.long	0x1175
	.byte	0xa0
	.uleb128 0x8
	.long	.LASF334
	.byte	0x12
	.byte	0x6b
	.long	0x119f
	.byte	0xa8
	.uleb128 0x8
	.long	.LASF335
	.byte	0x12
	.byte	0x6c
	.long	0x11ce
	.byte	0xb0
	.uleb128 0x8
	.long	.LASF336
	.byte	0x12
	.byte	0x6d
	.long	0x11d9
	.byte	0xb8
	.uleb128 0x8
	.long	.LASF337
	.byte	0x12
	.byte	0x6e
	.long	0x1208
	.byte	0xc0
	.uleb128 0x8
	.long	.LASF338
	.byte	0x12
	.byte	0x6f
	.long	0x1213
	.byte	0xc8
	.uleb128 0x8
	.long	.LASF339
	.byte	0x12
	.byte	0x70
	.long	0x1242
	.byte	0xd0
	.uleb128 0x8
	.long	.LASF340
	.byte	0x12
	.byte	0x73
	.long	0x119f
	.byte	0xd8
	.uleb128 0x8
	.long	.LASF341
	.byte	0x12
	.byte	0x74
	.long	0x11ce
	.byte	0xe0
	.uleb128 0x8
	.long	.LASF342
	.byte	0x12
	.byte	0x75
	.long	0x11d9
	.byte	0xe8
	.uleb128 0x8
	.long	.LASF343
	.byte	0x12
	.byte	0x76
	.long	0x1208
	.byte	0xf0
	.uleb128 0x8
	.long	.LASF344
	.byte	0x12
	.byte	0x77
	.long	0x1213
	.byte	0xf8
	.uleb128 0x1b
	.long	.LASF345
	.byte	0x12
	.byte	0x78
	.long	0x1242
	.value	0x100
	.uleb128 0x1b
	.long	.LASF346
	.byte	0x12
	.byte	0x7b
	.long	0x119f
	.value	0x108
	.uleb128 0x1b
	.long	.LASF347
	.byte	0x12
	.byte	0x7c
	.long	0x11ce
	.value	0x110
	.uleb128 0x1b
	.long	.LASF348
	.byte	0x12
	.byte	0x7d
	.long	0x11d9
	.value	0x118
	.uleb128 0x1b
	.long	.LASF349
	.byte	0x12
	.byte	0x7e
	.long	0x1208
	.value	0x120
	.uleb128 0x1b
	.long	.LASF350
	.byte	0x12
	.byte	0x7f
	.long	0x1213
	.value	0x128
	.uleb128 0x1b
	.long	.LASF351
	.byte	0x12
	.byte	0x80
	.long	0x1242
	.value	0x130
	.byte	0
	.uleb128 0x4
	.long	.LASF352
	.byte	0x12
	.byte	0x81
	.long	0x124d
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x5
	.long	0x144c
	.uleb128 0xc
	.long	.LASF353
	.sleb128 9
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x9
	.long	0x1560
	.uleb128 0xc
	.long	.LASF354
	.sleb128 0
	.uleb128 0xc
	.long	.LASF355
	.sleb128 4
	.uleb128 0xc
	.long	.LASF356
	.sleb128 8
	.uleb128 0xc
	.long	.LASF357
	.sleb128 16
	.uleb128 0xc
	.long	.LASF358
	.sleb128 20
	.uleb128 0xc
	.long	.LASF359
	.sleb128 24
	.uleb128 0xc
	.long	.LASF360
	.sleb128 28
	.uleb128 0xc
	.long	.LASF361
	.sleb128 32
	.uleb128 0xc
	.long	.LASF362
	.sleb128 36
	.uleb128 0xc
	.long	.LASF363
	.sleb128 40
	.uleb128 0xc
	.long	.LASF364
	.sleb128 44
	.uleb128 0xc
	.long	.LASF365
	.sleb128 48
	.uleb128 0xc
	.long	.LASF366
	.sleb128 256
	.uleb128 0xc
	.long	.LASF367
	.sleb128 260
	.uleb128 0xc
	.long	.LASF368
	.sleb128 264
	.uleb128 0xc
	.long	.LASF369
	.sleb128 268
	.uleb128 0xc
	.long	.LASF370
	.sleb128 272
	.uleb128 0xc
	.long	.LASF371
	.sleb128 276
	.uleb128 0xc
	.long	.LASF372
	.sleb128 280
	.uleb128 0xc
	.long	.LASF373
	.sleb128 284
	.uleb128 0xc
	.long	.LASF374
	.sleb128 288
	.uleb128 0xc
	.long	.LASF375
	.sleb128 292
	.uleb128 0xc
	.long	.LASF376
	.sleb128 296
	.uleb128 0xc
	.long	.LASF377
	.sleb128 300
	.uleb128 0xc
	.long	.LASF378
	.sleb128 304
	.uleb128 0xc
	.long	.LASF379
	.sleb128 308
	.uleb128 0xc
	.long	.LASF380
	.sleb128 312
	.uleb128 0xc
	.long	.LASF381
	.sleb128 4
	.uleb128 0xc
	.long	.LASF382
	.sleb128 328
	.uleb128 0xc
	.long	.LASF383
	.sleb128 4
	.uleb128 0xc
	.long	.LASF384
	.sleb128 344
	.uleb128 0xc
	.long	.LASF385
	.sleb128 348
	.uleb128 0xc
	.long	.LASF386
	.sleb128 512
	.uleb128 0xc
	.long	.LASF387
	.sleb128 4
	.uleb128 0xc
	.long	.LASF388
	.sleb128 528
	.uleb128 0xc
	.long	.LASF389
	.sleb128 4
	.uleb128 0xc
	.long	.LASF390
	.sleb128 544
	.uleb128 0xc
	.long	.LASF391
	.sleb128 4
	.uleb128 0xc
	.long	.LASF392
	.sleb128 4194304
	.uleb128 0xc
	.long	.LASF393
	.sleb128 2097152
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x41
	.long	0x15a5
	.uleb128 0xc
	.long	.LASF394
	.sleb128 0
	.uleb128 0xc
	.long	.LASF395
	.sleb128 1
	.uleb128 0xc
	.long	.LASF396
	.sleb128 1
	.uleb128 0xc
	.long	.LASF397
	.sleb128 2
	.uleb128 0xc
	.long	.LASF398
	.sleb128 2
	.uleb128 0xc
	.long	.LASF399
	.sleb128 4
	.uleb128 0xc
	.long	.LASF400
	.sleb128 3
	.uleb128 0xc
	.long	.LASF401
	.sleb128 8
	.uleb128 0xc
	.long	.LASF402
	.sleb128 4
	.uleb128 0xc
	.long	.LASF403
	.sleb128 16
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x54
	.long	0x166e
	.uleb128 0xc
	.long	.LASF404
	.sleb128 0
	.uleb128 0xc
	.long	.LASF405
	.sleb128 1
	.uleb128 0xc
	.long	.LASF406
	.sleb128 1
	.uleb128 0xc
	.long	.LASF407
	.sleb128 2
	.uleb128 0xc
	.long	.LASF408
	.sleb128 3
	.uleb128 0xc
	.long	.LASF409
	.sleb128 8
	.uleb128 0xc
	.long	.LASF410
	.sleb128 4
	.uleb128 0xc
	.long	.LASF411
	.sleb128 16
	.uleb128 0xc
	.long	.LASF412
	.sleb128 5
	.uleb128 0xc
	.long	.LASF413
	.sleb128 32
	.uleb128 0xc
	.long	.LASF414
	.sleb128 6
	.uleb128 0xc
	.long	.LASF415
	.sleb128 15
	.uleb128 0xc
	.long	.LASF416
	.sleb128 960
	.uleb128 0xc
	.long	.LASF417
	.sleb128 10
	.uleb128 0xc
	.long	.LASF418
	.sleb128 15
	.uleb128 0xc
	.long	.LASF419
	.sleb128 15360
	.uleb128 0xc
	.long	.LASF420
	.sleb128 14
	.uleb128 0xc
	.long	.LASF421
	.sleb128 15
	.uleb128 0xc
	.long	.LASF422
	.sleb128 245760
	.uleb128 0xc
	.long	.LASF423
	.sleb128 18
	.uleb128 0xc
	.long	.LASF424
	.sleb128 262144
	.uleb128 0xc
	.long	.LASF425
	.sleb128 19
	.uleb128 0xc
	.long	.LASF426
	.sleb128 524288
	.uleb128 0xc
	.long	.LASF427
	.sleb128 20
	.uleb128 0xc
	.long	.LASF428
	.sleb128 1048576
	.uleb128 0xc
	.long	.LASF429
	.sleb128 21
	.uleb128 0xc
	.long	.LASF430
	.sleb128 2097152
	.uleb128 0xc
	.long	.LASF431
	.sleb128 22
	.uleb128 0xc
	.long	.LASF432
	.sleb128 4194304
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x7b
	.long	0x16a7
	.uleb128 0xc
	.long	.LASF433
	.sleb128 0
	.uleb128 0xc
	.long	.LASF434
	.sleb128 1
	.uleb128 0xc
	.long	.LASF435
	.sleb128 1
	.uleb128 0xc
	.long	.LASF436
	.sleb128 2
	.uleb128 0xc
	.long	.LASF437
	.sleb128 2
	.uleb128 0xc
	.long	.LASF438
	.sleb128 4
	.uleb128 0xc
	.long	.LASF439
	.sleb128 3
	.uleb128 0xc
	.long	.LASF440
	.sleb128 8
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x84
	.long	0x16d0
	.uleb128 0xc
	.long	.LASF441
	.sleb128 18
	.uleb128 0xc
	.long	.LASF442
	.sleb128 783173
	.uleb128 0xc
	.long	.LASF443
	.sleb128 1566346
	.uleb128 0xc
	.long	.LASF444
	.sleb128 3132692
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0x8e
	.long	0x16e7
	.uleb128 0xc
	.long	.LASF445
	.sleb128 0
	.uleb128 0xc
	.long	.LASF446
	.sleb128 65535
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xa7
	.long	0x1714
	.uleb128 0xc
	.long	.LASF447
	.sleb128 0
	.uleb128 0xc
	.long	.LASF448
	.sleb128 7
	.uleb128 0xc
	.long	.LASF449
	.sleb128 7
	.uleb128 0xc
	.long	.LASF450
	.sleb128 3
	.uleb128 0xc
	.long	.LASF451
	.sleb128 7
	.uleb128 0xc
	.long	.LASF452
	.sleb128 56
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xb1
	.long	0x173b
	.uleb128 0xc
	.long	.LASF453
	.sleb128 0
	.uleb128 0xc
	.long	.LASF454
	.sleb128 1
	.uleb128 0xc
	.long	.LASF455
	.sleb128 2
	.uleb128 0xc
	.long	.LASF456
	.sleb128 3
	.uleb128 0xc
	.long	.LASF457
	.sleb128 4
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xbb
	.long	0x1750
	.uleb128 0xc
	.long	.LASF458
	.sleb128 0
	.uleb128 0xc
	.long	.LASF459
	.sleb128 1
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xc1
	.long	0x176b
	.uleb128 0xc
	.long	.LASF460
	.sleb128 0
	.uleb128 0xc
	.long	.LASF461
	.sleb128 3
	.uleb128 0xc
	.long	.LASF462
	.sleb128 3
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xc7
	.long	0x1786
	.uleb128 0xc
	.long	.LASF463
	.sleb128 0
	.uleb128 0xc
	.long	.LASF464
	.sleb128 1
	.uleb128 0xc
	.long	.LASF465
	.sleb128 2
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xcf
	.long	0x17a1
	.uleb128 0xc
	.long	.LASF466
	.sleb128 0
	.uleb128 0xc
	.long	.LASF467
	.sleb128 3
	.uleb128 0xc
	.long	.LASF468
	.sleb128 3
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x13
	.byte	0xd5
	.long	0x17c2
	.uleb128 0xc
	.long	.LASF469
	.sleb128 0
	.uleb128 0xc
	.long	.LASF470
	.sleb128 1
	.uleb128 0xc
	.long	.LASF471
	.sleb128 2
	.uleb128 0xc
	.long	.LASF472
	.sleb128 3
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x14
	.byte	0x7
	.long	0x1aa3
	.uleb128 0xc
	.long	.LASF473
	.sleb128 0
	.uleb128 0xc
	.long	.LASF474
	.sleb128 1
	.uleb128 0xc
	.long	.LASF475
	.sleb128 2
	.uleb128 0xc
	.long	.LASF476
	.sleb128 3
	.uleb128 0xc
	.long	.LASF477
	.sleb128 4
	.uleb128 0xc
	.long	.LASF478
	.sleb128 9
	.uleb128 0xc
	.long	.LASF479
	.sleb128 10
	.uleb128 0xc
	.long	.LASF480
	.sleb128 11
	.uleb128 0xc
	.long	.LASF481
	.sleb128 12
	.uleb128 0xc
	.long	.LASF482
	.sleb128 13
	.uleb128 0xc
	.long	.LASF483
	.sleb128 14
	.uleb128 0xc
	.long	.LASF484
	.sleb128 17
	.uleb128 0xc
	.long	.LASF485
	.sleb128 18
	.uleb128 0xc
	.long	.LASF486
	.sleb128 19
	.uleb128 0xc
	.long	.LASF487
	.sleb128 20
	.uleb128 0xc
	.long	.LASF488
	.sleb128 21
	.uleb128 0xc
	.long	.LASF489
	.sleb128 22
	.uleb128 0xc
	.long	.LASF490
	.sleb128 23
	.uleb128 0xc
	.long	.LASF491
	.sleb128 24
	.uleb128 0xc
	.long	.LASF492
	.sleb128 25
	.uleb128 0xc
	.long	.LASF493
	.sleb128 26
	.uleb128 0xc
	.long	.LASF494
	.sleb128 27
	.uleb128 0xc
	.long	.LASF495
	.sleb128 28
	.uleb128 0xc
	.long	.LASF496
	.sleb128 29
	.uleb128 0xc
	.long	.LASF497
	.sleb128 39
	.uleb128 0xc
	.long	.LASF498
	.sleb128 40
	.uleb128 0xc
	.long	.LASF499
	.sleb128 40
	.uleb128 0xc
	.long	.LASF500
	.sleb128 41
	.uleb128 0xc
	.long	.LASF501
	.sleb128 42
	.uleb128 0xc
	.long	.LASF502
	.sleb128 43
	.uleb128 0xc
	.long	.LASF503
	.sleb128 44
	.uleb128 0xc
	.long	.LASF504
	.sleb128 45
	.uleb128 0xc
	.long	.LASF505
	.sleb128 46
	.uleb128 0xc
	.long	.LASF506
	.sleb128 47
	.uleb128 0xc
	.long	.LASF507
	.sleb128 47
	.uleb128 0xc
	.long	.LASF508
	.sleb128 8
	.uleb128 0xc
	.long	.LASF509
	.sleb128 48
	.uleb128 0xc
	.long	.LASF510
	.sleb128 50
	.uleb128 0xc
	.long	.LASF511
	.sleb128 52
	.uleb128 0xc
	.long	.LASF512
	.sleb128 53
	.uleb128 0xc
	.long	.LASF513
	.sleb128 54
	.uleb128 0xc
	.long	.LASF514
	.sleb128 55
	.uleb128 0xc
	.long	.LASF515
	.sleb128 56
	.uleb128 0xc
	.long	.LASF516
	.sleb128 57
	.uleb128 0xc
	.long	.LASF517
	.sleb128 58
	.uleb128 0xc
	.long	.LASF518
	.sleb128 60
	.uleb128 0xc
	.long	.LASF519
	.sleb128 61
	.uleb128 0xc
	.long	.LASF520
	.sleb128 62
	.uleb128 0xc
	.long	.LASF521
	.sleb128 63
	.uleb128 0xc
	.long	.LASF522
	.sleb128 64
	.uleb128 0xc
	.long	.LASF523
	.sleb128 65
	.uleb128 0xc
	.long	.LASF524
	.sleb128 66
	.uleb128 0xc
	.long	.LASF525
	.sleb128 67
	.uleb128 0xc
	.long	.LASF526
	.sleb128 70
	.uleb128 0xc
	.long	.LASF527
	.sleb128 71
	.uleb128 0xc
	.long	.LASF528
	.sleb128 72
	.uleb128 0xc
	.long	.LASF529
	.sleb128 73
	.uleb128 0xc
	.long	.LASF530
	.sleb128 74
	.uleb128 0xc
	.long	.LASF531
	.sleb128 75
	.uleb128 0xc
	.long	.LASF532
	.sleb128 76
	.uleb128 0xc
	.long	.LASF533
	.sleb128 77
	.uleb128 0xc
	.long	.LASF534
	.sleb128 78
	.uleb128 0xc
	.long	.LASF535
	.sleb128 79
	.uleb128 0xc
	.long	.LASF536
	.sleb128 80
	.uleb128 0xc
	.long	.LASF537
	.sleb128 81
	.uleb128 0xc
	.long	.LASF538
	.sleb128 82
	.uleb128 0xc
	.long	.LASF539
	.sleb128 83
	.uleb128 0xc
	.long	.LASF540
	.sleb128 84
	.uleb128 0xc
	.long	.LASF541
	.sleb128 85
	.uleb128 0xc
	.long	.LASF542
	.sleb128 86
	.uleb128 0xc
	.long	.LASF543
	.sleb128 87
	.uleb128 0xc
	.long	.LASF544
	.sleb128 88
	.uleb128 0xc
	.long	.LASF545
	.sleb128 89
	.uleb128 0xc
	.long	.LASF546
	.sleb128 90
	.uleb128 0xc
	.long	.LASF547
	.sleb128 91
	.uleb128 0xc
	.long	.LASF548
	.sleb128 92
	.uleb128 0xc
	.long	.LASF549
	.sleb128 93
	.uleb128 0xc
	.long	.LASF550
	.sleb128 94
	.uleb128 0xc
	.long	.LASF551
	.sleb128 95
	.uleb128 0xc
	.long	.LASF552
	.sleb128 96
	.uleb128 0xc
	.long	.LASF553
	.sleb128 97
	.uleb128 0xc
	.long	.LASF554
	.sleb128 98
	.uleb128 0xc
	.long	.LASF555
	.sleb128 99
	.uleb128 0xc
	.long	.LASF556
	.sleb128 100
	.uleb128 0xc
	.long	.LASF557
	.sleb128 100
	.uleb128 0xc
	.long	.LASF558
	.sleb128 101
	.uleb128 0xc
	.long	.LASF559
	.sleb128 102
	.uleb128 0xc
	.long	.LASF560
	.sleb128 103
	.uleb128 0xc
	.long	.LASF561
	.sleb128 104
	.uleb128 0xc
	.long	.LASF562
	.sleb128 105
	.uleb128 0xc
	.long	.LASF563
	.sleb128 106
	.uleb128 0xc
	.long	.LASF564
	.sleb128 107
	.uleb128 0xc
	.long	.LASF565
	.sleb128 108
	.uleb128 0xc
	.long	.LASF566
	.sleb128 109
	.uleb128 0xc
	.long	.LASF567
	.sleb128 110
	.uleb128 0xc
	.long	.LASF568
	.sleb128 111
	.uleb128 0xc
	.long	.LASF569
	.sleb128 112
	.uleb128 0xc
	.long	.LASF570
	.sleb128 113
	.uleb128 0xc
	.long	.LASF571
	.sleb128 114
	.uleb128 0xc
	.long	.LASF572
	.sleb128 115
	.uleb128 0xc
	.long	.LASF573
	.sleb128 115
	.uleb128 0xc
	.long	.LASF574
	.sleb128 116
	.uleb128 0xc
	.long	.LASF575
	.sleb128 117
	.uleb128 0xc
	.long	.LASF576
	.sleb128 118
	.uleb128 0xc
	.long	.LASF577
	.sleb128 119
	.uleb128 0xc
	.long	.LASF578
	.sleb128 120
	.uleb128 0xc
	.long	.LASF579
	.sleb128 121
	.uleb128 0xc
	.long	.LASF580
	.sleb128 122
	.uleb128 0xc
	.long	.LASF581
	.sleb128 123
	.uleb128 0xc
	.long	.LASF582
	.sleb128 124
	.uleb128 0xc
	.long	.LASF583
	.sleb128 140
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x14
	.byte	0x9a
	.long	0x1ac0
	.uleb128 0xc
	.long	.LASF584
	.sleb128 4
	.uleb128 0xc
	.long	.LASF585
	.sleb128 4
	.uleb128 0xc
	.long	.LASF586
	.sleb128 783155
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x14
	.byte	0xa0
	.long	0x1ad9
	.uleb128 0xc
	.long	.LASF587
	.sleb128 -32767
	.uleb128 0xc
	.long	.LASF588
	.sleb128 32767
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x15
	.byte	0x80
	.long	0x1af7
	.uleb128 0xc
	.long	.LASF589
	.sleb128 4
	.uleb128 0xc
	.long	.LASF590
	.sleb128 1
	.uleb128 0xc
	.long	.LASF591
	.sleb128 3132620
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x18
	.long	0x1b06
	.uleb128 0xc
	.long	.LASF592
	.sleb128 1
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x19
	.long	0x1b18
	.uleb128 0xc
	.long	.LASF593
	.sleb128 3132620
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x1a
	.long	0x1b28
	.uleb128 0xc
	.long	.LASF594
	.sleb128 72
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x1b
	.long	0x1b37
	.uleb128 0xc
	.long	.LASF595
	.sleb128 1
	.byte	0
	.uleb128 0x4
	.long	.LASF596
	.byte	0x2
	.byte	0x22
	.long	0xfb
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x23
	.long	0x1b51
	.uleb128 0xc
	.long	.LASF597
	.sleb128 9
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x26
	.long	0x1b62
	.uleb128 0xc
	.long	.LASF598
	.sleb128 50000
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x29
	.long	0x1ba7
	.uleb128 0xc
	.long	.LASF599
	.sleb128 0
	.uleb128 0xc
	.long	.LASF600
	.sleb128 0
	.uleb128 0xc
	.long	.LASF601
	.sleb128 0
	.uleb128 0xc
	.long	.LASF602
	.sleb128 0
	.uleb128 0xc
	.long	.LASF603
	.sleb128 0
	.uleb128 0xc
	.long	.LASF604
	.sleb128 0
	.uleb128 0xc
	.long	.LASF605
	.sleb128 0
	.uleb128 0xc
	.long	.LASF606
	.sleb128 0
	.uleb128 0xc
	.long	.LASF607
	.sleb128 0
	.uleb128 0xc
	.long	.LASF608
	.sleb128 0
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0x35
	.long	0x1bc2
	.uleb128 0xc
	.long	.LASF609
	.sleb128 0
	.uleb128 0xc
	.long	.LASF610
	.sleb128 1
	.uleb128 0xc
	.long	.LASF611
	.sleb128 2
	.byte	0
	.uleb128 0x9
	.byte	0xc
	.byte	0x2
	.byte	0x3a
	.long	0x1bef
	.uleb128 0x8
	.long	.LASF612
	.byte	0x2
	.byte	0x3c
	.long	0xf0
	.byte	0
	.uleb128 0x8
	.long	.LASF613
	.byte	0x2
	.byte	0x3d
	.long	0xf0
	.byte	0x4
	.uleb128 0x8
	.long	.LASF614
	.byte	0x2
	.byte	0x3e
	.long	0xf0
	.byte	0x8
	.byte	0
	.uleb128 0x4
	.long	.LASF615
	.byte	0x2
	.byte	0x3f
	.long	0x1bc2
	.uleb128 0xb
	.byte	0x4
	.byte	0x2
	.byte	0xcb
	.long	0x1c0a
	.uleb128 0xc
	.long	.LASF616
	.sleb128 1024
	.byte	0
	.uleb128 0x1a
	.value	0xd20
	.byte	0x2
	.byte	0xeb
	.long	0x1c54
	.uleb128 0x8
	.long	.LASF285
	.byte	0x2
	.byte	0xed
	.long	0x1c54
	.byte	0
	.uleb128 0x1b
	.long	.LASF286
	.byte	0x2
	.byte	0xee
	.long	0x1c64
	.value	0x230
	.uleb128 0x1b
	.long	.LASF287
	.byte	0x2
	.byte	0xef
	.long	0x1c54
	.value	0x460
	.uleb128 0x1b
	.long	.LASF617
	.byte	0x2
	.byte	0xf0
	.long	0x1c74
	.value	0x690
	.uleb128 0x1b
	.long	.LASF289
	.byte	0x2
	.byte	0xf1
	.long	0x1c84
	.value	0xaf0
	.byte	0
	.uleb128 0x11
	.long	0x57
	.long	0x1c64
	.uleb128 0x12
	.long	0x7b
	.byte	0x8b
	.byte	0
	.uleb128 0x11
	.long	0x135
	.long	0x1c74
	.uleb128 0x12
	.long	0x7b
	.byte	0x8b
	.byte	0
	.uleb128 0x11
	.long	0x98
	.long	0x1c84
	.uleb128 0x12
	.long	0x7b
	.byte	0x8b
	.byte	0
	.uleb128 0x11
	.long	0x12a
	.long	0x1c94
	.uleb128 0x12
	.long	0x7b
	.byte	0x8b
	.byte	0
	.uleb128 0x1c
	.long	0x1db88
	.byte	0x2
	.byte	0xcd
	.long	0x1d99
	.uleb128 0xa
	.string	"pz"
	.byte	0x2
	.byte	0xcf
	.long	0xf34
	.byte	0
	.uleb128 0x8
	.long	.LASF618
	.byte	0x2
	.byte	0xd0
	.long	0x1d99
	.byte	0xc8
	.uleb128 0x8
	.long	.LASF255
	.byte	0x2
	.byte	0xd1
	.long	0x57
	.byte	0xd0
	.uleb128 0x8
	.long	.LASF619
	.byte	0x2
	.byte	0xd2
	.long	0x57
	.byte	0xd4
	.uleb128 0x8
	.long	.LASF620
	.byte	0x2
	.byte	0xd3
	.long	0x57
	.byte	0xd8
	.uleb128 0x8
	.long	.LASF621
	.byte	0x2
	.byte	0xd4
	.long	0x57
	.byte	0xdc
	.uleb128 0x8
	.long	.LASF622
	.byte	0x2
	.byte	0xd5
	.long	0x57
	.byte	0xe0
	.uleb128 0x8
	.long	.LASF623
	.byte	0x2
	.byte	0xd7
	.long	0x57
	.byte	0xe4
	.uleb128 0x8
	.long	.LASF624
	.byte	0x2
	.byte	0xd9
	.long	0x1d9f
	.byte	0xe8
	.uleb128 0x1b
	.long	.LASF625
	.byte	0x2
	.byte	0xda
	.long	0x1d9f
	.value	0x318
	.uleb128 0x1b
	.long	.LASF626
	.byte	0x2
	.byte	0xdb
	.long	0x1d9f
	.value	0x548
	.uleb128 0x1b
	.long	.LASF627
	.byte	0x2
	.byte	0xdc
	.long	0x1daf
	.value	0x778
	.uleb128 0x1d
	.long	.LASF628
	.byte	0x2
	.byte	0xdd
	.long	0x57
	.long	0x18e1c
	.uleb128 0x1d
	.long	.LASF629
	.byte	0x2
	.byte	0xde
	.long	0x57
	.long	0x18e20
	.uleb128 0x1d
	.long	.LASF272
	.byte	0x2
	.byte	0xdf
	.long	0xf29
	.long	0x18e28
	.uleb128 0x1d
	.long	.LASF630
	.byte	0x2
	.byte	0xe2
	.long	0xe5
	.long	0x18e60
	.uleb128 0x1d
	.long	.LASF631
	.byte	0x2
	.byte	0xe4
	.long	0x57
	.long	0x18e64
	.uleb128 0x1d
	.long	.LASF632
	.byte	0x2
	.byte	0xe9
	.long	0x1dc0
	.long	0x18e68
	.uleb128 0x1e
	.string	"r"
	.byte	0x2
	.byte	0xf2
	.long	0x1c0a
	.long	0x1ce68
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x1432
	.uleb128 0x11
	.long	0xe5
	.long	0x1daf
	.uleb128 0x12
	.long	0x7b
	.byte	0x8b
	.byte	0
	.uleb128 0x11
	.long	0x1b37
	.long	0x1dc0
	.uleb128 0x1f
	.long	0x7b
	.value	0xc350
	.byte	0
	.uleb128 0x11
	.long	0xf0
	.long	0x1dd7
	.uleb128 0x12
	.long	0x7b
	.byte	0x3
	.uleb128 0x1f
	.long	0x7b
	.value	0x3ff
	.byte	0
	.uleb128 0x4
	.long	.LASF633
	.byte	0x2
	.byte	0xf3
	.long	0x1c94
	.uleb128 0x20
	.byte	0x4
	.byte	0x2
	.value	0x1b2
	.long	0x1dfa
	.uleb128 0xc
	.long	.LASF634
	.sleb128 10
	.uleb128 0xc
	.long	.LASF635
	.sleb128 100000
	.byte	0
	.uleb128 0x20
	.byte	0x4
	.byte	0x2
	.value	0x41a
	.long	0x1e2e
	.uleb128 0xc
	.long	.LASF636
	.sleb128 10
	.uleb128 0xc
	.long	.LASF637
	.sleb128 12
	.uleb128 0xc
	.long	.LASF638
	.sleb128 13
	.uleb128 0xc
	.long	.LASF639
	.sleb128 11
	.uleb128 0xc
	.long	.LASF640
	.sleb128 50
	.uleb128 0xc
	.long	.LASF641
	.sleb128 18
	.uleb128 0xc
	.long	.LASF642
	.sleb128 19
	.byte	0
	.uleb128 0x20
	.byte	0x4
	.byte	0x2
	.value	0x425
	.long	0x1e4d
	.uleb128 0xc
	.long	.LASF643
	.sleb128 12058624
	.uleb128 0xc
	.long	.LASF644
	.sleb128 32
	.uleb128 0xc
	.long	.LASF645
	.sleb128 9
	.byte	0
	.uleb128 0x21
	.long	.LASF654
	.byte	0x1
	.byte	0x68
	.long	0x57
	.quad	.LFB14
	.quad	.LFE14-.LFB14
	.uleb128 0x1
	.byte	0x9c
	.long	0x1eb8
	.uleb128 0x22
	.string	"str"
	.byte	0x1
	.byte	0x68
	.long	0x9a
	.uleb128 0x3
	.byte	0x91
	.sleb128 -232
	.uleb128 0x23
	.long	.LASF162
	.byte	0x1
	.byte	0x68
	.long	0xa7
	.uleb128 0x3
	.byte	0x91
	.sleb128 -240
	.uleb128 0x23
	.long	.LASF646
	.byte	0x1
	.byte	0x68
	.long	0x3b2
	.uleb128 0x3
	.byte	0x91
	.sleb128 -248
	.uleb128 0x19
	.uleb128 0x24
	.string	"r"
	.byte	0x1
	.byte	0x6a
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -196
	.uleb128 0x24
	.string	"ap"
	.byte	0x1
	.byte	0x6b
	.long	0x60b
	.uleb128 0x3
	.byte	0x91
	.sleb128 -224
	.byte	0
	.uleb128 0x25
	.long	.LASF647
	.byte	0x2
	.value	0x134
	.quad	.LFB21
	.quad	.LFE21-.LFB21
	.uleb128 0x1
	.byte	0x9c
	.long	0x1eff
	.uleb128 0x26
	.string	"me"
	.byte	0x2
	.value	0x134
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x26
	.string	"n"
	.byte	0x2
	.value	0x134
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x26
	.string	"v"
	.byte	0x2
	.value	0x134
	.long	0xe5
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x1dd7
	.uleb128 0x25
	.long	.LASF648
	.byte	0x2
	.value	0x139
	.quad	.LFB22
	.quad	.LFE22-.LFB22
	.uleb128 0x1
	.byte	0x9c
	.long	0x1f5f
	.uleb128 0x26
	.string	"me"
	.byte	0x2
	.value	0x139
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x27
	.long	.LASF649
	.byte	0x2
	.value	0x13b
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x27
	.long	.LASF650
	.byte	0x2
	.value	0x13c
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x27
	.long	.LASF651
	.byte	0x2
	.value	0x13d
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.uleb128 0x25
	.long	.LASF652
	.byte	0x2
	.value	0x151
	.quad	.LFB23
	.quad	.LFE23-.LFB23
	.uleb128 0x1
	.byte	0x9c
	.long	0x1fb6
	.uleb128 0x26
	.string	"me"
	.byte	0x2
	.value	0x151
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x27
	.long	.LASF653
	.byte	0x2
	.value	0x153
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x28
	.string	"iv"
	.byte	0x2
	.value	0x154
	.long	0xe5
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x155
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x29
	.long	.LASF655
	.byte	0x2
	.value	0x171
	.long	0xe5
	.quad	.LFB24
	.quad	.LFE24-.LFB24
	.uleb128 0x1
	.byte	0x9c
	.long	0x2010
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x171
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x26
	.string	"n"
	.byte	0x2
	.value	0x171
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x26
	.string	"v"
	.byte	0x2
	.value	0x171
	.long	0xe5
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x173
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xf34
	.uleb128 0x25
	.long	.LASF656
	.byte	0x2
	.value	0x1a8
	.quad	.LFB25
	.quad	.LFE25-.LFB25
	.uleb128 0x1
	.byte	0x9c
	.long	0x2063
	.uleb128 0x26
	.string	"me"
	.byte	0x2
	.value	0x1a8
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x26
	.string	"n"
	.byte	0x2
	.value	0x1a8
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2a
	.long	.LASF659
	.long	0x2073
	.uleb128 0x9
	.byte	0x3
	.quad	__FUNCTION__.5952
	.byte	0
	.uleb128 0x11
	.long	0xa0
	.long	0x2073
	.uleb128 0x12
	.long	0x7b
	.byte	0x11
	.byte	0
	.uleb128 0xf
	.long	0x2063
	.uleb128 0x25
	.long	.LASF657
	.byte	0x2
	.value	0x1b9
	.quad	.LFB26
	.quad	.LFE26-.LFB26
	.uleb128 0x1
	.byte	0x9c
	.long	0x20bf
	.uleb128 0x26
	.string	"me"
	.byte	0x2
	.value	0x1b9
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x26
	.string	"n"
	.byte	0x2
	.value	0x1b9
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x26
	.string	"v"
	.byte	0x2
	.value	0x1b9
	.long	0xe5
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.uleb128 0x2b
	.long	.LASF658
	.byte	0x2
	.value	0x1be
	.long	0x57
	.quad	.LFB27
	.quad	.LFE27-.LFB27
	.uleb128 0x1
	.byte	0x9c
	.long	0x213a
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x1be
	.long	0x2010
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x1c0
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x1c2
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x1c3
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x28
	.string	"r"
	.byte	0x2
	.value	0x1c4
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x2a
	.long	.LASF659
	.long	0x214a
	.uleb128 0x9
	.byte	0x3
	.quad	__FUNCTION__.5973
	.byte	0
	.uleb128 0x11
	.long	0xa0
	.long	0x214a
	.uleb128 0x12
	.long	0x7b
	.byte	0xa
	.byte	0
	.uleb128 0xf
	.long	0x213a
	.uleb128 0x25
	.long	.LASF660
	.byte	0x2
	.value	0x22f
	.quad	.LFB28
	.quad	.LFE28-.LFB28
	.uleb128 0x1
	.byte	0x9c
	.long	0x218b
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x22f
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x231
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x2b
	.long	.LASF661
	.byte	0x2
	.value	0x237
	.long	0x57
	.quad	.LFB29
	.quad	.LFE29-.LFB29
	.uleb128 0x1
	.byte	0x9c
	.long	0x21e6
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x237
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x239
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x28
	.string	"nl"
	.byte	0x2
	.value	0x23b
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x23c
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.byte	0
	.uleb128 0x2b
	.long	.LASF662
	.byte	0x2
	.value	0x27b
	.long	0x57
	.quad	.LFB30
	.quad	.LFE30-.LFB30
	.uleb128 0x1
	.byte	0x9c
	.long	0x2226
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x27b
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x27d
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x2b
	.long	.LASF663
	.byte	0x2
	.value	0x297
	.long	0x57
	.quad	.LFB31
	.quad	.LFE31-.LFB31
	.uleb128 0x1
	.byte	0x9c
	.long	0x2280
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x297
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x299
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x29a
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x29b
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x2b
	.long	.LASF664
	.byte	0x2
	.value	0x2ac
	.long	0x12a
	.quad	.LFB32
	.quad	.LFE32-.LFB32
	.uleb128 0x1
	.byte	0x9c
	.long	0x23f6
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x2ac
	.long	0x2010
	.uleb128 0x3
	.byte	0x91
	.sleb128 -168
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x2ae
	.long	0x1eff
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x27
	.long	.LASF665
	.byte	0x2
	.value	0x2b0
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -132
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x2b1
	.long	0xe5
	.uleb128 0x3
	.byte	0x91
	.sleb128 -144
	.uleb128 0x27
	.long	.LASF653
	.byte	0x2
	.value	0x2b2
	.long	0xf0
	.uleb128 0x3
	.byte	0x91
	.sleb128 -148
	.uleb128 0x28
	.string	"nl"
	.byte	0x2
	.value	0x2b4
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x28
	.string	"ro"
	.byte	0x2
	.value	0x2b5
	.long	0xf0
	.uleb128 0x3
	.byte	0x91
	.sleb128 -136
	.uleb128 0x28
	.string	"dp"
	.byte	0x2
	.value	0x2b6
	.long	0x23f6
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x2b7
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -84
	.uleb128 0x27
	.long	.LASF666
	.byte	0x2
	.value	0x2b8
	.long	0x1b37
	.uleb128 0x3
	.byte	0x91
	.sleb128 -138
	.uleb128 0x2c
	.quad	.LBB2
	.quad	.LBE2-.LBB2
	.uleb128 0x28
	.string	"s0"
	.byte	0x2
	.value	0x2c6
	.long	0x1066
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x28
	.string	"s1"
	.byte	0x2
	.value	0x2c6
	.long	0x1066
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x28
	.string	"s2"
	.byte	0x2
	.value	0x2c6
	.long	0x1066
	.uleb128 0x3
	.byte	0x91
	.sleb128 -120
	.uleb128 0x28
	.string	"s3"
	.byte	0x2
	.value	0x2c6
	.long	0x1066
	.uleb128 0x3
	.byte	0x91
	.sleb128 -128
	.uleb128 0x28
	.string	"a0"
	.byte	0x2
	.value	0x2c7
	.long	0x1066
	.uleb128 0x1
	.byte	0x5c
	.uleb128 0x28
	.string	"a1"
	.byte	0x2
	.value	0x2c7
	.long	0x1066
	.uleb128 0x1
	.byte	0x5d
	.uleb128 0x28
	.string	"a2"
	.byte	0x2
	.value	0x2c7
	.long	0x1066
	.uleb128 0x1
	.byte	0x5e
	.uleb128 0x28
	.string	"a3"
	.byte	0x2
	.value	0x2c7
	.long	0x1066
	.uleb128 0x1
	.byte	0x5f
	.uleb128 0x2d
	.string	"w0"
	.byte	0x2
	.value	0x2c8
	.long	0xf0
	.uleb128 0x2d
	.string	"w1"
	.byte	0x2
	.value	0x2c8
	.long	0xf0
	.uleb128 0x2d
	.string	"w2"
	.byte	0x2
	.value	0x2c8
	.long	0xf0
	.uleb128 0x2d
	.string	"w3"
	.byte	0x2
	.value	0x2c8
	.long	0xf0
	.uleb128 0x28
	.string	"wp"
	.byte	0x2
	.value	0x2c9
	.long	0x23f6
	.uleb128 0x1
	.byte	0x53
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x1b37
	.uleb128 0x25
	.long	.LASF667
	.byte	0x2
	.value	0x31a
	.quad	.LFB33
	.quad	.LFE33-.LFB33
	.uleb128 0x1
	.byte	0x9c
	.long	0x247b
	.uleb128 0x2e
	.long	.LASF255
	.byte	0x2
	.value	0x31a
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x2e
	.long	.LASF668
	.byte	0x2
	.value	0x31a
	.long	0x98
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x26
	.string	"tid"
	.byte	0x2
	.value	0x31b
	.long	0x9cd
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x2e
	.long	.LASF171
	.byte	0x2
	.value	0x31c
	.long	0x98
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x31e
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x31f
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2f
	.long	.LASF659
	.long	0x247b
	.byte	0
	.uleb128 0xf
	.long	0x213a
	.uleb128 0x30
	.long	.LASF669
	.byte	0x2
	.value	0x32c
	.quad	.LFB34
	.quad	.LFE34-.LFB34
	.uleb128 0x1
	.byte	0x9c
	.long	0x24e5
	.uleb128 0x26
	.string	"pdr"
	.byte	0x2
	.value	0x32c
	.long	0x2010
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x2e
	.long	.LASF670
	.byte	0x2
	.value	0x32c
	.long	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x32e
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x330
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x28
	.string	"x"
	.byte	0x2
	.value	0x331
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x25
	.long	.LASF671
	.byte	0x2
	.value	0x398
	.quad	.LFB35
	.quad	.LFE35-.LFB35
	.uleb128 0x1
	.byte	0x9c
	.long	0x2615
	.uleb128 0x2e
	.long	.LASF255
	.byte	0x2
	.value	0x398
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x2e
	.long	.LASF668
	.byte	0x2
	.value	0x398
	.long	0x98
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x2e
	.long	.LASF672
	.byte	0x2
	.value	0x399
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x2e
	.long	.LASF216
	.byte	0x2
	.value	0x39a
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -84
	.uleb128 0x2e
	.long	.LASF285
	.byte	0x2
	.value	0x39a
	.long	0x616
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x2e
	.long	.LASF286
	.byte	0x2
	.value	0x39b
	.long	0xaf4
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x2e
	.long	.LASF287
	.byte	0x2
	.value	0x39b
	.long	0x616
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x2e
	.long	.LASF288
	.byte	0x2
	.value	0x39b
	.long	0xafa
	.uleb128 0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x39d
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x39f
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x28
	.string	"chn"
	.byte	0x2
	.value	0x3a0
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x28
	.string	"ct"
	.byte	0x2
	.value	0x3a1
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x28
	.string	"val"
	.byte	0x2
	.value	0x3a2
	.long	0xe5
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x27
	.long	.LASF653
	.byte	0x2
	.value	0x3a4
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x27
	.long	.LASF612
	.byte	0x2
	.value	0x3a6
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x27
	.long	.LASF613
	.byte	0x2
	.value	0x3a7
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x27
	.long	.LASF614
	.byte	0x2
	.value	0x3a8
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x31
	.long	.LASF696
	.byte	0x2
	.value	0x414
	.quad	.L103
	.byte	0
	.uleb128 0x25
	.long	.LASF673
	.byte	0x2
	.value	0x43d
	.quad	.LFB36
	.quad	.LFE36-.LFB36
	.uleb128 0x1
	.byte	0x9c
	.long	0x26a2
	.uleb128 0x2e
	.long	.LASF255
	.byte	0x2
	.value	0x43d
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x2e
	.long	.LASF668
	.byte	0x2
	.value	0x43d
	.long	0x98
	.uleb128 0x2
	.byte	0x91
	.sleb128 -64
	.uleb128 0x2e
	.long	.LASF620
	.byte	0x2
	.value	0x43e
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x2e
	.long	.LASF621
	.byte	0x2
	.value	0x43e
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x440
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x28
	.string	"w"
	.byte	0x2
	.value	0x441
	.long	0xf0
	.uleb128 0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x27
	.long	.LASF674
	.byte	0x2
	.value	0x443
	.long	0x57
	.uleb128 0x9
	.byte	0x3
	.quad	total_irqs.6120
	.byte	0
	.uleb128 0x2b
	.long	.LASF675
	.byte	0x2
	.value	0x454
	.long	0x57
	.quad	.LFB37
	.quad	.LFE37-.LFB37
	.uleb128 0x1
	.byte	0x9c
	.long	0x2760
	.uleb128 0x2e
	.long	.LASF255
	.byte	0x2
	.value	0x454
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -84
	.uleb128 0x2e
	.long	.LASF668
	.byte	0x2
	.value	0x454
	.long	0x98
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x2e
	.long	.LASF676
	.byte	0x2
	.value	0x455
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x2e
	.long	.LASF677
	.byte	0x2
	.value	0x455
	.long	0x616
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x2e
	.long	.LASF678
	.byte	0x2
	.value	0x456
	.long	0x3b2
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x458
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -64
	.uleb128 0x28
	.string	"n"
	.byte	0x2
	.value	0x459
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x27
	.long	.LASF679
	.byte	0x2
	.value	0x45a
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x27
	.long	.LASF680
	.byte	0x2
	.value	0x45b
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x27
	.long	.LASF622
	.byte	0x2
	.value	0x45c
	.long	0x57
	.uleb128 0x3
	.byte	0x91
	.sleb128 -76
	.byte	0
	.uleb128 0x25
	.long	.LASF681
	.byte	0x2
	.value	0x483
	.quad	.LFB38
	.quad	.LFE38-.LFB38
	.uleb128 0x1
	.byte	0x9c
	.long	0x27ab
	.uleb128 0x2e
	.long	.LASF255
	.byte	0x2
	.value	0x483
	.long	0x57
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x2e
	.long	.LASF668
	.byte	0x2
	.value	0x483
	.long	0x98
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x28
	.string	"me"
	.byte	0x2
	.value	0x485
	.long	0x1eff
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x11
	.long	0x1bef
	.long	0x27bb
	.uleb128 0x12
	.long	0x7b
	.byte	0x2
	.byte	0
	.uleb128 0x32
	.long	.LASF682
	.byte	0x2
	.byte	0x40
	.long	0x27ab
	.uleb128 0x9
	.byte	0x3
	.quad	pll_presets_table
	.uleb128 0x11
	.long	0xcc6
	.long	0x27e0
	.uleb128 0x12
	.long	0x7b
	.byte	0x7c
	.byte	0
	.uleb128 0x32
	.long	.LASF683
	.byte	0x2
	.byte	0x49
	.long	0x27d0
	.uleb128 0x9
	.byte	0x3
	.quad	chinfo
	.uleb128 0x11
	.long	0x6c3
	.long	0x2805
	.uleb128 0x12
	.long	0x7b
	.byte	0x3
	.byte	0
	.uleb128 0x32
	.long	.LASF684
	.byte	0x2
	.byte	0xf9
	.long	0x27f5
	.uleb128 0x9
	.byte	0x3
	.quad	adcNNN_timing_lkp
	.uleb128 0x11
	.long	0x6c3
	.long	0x282a
	.uleb128 0x12
	.long	0x7b
	.byte	0x5
	.byte	0
	.uleb128 0x27
	.long	.LASF685
	.byte	0x2
	.value	0x101
	.long	0x281a
	.uleb128 0x9
	.byte	0x3
	.quad	adcNNN_trig_type_lkp
	.uleb128 0x11
	.long	0x6c3
	.long	0x2850
	.uleb128 0x12
	.long	0x7b
	.byte	0x4
	.byte	0
	.uleb128 0x27
	.long	.LASF686
	.byte	0x2
	.value	0x10b
	.long	0x2840
	.uleb128 0x9
	.byte	0x3
	.quad	adcNNN_range_lkp
	.uleb128 0x11
	.long	0x6c3
	.long	0x2876
	.uleb128 0x12
	.long	0x7b
	.byte	0x2
	.byte	0
	.uleb128 0x27
	.long	.LASF687
	.byte	0x2
	.value	0x114
	.long	0x2866
	.uleb128 0x9
	.byte	0x3
	.quad	adcNNN_pll_preset_lkp
	.uleb128 0x11
	.long	0x9c2
	.long	0x289c
	.uleb128 0x12
	.long	0x7b
	.byte	0x9
	.byte	0
	.uleb128 0x27
	.long	.LASF688
	.byte	0x2
	.value	0x11b
	.long	0x288c
	.uleb128 0x9
	.byte	0x3
	.quad	adc1000_params
	.uleb128 0x33
	.long	.LASF689
	.byte	0xc
	.byte	0xaa
	.long	0x5df
	.uleb128 0x11
	.long	0x6c3
	.long	0x28c8
	.uleb128 0x34
	.byte	0
	.uleb128 0x33
	.long	.LASF690
	.byte	0x11
	.byte	0x33
	.long	0x28bd
	.uleb128 0x35
	.long	.LASF691
	.byte	0x2
	.value	0x48d
	.long	0xc61
	.uleb128 0x9
	.byte	0x3
	.quad	adc1000_driver_modrec
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x15
	.byte	0
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x15
	.byte	0
	.uleb128 0x27
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x6
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x2f
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x30
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x31
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x32
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x33
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x34
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x35
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF483:
	.string	"ADC250_CHAN_CALIBRATE"
.LASF148:
	.string	"PSP_T_INT"
.LASF442:
	.string	"ADC4X250_TIMER_1CH_MAX"
.LASF126:
	.string	"_unused2"
.LASF291:
	.string	"pzframe_retbufs_t"
.LASF112:
	.string	"_fileno"
.LASF299:
	.string	"VmeAddDevice"
.LASF647:
	.string	"Return1Param"
.LASF464:
	.string	"ADC4X250_CLK_SRC_VAL_FP"
.LASF619:
	.string	"handle"
.LASF463:
	.string	"ADC4X250_CLK_SRC_VAL_INT"
.LASF370:
	.string	"ADC4X250_R_ADC_TRIG_MODE"
.LASF141:
	.string	"CX_SS_C"
.LASF371:
	.string	"ADC4X250_R_ADC_TRIG_SOURCE"
.LASF233:
	.string	"chan_namespace"
.LASF245:
	.string	"PZFRAME_CHTYPE_MARKER"
.LASF316:
	.string	"a16wr8"
.LASF516:
	.string	"ADC250_CHAN_CUR_TIMING"
.LASF201:
	.string	"CXSD_DRIVER_MODREC_VERSION_MINOR"
.LASF322:
	.string	"a24wr8"
.LASF285:
	.string	"addrs"
.LASF117:
	.string	"_shortbuf"
.LASF677:
	.string	"businfo"
.LASF666:
	.string	"tmpv"
.LASF170:
	.string	"parser"
.LASF587:
	.string	"ADC250_MIN_VALUE"
.LASF154:
	.string	"PSP_T_MSTRING"
.LASF220:
	.string	"CxsdChanNameRec"
.LASF255:
	.string	"devid"
.LASF639:
	.string	"PARAM_STOP"
.LASF434:
	.string	"ADC4X250_CTRL_START"
.LASF343:
	.string	"a24rd16v"
.LASF53:
	.string	"CXRF_CAMAC_NO_Q"
.LASF517:
	.string	"ADC250_CHAN_CUR_FRQDIV"
.LASF52:
	.string	"CXRF_CAMAC_NO_X"
.LASF131:
	.string	"overflow_arg_area"
.LASF98:
	.string	"_flags"
.LASF341:
	.string	"a24rd8v"
.LASF418:
	.string	"ADC4X250_STATUS_PGA_OVERRNG_bits"
.LASF132:
	.string	"reg_save_area"
.LASF611:
	.string	"PLL_PRESET_VEPP5_RFMEAS"
.LASF7:
	.string	"__off_t"
.LASF509:
	.string	"ADC250_CHAN_PLL_LOCKED"
.LASF503:
	.string	"ADC250_CHAN_PGA_HW_VER"
.LASF410:
	.string	"ADC4X250_STATUS_ADC_CMPLT_shift"
.LASF675:
	.string	"adc1000_init_d"
.LASF615:
	.string	"pll_preset_t"
.LASF648:
	.string	"ReturnDevInfo"
.LASF518:
	.string	"ADC250_CHAN_CUR_RANGE0"
.LASF519:
	.string	"ADC250_CHAN_CUR_RANGE1"
.LASF520:
	.string	"ADC250_CHAN_CUR_RANGE2"
.LASF521:
	.string	"ADC250_CHAN_CUR_RANGE3"
.LASF658:
	.string	"InitParams"
.LASF685:
	.string	"adcNNN_trig_type_lkp"
.LASF118:
	.string	"_lock"
.LASF502:
	.string	"ADC250_CHAN_BASE_HW_VER"
.LASF257:
	.string	"param_istart"
.LASF82:
	.string	"CXCF_FLAG_COLOR_MASK"
.LASF180:
	.string	"theval"
.LASF468:
	.string	"ADC4X250_PGA_RANGE_mask"
.LASF546:
	.string	"ADC250_CHAN_CLB_ZERO0"
.LASF547:
	.string	"ADC250_CHAN_CLB_ZERO1"
.LASF548:
	.string	"ADC250_CHAN_CLB_ZERO2"
.LASF549:
	.string	"ADC250_CHAN_CLB_ZERO3"
.LASF27:
	.string	"nsec"
.LASF241:
	.string	"PZFRAME_CHTYPE_INDIVIDUAL"
.LASF196:
	.string	"psp_var_t"
.LASF489:
	.string	"ADC250_CHAN_TIMING"
.LASF582:
	.string	"ADC250_CHAN_NUM_LINES"
.LASF229:
	.string	"layerver"
.LASF422:
	.string	"ADC4X250_STATUS_CALIB_FAILED_mask"
.LASF500:
	.string	"ADC250_CHAN_BASE_SW_VER"
.LASF565:
	.string	"ADC250_CHAN_AVG0"
.LASF566:
	.string	"ADC250_CHAN_AVG1"
.LASF567:
	.string	"ADC250_CHAN_AVG2"
.LASF568:
	.string	"ADC250_CHAN_AVG3"
.LASF596:
	.string	"ADC250_DATUM_T"
.LASF43:
	.string	"CXDTYPE_UINT16"
.LASF336:
	.string	"a16wr16v"
.LASF449:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_mask"
.LASF626:
	.string	"prv_args"
.LASF526:
	.string	"ADC250_CHAN_LINE0ON"
.LASF267:
	.string	"abort_measurements"
.LASF661:
	.string	"StartMeasurements"
.LASF22:
	.string	"uint8"
.LASF104:
	.string	"_IO_write_end"
.LASF363:
	.string	"ADC4X250_R_CAPABILITIES"
.LASF198:
	.string	"sl_tid_t"
.LASF128:
	.string	"__va_list_tag"
.LASF75:
	.string	"CXCF_FLAG_COLOR_RED"
.LASF47:
	.string	"CXDTYPE_DOUBLE"
.LASF623:
	.string	"BASE_SW_VER"
.LASF173:
	.string	"defval"
.LASF261:
	.string	"param_run_mode"
.LASF44:
	.string	"CXDTYPE_UINT32"
.LASF561:
	.string	"ADC250_CHAN_MAX0"
.LASF562:
	.string	"ADC250_CHAN_MAX1"
.LASF563:
	.string	"ADC250_CHAN_MAX2"
.LASF564:
	.string	"ADC250_CHAN_MAX3"
.LASF429:
	.string	"ADC4X250_STATUS_CMD_QUEUE_OVERFLOW_shift"
.LASF145:
	.string	"PSP_T_INCLUDE"
.LASF696:
	.string	"NEXT_CHANNEL"
.LASF293:
	.string	"VME_LYR_API_VERSION_MAJOR"
.LASF650:
	.string	"v_ver"
.LASF50:
	.string	"CXRF_SERVER_MASK"
.LASF301:
	.string	"VmeHalIoCtl"
.LASF358:
	.string	"ADC4X250_R_INT_STATUS"
.LASF470:
	.string	"ADC4X250_RANGE_VAL_1V"
.LASF659:
	.string	"__FUNCTION__"
.LASF348:
	.string	"a32wr16v"
.LASF617:
	.string	"val_ps"
.LASF345:
	.string	"a24rd32v"
.LASF300:
	.string	"VmeGetDevInfo"
.LASF655:
	.string	"ValidateParam"
.LASF404:
	.string	"ADC4X250_STATUS_RUN_IDLE_shift"
.LASF471:
	.string	"ADC4X250_RANGE_VAL_2V"
.LASF56:
	.string	"CXRF_OVERLOAD"
.LASF230:
	.string	"extensions"
.LASF390:
	.string	"ADC4X250_R_CALIB_CONST_GAIN_CHx_base"
.LASF352:
	.string	"vme_vmt_t"
.LASF221:
	.string	"type"
.LASF64:
	.string	"CXRF_SERVER_HWERR_MASK"
.LASF276:
	.string	"reserved_int1"
.LASF277:
	.string	"reserved_int2"
.LASF66:
	.string	"CXCF_FLAG_CALCERR"
.LASF592:
	.string	"DEVDEP_NUM_LINES"
.LASF287:
	.string	"nelems"
.LASF269:
	.string	"prepare_retbufs"
.LASF398:
	.string	"ADC4X250_INT_ENA_CALIB_CMPLT_shift"
.LASF45:
	.string	"CXDTYPE_UINT64"
.LASF304:
	.string	"VmeAxxWr16"
.LASF630:
	.string	"start_mask"
.LASF353:
	.string	"ADC4X250_ADDRESS_MODIFIER"
.LASF462:
	.string	"ADC4X250_CLK_SRC_mask"
.LASF472:
	.string	"ADC4X250_RANGE_VAL_4V"
.LASF290:
	.string	"timestamps"
.LASF393:
	.string	"ADC4X250_DATA_ADDR_CHx_incr"
.LASF67:
	.string	"CXCF_FLAG_DEFUNCT"
.LASF624:
	.string	"cur_args"
.LASF607:
	.string	"PZFRAME_CHTYPE_AUTOUPDATED2"
.LASF499:
	.string	"ADC250_CHAN_HWINFO_first"
.LASF608:
	.string	"PZFRAME_CHTYPE_AUTOUPDATED4"
.LASF127:
	.string	"__gnuc_va_list"
.LASF268:
	.string	"read_measurements"
.LASF243:
	.string	"PZFRAME_CHTYPE_STATUS"
.LASF111:
	.string	"_chain"
.LASF482:
	.string	"ADC250_CHAN_WAITTIME"
.LASF527:
	.string	"ADC250_CHAN_LINE1ON"
.LASF375:
	.string	"ADC4X250_R_TEST_STATUS"
.LASF175:
	.string	"maxval"
.LASF651:
	.string	"v_uniq"
.LASF347:
	.string	"a32rd8v"
.LASF0:
	.string	"unsigned char"
.LASF576:
	.string	"ADC250_CHAN_LINE2TOTALMIN"
.LASF437:
	.string	"ADC4X250_CTRL_CALIB_shift"
.LASF620:
	.string	"irq_n"
.LASF314:
	.string	"get_dev_info"
.LASF61:
	.string	"CXRF_DRV_PROBL"
.LASF695:
	.string	"_IO_lock_t"
.LASF262:
	.string	"param_run"
.LASF362:
	.string	"ADC4X250_R_ADDRSPACE"
.LASF306:
	.string	"VmeAxxWr32"
.LASF311:
	.string	"VmeAxxRd16v"
.LASF23:
	.string	"float"
.LASF338:
	.string	"a16wr32v"
.LASF36:
	.string	"CXDTYPE_REPR_TEXT"
.LASF167:
	.string	"separators"
.LASF356:
	.string	"ADC4X250_R_UNIQ_ID"
.LASF203:
	.string	"DEVSTATE_OFFLINE"
.LASF193:
	.string	"p_string"
.LASF88:
	.string	"cx_module_term_proc"
.LASF38:
	.string	"CXDTYPE_INT8"
.LASF70:
	.string	"CXCF_FLAG_NOTFOUND"
.LASF496:
	.string	"ADC250_CHAN_TRIG_INPUT"
.LASF687:
	.string	"adcNNN_pll_preset_lkp"
.LASF453:
	.string	"ADC4X250_TRIG_ENA_VAL_DISABLE"
.LASF244:
	.string	"PZFRAME_CHTYPE_AUTOUPDATED"
.LASF85:
	.string	"CX_VALUE_DISABLED_MASK"
.LASF15:
	.string	"tv_usec"
.LASF628:
	.string	"do_return"
.LASF258:
	.string	"param_waittime"
.LASF294:
	.string	"VME_LYR_API_VERSION_MINOR"
.LASF206:
	.string	"DRVA_READ"
.LASF478:
	.string	"ADC250_CHAN_MARKER"
.LASF58:
	.string	"CXRF_INVAL"
.LASF578:
	.string	"ADC250_CHAN_LINE0TOTALMAX"
.LASF579:
	.string	"ADC250_CHAN_LINE1TOTALMAX"
.LASF598:
	.string	"MAX_ALLOWED_NUMPTS"
.LASF103:
	.string	"_IO_write_ptr"
.LASF350:
	.string	"a32wr32v"
.LASF580:
	.string	"ADC250_CHAN_LINE2TOTALMAX"
.LASF581:
	.string	"ADC250_CHAN_LINE3TOTALMAX"
.LASF26:
	.string	"cxdtype_t"
.LASF589:
	.string	"ADC1000_NUM_ADCS"
.LASF137:
	.string	"va_list"
.LASF11:
	.string	"__suseconds_t"
.LASF395:
	.string	"ADC4X250_INT_ENA_ALL"
.LASF199:
	.string	"CXSD_DRIVER_MODREC_MAGIC"
.LASF673:
	.string	"adc1000_irq_p"
.LASF323:
	.string	"a24rd8"
.LASF594:
	.string	"DEVDEP_PRETRIG"
.LASF528:
	.string	"ADC250_CHAN_LINE2ON"
.LASF249:
	.string	"PZFRAME_R_READY"
.LASF239:
	.string	"PZFRAME_CHTYPE_BIGC"
.LASF162:
	.string	"size"
.LASF643:
	.string	"FASTADC_SPACE_SIZE"
.LASF550:
	.string	"ADC250_CHAN_CLB_GAIN0"
.LASF551:
	.string	"ADC250_CHAN_CLB_GAIN1"
.LASF552:
	.string	"ADC250_CHAN_CLB_GAIN2"
.LASF553:
	.string	"ADC250_CHAN_CLB_GAIN3"
.LASF49:
	.string	"CXDTYPE_UCTEXT"
.LASF692:
	.string	"GNU C 4.8.5 20150623 (Red Hat 4.8.5-11) -mtune=generic -march=x86-64 -g -O0 -fPIC"
.LASF39:
	.string	"CXDTYPE_INT16"
.LASF318:
	.string	"a16wr16"
.LASF308:
	.string	"VmeAxxWr8v"
.LASF163:
	.string	"rsrvd1"
.LASF164:
	.string	"rsrvd2"
.LASF13:
	.string	"size_t"
.LASF251:
	.string	"PZFRAME_R_ERROR"
.LASF633:
	.string	"adc1000_privrec_t"
.LASF28:
	.string	"cx_time_t"
.LASF234:
	.string	"init_dev"
.LASF451:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits"
.LASF231:
	.string	"chan_nsegs"
.LASF431:
	.string	"ADC4X250_STATUS_PLL_WAS_UNLOCKED_shift"
.LASF388:
	.string	"ADC4X250_R_CALIB_CONST_ZERO_CHx_base"
.LASF591:
	.string	"ADC1000_MAX_NUMPTS"
.LASF292:
	.string	"pzframe_drv_t"
.LASF511:
	.string	"ADC250_CHAN_XS_PER_POINT"
.LASF421:
	.string	"ADC4X250_STATUS_CALIB_FAILED_bits"
.LASF256:
	.string	"param_shot"
.LASF107:
	.string	"_IO_save_base"
.LASF40:
	.string	"CXDTYPE_INT32"
.LASF342:
	.string	"a24wr16v"
.LASF320:
	.string	"a16wr32"
.LASF313:
	.string	"VmeAxxRd32v"
.LASF610:
	.string	"PLL_PRESET_INTERNAL"
.LASF652:
	.string	"ReturnClbInfo"
.LASF93:
	.string	"init_mod"
.LASF441:
	.string	"ADC4X250_TIMER_PRETRIG"
.LASF396:
	.string	"ADC4X250_INT_ENA_ADC_CMPLT_shift"
.LASF603:
	.string	"PZFRAME_CHTYPE_VALIDATE2"
.LASF604:
	.string	"PZFRAME_CHTYPE_VALIDATE4"
.LASF328:
	.string	"a32wr8"
.LASF584:
	.string	"ADC250_NUM_ADCS"
.LASF409:
	.string	"ADC4X250_STATUS_CALIB"
.LASF179:
	.string	"psp_var_p_lookup_t"
.LASF508:
	.string	"ADC250_CHAN_HWINFO_count"
.LASF172:
	.string	"psp_var_p_plugin_t"
.LASF632:
	.string	"adcbuf"
.LASF400:
	.string	"ADC4X250_INT_ENA_PGA_OVERRNG_shift"
.LASF678:
	.string	"auxinfo"
.LASF660:
	.string	"StopDevice"
.LASF246:
	.string	"chtype"
.LASF121:
	.string	"__pad2"
.LASF34:
	.string	"CXDTYPE_REPR_INT"
.LASF140:
	.string	"CX_US_C"
.LASF593:
	.string	"DEVDEP_MAX_NUMPTS"
.LASF446:
	.string	"ADC4X250_ADC_DECIM_RATIO_MAX"
.LASF595:
	.string	"DEVDEP_XS_PER_POINT"
.LASF84:
	.string	"CX_VALUE_LIT_MASK"
.LASF271:
	.string	"reserved_func_ptr2"
.LASF177:
	.string	"list"
.LASF278:
	.string	"measurement_start"
.LASF30:
	.string	"CXDTYPE_REPR_MASK"
.LASF373:
	.string	"ADC4X250_R_ADC_TRIG_LEVEL"
.LASF469:
	.string	"ADC4X250_RANGE_VAL_0_5V"
.LASF224:
	.string	"privrecsize"
.LASF289:
	.string	"rflags"
.LASF31:
	.string	"CXDTYPE_REPR_SHIFT"
.LASF247:
	.string	"refchn"
.LASF637:
	.string	"PARAM_ISTART"
.LASF612:
	.string	"clk_src"
.LASF41:
	.string	"CXDTYPE_INT64"
.LASF504:
	.string	"ADC250_CHAN_PGA_VAR"
.LASF529:
	.string	"ADC250_CHAN_LINE3ON"
.LASF569:
	.string	"ADC250_CHAN_INT0"
.LASF570:
	.string	"ADC250_CHAN_INT1"
.LASF571:
	.string	"ADC250_CHAN_INT2"
.LASF572:
	.string	"ADC250_CHAN_INT3"
.LASF240:
	.string	"PZFRAME_CHTYPE_PZFRAME_STD"
.LASF554:
	.string	"ADC250_CHAN_CUR_PLL1_CTRL"
.LASF498:
	.string	"ADC250_CHAN_DEVICE_ID"
.LASF574:
	.string	"ADC250_CHAN_LINE0TOTALMIN"
.LASF452:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_mask"
.LASF134:
	.string	"_next"
.LASF537:
	.string	"ADC250_CHAN_LINE3RANGEMAX"
.LASF679:
	.string	"bus_major"
.LASF194:
	.string	"p_mstring"
.LASF575:
	.string	"ADC250_CHAN_LINE1TOTALMIN"
.LASF74:
	.string	"CXCF_FLAG_ALARM_RELAX"
.LASF71:
	.string	"CXCF_FLAG_COLOR_WEIRD"
.LASF693:
	.string	"adc1000_drv.c"
.LASF254:
	.string	"_pzframe_drv_t_struct"
.LASF577:
	.string	"ADC250_CHAN_LINE3TOTALMIN"
.LASF642:
	.string	"PARAM_RUN"
.LASF253:
	.string	"pzframe_start_measurements_t"
.LASF83:
	.string	"CXCF_FLAG_4WRONLY_MASK"
.LASF616:
	.string	"PER_ADC_BUF_LEN"
.LASF417:
	.string	"ADC4X250_STATUS_PGA_OVERRNG_shift"
.LASF86:
	.string	"CX_VALUE_COMMAND"
.LASF174:
	.string	"minval"
.LASF305:
	.string	"VmeAxxRd16"
.LASF263:
	.string	"reserved_param1"
.LASF264:
	.string	"reserved_param2"
.LASF274:
	.string	"measuring_now"
.LASF80:
	.string	"CXCF_FLAG_SYSERR_MASK"
.LASF265:
	.string	"start_measurements"
.LASF426:
	.string	"ADC4X250_STATUS_PGA_FLASH_FAILED"
.LASF359:
	.string	"ADC4X250_R_INT_VECTOR"
.LASF406:
	.string	"ADC4X250_STATUS_WAIT_FOR_TRIG_shift"
.LASF68:
	.string	"CXCF_FLAG_OTHEROP"
.LASF227:
	.string	"max_businfo_n"
.LASF334:
	.string	"a16wr8v"
.LASF664:
	.string	"ReadMeasurements"
.LASF364:
	.string	"ADC4X250_R_COMPILATION_TIMESTAMP_H"
.LASF307:
	.string	"VmeAxxRd32"
.LASF62:
	.string	"CXRF_NO_DRV"
.LASF78:
	.string	"CXCF_FLAG_HWERR_MASK"
.LASF656:
	.string	"ActivatePLLPreset"
.LASF354:
	.string	"ADC4X250_R_DEVICE_ID"
.LASF510:
	.string	"ADC250_CHAN_ELAPSED"
.LASF372:
	.string	"ADC4X250_R_ADC_RECORDER_ADDR"
.LASF344:
	.string	"a24wr32v"
.LASF689:
	.string	"stderr"
.LASF204:
	.string	"DEVSTATE_NOTREADY"
.LASF416:
	.string	"ADC4X250_STATUS_TRIG_ORDER_mask"
.LASF613:
	.string	"pll1"
.LASF91:
	.string	"name"
.LASF670:
	.string	"add_rflags"
.LASF135:
	.string	"_sbuf"
.LASF330:
	.string	"a32wr16"
.LASF109:
	.string	"_IO_save_end"
.LASF187:
	.string	"p_psp"
.LASF275:
	.string	"run_mode"
.LASF25:
	.string	"rflags_t"
.LASF143:
	.string	"PSP_T_NOP"
.LASF439:
	.string	"ADC4X250_CTRL_VME_SOFT_IRQ_shift"
.LASF286:
	.string	"dtypes"
.LASF681:
	.string	"adc1000_term_d"
.LASF29:
	.string	"CXDTYPE_SIZE_MASK"
.LASF130:
	.string	"fp_offset"
.LASF688:
	.string	"adc1000_params"
.LASF10:
	.string	"__time_t"
.LASF638:
	.string	"PARAM_WAITTIME"
.LASF129:
	.string	"gp_offset"
.LASF9:
	.string	"sizetype"
.LASF386:
	.string	"ADC4X250_R_CALIB_CONST_DYN_CHx_base"
.LASF153:
	.string	"PSP_T_STRING"
.LASF487:
	.string	"ADC250_CHAN_PTSOFS"
.LASF445:
	.string	"ADC4X250_ADC_DECIM_RATIO_MIN"
.LASF332:
	.string	"a32wr32"
.LASF440:
	.string	"ADC4X250_CTRL_VME_SOFT_IRQ"
.LASF165:
	.string	"psp_var_p_include_t"
.LASF315:
	.string	"hal_ioctl"
.LASF1:
	.string	"short unsigned int"
.LASF4:
	.string	"signed char"
.LASF402:
	.string	"ADC4X250_INT_ENA_CALIB_FAILED_shift"
.LASF557:
	.string	"ADC250_CHAN_STATS_first"
.LASF382:
	.string	"ADC4X250_R_APD_HV_CTRL_CHx_base"
.LASF188:
	.string	"p_plugin"
.LASF272:
	.string	"retbufs"
.LASF216:
	.string	"count"
.LASF534:
	.string	"ADC250_CHAN_LINE0RANGEMAX"
.LASF674:
	.string	"total_irqs"
.LASF535:
	.string	"ADC250_CHAN_LINE1RANGEMAX"
.LASF186:
	.string	"p_include"
.LASF536:
	.string	"ADC250_CHAN_LINE2RANGEMAX"
.LASF297:
	.string	"vme_irq_proc"
.LASF8:
	.string	"__off64_t"
.LASF79:
	.string	"CXCF_FLAG_SFERR_MASK"
.LASF436:
	.string	"ADC4X250_CTRL_ADC_BREAK_ACK"
.LASF185:
	.string	"psp_var_p_real_t"
.LASF368:
	.string	"ADC4X250_R_TIMER"
.LASF101:
	.string	"_IO_read_base"
.LASF460:
	.string	"ADC4X250_CLK_SRC_shift"
.LASF119:
	.string	"_offset"
.LASF310:
	.string	"VmeAxxWr16v"
.LASF618:
	.string	"lvmt"
.LASF89:
	.string	"magicnumber"
.LASF183:
	.string	"psp_var_p_string_t"
.LASF273:
	.string	"state"
.LASF106:
	.string	"_IO_buf_end"
.LASF391:
	.string	"ADC4X250_R_CALIB_CONST_GAIN_CHx_incr"
.LASF497:
	.string	"ADC250_CHAN_PLL_PRESET"
.LASF319:
	.string	"a16rd16"
.LASF657:
	.string	"Init1Param"
.LASF149:
	.string	"PSP_T_SELECTOR"
.LASF309:
	.string	"VmeAxxRd8v"
.LASF419:
	.string	"ADC4X250_STATUS_PGA_OVERRNG_mask"
.LASF266:
	.string	"trggr_measurements"
.LASF125:
	.string	"_mode"
.LASF102:
	.string	"_IO_write_base"
.LASF360:
	.string	"ADC4X250_R_INT_LINE"
.LASF54:
	.string	"CXRF_IO_TIMEOUT"
.LASF467:
	.string	"ADC4X250_PGA_RANGE_bits"
.LASF505:
	.string	"ADC250_CHAN_BASE_UNIQ_ID"
.LASF302:
	.string	"VmeAxxWr8"
.LASF57:
	.string	"CXRF_UNSUPPORTED"
.LASF424:
	.string	"ADC4X250_STATUS_PGA_NOT_DETECTED"
.LASF668:
	.string	"devptr"
.LASF6:
	.string	"long int"
.LASF515:
	.string	"ADC250_CHAN_CUR_NUMPTS"
.LASF694:
	.string	"/home/user/work/hw4cx/drivers/vme/local"
.LASF640:
	.string	"PARAM_ELAPSED"
.LASF157:
	.string	"psp_lkp_t"
.LASF413:
	.string	"ADC4X250_STATUS_PLL_LOCKED"
.LASF150:
	.string	"PSP_T_LOOKUP"
.LASF646:
	.string	"format"
.LASF133:
	.string	"_IO_marker"
.LASF95:
	.string	"cx_module_rec_t"
.LASF448:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_bits"
.LASF96:
	.string	"timeval"
.LASF573:
	.string	"ADC250_CHAN_STATS_last"
.LASF684:
	.string	"adcNNN_timing_lkp"
.LASF171:
	.string	"privptr"
.LASF156:
	.string	"label"
.LASF346:
	.string	"a32wr8v"
.LASF238:
	.string	"PZFRAME_CHTYPE_UNSUPPORTED"
.LASF412:
	.string	"ADC4X250_STATUS_PLL_LOCKED_shift"
.LASF513:
	.string	"ADC250_CHAN_XS_FACTOR"
.LASF283:
	.string	"pzframe_read_measurements_t"
.LASF444:
	.string	"ADC4X250_TIMER_4CH_MAX"
.LASF226:
	.string	"min_businfo_n"
.LASF138:
	.string	"CX_GS_C"
.LASF663:
	.string	"AbortMeasurements"
.LASF76:
	.string	"CXCF_FLAG_COLOR_YELLOW"
.LASF213:
	.string	"CxsdDevInitFunc"
.LASF450:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_shift"
.LASF21:
	.string	"uint16"
.LASF270:
	.string	"reserved_func_ptr1"
.LASF649:
	.string	"v_devid"
.LASF3:
	.string	"long unsigned int"
.LASF279:
	.string	"value_istart"
.LASF601:
	.string	"PZFRAME_CHTYPE_BIGC2"
.LASF602:
	.string	"PZFRAME_CHTYPE_BIGC4"
.LASF166:
	.string	"descr"
.LASF583:
	.string	"ADC250_NUMCHANS"
.LASF235:
	.string	"term_dev"
.LASF329:
	.string	"a32rd8"
.LASF12:
	.string	"char"
.LASF680:
	.string	"bus_minor"
.LASF232:
	.string	"chan_info"
.LASF530:
	.string	"ADC250_CHAN_LINE0RANGEMIN"
.LASF42:
	.string	"CXDTYPE_UINT8"
.LASF405:
	.string	"ADC4X250_STATUS_RUN_IDLE"
.LASF531:
	.string	"ADC250_CHAN_LINE1RANGEMIN"
.LASF19:
	.string	"uint32"
.LASF159:
	.string	"table"
.LASF532:
	.string	"ADC250_CHAN_LINE2RANGEMIN"
.LASF105:
	.string	"_IO_buf_base"
.LASF533:
	.string	"ADC250_CHAN_LINE3RANGEMIN"
.LASF403:
	.string	"ADC4X250_INT_ENA_CALIB_FAILED"
.LASF217:
	.string	"dtype"
.LASF178:
	.string	"psp_var_p_selector_t"
.LASF312:
	.string	"VmeAxxWr32v"
.LASF182:
	.string	"psp_var_p_flag_t"
.LASF73:
	.string	"CXCF_FLAG_ALARM_ALARM"
.LASF100:
	.string	"_IO_read_end"
.LASF250:
	.string	"PZFRAME_R_DOING"
.LASF65:
	.string	"CXRF_SERVER_SFERR_MASK"
.LASF94:
	.string	"term_mod"
.LASF69:
	.string	"CXCF_FLAG_PRGLYCHG"
.LASF324:
	.string	"a24wr16"
.LASF63:
	.string	"CXRF_OFFLINE"
.LASF97:
	.string	"_IO_FILE"
.LASF168:
	.string	"equals_c"
.LASF361:
	.string	"ADC4X250_R_BASEADDR"
.LASF461:
	.string	"ADC4X250_CLK_SRC_bits"
.LASF335:
	.string	"a16rd8v"
.LASF430:
	.string	"ADC4X250_STATUS_CMD_QUEUE_OVERFLOW"
.LASF379:
	.string	"ADC4X250_R_PLL2_CTRL"
.LASF215:
	.string	"CxsdDevChanProc"
.LASF665:
	.string	"numduplets"
.LASF384:
	.string	"ADC4X250_R_THERMOSENSOR_CSR"
.LASF367:
	.string	"ADC4X250_R_CTRL"
.LASF259:
	.string	"param_stop"
.LASF411:
	.string	"ADC4X250_STATUS_ADC_CMPLT"
.LASF597:
	.string	"ADC250_DTYPE"
.LASF197:
	.string	"psp_paramdescr_t"
.LASF588:
	.string	"ADC250_MAX_VALUE"
.LASF590:
	.string	"ADC1000_NUM_LINES"
.LASF378:
	.string	"ADC4X250_R_PLL1_CTRL"
.LASF176:
	.string	"psp_var_p_int_t"
.LASF142:
	.string	"PSP_T_NULL"
.LASF455:
	.string	"ADC4X250_TRIG_ENA_VAL_EXT"
.LASF365:
	.string	"ADC4X250_R_COMPILATION_TIMESTAMP_L"
.LASF414:
	.string	"ADC4X250_STATUS_TRIG_ORDER_shift"
.LASF622:
	.string	"jumpers"
.LASF331:
	.string	"a32rd16"
.LASF326:
	.string	"a24wr32"
.LASF389:
	.string	"ADC4X250_R_CALIB_CONST_ZERO_CHx_incr"
.LASF219:
	.string	"CxsdChanInfoRec"
.LASF120:
	.string	"__pad1"
.LASF122:
	.string	"__pad3"
.LASF123:
	.string	"__pad4"
.LASF124:
	.string	"__pad5"
.LASF662:
	.string	"TrggrMeasurements"
.LASF691:
	.string	"adc1000_driver_modrec"
.LASF189:
	.string	"p_int"
.LASF399:
	.string	"ADC4X250_INT_ENA_CALIB_CMPLT"
.LASF355:
	.string	"ADC4X250_R_VERSION"
.LASF110:
	.string	"_markers"
.LASF136:
	.string	"_pos"
.LASF210:
	.string	"IS_AUTOUPDATED_TRUSTED"
.LASF160:
	.string	"_psp_paramdescr_t_struct"
.LASF87:
	.string	"cx_module_init_func"
.LASF237:
	.string	"CxsdDriverModRec"
.LASF480:
	.string	"ADC250_CHAN_STOP"
.LASF490:
	.string	"ADC250_CHAN_FRQDIV"
.LASF447:
	.string	"ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_shift"
.LASF33:
	.string	"CXDTYPE_REPR_UNKNOWN"
.LASF473:
	.string	"ADC250_CHAN_DATA"
.LASF184:
	.string	"psp_var_p_mstring_t"
.LASF146:
	.string	"PSP_T_PSP"
.LASF542:
	.string	"ADC250_CHAN_CLB_DYN0"
.LASF543:
	.string	"ADC250_CHAN_CLB_DYN1"
.LASF544:
	.string	"ADC250_CHAN_CLB_DYN2"
.LASF545:
	.string	"ADC250_CHAN_CLB_DYN3"
.LASF333:
	.string	"a32rd32"
.LASF190:
	.string	"p_selector"
.LASF457:
	.string	"ADC4X250_TRIG_ENA_VAL_BP_SYNC"
.LASF24:
	.string	"double"
.LASF218:
	.string	"max_nelems"
.LASF481:
	.string	"ADC250_CHAN_ISTART"
.LASF479:
	.string	"ADC250_CHAN_SHOT"
.LASF458:
	.string	"ADC4X250_PLL_UPDATE_PLL_CTRL_UPDATE_shift"
.LASF35:
	.string	"CXDTYPE_REPR_FLOAT"
.LASF556:
	.string	"ADC250_CHAN_MIN0"
.LASF394:
	.string	"ADC4X250_INT_ENA_ALL_shift"
.LASF559:
	.string	"ADC250_CHAN_MIN2"
.LASF560:
	.string	"ADC250_CHAN_MIN3"
.LASF491:
	.string	"ADC250_CHAN_RANGE0"
.LASF492:
	.string	"ADC250_CHAN_RANGE1"
.LASF493:
	.string	"ADC250_CHAN_RANGE2"
.LASF494:
	.string	"ADC250_CHAN_RANGE3"
.LASF209:
	.string	"IS_AUTOUPDATED_YES"
.LASF92:
	.string	"comment"
.LASF486:
	.string	"ADC250_CHAN_RUN"
.LASF631:
	.string	"data_rqd"
.LASF686:
	.string	"adcNNN_range_lkp"
.LASF48:
	.string	"CXDTYPE_TEXT"
.LASF51:
	.string	"CXRF_CLIENT_MASK"
.LASF222:
	.string	"data"
.LASF200:
	.string	"CXSD_DRIVER_MODREC_VERSION_MAJOR"
.LASF211:
	.string	"IS_AUTOUPDATED_unused3"
.LASF369:
	.string	"ADC4X250_R_ADC_DECIM_RATIO"
.LASF284:
	.string	"pzframe_prepare_retbufs_t"
.LASF438:
	.string	"ADC4X250_CTRL_CALIB"
.LASF191:
	.string	"p_lookup"
.LASF427:
	.string	"ADC4X250_STATUS_FPGARTX_TIMEOUT_FAILED_shift"
.LASF236:
	.string	"do_rw"
.LASF248:
	.string	"pzframe_chinfo_t"
.LASF425:
	.string	"ADC4X250_STATUS_PGA_FLASH_FAILED_shift"
.LASF147:
	.string	"PSP_T_PLUGIN"
.LASF627:
	.string	"retdata"
.LASF614:
	.string	"pll2"
.LASF507:
	.string	"ADC250_CHAN_HWINFO_last"
.LASF433:
	.string	"ADC4X250_CTRL_START_shift"
.LASF337:
	.string	"a16rd16v"
.LASF151:
	.string	"PSP_T_FLAG"
.LASF484:
	.string	"ADC250_CHAN_CALC_STATS"
.LASF225:
	.string	"paramtable"
.LASF645:
	.string	"FASTADC_ADDRESS_MODIFIER"
.LASF495:
	.string	"ADC250_CHAN_TRIG_TYPE"
.LASF522:
	.string	"ADC250_CHAN_OVERFLOW0"
.LASF523:
	.string	"ADC250_CHAN_OVERFLOW1"
.LASF524:
	.string	"ADC250_CHAN_OVERFLOW2"
.LASF525:
	.string	"ADC250_CHAN_OVERFLOW3"
.LASF14:
	.string	"tv_sec"
.LASF340:
	.string	"a24wr8v"
.LASF321:
	.string	"a16rd32"
.LASF435:
	.string	"ADC4X250_CTRL_ADC_BREAK_ACK_shift"
.LASF514:
	.string	"ADC250_CHAN_CUR_PTSOFS"
.LASF16:
	.string	"long long unsigned int"
.LASF20:
	.string	"int16"
.LASF161:
	.string	"offset"
.LASF115:
	.string	"_cur_column"
.LASF512:
	.string	"ADC250_CHAN_XS_DIVISOR"
.LASF55:
	.string	"CXRF_REM_C_PROBL"
.LASF376:
	.string	"ADC4X250_R_PLL_UPDATE"
.LASF682:
	.string	"pll_presets_table"
.LASF392:
	.string	"ADC4X250_DATA_ADDR_CHx_base"
.LASF432:
	.string	"ADC4X250_STATUS_PLL_WAS_UNLOCKED"
.LASF506:
	.string	"ADC250_CHAN_PGA_UNIQ_ID"
.LASF555:
	.string	"ADC250_CHAN_CUR_PLL2_CTRL"
.LASF428:
	.string	"ADC4X250_STATUS_FPGARTX_TIMEOUT_FAILED"
.LASF474:
	.string	"ADC250_CHAN_LINE0"
.LASF475:
	.string	"ADC250_CHAN_LINE1"
.LASF476:
	.string	"ADC250_CHAN_LINE2"
.LASF477:
	.string	"ADC250_CHAN_LINE3"
.LASF207:
	.string	"DRVA_WRITE"
.LASF377:
	.string	"ADC4X250_R_CLK_SRC"
.LASF228:
	.string	"layer"
.LASF77:
	.string	"CXCF_FLAG_CDR_MASK"
.LASF466:
	.string	"ADC4X250_PGA_RANGE_shift"
.LASF298:
	.string	"vme_stat_proc"
.LASF349:
	.string	"a32rd16v"
.LASF672:
	.string	"action"
.LASF108:
	.string	"_IO_backup_base"
.LASF669:
	.string	"PrepareRetbufs"
.LASF99:
	.string	"_IO_read_ptr"
.LASF59:
	.string	"CXRF_WRONG_DEV"
.LASF158:
	.string	"psp_pluginp_t"
.LASF683:
	.string	"chinfo"
.LASF488:
	.string	"ADC250_CHAN_NUMPTS"
.LASF18:
	.string	"int32"
.LASF385:
	.string	"ADC4X250_R_THERMOSENSOR_TEMP"
.LASF32:
	.string	"CXDTYPE_USGN_MASK"
.LASF208:
	.string	"IS_AUTOUPDATED_NOT"
.LASF381:
	.string	"ADC4X250_R_PGA_RANGE_CHx_incr"
.LASF242:
	.string	"PZFRAME_CHTYPE_VALIDATE"
.LASF81:
	.string	"CXCF_FLAG_ALARM_MASK"
.LASF654:
	.string	"check_snprintf"
.LASF303:
	.string	"VmeAxxRd8"
.LASF192:
	.string	"p_flag"
.LASF605:
	.string	"PZFRAME_CHTYPE_STATUS2"
.LASF606:
	.string	"PZFRAME_CHTYPE_STATUS4"
.LASF288:
	.string	"values"
.LASF205:
	.string	"DEVSTATE_OPERATING"
.LASF114:
	.string	"_old_offset"
.LASF636:
	.string	"PARAM_SHOT"
.LASF155:
	.string	"psp_paramtype_t"
.LASF397:
	.string	"ADC4X250_INT_ENA_ADC_CMPLT"
.LASF629:
	.string	"force_read"
.LASF501:
	.string	"ADC250_CHAN_PGA_SW_VER"
.LASF17:
	.string	"long long int"
.LASF387:
	.string	"ADC4X250_R_CALIB_CONST_DYN_CHx_incr"
.LASF60:
	.string	"CXRF_CFG_PROBL"
.LASF586:
	.string	"ADC250_MAX_NUMPTS"
.LASF113:
	.string	"_flags2"
.LASF539:
	.string	"ADC250_CHAN_CLB_FAIL1"
.LASF443:
	.string	"ADC4X250_TIMER_2CH_MAX"
.LASF223:
	.string	"CxsdDevExtRef"
.LASF181:
	.string	"is_defval"
.LASF295:
	.string	"VME_LYR_API_VERSION"
.LASF252:
	.string	"PZFRAME_R_IGNOR"
.LASF366:
	.string	"ADC4X250_R_STATUS"
.LASF46:
	.string	"CXDTYPE_SINGLE"
.LASF558:
	.string	"ADC250_CHAN_MIN1"
.LASF72:
	.string	"CXCF_FLAG_CDA_MASK"
.LASF282:
	.string	"pzframe_abort_measurements_t"
.LASF383:
	.string	"ADC4X250_R_APD_HV_CTRL_CHx_incr"
.LASF634:
	.string	"HEARTBEAT_FREQ"
.LASF408:
	.string	"ADC4X250_STATUS_CALIB_shift"
.LASF152:
	.string	"PSP_T_REAL"
.LASF585:
	.string	"ADC250_NUM_LINES"
.LASF407:
	.string	"ADC4X250_STATUS_WAIT_FOR_TRIG"
.LASF420:
	.string	"ADC4X250_STATUS_CALIB_FAILED_shift"
.LASF667:
	.string	"adcNNN_hbt"
.LASF374:
	.string	"ADC4X250_R_TEST_CTRL"
.LASF325:
	.string	"a24rd16"
.LASF671:
	.string	"adc1000_rw_p"
.LASF37:
	.string	"CXDTYPE_UNKNOWN"
.LASF485:
	.string	"ADC250_CHAN_RUN_MODE"
.LASF357:
	.string	"ADC4X250_R_INT_ENA"
.LASF339:
	.string	"a16rd32v"
.LASF401:
	.string	"ADC4X250_INT_ENA_PGA_OVERRNG"
.LASF169:
	.string	"psp_var_p_psp_t"
.LASF281:
	.string	"pzframe_trggr_measurements_t"
.LASF690:
	.string	"pzframe_drv_run_mode_lkp"
.LASF465:
	.string	"ADC4X250_CLK_SRC_VAL_BP"
.LASF456:
	.string	"ADC4X250_TRIG_ENA_VAL_BP"
.LASF139:
	.string	"CX_RS_C"
.LASF454:
	.string	"ADC4X250_TRIG_ENA_VAL_INT"
.LASF380:
	.string	"ADC4X250_R_PGA_RANGE_CHx_base"
.LASF599:
	.string	"NLGE2"
.LASF600:
	.string	"NLGE4"
.LASF144:
	.string	"PSP_T_VOID"
.LASF214:
	.string	"CxsdDevTermProc"
.LASF625:
	.string	"nxt_args"
.LASF90:
	.string	"version"
.LASF459:
	.string	"ADC4X250_PLL_UPDATE_PLL_CTRL_UPDATE_mask"
.LASF212:
	.string	"DO_IGNORE_UPD_CYCLE"
.LASF2:
	.string	"unsigned int"
.LASF676:
	.string	"businfocount"
.LASF423:
	.string	"ADC4X250_STATUS_PGA_NOT_DETECTED_shift"
.LASF609:
	.string	"PLL_PRESET_NONE"
.LASF641:
	.string	"PARAM_RUN_MODE"
.LASF327:
	.string	"a24rd32"
.LASF280:
	.string	"value_waittime"
.LASF644:
	.string	"FASTADC_ADDRESS_SIZE"
.LASF260:
	.string	"param_elapsed"
.LASF317:
	.string	"a16rd8"
.LASF5:
	.string	"short int"
.LASF296:
	.string	"VME_LYR_OPTION_NONE"
.LASF621:
	.string	"irq_vect"
.LASF116:
	.string	"_vtable_offset"
.LASF415:
	.string	"ADC4X250_STATUS_TRIG_ORDER_bits"
.LASF202:
	.string	"CXSD_DRIVER_MODREC_VERSION"
.LASF351:
	.string	"a32rd32v"
.LASF635:
	.string	"HEARTBEAT_USECS"
.LASF538:
	.string	"ADC250_CHAN_CLB_FAIL0"
.LASF540:
	.string	"ADC250_CHAN_CLB_FAIL2"
.LASF541:
	.string	"ADC250_CHAN_CLB_FAIL3"
.LASF653:
	.string	"status"
.LASF195:
	.string	"p_real"
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-11)"
	.section	.note.GNU-stack,"",@progbits
