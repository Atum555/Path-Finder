#include <fstream>
#include <sstream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

/**
 * @brief Parses location and distance data files and constructs a graph.
 *
 * This function reads two CSV files: one containing location details and
 * another containing distance information between locations. It creates a
 * graph where each vertex represents a location and each bidirectional edge
 * represents the distance (walking and driving) between two locations.
 *
 * The function performs the following steps:
 * 1. Reads and ignores the header line of the locations file.
 * 2. For each subsequent line in the locations file:
 *    - Parses the location name, id, code, and parking availability.
 *    - Trims whitespace from each parsed token.
 *    - Creates a Location object and adds it as a vertex in the graph.
 *
 * 3. Reads and ignores the header line of the distances file.
 * 4. For each subsequent line in the distances file:
 *    - Parses two location codes, the driving distance, and the walking distance.
 *    - Replaces the driving distance with INF if the token equals "X".
 *    - Trims whitespace for each parsed token.
 *    - Uses the location codes to locate their corresponding vertices in the graph.
 *    - Adds a bidirectional edge between the vertices with the associated Distance data.
 *
 * @param locations An input file stream containing location data
 *                  (format: location, id, code, parking flag).
 * @param distances An input file stream containing distance data
 *                  (format: code1, code2, driving distance/token "X", walking distance).
 *
 * @return Graph<Location, Distance>* Pointer to the constructed graph.
 *
 * @note The memory allocated for the graph is managed by the caller.
 * @note This function uses helper functions such as trim() for whitespace removal
 *       and findVertex() for locating graph vertices by location code.
 */
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
