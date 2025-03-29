#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include <fstream>
#include <string>
#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"

struct Path {
    std::vector<Vertex<Location, Distance> *> nodes;
    double                                    distance = 0;

    Path &operator+=(const Path &other);

    bool operator==(const Path &other) const;
};

std::string &ltrim(std::string &s);

std::string &rtrim(std::string &s);

std::string &trim(std::string &s);

Graph<Location, Distance> *parseDataFiles(std::ifstream &locations, std::ifstream &distances);

Vertex<Location, Distance> *findVertex(Graph<Location, Distance> *g, const std::string &identifier);

Edge<Location, Distance> *
findEdge(Graph<Location, Distance> *g, const std::string &sourceId, const std::string &destinationId);

void runMenu(Graph<Location, Distance> *graph);

void runBatchMode(Graph<Location, Distance> *graph, std::ifstream &inputFile, std::ostream &out);

Path findShortestPath(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, bool driving
);

Path findShortestPathMultipleNodes(
    Graph<Location, Distance> *g, const std::vector<Vertex<Location, Distance> *> nodesToConnect,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, bool driving
);

std::pair<Path, Path> findPathForDrivingWalking(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, const double maxWalkingTime, const bool secondBest
);

#endif
