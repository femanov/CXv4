/*********************************************************************

  libvmedirect_vect_io.h - libvmedirect vector I/O extensions

  2020-01-21  initial version by D.Yu.Bolkhovityanov@inp.nsk.su

*********************************************************************/
#ifndef _LIBVMEDIRECT_VECT_IO_H


#ifndef _LIBVMEDIRECT_H
#include "libvmedirect.h"
#endif

static __inline__ int
libvme_read_a16_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count)
{
    register int status;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *byte = *(unsigned char *)(__libvme_a16 + bus_addr);
    memcpy(bytes, (unsigned char *)(__libvme_a16 + bus_addr), count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a24_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count) 
{
    register int status;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *byte = *(unsigned char *)(__libvme_a24 + bus_addr);
    memcpy(bytes, (unsigned char *)(__libvme_a24 + bus_addr), count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a32_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count) 
{
    register int status, reg0, overlay, offset;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *byte = *(unsigned char *)(__libvme_a32 + offset);
    memcpy(bytes, (unsigned char *)(__libvme_a32 + offset), count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a16_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status;
    register int n;
    register unsigned short *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *word = *(unsigned short *)(__libvme_a16 + bus_addr);
    for (n = count, srcp = (unsigned short *)(__libvme_a16 + bus_addr), dstp = words;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a24_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status;
    register int n;
    register unsigned short *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *word = *(unsigned short *)(__libvme_a24 + bus_addr);
    for (n = count, srcp = (unsigned short *)(__libvme_a24 + bus_addr), dstp = words;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a32_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status, reg0, overlay, offset;
    register int n;
    register unsigned short *srcp, *dstp;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *word = *(unsigned short *)(__libvme_a32 + offset);
    for (n = count, srcp = (unsigned short *)(__libvme_a16 + offset), dstp = words;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a16_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count) 
{
    register int status;
    register int n;
    register unsigned int *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *dword = *(unsigned int *)(__libvme_a16 + bus_addr);
    for (n = count, srcp = (unsigned int *)(__libvme_a16 + bus_addr), dstp = dwords;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a24_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count) 
{
    register int status;
    register int n;
    register unsigned int *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *dword = *(unsigned int *)(__libvme_a24 + bus_addr);
    for (n = count, srcp = (unsigned int *)(__libvme_a24 + bus_addr), dstp = dwords;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_read_a32_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count) 
{
    register int status, reg0, overlay, offset;
    register int n;
    register unsigned int *srcp, *dstp;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *dword = *(unsigned int *)(__libvme_a32 + offset);
    for (n = count, srcp = (unsigned int *)(__libvme_a32 + offset), dstp = dwords;
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}

static __inline__ int
libvme_write_a16_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count) 
{
    register int status;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned char *)(__libvme_a16 + bus_addr) = byte;
    memcpy((unsigned char *)(__libvme_a16 + bus_addr), bytes, count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a24_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count) 
{
    register int status;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned char *)(__libvme_a24 + bus_addr) = byte;
    memcpy((unsigned char *)(__libvme_a24 + bus_addr), bytes, count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a32_byte_vect(unsigned int bus_addr, unsigned char *bytes, int count) 
{
    register int status, reg0, overlay, offset;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *(unsigned char *)(__libvme_a32 + offset) = byte;
    memcpy((unsigned char *)(__libvme_a32 + offset), bytes, count);
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a16_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status;
    register int n;
    register unsigned short *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned short *)(__libvme_a16 + bus_addr) = word;
    for (n = count, srcp = words, dstp = (unsigned short *)(__libvme_a16 + bus_addr);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a24_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status;
    register int n;
    register unsigned short *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned short *)(__libvme_a24 + bus_addr) = word;
    for (n = count, srcp = words, dstp = (unsigned short *)(__libvme_a24 + bus_addr);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a32_word_vect(unsigned int bus_addr, unsigned short *words, int count) 
{
    register int status, reg0, overlay, offset;
    register int n;
    register unsigned short *srcp, *dstp;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *(unsigned short *)(__libvme_a32 + offset) = word;
    for (n = count, srcp = words, dstp = (unsigned short *)(__libvme_a32 + bus_addr);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a16_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count)
{
    register int status;
    register int n;
    register unsigned int *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned int *)(__libvme_a16 + bus_addr) = dword;
    for (n = count, srcp = dwords, dstp = (unsigned int *)(__libvme_a16 + bus_addr);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a24_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count) 
{
    register int status;
    register int n;
    register unsigned int *srcp, *dstp;

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }
//    *(unsigned int *)(__libvme_a24 + bus_addr) = dword;
    for (n = count, srcp = dwords, dstp = (unsigned int *)(__libvme_a24 + bus_addr);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}
static __inline__ int
libvme_write_a32_dword_vect(unsigned int bus_addr, unsigned int *dwords, int count) 
{
    register int status, reg0, overlay, offset;
    register int n;
    register unsigned int *srcp, *dstp;

    overlay = (bus_addr >> 26) & 0x3f;	/* 64 pages */
    offset = bus_addr & 0x3ffffff;	/* 64 M page */

    while (atomic_dec_if_positive(&__libvme_shm_ptr[0]) == -1) {
	__libvme_mutual_access++;
	sched_yield();
    }

    reg0 = __libvme_regs[0];
    reg0 &= ~VMEIF_REG0_OVERLAY_MASK;
    reg0 |= overlay << VMEIF_REG0_OVERLAY_SHIFTS;
    __libvme_regs[0] = reg0;
    
//    *(unsigned int *)(__libvme_a32 + offset) = dword;
    for (n = count, srcp = dwords, dstp = (unsigned int *)(__libvme_a32 + offset);
         n > 0;
         n--)
        *dstp++ = *srcp++;
    status = *__libvme_reg2;
    atomic_inc(&__libvme_shm_ptr[0]);
    if (status & VMEIF_REG2_BERR) {*__libvme_reg2 = VMEIF_REG2_BERR; return -1; }
    return 0;
}


#endif /* _LIBVMEDIRECT_VECT_IO_H */
