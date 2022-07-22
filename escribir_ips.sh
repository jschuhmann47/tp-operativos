#!/bin/bash
TARGET_KEY=$1
REPLACEMENT_VALUE=$2
CWD=$PWD
cd consola
cd cfg
for y in *.cfg; do 
    sed -i "s/\($TARGET_KEY *= *\).*/\1$REPLACEMENT_VALUE/" $y
done
cd $CWD
cd cpu
cd cfg
for y in *.cfg; do 
    sed -i "s/\($TARGET_KEY *= *\).*/\1$REPLACEMENT_VALUE/" $y
done
cd $CWD
cd kernel
cd cfg
for y in *.cfg; do 
    sed -i "s/\($TARGET_KEY *= *\).*/\1$REPLACEMENT_VALUE/" $y
done
cd $CWD
cd memoria_swap
cd cfg
for y in *.cfg; do 
    sed -i "s/\($TARGET_KEY *= *\).*/\1$REPLACEMENT_VALUE/" $y
done
