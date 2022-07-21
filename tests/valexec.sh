#!/bin/bash
path="$1"
tamanio="$2"
valgrind --tool=memcheck --leak-check=yes --show-possibly-lost=no --show-reachable=no --num-callers=20 --track-origins=yes ../consola/bin/consola.out $path $tamanio