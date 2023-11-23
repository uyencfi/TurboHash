#!/bin/sh

prefix=/home/uyen/TurboHash/lib
exec_prefix=/home/uyen/TurboHash/lib
libdir=${exec_prefix}/lib

LD_PRELOAD=${libdir}/libjemalloc.so.2
export LD_PRELOAD
exec "$@"
