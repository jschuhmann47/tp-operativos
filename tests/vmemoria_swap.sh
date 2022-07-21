#!/bin/bash
loginfo="$1"
valgrind --tool=memcheck --leak-check=yes --show-possibly-lost=no --show-reachable=no --num-callers=20 --track-origins=yes ../memoria_swap/bin/memoria_swap.out $loginfo