#include "tree.hpp"
#include <sstream>
#include <string>
#include <iostream>
Tree::Tree(int numNodes) : edges(numNodes, std::vector<float>(numNodes)), numNodes(numNodes), names(numNodes, ""){}

void Tree::add_edge(int first, int second, float weight) {
  if (first < numNodes && second < numNodes && first >= 0 && second >= 0) {
    edges[first][second] = weight;
    edges[second][first]  = weight;
  } else {
    std::cout << "Node index out of bounds! " << first << " " << second << std::endl;
  }
}

int Tree::getNumNodes() {return numNodes;}


void Tree::setNames(const std::vector<std::string> &names) {

  for (int i = 0; i < names.size(); i++) {
    this->names[i] = names[i];
  }
  
}

std::string Tree::getNodeNewick(int node) {


  std::ostringstream newick("");
  newick << "(";
  // keep track of how many nodes it's connected to, if it's just one it's a leaf
  int numConnected = 0;
  bool firstNode = true;
  for (int i = 0; i < node; i++) {
    if (edges[i][node]) {
      if (!firstNode) {
	newick << ",";
      } else {
	firstNode = false;
      }
      
      newick << getNodeNewick(i) << ":" << edges[i][node];
      numConnected++;
    }
  }

  // Closing parenthesis
  newick << ")";

  if (numConnected > 1) {
      return newick.str();
  } else {
    return names[node];
  }
} 

std::string Tree::toNewickString() {
  return getNodeNewick(edges.size() - 1) + ";";
}
