#include "tree.hpp"
#include <iostream>
Tree::Tree(int numNodes) : edges(numNodes, std::vector<float>(numNodes)), numNodes(numNodes){}

void Tree::add_edge(int first, int second, int weight) {
  if (first < numNodes && second < numNodes && first >= 0 && second >= 0) {
    edges[first][second] = weight;
    edges[second][first]  = weight;
  } else {
    std::cout << "Node index out of bounds!" <<  std::endl;
  }
}

int Tree::getNumNodes() {return numNodes;}
