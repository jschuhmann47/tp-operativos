#Globales

CC = gcc
OUTFILE = memoria.out

#Reglas

SRC = $ (shell find ./src - type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)

SHARED = $ (shell find ../shared/obj/*.o) 

build:makefolder makeshared $(OBJ)
	$(CC) -o $(OUTFILE) main.c $(OBJ) $(SHARED)

valegrind: build
	valegrind --leak-check=full --show-leak-kinds=all --log-file=valegrind.log ./$(OUTFILE)

LINK = lpthread -lcommons -lm
CFLAGS = -c

debug: CFLAGS := $(CFLAGS) -g
debug: build

./obj/%.o: .src/%.c
	$(CC) $ CFLAGS $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

makeshared:
	make -C ../shared

makeshared_debug:
	make debug  -C ../shared

#Clean

rm -rf obj
rm -rf *.out
rm -rf *.log 
