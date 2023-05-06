# Globales
CC = gcc
OUTFILE = CPU

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)
DEPENDENCIES = src/utils.c

SHARED = $(shell find ../shared/obj/*.o)

build:
	$(CC) main_cpu.c $(DEPENDENCIES) -o $(OUTFILE) $(LINK)


LINK = -lcommons -lpthread -lm -lreadline 
CFLAGS = -c

clean:
	rm -rf *.log
	rm $(OUTFILE)
	make clean -C ../shared