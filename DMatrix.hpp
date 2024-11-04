#include <vector>
#include <string>

struct DMatrix {
  std::vector<std::vector<float>> distances;
  std::vector<int> activeNodes;

  // Gets or sets the distance of the following taxa
  float &at(int first, int second);    
  void buildMatrix(std::string filename);
  
  // Construct a vector with spaces for internal nodes
  DMatrix(int numTaxa);

};
