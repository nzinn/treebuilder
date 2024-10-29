#include <vector>

class Tree {

  int numNodes;
  std::vector<std::vector<float>> edges;
  

public:
  Tree(int numNodes);
  void add_edge(int first, int second, float weight);
  void print();
  int getNumNodes();
};

