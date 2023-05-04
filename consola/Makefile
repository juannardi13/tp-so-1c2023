# Globales
CC = gcc
OUTFILE = Consola

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)
DEPENDENCIES = src/utils.c
PARAMETERS = cfg/pseudocodigo.txt cfg/consola.config

SHARED = $(shell find ../shared/obj/*.o)

build:
	$(CC) main_consola.c $(DEPENDENCIES) -o $(OUTFILE) $(LINK)

run:
	./$(OUTFILE) $(PARAMETERS)

LINK = -lcommons -lpthread -lm -lreadline 
CFLAGS = -c

clean:
	rm -rf *.log
	rm $(OUTFILE)
	make clean -C ../shared