#include "NNalgorithm.hpp"
#include "DMatrix.hpp"
#include "tree.hpp"
#include <iostream>
#include <limits>



int sumDistances(struct DMatrix &matrix, int taxa);
void getNearestNeighbors(struct DMatrix &matrix, int &first, int &second);
void updateDistances(struct DMatrix &matrix, int first, int second,
                     int newNode);
DMatrix *phylipToMatrix(std::istream &phylipStream);


void phylipToNewick(std::istream &phylipStream, std::string &newickString, std::vector<std::string> &taxaNames) {

  struct DMatrix *matrix = phylipToMatrix(phylipStream);
  Tree *tree = new Tree(matrix->distances.size());

  tree->setNames(matrix->names);

  // index of internal node to join to
  int internalNodeIdx = matrix->activeNodes.size();

  while (matrix->activeNodes.size() > 2) {
    int firstTaxon, secondTaxon;

    getNearestNeighbors(*matrix, firstTaxon, secondTaxon);

    // Calculate distances from joined nodes to internal node
    float firstDistance = 0.5 * matrix->at(firstTaxon, secondTaxon) +
                          (float)(sumDistances(*matrix, firstTaxon) -
                                  sumDistances(*matrix, secondTaxon)) /
                              (float)(2 * (matrix->activeNodes.size() - 2));

    float secondDistance = matrix->at(firstTaxon, secondTaxon) - firstDistance;

    tree->add_edge(internalNodeIdx, firstTaxon, firstDistance);
    tree->add_edge(internalNodeIdx, secondTaxon, secondDistance);

    // Calculate distances from other taxa to the new node
    updateDistances(*matrix, firstTaxon, secondTaxon, internalNodeIdx);

    // Remove joined taxa

    std::vector<int> newActive;

    for (const auto &i : matrix->activeNodes) {
      if (i != firstTaxon && i != secondTaxon) {
        newActive.push_back(i);
      }
    }

    newActive.push_back(internalNodeIdx);

    matrix->activeNodes.swap(newActive);
    internalNodeIdx++;
  }

  // Connect the remaining two nodes

  float distance = matrix->at(matrix->activeNodes[0], matrix->activeNodes[1]);

  tree->add_edge(matrix->activeNodes[0], matrix->activeNodes[1], distance);

  newickString = tree->toNewickString();

  for (const auto name : matrix->names) {
    taxaNames.push_back(name);
  }
  
  delete matrix;
  delete tree;

}
// Given a distance matrix, put the index of the nearest neighbors in first and
// second
void getNearestNeighbors(struct DMatrix &matrix, int &first, int &second) {

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
void updateDistances(struct DMatrix &matrix, int first, int second,
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
int sumDistances(struct DMatrix &matrix, int taxa) {
  int sum = 0;
  for (int i = 0; i < matrix.activeNodes.size(); i++) {
    sum += matrix.at(matrix.activeNodes[i], taxa);
  }

  return sum;
}

// Gets the hamming distance of the strings, ignoring gaps
int getHammingDistance(const std::string &first, const std::string &second) {
  int length = first.length();
  // Number of mismatches, excluding gaps
  int diff = 0;

  
  for (int i = 0; i < length; i++) {
    if (first[i] != second[i] && (first[i] != '-' || second[i] != '-')) {
      diff++;
    }
  }

  return diff;
}

// Takes a istream in phylip format and returns a distance matrix
DMatrix *phylipToMatrix(std::istream &phylipStream) {

  int numTaxa;
  phylipStream >> numTaxa;

  // Vector to hold sequence slices
  std::vector<std::string> sequences(numTaxa);
  struct DMatrix *matrix = new DMatrix(numTaxa);

  // Get rid of the first line
  getline(phylipStream, sequences[0]);

  bool firstRead = true;

  while (getline(phylipStream, sequences[0])) {

    for (int i = 1; i < numTaxa; i++) {
      getline(phylipStream, sequences[i]);
    }

    if (firstRead) {
      for (int i = 0; i < numTaxa; i++) {
        matrix->names[i] = sequences[i].substr(0, 10);
      }
    }

    for (int i = 0; i < numTaxa; i++) {
      for (int j = i + 1; j < numTaxa; j++) {

        std::string first;
        std::string second;
        if (firstRead) {
          first = sequences[i].substr(9, sequences[i].length() - 1);
          second = sequences[j].substr(9, sequences[j].length() - 1);
        } else {
          first = sequences[i];
          second = sequences[j];
        }

        matrix->at(i, j) += getHammingDistance(first, second);
      }
    }

    if (firstRead) {
      firstRead = false;
    }

    // get rid of empty line
    getline(phylipStream, sequences[0]);
  }

  return matrix;
}
