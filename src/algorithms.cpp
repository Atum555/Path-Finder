#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "data_structs/priorityQueue.h"
#include "functions.h"

using namespace std;

struct PossiblePath {
    Vertex<Location, Distance> *parkingNode;
    Path                        drivingPath;
    Path                        walkingPath;
    Path                        completePath;
};

bool operator==(const PossiblePath &path1, const PossiblePath &path2) {
    return (path1.walkingPath == path2.walkingPath) and (path1.drivingPath == path2.drivingPath);
}

bool relaxEdge(Edge<Location, Distance> *edge, bool driving) {
    // Sanity check
    if (edge == nullptr) return false;
    if (not edge->isSelected()) return false;

    Vertex<Location, Distance> *origin       = edge->getOrigin();
    Vertex<Location, Distance> *destination  = edge->getDestination();
    double                      edgeDistance = driving ? edge->getInfo().getDriving() : edge->getInfo().getWalking();

    if (destination->getDistance() > origin->getDistance() + edgeDistance) {
        destination->setDistance(origin->getDistance() + edgeDistance);
        destination->setPath(edge);
        return true;
    }
    return false;
}

void prepareGraph(
    Graph<Location, Distance> *g, const vector<Vertex<Location, Distance> *> &nodesToInclude,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid
) {
    // Sanity check
    if (g == nullptr) return;

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
        if (v == nullptr) continue; // Sanity check
        if (find(nodesToInclude.begin(), nodesToInclude.end(), v) != nodesToInclude.end()) continue;
        for (Edge<Location, Distance> *e : v->getOutgoing()) e->setSelected(false);
        for (Edge<Location, Distance> *e : v->getIncoming()) e->setSelected(false);
    }

    // Remove forbidden edges from search
    for (Edge<Location, Distance> *e : edgesToAvoid) {
        if (e == nullptr) continue; // Sanity check
        e->setSelected(false);
        e->getReverse()->setSelected(false);
    }
}

Path getPath(Vertex<Location, Distance> *start, Vertex<Location, Distance> *end) {
    Vertex<Location, Distance> *v = end;
    Path                        path;

    // Sanity check
    if (start == nullptr or end == nullptr) return path;
    if (end->getPath() == nullptr) return path; // No Path was found

    path.distance = end->getDistance();

    while (v != nullptr) {
        path.nodes.push_back(v);
        v = (v->getPath() != nullptr) ? v->getPath()->getOrigin() : nullptr;
    }

    reverse(path.nodes.begin(), path.nodes.end());
    return path;
}

Path findShortestPath(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid,
    bool driving
) {
    // Sanity check
    if (g == nullptr or start == nullptr or end == nullptr) return Path();

    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, { start, end }, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    PriorityQueue<Vertex<Location, Distance>> queue;
    start->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Dijkstra
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, driving)) queue.decreaseKey(e->getDestination());

    return getPath(start, end);
}

Path findShortestPathMultipleNodes(
    Graph<Location, Distance> *g, const vector<Vertex<Location, Distance> *> nodesToConnect,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid,
    bool driving
) {
    // Sanity check
    if (g == nullptr) return Path();
    for (Vertex<Location, Distance> *v : nodesToConnect)
        if (v == nullptr) return Path();

    // Initialize path
    Path path;

    // Add paths between every 2 subsequent nodes
    for (auto itr = nodesToConnect.begin(); itr != nodesToConnect.end() - 1; itr++) {
        Path subPath = findShortestPath(g, *itr, *(itr + 1), nodesToAvoid, edgesToAvoid, driving);
        if (subPath.nodes.size() == 0) return Path();
        path += subPath;
    }


    return path;
}

pair<Path, Path> findPathForDrivingWalking(
    Graph<Location, Distance> *g, Vertex<Location, Distance> *start, Vertex<Location, Distance> *end,
    const vector<Vertex<Location, Distance> *> &nodesToAvoid, const vector<Edge<Location, Distance> *> &edgesToAvoid,
    const double maxWalkingTime, const bool secondBest
) {
    // Sanity check
    if (g == nullptr or start == nullptr or end == nullptr) return pair{ Path(), Path() };

    // Initialize vector to save node info for each node
    vector<PossiblePath> possiblePaths;

    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, { start, end }, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    PriorityQueue<Vertex<Location, Distance>> queue;
    end->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Apply Dijkstra from the end node, taking into account the walking time
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, false)) queue.decreaseKey(e->getDestination());

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
            possiblePath.walkingPath = getPath(end, v);
            reverse(possiblePath.walkingPath.nodes.begin(), possiblePath.walkingPath.nodes.end());
            if (possiblePath.walkingPath.distance <= maxWalkingTime) possiblePaths.push_back(possiblePath);
        }
    }

    // If no possible paths exist stop searching
    if (possiblePaths.empty()) return pair{ Path(), Path() };

    // Prepare graph
    // Clear distance and path values
    // Select edges
    prepareGraph(g, { start, end }, nodesToAvoid, edgesToAvoid);

    // Init Priority Queue
    queue = PriorityQueue<Vertex<Location, Distance>>();
    start->setDistance(0);
    for (Vertex<Location, Distance> *v : g->getVertexSet()) queue.insert(v);

    // Apply Dijkstra from the start node, taking into account the driving time
    while (not queue.empty())
        for (Edge<Location, Distance> *e : queue.extractMin()->getOutgoing())
            if (relaxEdge(e, true)) queue.decreaseKey(e->getDestination());

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
        (*it).drivingPath = getPath(start, parkingNode);

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

    // If we want the second best path
    if (secondBest) {
        PossiblePath secondBestPath = PossiblePath{
            nullptr, Path{ {}, INF },
             Path{ {}, INF },
             Path{ {}, INF }
        };

        // Get second best path
        // Smallest total distance
        // Or in case of equal total distance, largest walking distance
        for (auto it = possiblePaths.begin(); it != possiblePaths.end(); it++) {
            if ((*it) == bestPath) continue;
            if ((*it).completePath.distance < secondBestPath.completePath.distance) secondBestPath = *it;
            else if ((*it).completePath.distance == secondBestPath.completePath.distance)
                if ((*it).walkingPath.distance > secondBestPath.walkingPath.distance) secondBestPath = *it;
        }
        return pair{ secondBestPath.drivingPath, secondBestPath.walkingPath };
    }

    return pair{ bestPath.drivingPath, bestPath.walkingPath };
}
