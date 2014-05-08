CXX=g++
CXXFLAGS+=-Wall -Wextra -O2
ALLOBJ=btree.o main.o printing.o global_locked.o
ALLDEFN=def.h btree.h printing.h global_locked.h
SRC=btree.cpp main.cpp printing.cpp global_locked.cpp
main: $(ALLOBJ)
	$(CXX) $(CXXFLAGS) $(ALLOBJ) -lm -o main
