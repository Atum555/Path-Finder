#include <string>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

/**
 * @brief Finds a vertex in the graph by its identifier.
 *
 * This function iterates through the vertex set of the provided graph and compares
 * each vertex's identifier with the given id. If a matching vertex is found, a pointer
 * to that vertex is returned. If the graph pointer is nullptr or no vertex with the given id
 * is found, the function returns nullptr.
 *
 * @tparam Location The type representing the location information stored in the vertex.
 * @tparam Distance The type representing the distance information associated with the vertex.
 * @param g A pointer to the graph containing the set of vertices. Must not be nullptr.
 * @param id The identifier of the vertex to locate.
 * @return Vertex<Location, Distance>* A pointer to the found vertex, or nullptr if the vertex does not exist or the graph is invalid.
 */
Vertex<Location, Distance> *findVertexById(Graph<Location, Distance> *g, const string &id) {
    if (g == nullptr) return nullptr;

    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getId() == id) return v;
    return nullptr;
}

/**
 * @brief Searches for a vertex in the graph with the specified code.
 *
 * This function iterates through the set of vertices in the provided graph and returns
 * a pointer to the vertex whose associated Location object's code matches the given code.
 * If no matching vertex is found or if the graph pointer is null, the function returns nullptr.
 *
 * @tparam Location The type representing the location information contained in the vertex.
 * @tparam Distance The type representing the distance information associated with the vertex.
 * @param g Pointer to the Graph object containing vertices.
 * @param code The code string used to identify the desired vertex.
 * @return Pointer to the matching Vertex if found, otherwise nullptr.
 */
Vertex<Location, Distance> *findVertexByCode(Graph<Location, Distance> *g, const string &code) {
    if (g == nullptr) return nullptr;

    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getCode() == code) return v;
    return nullptr;
}

/**
 * @brief Finds a vertex in the graph using an identifier.
 *
 * This function attempts to locate a vertex in the given graph by first trimming the provided
 * identifier and then searching for a matching vertex in two ways:
 * - By comparing the identifier with the vertex's ID.
 * - If not found, by comparing the identifier with the vertex's code.
 *
 * If the graph pointer is null or no matching vertex is found in either search, the function returns null.
 *
 * @tparam Location The type representing location information associated with the vertex.
 * @tparam Distance The type representing the distance metric for the vertex.
 * @param g A pointer to the graph in which to search for the vertex.
 * @param identifier The identifier string used to match against the vertex's ID or code.
 * @return A pointer to the matching vertex, or null if no match is found.
 */
Vertex<Location, Distance> *findVertex(Graph<Location, Distance> *g, const string &identifier) {
    if (g == nullptr) return nullptr;

    string _identifier = identifier;
    trim(_identifier);

    Vertex<Location, Distance> *vertex;

    vertex = findVertexById(g, _identifier);
    if (vertex == nullptr) vertex = findVertexByCode(g, _identifier);

    return vertex;
}

/**
 * Searches for an edge in the graph that originates from the vertex with the given source identifier
 * and terminates at the vertex with the provided destination identifier.
 *
 * This function performs the following operations:
 * 1. Checks if the graph pointer is valid.
 * 2. Retrieves the source and destination vertices corresponding to the provided identifiers.
 * 3. Iterates through the outgoing edges of the source vertex, returning the edge that connects to the destination vertex.
 * 4. Returns nullptr if the graph is invalid, either vertex does not exist, or if no connecting edge is found.
 *
 * @param g A pointer to the Graph object containing vertices and edges.
 * @param sourceId A constant reference to a string representing the ID of the source vertex.
 * @param destinationId A constant reference to a string representing the ID of the destination vertex.
 * @return A pointer to the Edge connecting the source to the destination if it exists; otherwise, returns nullptr.
 */
Edge<Location, Distance> *findEdge(Graph<Location, Distance> *g, const string &sourceId, const string &destinationId) {
    if (g == nullptr) return nullptr;
    Vertex<Location, Distance> *source      = findVertex(g, sourceId);
    Vertex<Location, Distance> *destination = findVertex(g, destinationId);

    if (source == nullptr or destination == nullptr) return nullptr;

    for (Edge<Location, Distance> *edge : source->getOutgoing())
        if (edge->getDestination() == destination) return edge;

    return nullptr;
}
