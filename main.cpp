#include "tree.hpp"
#include <limits>

struct Dmatrix {
  std::vector<std::vector<float>> distances;
  std::vector<int> activeNodes;

  float &at(int first, int second) {
    int smallest = first < second ? first : second;
    return distances[smallest][first + second - smallest];
  }

  // Construct a vector with spaces for internal nodes
  Dmatrix(int numTaxa)
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
};

int sumDistances(struct Dmatrix &matrix, int taxa);
void getNearestNeighbors(struct Dmatrix &matrix, int &first, int &second);
void updateDistances(struct Dmatrix &matrix, int first, int second,
                     int newNode);

int main(int argc, char **args) {

  struct Dmatrix matrix = Dmatrix(5);

  Tree *tree = new Tree(matrix.distances.size());

  matrix.at(0, 1) = 5;
  matrix.at(0, 2) = 9;
  matrix.at(0, 3) = 9;
  matrix.at(0, 4) = 8;
  matrix.at(1, 2) = 10;
  matrix.at(1, 3) = 10;
  matrix.at(1, 4) = 9;
  matrix.at(2, 3) = 8;
  matrix.at(2, 4) = 7;
  matrix.at(3, 4) = 3;

  // index of internal node to join to
  int internalNodeIdx = matrix.activeNodes.size();

  while (matrix.activeNodes.size() > 2) {
    int firstTaxon, secondTaxon;

    getNearestNeighbors(matrix, firstTaxon, secondTaxon);

    // Calculate distances from joined nodes to internal node
    float firstDistance = 0.5 * matrix.at(firstTaxon, secondTaxon) +
      (float) ( sumDistances(matrix, firstTaxon) - sumDistances(matrix, secondTaxon)) /
      (float) (2 * (matrix.activeNodes.size() - 2));

    float secondDistance = matrix.at(firstTaxon, secondTaxon) - firstDistance;

    tree->add_edge(internalNodeIdx, firstTaxon, firstDistance);
    tree->add_edge(internalNodeIdx, secondTaxon, secondDistance);

    // Calculate distances from other taxa to the new node
    updateDistances(matrix, firstTaxon, secondTaxon, internalNodeIdx);

    // Remove joined taxa

    std::vector<int> newActive;

    for (const auto &i : matrix.activeNodes) {
      if (i != firstTaxon && i != secondTaxon) {
        newActive.push_back(i);
      }
    }

    newActive.push_back(internalNodeIdx);

    matrix.activeNodes.swap(newActive);
    internalNodeIdx++;
  }

  // Connect the remaining two nodes

  float distance = matrix.at(matrix.activeNodes[0], matrix.activeNodes[1]);

  tree->add_edge(matrix.activeNodes[0], matrix.activeNodes[1], distance);

  tree->print();

  delete tree;
}

void getNearestNeighbors(struct Dmatrix &matrix, int &first, int &second) {

  int smallestQ = std::numeric_limits<int>::max();

  int numTaxa = matrix.activeNodes.size();

  for (int i = 0; i < numTaxa; i++) {
    for (int j = i + 1; j < numTaxa; j++) {

      int firstActive = matrix.activeNodes[i];
      int secondActive = matrix.activeNodes[j];

      int Q = (numTaxa - 2) * matrix.at(firstActive, secondActive) -
              sumDistances(matrix, firstActive) -
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
void updateDistances(struct Dmatrix &matrix, int first, int second,
                     int newNode) {

  int numTaxa = matrix.activeNodes.size();

  // Remove first and second from distance matrix
  for (int i = 0; i < numTaxa; i++) {
    if (matrix.activeNodes[i] != first && matrix.activeNodes[i] != second &&
        matrix.activeNodes[i] != newNode) {

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
