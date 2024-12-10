#ifndef NN_H
#define NN_H

#include <string>
#include <vector>
void phylipToNewick(std::istream &phylipStream, std::string &newickString, std::vector<std::string> &taxaName);
#endif
