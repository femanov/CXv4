#!/bin/sh


set -e

CXDIR=$HOME/cx

CXINSTDIR=$HOME/4pult

make -C $CXDIR/4cx/src install PREFIX=$CXINSTDIR NOX=1

make -C $CXDIR/frgn4cx install PREFIX=$CXINSTDIR EPICS_BASE_DIR=$HOME/epics/base NOX=1





