#include "DMatrix.hpp"

float &DMatrix::at(int first, int second) {
  int smallest = first < second ? first : second;
  return distances[smallest][first + second - smallest];
}

// Construct a vector with spaces for internal nodes
DMatrix::DMatrix(int numTaxa)
    : distances(numTaxa + numTaxa - 2,
                std::vector<float>(numTaxa + numTaxa - 2)) {
  // Add initial nodes to activeNodes
  for (int i = 0; i < numTaxa; i++) {
    activeNodes.push_back(i);
  }

  // Intialize zero distances

  for (int i = 0; i < distances.size(); i++) {
    distances[i][i] = 0;
  }
}

