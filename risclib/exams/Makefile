# Makefile for all exams

ELF = math1.elf math2.elf math3.elf \
		logic1.elf logic2.elf logic3.elf \
		loop1.elf loop2.elf loop3.elf loop4.elf loop5.elf \
		loop6.elf loop7.elf loop8.elf loop9.elf loopA.elf \
		funct1.elf funct2.elf funct3.elf funct4.elf funct5.elf \
		stack1.elf stack2.elf stack3.elf stack4.elf \
		stack5.elf stack6.elf stack7.elf \
		recur1.elf

OBJ = $(ELF:.elf=.o)

ARGS = -non_shared -L../lib -lrisclib -Wa,-W

all: $(ELF)

$(ELF:.*): $(OBJ:.*)

%.o: %.S
	gcc -g -c $< -o $@ -Wa,-W

%.elf: %.o
	gcc -g -o $@ $< $(ARGS)

funct4.elf: funct4.o funct3.S
	gcc -c funct3.S -o tmp.o -D_MODULE -Wa,-W
	gcc -o funct4.elf funct4.o tmp.o $(ARGS)
	rm tmp.o

funct5.elf: funct5.o funct3.S funct4.S
	gcc -c funct3.S -o tmp0.o -D_MODULE -Wa,-W
	gcc -c funct4.S -o tmp1.o -D_MODULE -Wa,-W
	gcc -o funct5.elf funct5.o tmp0.o tmp1.o $(ARGS)
	rm tmp0.o tmp1.o

clean:
	rm -f $(ELF)
	rm -f *.o
