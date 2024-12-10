treebuilder: main.o tree.o DMatrix.o NNalgorithm.o 
	clang++ -lzip -lcurl main.o tree.o DMatrix.o NNalgorithm.o -o treebuilder -g
main.o: main.cpp
	clang++ main.cpp -c -g
tree.o:	tree.cpp
	clang++ tree.cpp -c -g
DMatrix.o: DMatrix.cpp
	clang++ DMatrix.cpp -c -g 
NNalgorithm.o:
	clang++ NNalgorithm.cpp -c -g
