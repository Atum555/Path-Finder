#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "data_structs/priorityQueue.h"
#include "functions.h"

using namespace std;

enum EdgeType { driving, walking };

bool relaxEdge(Edge<Location, Distance> *edge, EdgeType type) {
    if (not edge->isSelected()) return false;

    Vertex<Location, Distance> *origin      = edge->getOrigin();
    Vertex<Location, Distance> *destination = edge->getDestination();
    double edgeDistance = (type == EdgeType::driving) ? edge->getInfo().getDriving() : edge->getInfo().getWalking();

    if (destination->getDistance() > origin->getDistance() + edgeDistance) {
        destination->setDistance(origin->getDistance() + edgeDistance);
        destination->setPath(edge);
        return true;
    }
    return false;
}

void prepareGraph(
    Graph<Location, Distance> *g, const vector<Vertex<Location, Distance> *> &nodesToAvoid,
    const vector<Edge<Location, Distance> *> &edgesToAvoid
) {
    // Clear distance and path values
    for (Vertex<Location, Distance> *v : g->getVertexSet()) {
        v->setDistance(INF);
        v->setPath(nullptr);

        // Mark all edges as selected
        // Only selected edges can be used
        for (Edge<Location, Distance> *e : v->getOutgoing()) e->setSelected(true);
    }

    // Remove edges connecting to forbidden nodes from search
    for (Vertex<Location, Distance> *v : nodesToAvoid) {
        for (Edge<Location, Distance> *e : v->getOutgoing()) e->setSelected(false);
        for (Edge<Location, Distance> *e : v->getIncoming()) e->setSelected(false);
    }

    // Remove forbidden edges from search
    for (Edge<Location, Distance> *e : edgesToAvoid) e->setSelected(false);
}

Path getPath(Vertex<Location, Distance> *start, Vertex<Location, Distance> *end, EdgeType type) {
    Vertex<Location, Distance> *v = end;
    Path                        path;

    if (end->getPath() == nullptr) return path; // No Path was found

    while (v != nullptr) {
        path.nodes.push_back(v);

        if (v->getPath() != nullptr) {
            // Sum Distance
            Distance edgeDistance  = v->getPath()->getInfo();
            path.distance         += type == EdgeType::driving ? edgeDistance.getDriving() : edgeDistance.getWalking();

            v = v->getPath()->getOrigin();
        } else v = nullptr;
    }

    reverse(path.nodes.begin(), path.nodes.end());
    return path;
}

Path findShortestPathForDriving(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid
) {
    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    PriorityQueue<Vertex<Location, Distance>> queue;
    start->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Dijkstra
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, EdgeType::driving)) queue.decreaseKey(e->getDestination());

    return getPath(start, end, EdgeType::driving);
}
