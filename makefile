treebuilder: main.o tree.o DMatrix.o
	clang++ main.o tree.o DMatrix.o -o treebuilder
main.o: main.cpp
	clang++ main.cpp -c
tree.o:	tree.cpp
	clang++ tree.cpp -c
DMatrix.o: DMatrix.cpp
	clang++ DMatrix.cpp -c
