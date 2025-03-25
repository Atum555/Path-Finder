#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "data_structs/priorityQueue.h"
#include "functions.h"

using namespace std;

enum EdgeType { driving, walking };

struct PossiblePath {
    Vertex<Location, Distance> *parkingNode;
    Path                        drivingPath;
    Path                        walkingPath;
    Path                        completePath;
};

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
    for (Edge<Location, Distance> *e : edgesToAvoid) {
        e->setSelected(false);
        e->getReverse()->setSelected(false);
    }
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

Path findShortestPathForDrivingMultipleNodes(
    Graph<Location, Distance> *g, const vector<Vertex<Location, Distance> *> nodesToConnect,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid
) {
    // Initialize path
    Path path;

    // Add paths between every 2 subsequent nodes
    for (auto itr = nodesToConnect.begin(); itr != nodesToConnect.end() - 1; itr++)
        path += findShortestPathForDriving(g, *itr, *(itr + 1), nodesToAvoid, edgesToAvoid);

    return path;
}

pair<Path, Path> findPathForDrivingWalking(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid,
    const double maxWalkingTime
) {
    // Initialize vector to save node info for each node
    vector<PossiblePath> possiblePaths;

    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    PriorityQueue<Vertex<Location, Distance>> queue;
    end->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Apply Dijkstra from the end node, taking into account the walking time
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, EdgeType::walking)) queue.decreaseKey(e->getDestination());

    // Update possible path list
    for (Vertex<Location, Distance> *v : g->getVertexSet()) {
        // If start node or end node, ignore
        if (v == start or v == end) continue;

        // Create a possible path from nodes with parking,
        // that have a walking path to the end node
        // with a distance smaller or equal to the max walking distance
        if (v->getInfo().hasParking() and v->getPath() != nullptr) {
            PossiblePath possiblePath;
            possiblePath.parkingNode = v;
            possiblePath.walkingPath = getPath(end, v, EdgeType::walking);
            reverse(possiblePath.walkingPath.nodes.begin(), possiblePath.walkingPath.nodes.end());
            possiblePaths.push_back(possiblePath);
        }
    }

    // If no possible paths exist stop searching
    if (possiblePaths.empty()) return pair{ Path(), Path() };

    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    queue = PriorityQueue<Vertex<Location, Distance>>();
    start->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Apply Dijkstra from the start node, taking into account the driving time
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, EdgeType::driving)) queue.decreaseKey(e->getDestination());

    // Update possible path list with driving paths
    auto it = possiblePaths.begin();
    while (it != possiblePaths.end()) {
        Vertex<Location, Distance> *parkingNode = (*it).parkingNode;

        // Remove paths that don't connect to the start node
        if (parkingNode->getPath() == nullptr) {
            it = possiblePaths.erase(it);
            continue;
        }

        // Add Driving path
        (*it).drivingPath = getPath(start, parkingNode, EdgeType::driving);

        // Calculate complete path
        (*it).completePath += (*it).drivingPath;
        (*it).completePath += (*it).walkingPath;
        it++;
    }

    // If no possible paths exist stop searching
    if (possiblePaths.empty()) return pair{ Path(), Path() };

    PossiblePath bestPath = possiblePaths.front();

    // Get best path
    // Smallest total distance
    // Or in case of equal total distance, largest walking distance
    for (auto it = possiblePaths.begin() + 1; it != possiblePaths.end(); it++)
        if ((*it).completePath.distance < bestPath.completePath.distance) bestPath = *it;
        else if ((*it).completePath.distance == bestPath.completePath.distance)
            if ((*it).walkingPath.distance > bestPath.walkingPath.distance) bestPath = *it;

    return pair{ bestPath.drivingPath, bestPath.walkingPath };
}
