	.file	"test_rfmeas_a32r16.c"
	.globl sequence
	.section	".data"
	.align 2
	.type	sequence,@object
	.size	sequence,56
sequence:
	.long	16
	.long	-2147483646
	.long	16
	.long	-2147483646
	.long	32
	.long	-1979711480
	.long	32
	.long	-1979711480
	.long	16
	.long	-2147483646
	.long	16
	.long	-2147483646
	.long	0
	.long	0
	.section	.rodata
	.align 2
.LC0:
	.string	"libvme_init()=%d\n"
	.align 2
.LC1:
	.string	"libvme_ctl(VME_SYSTEM_W,%d)=%d\n"
	.align 2
.LC2:
	.string	"libvme_ctl(VME_AM_W,0x%02x)=%d\n"
	.align 2
.LC3:
	.string	"%d:0x%08x "
	.align 2
.LC4:
	.string	"%02d 0x%04x\n"
	.align 2
.LC5:
	.string	"%02d 0x%08x\n"
	.section	".text"
	.align 2
	.globl main
	.type	main,@function
main:
	stwu 1,-64(1)
	mflr 0
	stw 31,60(1)
	stw 0,68(1)
	mr 31,1
	bl libvme_init
	mr 0,3
	stw 0,8(31)
	lis 9,stderr@ha
	lwz 3,stderr@l(9)
	lis 9,.LC0@ha
	la 4,.LC0@l(9)
	lwz 5,8(31)
	bl fprintf
	lwz 0,8(31)
	cmpwi 0,0,0
	bge 0,.L101
	li 0,1
	stw 0,32(31)
	b .L100
.L101:
	li 0,1
	stw 0,12(31)
	addi 0,31,12
	li 3,16
	mr 4,0
	bl libvme_ctl
	mr 0,3
	stw 0,8(31)
	lis 9,stderr@ha
	lwz 3,stderr@l(9)
	lis 9,.LC1@ha
	la 4,.LC1@l(9)
	lwz 5,12(31)
	lwz 6,8(31)
	bl fprintf
	li 0,9
	stw 0,16(31)
	addi 0,31,16
	li 3,1
	mr 4,0
	bl libvme_ctl
	mr 0,3
	stw 0,8(31)
	lis 9,stderr@ha
	lwz 3,stderr@l(9)
	lis 9,.LC2@ha
	la 4,.LC2@l(9)
	lwz 5,16(31)
	lwz 6,8(31)
	bl fprintf
	lis 9,sequence@ha
	la 0,sequence@l(9)
	stw 0,20(31)
.L102:
	lwz 9,20(31)
	lwz 0,0(9)
	cmpwi 0,0,0
	bne 0,.L105
	b .L103
.L105:
	lwz 11,20(31)
	lwz 10,20(31)
	lis 9,.LC3@ha
	la 3,.LC3@l(9)
	lwz 4,0(11)
	lwz 5,4(10)
	bl printf
	lwz 9,20(31)
	lwz 0,0(9)
	cmpwi 0,0,16
	bne 0,.L106
	lwz 9,20(31)
	addi 0,31,24
	lwz 3,4(9)
	mr 4,0
	bl libvme_read_a32_word
	mr 0,3
	stw 0,8(31)
	lhz 0,24(31)
	rlwinm 0,0,0,0xffff
	lis 9,.LC4@ha
	la 3,.LC4@l(9)
	lwz 4,8(31)
	mr 5,0
	bl printf
	b .L104
.L106:
	lwz 9,20(31)
	addi 0,31,28
	lwz 3,4(9)
	mr 4,0
	bl libvme_read_a32_dword
	mr 0,3
	stw 0,8(31)
	lis 9,.LC5@ha
	la 3,.LC5@l(9)
	lwz 4,8(31)
	lwz 5,28(31)
	bl printf
.L104:
	lwz 9,20(31)
	addi 0,9,8
	stw 0,20(31)
	b .L102
.L103:
	li 0,0
	stw 0,32(31)
.L100:
	lwz 3,32(31)
	lwz 11,0(1)
	lwz 0,4(11)
	mtlr 0
	lwz 31,-4(11)
	mr 1,11
	blr
.Lfe1:
	.size	main,.Lfe1-main
	.align 2
	.type	libvme_read_a32_word,@function
libvme_read_a32_word:
	stwu 1,-48(1)
	mflr 0
	stw 29,36(1)
	stw 31,44(1)
	stw 0,52(1)
	mr 31,1
	stw 3,8(31)
	stw 4,12(31)
	lwz 0,8(31)
	srwi 0,0,26
	stw 0,20(31)
	lwz 0,20(31)
	rlwinm 0,0,0,26,31
	stw 0,20(31)
	lwz 0,8(31)
	rlwinm 9,0,0,6,31
	stw 9,24(31)
.L36:
	lis 9,__libvme_shm_ptr@ha
	lwz 3,__libvme_shm_ptr@l(9)
	bl atomic_dec_if_positive
	mr 9,3
	li 0,-1
	cmpw 0,9,0
	beq 0,.L38
	b .L37
.L38:
	lis 11,__libvme_mutual_access@ha
	lis 9,__libvme_mutual_access@ha
	lwz 9,__libvme_mutual_access@l(9)
	addi 0,9,1
	stw 0,__libvme_mutual_access@l(11)
	bl sched_yield
	b .L36
.L37:
	lis 9,__libvme_regs@ha
	lwz 9,__libvme_regs@l(9)
	lwz 0,0(9)
	rlwinm 0,0,0,23,16
	lwz 10,20(31)
	slwi 9,10,9
	or 0,0,9
	lis 9,__libvme_regs@ha
	lwz 9,__libvme_regs@l(9)
	stw 0,0(9)
	lwz 11,12(31)
	lis 9,__libvme_a32@ha
	lwz 0,__libvme_a32@l(9)
	lwz 10,24(31)
	add 9,10,0
	lhz 0,0(9)
	sth 0,0(11)
	lis 9,__libvme_reg2@ha
	lwz 9,__libvme_reg2@l(9)
	lwz 29,0(9)
	lis 9,__libvme_shm_ptr@ha
	lwz 3,__libvme_shm_ptr@l(9)
	bl atomic_inc
	rlwinm 0,29,0,26,26
	cmpwi 0,0,0
	beq 0,.L39
	lis 9,__libvme_reg2@ha
	lwz 9,__libvme_reg2@l(9)
	li 0,32
	stw 0,0(9)
	li 0,-1
	stw 0,16(31)
	b .L35
.L39:
	li 9,0
	stw 9,16(31)
.L35:
	lwz 3,16(31)
	lwz 11,0(1)
	lwz 0,4(11)
	mtlr 0
	lwz 29,-12(11)
	lwz 31,-4(11)
	mr 1,11
	blr
.Lfe2:
	.size	libvme_read_a32_word,.Lfe2-libvme_read_a32_word
	.align 2
	.type	libvme_read_a32_dword,@function
libvme_read_a32_dword:
	stwu 1,-48(1)
	mflr 0
	stw 29,36(1)
	stw 31,44(1)
	stw 0,52(1)
	mr 31,1
	stw 3,8(31)
	stw 4,12(31)
	lwz 0,8(31)
	srwi 0,0,26
	stw 0,20(31)
	lwz 0,20(31)
	rlwinm 0,0,0,26,31
	stw 0,20(31)
	lwz 0,8(31)
	rlwinm 9,0,0,6,31
	stw 9,24(31)
.L51:
	lis 9,__libvme_shm_ptr@ha
	lwz 3,__libvme_shm_ptr@l(9)
	bl atomic_dec_if_positive
	mr 9,3
	li 0,-1
	cmpw 0,9,0
	beq 0,.L53
	b .L52
.L53:
	lis 11,__libvme_mutual_access@ha
	lis 9,__libvme_mutual_access@ha
	lwz 9,__libvme_mutual_access@l(9)
	addi 0,9,1
	stw 0,__libvme_mutual_access@l(11)
	bl sched_yield
	b .L51
.L52:
	lis 9,__libvme_regs@ha
	lwz 9,__libvme_regs@l(9)
	lwz 0,0(9)
	rlwinm 0,0,0,23,16
	lwz 10,20(31)
	slwi 9,10,9
	or 0,0,9
	lis 9,__libvme_regs@ha
	lwz 9,__libvme_regs@l(9)
	stw 0,0(9)
	lwz 11,12(31)
	lis 9,__libvme_a32@ha
	lwz 0,__libvme_a32@l(9)
	lwz 10,24(31)
	add 9,10,0
	lwz 0,0(9)
	stw 0,0(11)
	lis 9,__libvme_reg2@ha
	lwz 9,__libvme_reg2@l(9)
	lwz 29,0(9)
	lis 9,__libvme_shm_ptr@ha
	lwz 3,__libvme_shm_ptr@l(9)
	bl atomic_inc
	rlwinm 0,29,0,26,26
	cmpwi 0,0,0
	beq 0,.L54
	lis 9,__libvme_reg2@ha
	lwz 9,__libvme_reg2@l(9)
	li 0,32
	stw 0,0(9)
	li 0,-1
	stw 0,16(31)
	b .L50
.L54:
	li 9,0
	stw 9,16(31)
.L50:
	lwz 3,16(31)
	lwz 11,0(1)
	lwz 0,4(11)
	mtlr 0
	lwz 29,-12(11)
	lwz 31,-4(11)
	mr 1,11
	blr
.Lfe3:
	.size	libvme_read_a32_dword,.Lfe3-libvme_read_a32_dword
	.align 2
	.type	atomic_inc,@function
atomic_inc:
	stwu 1,-32(1)
	stw 31,28(1)
	mr 31,1
	stw 3,8(31)
	lwz 11,8(31)
	lwz 0,8(31)
	lwz 9,8(31)
#APP
	1:	lwarx	10,0,0		# atomic_inc
	addic	10,10,1
	stwcx.	10,0,0
	bne-	1b
#NO_APP
	mr 0,10
	stw 0,12(31)
	lwz 11,0(1)
	lwz 31,-4(11)
	mr 1,11
	blr
.Lfe4:
	.size	atomic_inc,.Lfe4-atomic_inc
	.align 2
	.type	atomic_dec_if_positive,@function
atomic_dec_if_positive:
	stwu 1,-32(1)
	stw 31,28(1)
	mr 31,1
	stw 3,8(31)
	lwz 0,8(31)
#APP
	1:	lwarx	9,0,0		# atomic_dec_if_positive
	addic.	9,9,-1
	blt-	2f
	stwcx.	9,0,0
	bne-	1b
2:
#NO_APP
	mr 0,9
	stw 0,12(31)
	lwz 0,12(31)
	mr 3,0
	lwz 11,0(1)
	lwz 31,-4(11)
	mr 1,11
	blr
.Lfe5:
	.size	atomic_dec_if_positive,.Lfe5-atomic_dec_if_positive
	.ident	"GCC: (GNU) 3.2.2 20030217 (Yellow Dog Linux 3.0 3.2.2-2a_1)"
