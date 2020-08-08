# Makefile for all tests

ELF = rdwr.elf case.elf base.elf

OBJ = $(ELF:.elf=.o)

ARGS = -non_shared -L../lib -lrisclib -Wa,-W

all: $(ELF)

$(ELF:.*): $(OBJ:.*)

%.o: %.S
	gcc -g -c $< -o $@ -Wa,-W

%.elf: %.o
	gcc -g -o $@ $< $(ARGS)

clean:
	rm -f $(ELF)
	rm -f *.o
