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
.LC5:
	.string	"%02d 0x%08x\n"
	.align 2
.LC4:
	.string	"%02d 0x%04x\n"
	.section	".text"
	.align 2
	.globl main
	.type	main,@function
main:
	stwu 1,-64(1)
	mflr 0
	stw 30,56(1)
	stw 31,60(1)
	stw 0,68(1)
	stw 27,44(1)
	stw 28,48(1)
	stw 29,52(1)
	lis 30,stderr@ha
	bl libvme_init
	mr 31,3
	lis 4,.LC0@ha
	lwz 3,stderr@l(30)
	la 4,.LC0@l(4)
	mr 5,31
	bl fprintf
	cmpwi 0,31,0
	li 3,1
	blt- 0,.L1
	li 0,1
	addi 4,1,12
	li 3,16
	stw 0,12(1)
	bl libvme_ctl
	mr 31,3
	lwz 5,12(1)
	lwz 3,stderr@l(30)
	lis 4,.LC1@ha
	mr 6,31
	la 4,.LC1@l(4)
	bl fprintf
	li 0,9
	addi 4,1,16
	li 3,1
	stw 0,16(1)
	bl libvme_ctl
	mr 31,3
	lis 4,.LC2@ha
	lwz 3,stderr@l(30)
	lwz 5,16(1)
	la 4,.LC2@l(4)
	mr 6,31
	bl fprintf
	lis 9,sequence@ha
	lwz 0,sequence@l(9)
	la 29,sequence@l(9)
	cmpwi 0,0,0
	beq- 0,.L27
.L25:
	lwz 5,4(29)
	lis 3,.LC3@ha
	mr 4,0
	la 3,.LC3@l(3)
	bl printf
	lwz 0,0(29)
	cmpwi 0,0,16
	beq- 0,.L33
	lwz 10,4(29)
	lis 9,__libvme_shm_ptr@ha
	rlwinm 27,10,0,6,31
	lwz 11,__libvme_shm_ptr@l(9)
	srwi 28,10,26
#APP
	1:	lwarx	0,0,11		# atomic_dec_if_positive
	addic.	0,0,-1
	blt-	2f
	stwcx.	0,0,11
	bne-	1b
2:
#NO_APP
	cmpwi 0,0,-1
	beq- 0,.L34
.L31:
	lis 9,__libvme_regs@ha
	lwz 8,__libvme_regs@l(9)
	lis 11,__libvme_a32@ha
	lwz 0,0(8)
	lwz 7,__libvme_a32@l(11)
	rlwimi 0,28,9,17,22
	stw 0,0(8)
	lwzx 10,7,27
	lis 9,__libvme_reg2@ha
	lwz 8,__libvme_reg2@l(9)
	stw 10,20(1)
	lis 11,__libvme_shm_ptr@ha
	lwz 9,__libvme_shm_ptr@l(11)
	lwz 0,0(8)
#APP
	1:	lwarx	11,0,9		# atomic_inc
	addic	11,11,1
	stwcx.	11,0,9
	bne-	1b
#NO_APP
	andi. 9,0,32
	li 3,0
	beq- 0,.L24
	li 0,32
	li 3,-1
	stw 0,0(8)
.L24:
	mr 31,3
	lwz 5,20(1)
	lis 3,.LC5@ha
	la 3,.LC5@l(3)
	mr 4,31
.L32:
	bl printf
	lwzu 0,8(29)
	cmpwi 0,0,0
	bne+ 0,.L25
.L27:
	li 3,0
.L1:
	lwz 0,68(1)
	lwz 27,44(1)
	lwz 28,48(1)
	lwz 29,52(1)
	lwz 30,56(1)
	lwz 31,60(1)
	mtlr 0
	addi 1,1,64
	blr
.L34:
	lis 31,__libvme_mutual_access@ha
	lis 30,__libvme_shm_ptr@ha
.L21:
	lwz 9,__libvme_mutual_access@l(31)
	addi 9,9,1
	stw 9,__libvme_mutual_access@l(31)
	bl sched_yield
	lwz 0,__libvme_shm_ptr@l(30)
#APP
	1:	lwarx	9,0,0		# atomic_dec_if_positive
	addic.	9,9,-1
	blt-	2f
	stwcx.	9,0,0
	bne-	1b
2:
#NO_APP
	cmpwi 0,9,-1
	beq+ 0,.L21
	b .L31
.L33:
	lwz 10,4(29)
	lis 9,__libvme_shm_ptr@ha
	rlwinm 27,10,0,6,31
	lwz 11,__libvme_shm_ptr@l(9)
	srwi 28,10,26
#APP
	1:	lwarx	0,0,11		# atomic_dec_if_positive
	addic.	0,0,-1
	blt-	2f
	stwcx.	0,0,11
	bne-	1b
2:
#NO_APP
	cmpwi 0,0,-1
	beq- 0,.L35
.L29:
	lis 9,__libvme_regs@ha
	lis 11,__libvme_a32@ha
	lwz 10,__libvme_regs@l(9)
	lwz 9,__libvme_a32@l(11)
	lis 11,__libvme_reg2@ha
	lwz 0,0(10)
	lhzx 8,9,27
	lwz 7,__libvme_reg2@l(11)
	rlwimi 0,28,9,17,22
	stw 0,0(10)
	lis 11,__libvme_shm_ptr@ha
	sth 8,8(1)
	lwz 9,__libvme_shm_ptr@l(11)
	lwz 0,0(7)
#APP
	1:	lwarx	11,0,9		# atomic_inc
	addic	11,11,1
	stwcx.	11,0,9
	bne-	1b
#NO_APP
	andi. 9,0,32
	li 3,0
	beq- 0,.L15
	li 0,32
	li 3,-1
	stw 0,0(7)
.L15:
	mr 31,3
	lhz 5,8(1)
	lis 3,.LC4@ha
	la 3,.LC4@l(3)
	mr 4,31
	b .L32
.L35:
	lis 31,__libvme_mutual_access@ha
	lis 30,__libvme_shm_ptr@ha
.L12:
	lwz 9,__libvme_mutual_access@l(31)
	addi 9,9,1
	stw 9,__libvme_mutual_access@l(31)
	bl sched_yield
	lwz 0,__libvme_shm_ptr@l(30)
#APP
	1:	lwarx	9,0,0		# atomic_dec_if_positive
	addic.	9,9,-1
	blt-	2f
	stwcx.	9,0,0
	bne-	1b
2:
#NO_APP
	cmpwi 0,9,-1
	beq+ 0,.L12
	b .L29
.Lfe1:
	.size	main,.Lfe1-main
	.ident	"GCC: (GNU) 3.2.2 20030217 (Yellow Dog Linux 3.0 3.2.2-2a_1)"
