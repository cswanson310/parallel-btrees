CXX=g++
CXXFLAGS+=-Wall -Wextra -O2
ALLOBJ=btree.o
ALLDEFN=def.h
SRC=btree.cpp
main: $(ALLOBJ)
	$(CXX) $(CXXFLAGS) $(ALLOBJ) -lm -o main
