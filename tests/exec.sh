#!/bin/bash
PATH="$1"
TAMANIO="$2"
TESTSDIR=$PWD
cd ..
cd consola
./bin/consola.out $TESTSDIR/$PATH $TAMANIO
cd $PWD