#include "functions.h"

#include <algorithm>
#include <stdexcept>

using namespace std;

void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); }));
}

void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), s.end());
}

void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

Path &Path::operator+=(const Path &other) {
    if (this->nodes.empty()) {
        this->nodes    = other.nodes;
        this->distance = other.distance;
        return *this;
    }

    // Check if paths are compatible
    if (this->nodes.back() != other.nodes.front()) throw runtime_error("Trying to add not compatible paths!");

    // Merge paths
    this->nodes.insert(this->nodes.end(), other.nodes.begin() + 1, other.nodes.end());

    // Update distance
    this->distance += other.distance;

    return *this;
}
