#include <fstream>
#include <sstream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

Graph<Location, Distance> *parseDataFiles(ifstream &locations, ifstream &distances) {
    // Create Graph
    Graph<Location, Distance> *graph = new Graph<Location, Distance>();

    string token;
    string line;

    //! LOCATIONS

    getline(locations, line); // Ignore first line

    string id;
    string code;
    string location;
    bool   parking;

    // Parse all locations
    while (getline(locations, line)) {
        stringstream lineSS(line);

        // Read Location
        getline(lineSS, location, ',');
        trim(location);

        // Read Id
        getline(lineSS, id, ',');
        trim(id);

        // Read Code
        getline(lineSS, code, ',');
        trim(code);

        // Read Parking
        getline(lineSS, token);
        trim(token);
        parking = token == "1";

        // Add Node to the Graph
        graph->addVertex(Location(id, code, location, parking));
    }

    //! DISTANCES

    getline(distances, line); // Ignore first line

    string code1;
    string code2;
    double walking;
    double driving;

    // Parse all distances
    while (getline(distances, line)) {
        stringstream lineSS(line);

        // Read Location1
        getline(lineSS, code1, ',');
        trim(code1);

        // Read Location2
        getline(lineSS, code2, ',');
        trim(code2);

        // Read Driving
        getline(lineSS, token, ',');
        trim(token);
        driving = token == "X" ? INF : stod(token);

        // Read Walking
        getline(lineSS, token, ',');
        trim(token);
        walking = stod(token);

        // Get Nodes
        Vertex<Location, Distance> *v1 = findVertex(graph, code1);
        Vertex<Location, Distance> *v2 = findVertex(graph, code2);

        // Add Edge to Graph
        graph->addBidirectionalEdge(v1, v2, Distance(walking, driving));
    }

    return graph;
}
