#include <vector>

class Tree {

  int numNodes;
  std::vector<std::vector<float>> edges;
  
  void add_edge(int first, int second, int weight);

public:
  Tree(int numNodes);
  
  int getNumNodes();
};

