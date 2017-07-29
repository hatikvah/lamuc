cc = g++
elf = lamuc
cflags = -c -g
obj = lamuc.o load.o lex.o
src = lamuc.cc load.cc lex.cc

all: $(elf)

$(elf):$(obj)
	g++ $(obj) -o $@

lex.o : lex.cc
	$(cc) $(cflags) $<
lamuc.o : lamuc.cc
	$(cc) $(cflags) $< 
load.o : load.cc
	$(cc) $(cflags) $<
clean : 
	rm -rf *.o $(elf) 
	
