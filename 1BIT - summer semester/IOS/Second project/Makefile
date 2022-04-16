#***************************************************************************
# 
# Soubor: Makefile
# Datum vytvoreni: 2021-04-30
# Datum posledni zmeny: 2021-05-01
# Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
# Projekt: VUT FIT predmet IOS Projekt 2
# Popis: Makefile pro proj2
#
#***************************************************************************

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -pthread

EXECUTABLE = proj2
OBJS = main.o mem_handler.o proc_args.o processes.o write.o
ZIP_FILE = $(EXECUTABLE).zip

.PHONY: all pack run clean valgrind

all: $(EXECUTABLE)

pack: $(ZIP_FILE)

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(EXECUTABLE) *.o *.out $(ZIP_FILE)

$(ZIP_FILE): *.c *.h Makefile
	zip $@ $^

valgrind: $(EXECUTABLE)
	valgrind ./$(EXECUTABLE) $(ARGS)