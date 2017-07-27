cc = g++
elf = lamuc
cflags = -c 
obj = lamuc.o load.o
src = lamuc.cc load.cc

all: $(elf)

$(elf):$(obj)
	g++ $(obj) -o $@

lamuc.o : lamuc.cc
	$(cc) $(cflags) $< 
load.o : load.cc
	$(cc) $(cflags) $<
clean : 
	rm -rf *.o $(elf) 
	
