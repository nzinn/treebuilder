
#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>

class Tree {

  int numNodes;
  std::vector<std::vector<float>> edges;
  std::vector<std::string> names;

  // Get the newick string rooted at node
  std::string getNodeNewick(int node);
  
public:
  Tree(int numNodes);
  void add_edge(int first, int second, float weight);
  void print();

  void setNames(const std::vector<std::string> &names);
  int getNumNodes();
  std::string toNewickString();
};
#endif
