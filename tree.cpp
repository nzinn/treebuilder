#include "tree.hpp"
#include <iostream>
#include <iomanip>
Tree::Tree(int numNodes) : edges(numNodes, std::vector<float>(numNodes)), numNodes(numNodes){}

void Tree::add_edge(int first, int second, float weight) {
  if (first < numNodes && second < numNodes && first >= 0 && second >= 0) {
    edges[first][second] = weight;
    edges[second][first]  = weight;
  } else {
    std::cout << "Node index out of bounds! " << first << " " << second << std::endl;
  }
}

int Tree::getNumNodes() {return numNodes;}

void Tree::print() {
  for (int i = 0; i < numNodes; i++) {
    for (int j = 0; j < numNodes; j++) {
      std::cout << std::fixed << std::setprecision(1) << edges[i][j] << " ";
    }

    std::cout << std::endl;;
  }

}
