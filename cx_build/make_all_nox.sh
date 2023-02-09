#!/bin/sh

set -e

CXDIR=$HOME/cx

# create directiries for exports
make -C $CXDIR/4cx/src NOX=1 create-exports
# make inplace
make -C $CXDIR/4cx/src NOX=1
# copy what just maked to exports
make -C $CXDIR/4cx/src exports NOX=1

make -C $CXDIR/4cx/src exports create-gitignore
make -C $CXDIR/frgn4cx EPICS_BASE_DIR=$HOME/epics/base NOX=1

make -C $CXDIR/frgn4cx EPICS_BASE_DIR=$HOME/epics/base create-gitignore
