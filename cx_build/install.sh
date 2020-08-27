#!/bin/sh


set -e

CXDIR=$HOME/cx

CXINSTDIR=$HOME/4pult

make -C $CXDIR/4cx/src install PREFIX=$CXINSTDIR

make -C $CXDIR/frgn4cx install PREFIX=$CXINSTDIR

make -C $CXDIR/hw4cx install PREFIX=$CXINSTDIR

make -C $CXDIR/sw4cx install PREFIX=$CXINSTDIR




