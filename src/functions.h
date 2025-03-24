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
};

void ltrim(std::string &s);

void rtrim(std::string &s);

void trim(std::string &s);

Graph<Location, Distance> *parseDataFiles(std::ifstream &locations, std::ifstream &distances);

Vertex<Location, Distance> *findVertexById(Graph<Location, Distance> *g, int id);

Vertex<Location, Distance> *findVertexByCode(Graph<Location, Distance> *g, const std::string &code);

Edge<Location, Distance> *findEdgeById(Graph<Location, Distance> *g, int sourceId, int destinationId);

void runMenu(Graph<Location, Distance> *graph);

void runBatchMode(Graph<Location, Distance> *graph, std::ifstream &inputFile, std::ostream &outputFile);

Path findShortestPathForDriving(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *>   &edgesToAvoid
);

Path findShortestPathForDrivingMultipleNodes(
    Graph<Location, Distance> *g, const std::vector<Vertex<Location, Distance> *> nodesToConnect,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *>   &edgesToAvoid
);

#endif
