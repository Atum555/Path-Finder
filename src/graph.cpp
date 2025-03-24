#include <string>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"

using namespace std;

Vertex<Location, Distance> *findVertexById(Graph<Location, Distance> *g, int id) {
    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getId() == id) return v;
    return nullptr;
}

Vertex<Location, Distance> *findVertexByCode(Graph<Location, Distance> *g, const string &code) {
    for (Vertex<Location, Distance> *v : g->getVertexSet())
        if (v->getInfo().getCode() == code) return v;
    return nullptr;
}

Edge<Location, Distance> *findEdgeById(Graph<Location, Distance> *g, int sourceId, int destinationId) {
    Vertex<Location, Distance> *source      = findVertexById(g, sourceId);
    Vertex<Location, Distance> *destination = findVertexById(g, destinationId);

    for (Edge<Location, Distance> *edge : source->getOutgoing())
        if (edge->getDestination() == destination) return edge;

    return nullptr;
}
