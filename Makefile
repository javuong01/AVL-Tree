CC=g++
DEV=-Wall -g -std=c++14
OPT=-O3 -std=c++14

.PHONY: all
all: BSTSanityCheck CreateData AVLCommands 

CreateData: CreateData.cxx json.hpp
	$(CC) $(OPT) CreateData.cxx -o CreateData.exe

BSTSanityCheck: BSTSanityCheck.cxx BST.o AVL.o
	$(CC) $(DEV) BSTSanityCheck.cxx BST.o AVL.o -o BSTSanityCheck.exe

AVLCommands: AVLCommands.cxx BST.o AVL.o 
	$(CC) $(DEV) AVLCommands.cxx BST.o AVL.o -o AVLCommands.exe

BST.o: BST.cpp BST.h
	$(CC) $(DEV) -c BST.cpp

AVL.o: AVL.cpp AVL.h
	$(CC) $(DEV) -c AVL.cpp

# Build
.PHONY: clean
clean:
	rm -f *.o
	rm -f *.exe
	rm -rf *dSYM

.PHONY: update
update:
	make clean
	make all
