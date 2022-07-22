#!/bin/bash
PATH="$1"
TAMANIO="$2"
TESTSDIR=$PWD
cd ..
cd consola
valgrind --tool=memcheck --leak-check=yes --show-possibly-lost=no --show-reachable=no --num-callers=20 --track-origins=yes ./bin/consola.out $TESTSDIR/$PATH $TAMANIO
cd $PWD