treebuilder: main.o tree.o DMatrix.o NNalgorithm.o 
	clang++ -lzip -lcurl main.o tree.o DMatrix.o NNalgorithm.o -o treebuilder -v
main.o: main.cpp
	clang++ main.cpp -c 
tree.o:	tree.cpp
	clang++ tree.cpp -c 
DMatrix.o: DMatrix.cpp
	clang++ DMatrix.cpp -c 
NNalgorithm.o:
	clang++ NNalgorithm.cpp -c 
