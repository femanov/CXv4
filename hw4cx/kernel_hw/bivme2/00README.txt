hw4cx/kernel_hw/bivme2/00README.txt

This is a version of BIVME2 interrupts driver (vmei) with support for IRQ
buffering.

Original driver by Vitaly Mamkin circa 2004.

28-01-2020	Fix: unmasking interrupts immediately at end of
		vme_interrupt(); compatible with ROAK only, NOT with RORA
20-12-2019	Buffering support added by Dmitry Bolkhovityanov

----------------------------------------------------------------------

To build, the powerpc-sdk-linux-v4.tgz content is required.

- Unpack powerpc-sdk-linux-v4.tgz somewhere (e.g. /tmp/ppc860).
- Issue a "make OS_BASE=/tmp/ppc860/linux" command (replace "/tmp/ppc860"
  with an actual location).

----------------------------------------------------------------------
