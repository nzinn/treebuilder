#include "tree.hpp"
#include <iostream>
#include <limits>


struct Dmatrix {
  std::vector<std::vector<float>> distances;
  std::vector<int> activeNodes;

  float &at(int first, int second) {
    int smallest = first < second ? first : second;
    return distances[smallest][first + second - smallest];
  }

  // Construct a vector with spaces for internal nodes
  Dmatrix(int numTaxa) : distances(numTaxa + numTaxa - 1, std::vector<float>(numTaxa + numTaxa - 1)) {
    // Add initial nodes to activeNodes
    for (int i = 0; i < numTaxa; i++) {
      activeNodes.push_back(i);
    }

    // Intialize zero distances

    for (int i = 0; i < distances.size(); i++) {
      distances[i][i] = 0;
    }
  }
};

int sumDistances(struct Dmatrix &matrix, int taxa);




int main(int argc, char **args) {

  Dmatrix *matrix = new Dmatrix(2);

  matrix->at(0, 1) = 1;

}

void getTaxaPair(struct Dmatrix &matrix, int &first,
                 int &second) {

  int smallestQ = std::numeric_limits<int>::max();

  int numTaxa = matrix.activeNodes.size();

  for (int i = 0; i < numTaxa; i++) {
    for (int j = i + 1; j < numTaxa; j++) {

      int firstActive = matrix.activeNodes[i];
      int secondActive = matrix.activeNodes[j];
      
      int Q = (numTaxa - 2) * matrix.at(firstActive, secondActive) - sumDistances(matrix, firstActive) -
              sumDistances(matrix, secondActive);

      if (Q < smallestQ) {
        smallestQ = Q;
        first = matrix.activeNodes[i];
        second = matrix.activeNodes[j];
      }
    }
  }
}

// Updates the distance matrix after joining first and second
void updateDistances(struct Dmatrix& matrix, int first,
                     int second, int newNode) {

  int numTaxa = matrix.activeNodes.size();

  // Remove first and second from distance matrix
  for (int i = 0; i < numTaxa; i++) {
    if (matrix.activeNodes[i] != first && matrix.activeNodes[i] != second && matrix.activeNodes[i] != newNode) {

      // distance from node i to first
      float fiDistance = matrix.at(matrix.activeNodes[i], first);
      // distance from node i to second
      float siDistance = matrix.at(matrix.activeNodes[i], second);
      // distance from first to second
      float fsDistance = matrix.at(first, second);

      float newDistance = 0.5 * (fiDistance + siDistance - fsDistance);
      
      matrix.at(newNode, matrix.activeNodes[i]) = newDistance;
    }
  }

}


// Sums the distances between a single taxa and the rest
int sumDistances(struct Dmatrix &matrix, int taxa) {
  int sum = 0;
  for (int i = 0; i < matrix.activeNodes.size(); i++) {
    sum += matrix.at(matrix.activeNodes[i], taxa);
  }

  return sum;
}
