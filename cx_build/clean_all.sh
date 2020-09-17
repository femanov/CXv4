#!/bin/sh


CXDIR=$HOME/cx


make -kC $CXDIR/4cx/src maintainer-clean clean
make -kC $CXDIR/frgn4cx maintainer-clean clean
make -kC $CXDIR/sw4cx maintainer-clean clean
make -kC $CXDIR/hw4cx maintainer-clean clean

