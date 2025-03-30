#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include <fstream>
#include <string>
#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"

/**
 * @brief Represents a path within a graph.
 *
 * The Path struct encapsulates a sequence of vertices (nodes) traversed in a graph and the
 * corresponding total distance of that traversal. It is designed to facilitate path operations,
 * such as combining two paths and comparing paths for equality.
 *
 * Members:
 * - nodes: A vector containing pointers to vertices. Each vertex is templated with Location and
 *          Distance, representing a point in the graph and the weight associated with edges.
 * - distance: A double representing the accumulated distance along the path.
 *
 * Operator Overloads:
 * - operator+=: Combines the current path with another by appending the nodes from the other
 *               path and adding its distance to the current distance.
 * - operator==: Compares two Path objects for equality, typically by checking both the sequence
 *               of nodes and the total distance.
 */
struct Path {
    std::vector<Vertex<Location, Distance> *> nodes;
    double                                    distance = 0;

    Path &operator+=(const Path &other);

    bool operator==(const Path &other) const;
};

/**
 * @brief Removes leading whitespace from a string.
 *
 * This function modifies the input string by removing any whitespace characters
 * from the beginning of the string and returns a reference to the modified string.
 *
 * @param s A reference to the string to be trimmed.
 * @return A reference to the modified (left-trimmed) string.
 */
std::string &ltrim(std::string &s);

/**
 * @brief Removes trailing whitespace from the provided string.
 *
 * This function modifies the input string by eliminating any whitespace characters
 * (such as spaces, tabs, and newline characters) that appear at the end of the string.
 *
 * @param s A reference to the string to be trimmed.
 * @return A reference to the modified string with trailing whitespace removed.
 */
std::string &rtrim(std::string &s);

/**
 * @brief Removes leading and trailing whitespace from the provided string.
 *
 * This function processes the input string by stripping off all
 * whitespace characters from both the beginning and the end. The operation
 * modifies the original string directly.
 *
 * @param s The string to be trimmed.
 * @return A reference to the trimmed string.
 */
std::string &trim(std::string &s);

/**
 * @brief Parses location and distance data files to construct a graph.
 *
 * This function reads input from two file streams: one containing location data and the 
 * other containing distance data. It processes the data to create and initialize a graph,
 * where each node represents a location and each edge represents a distance between 
 * locations.
 *
 * @param locations An ifstream object containing the location information.
 * @param distances An ifstream object containing the distance measurements between locations.
 *
 * @return A pointer to a Graph object templated with Location as node type and Distance 
 * as edge weight type, representing the constructed graph based on the input files.
 *
 * @note The caller is responsible for ensuring that the file streams are open and the data 
 * within them is in the expected format. Error handling for invalid or corrupt data should 
 * be managed outside this function.
 */
Graph<Location, Distance> *parseDataFiles(std::ifstream &locations, std::ifstream &distances);

/**
 * @brief Searches for a vertex in the specified graph matching the given identifier.
 *
 * This function iterates over the vertices in the graph to locate and return the vertex 
 * with the provided identifier. If the vertex is not found, it returns a null pointer.
 *
 * @param g Pointer to the graph in which the search is performed.
 * @param identifier The unique identifier of the vertex to retrieve.
 * @return A pointer to the found vertex, or null if no matching vertex exists.
 */
Vertex<Location, Distance> *findVertex(Graph<Location, Distance> *g, const std::string &identifier);

/**
 * @brief Finds and retrieves an edge connecting two specified vertices in the graph.
 *
 * This function searches for and returns an edge that connects the vertex with the given
 * source identifier to the vertex with the specified destination identifier within the provided graph.
 * If no such edge exists, the function returns a nullptr.
 *
 * @param g A pointer to the graph in which the edge is to be searched.
 * @param sourceId The identifier of the source vertex.
 * @param destinationId The identifier of the destination vertex.
 *
 * @return A pointer to the edge connecting the specified vertices if it exists; otherwise, a nullptr.
 */
Edge<Location, Distance> *
findEdge(Graph<Location, Distance> *g, const std::string &sourceId, const std::string &destinationId);

/**
 * @brief Runs the interactive menu mode for graph operations.
 *
 * This function initiates a console-based menu that allows users to interact with the graph.
 * Users can select various options to perform operations on the provided graph data structure.
 *
 * @param graph A pointer to a Graph object containing Location nodes and representing the distances as its edge weights.
 */
void runMenuMode(Graph<Location, Distance> *graph);

/**
 * @brief Processes batch operations on the graph.
 *
 * This function parses and executes a series of commands read from the input file stream,
 * applies those operations on the provided graph, and writes the results or any output messages
 * to the specified output stream.
 *
 * @param graph Pointer to a Graph object containing nodes of type Location and weighted edges of type Distance.
 * @param inputFile Stream to read the batch commands or data from.
 * @param out Stream to output the results of the batch processing.
 */
void runBatchMode(Graph<Location, Distance> *graph, std::ifstream &inputFile, std::ostream &out);

/**
 * @brief Computes the shortest path between two vertices in a graph using Dijkstra's algorithm.
 *
 * This function finds the shortest path in the given graph from the start vertex to the end vertex,
 * taking into account any vertices and edges that should be avoided and whether specific driving constraints
 * apply. The graph is prepared by clearing existing distances and filtering out undesired nodes and edges.
 * It then initializes a priority queue where the start vertex is set with a distance of 0 and all other vertices
 * are set with an initial large distance. The algorithm extracts the vertex with the minimum distance and relaxes
 * its outgoing edges, updating their distances if a shorter path is found.
 *
 * Time Complexity: O((V + E) log V) in the worst case, where V is the number of vertices and E is the number of edges.
 *
 * @param g Pointer to the graph on which the shortest path is computed.
 * @param start Pointer to the starting vertex.
 * @param end Pointer to the destination vertex.
 * @param nodesToAvoid Vector of pointers to vertices that should be avoided.
 * @param edgesToAvoid Vector of pointers to edges that should be avoided.
 * @param driving Boolean flag indicating whether driving constraints should be taken into account.
 *
 * @return A Path object representing the shortest path from start to end.
 */
Path findShortestPath(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, bool driving
);

/**
 * @brief Computes the shortest path connecting a series of nodes in a graph.
 *
 * This function computes the shortest path that sequentially connects all vertices
 * specified in the nodesToConnect vector. For each consecutive pair of nodes, it calls
 * the findShortestPath function to obtain the shortest segment between them. The resulting
 * segments are then concatenated to form the complete path. If any segment path is not found,
 * or if any of the required nodes or the graph pointer is null, the function returns an empty Path.
 *
 * @param g Pointer to the graph containing vertices and edges.
 * @param nodesToConnect Vector of pointers to vertices that need to be connected in the given order.
 * @param nodesToAvoid Vector of pointers to vertices that should be avoided during pathfinding.
 * @param edgesToAvoid Vector of pointers to edges that should be avoided during pathfinding.
 * @param driving Boolean flag indicating whether to consider driving mode constraints.
 *
 * @return A Path object that aggregates all the shortest paths between the nodes in the specified order.
 *         Returns an empty Path if the graph is null, if any vertex in nodesToConnect is null,
 *         or if a shortest path cannot be found for any consecutive pair.
 *
 * @note Time Complexity:
 * The function iterates over each pair of consecutive nodes in nodesToConnect. If there are n nodes,
 * it makes (n - 1) calls to findShortestPath. Let T be the time complexity of findShortestPath; then,
 * the overall time complexity is O((n - 1) * T). For example, if findShortestPath uses Dijkstra's
 * algorithm implemented with a binary heap (with time complexity O((V + E) log V)),
 * the overall complexity becomes O(n * (V + E) log V).
 */
Path findShortestPathMultipleNodes(
    Graph<Location, Distance> *g, const std::vector<Vertex<Location, Distance> *> nodesToConnect,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, bool driving
);

/**
 * @brief Finds a pair of paths combining driving and walking segments between a start and an end location.
 *
 * This function searches for the best (or second-best, if requested) complementary path that consists of a driving
 * segment from the start vertex to a parking node and a walking segment from that parking node to the end vertex. The
 * walking segment is only considered if its distance is within the given maximum walking time. The algorithm avoids
 * specified nodes and edges by consulting the provided avoidance lists. It uses Dijkstra's algorithm twice: first,
 * running from the end vertex to establish valid parking nodes that allow for an acceptable walking path; then running
 * from the start vertex to compute the driving path to each valid parking node. The candidate path with the smallest
 * total distance is chosen, with the walking distance acting as a tie breaker when needed.
 *
 * @param g Pointer to the graph (vertices represent locations and edges represent distances).
 * @param start Pointer to the start vertex for the driving segment.
 * @param end Pointer to the end vertex for the walking segment.
 * @param nodesToAvoid Vector containing pointers to vertices to be avoided in the path search.
 * @param edgesToAvoid Vector containing pointers to edges to be avoided in the path search.
 * @param maxWalkingTime Maximum allowed walking distance (or time), used as a threshold for the walking segment.
 * @param secondBest If true, returns the second best complete path; otherwise, returns the best complete path.
 *
 * @return A std::pair where the first element is the driving path from the start vertex to a chosen parking node,
 *         and the second element is the walking path from that parking node to the end vertex. If no valid
 *         path is found, both elements of the pair will be empty.
 *
 * @note The function performs two runs of Dijkstra's algorithm. Each run has a time complexity of O((|V| + |E|) log
 * |V|), where |V| is the number of vertices and |E| is the number of edges.
 *
 * @note Overall, the time complexity is approximately O((|V| + |E|) log |V|) for each Dijkstra run plus additional
 *       overhead for iterating over the vertices, so the dominant factor is the Dijkstra executions.
 *
 * @warning This function assumes that the graph preparation (using prepareGraph()), edge relaxation (using
 * relaxEdge()), and path construction (using getPath()) functions are implemented correctly and that the input graph is
 *          valid.
 */
std::pair<Path, Path> findPathForDrivingWalking(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const std::vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const std::vector<Edge<Location, Distance> *> &edgesToAvoid, const double maxWalkingTime, const bool secondBest
);

#endif
