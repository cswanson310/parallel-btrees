CXX=g++
CXXFLAGS+=-Wall -Wextra -O2
ALLOBJ=btree.o main.o
ALLDEFN=def.h btree.h
SRC=btree.cpp main.cpp
main: $(ALLOBJ)
	$(CXX) $(CXXFLAGS) $(ALLOBJ) -lm -o main
