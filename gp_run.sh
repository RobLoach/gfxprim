#!/bin/sh

# Very simple script to run python, ipython and GP executables

BDIR=`pwd`

export LD_LIBRARY_PATH=$BDIR/build/
export PYTHONPATH=$BDIR/pylib/

exec "$@"
