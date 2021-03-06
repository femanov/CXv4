#!/bin/sh

set -e

CXDIR=$HOME/cx

# create directiries for exports
make -C $CXDIR/4cx/src create-exports
#make -C $CXDIR/4cx/src gitignore
# make inplace
make -C $CXDIR/4cx/src
# copy what just maked to exports
make -C $CXDIR/4cx/src exports create-gitignore
make -C $CXDIR/frgn4cx EPICS_BASE_DIR=$HOME/epics/base
make -C $CXDIR/hw4cx
make -C $CXDIR/sw4cx

make -C $CXDIR/frgn4cx EPICS_BASE_DIR=$HOME/epics/base create-gitignore
make -C $CXDIR/hw4cx create-gitignore
make -C $CXDIR/sw4cx create-gitignore

