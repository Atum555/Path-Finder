#include <string>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

Vertex<Location, Distance> *findVertexById(Graph<Location, Distance> *g, const string &id) {
    if (g == nullptr) return nullptr;

    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getId() == id) return v;
    return nullptr;
}

Vertex<Location, Distance> *findVertexByCode(Graph<Location, Distance> *g, const string &code) {
    if (g == nullptr) return nullptr;

    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getCode() == code) return v;
    return nullptr;
}

Vertex<Location, Distance> *findVertex(Graph<Location, Distance> *g, const string &identifier) {
    if (g == nullptr) return nullptr;

    string _identifier = identifier;
    trim(_identifier);

    Vertex<Location, Distance> *vertex;

    vertex = findVertexById(g, _identifier);
    if (vertex == nullptr) vertex = findVertexByCode(g, _identifier);

    return vertex;
}

Edge<Location, Distance> *findEdge(Graph<Location, Distance> *g, const string &sourceId, const string &destinationId) {
    if (g == nullptr) return nullptr;
    Vertex<Location, Distance> *source      = findVertex(g, sourceId);
    Vertex<Location, Distance> *destination = findVertex(g, destinationId);

    if (source == nullptr or destination == nullptr) return nullptr;

    for (Edge<Location, Distance> *edge : source->getOutgoing())
        if (edge->getDestination() == destination) return edge;

    return nullptr;
}
