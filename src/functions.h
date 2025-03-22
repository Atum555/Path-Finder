#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include <fstream>
#include <string>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"

void ltrim(std::string &s);

void rtrim(std::string &s);

void trim(std::string &s);

Graph<Location, Distance> *parseDataFiles(std::ifstream &locations, std::ifstream &distances);

Vertex<Location, Distance> *findVertexById(Graph<Location, Distance> *g, int id);

Vertex<Location, Distance> *findVertexByCode(Graph<Location, Distance> *g, const std::string &code);

void runMenu(Graph<Location, Distance> *graph);

void runBatchMode(Graph<Location, Distance> *graph, const std::string &inputFile);
void runBatchMode(Graph<Location, Distance> *graph, const std::string &inputFile, const std::string &outputFile);

#endif
