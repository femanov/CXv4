#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "libvmedirect.h"

typedef struct
{
    int           bitsize;
    unsigned int  bus_addr;
} op_descr_t;

op_descr_t sequence[] =
{
    {16, 0x80000002},
    {16, 0x80000002},
    {32, 0x8a000008},
    {32, 0x8a000008},
    {16, 0x80000002},
    {16, 0x80000002},
    {0, 0}
};

int main(void)
{
  int  r;
  int  v;
  int  am;

  op_descr_t *op;
  unsigned short  word;
  unsigned int    dword;

    r = libvme_init();
    fprintf(stderr, "libvme_init()=%d\n", r);
    if (r < 0) return 1;

    v = 1;
    r = libvme_ctl(VME_SYSTEM_W, &v);
    fprintf(stderr, "libvme_ctl(VME_SYSTEM_W,%d)=%d\n", v, r);

    am = 0x09;
    r = libvme_ctl(VME_AM_W, &am);
    fprintf(stderr, "libvme_ctl(VME_AM_W,0x%02x)=%d\n", am, r);

    for (op = sequence;  op->bitsize != 0;  op++)
    {
        printf("%d:0x%08x ", op->bitsize, op->bus_addr);
        if (op->bitsize == 16)
        {
            r = libvme_read_a32_word (op->bus_addr, &word);
            printf("%02d 0x%04x\n", r, word);
        }
        else
        {
            r = libvme_read_a32_dword(op->bus_addr, &dword);
            printf("%02d 0x%08x\n", r, dword);
        }
    }

    return 0;
}
