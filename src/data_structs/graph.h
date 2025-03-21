// Original code by Gonçalo Leão
// Updated by DA 2024/2025 Team

#ifndef __GRAPH__
#define __GRAPH__

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

template <class T, class W>
class Vertex;
template <class T, class W>
class Edge;
template <class T, class W>
class Graph;

#define INF std::numeric_limits<double>::max()

template <class T, class W>
class Vertex {
  public:
    Vertex(T info);
    ~Vertex();
    bool operator<(Vertex<T, W> &vertex) const;

    T                         getInfo() const;
    std::vector<Edge<T, W> *> getOutgoing() const;
    std::vector<Edge<T, W> *> getIncoming() const;
    bool                      isVisited() const;
    bool                      isProcessing() const;
    double                    getDistance() const;
    Edge<T, W>               *getPath() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);
    void setDist(double dist);
    void setPath(Edge<T, W> *path);

    Edge<T, W> *addEdge(Vertex<T, W> *destination, W info);
    bool        removeOutgoingEdge(Edge<T, W> *edge);
    bool        removeIncomingEdge(Edge<T, W> *edge);

  protected:
    T                         _info;
    std::vector<Edge<T, W> *> _outgoing;
    std::vector<Edge<T, W> *> _incoming;

    bool        _visited    = false;
    bool        _processing = false;
    double      _distance   = 0;
    Edge<T, W> *_path       = nullptr;

    int _queueIndex = 0; // required by MutablePriorityQueue
};

template <class T, class W>
class Edge {
  public:
    Edge(Vertex<T, W> *orig, Vertex<T, W> *dest, W info);
    ~Edge();

    W             getInfo() const;
    Vertex<T, W> *getDestination() const;
    Vertex<T, W> *getOrigin() const;
    Edge<T, W>   *getReverse() const;
    bool          isSelected() const;

    void setReverse(Edge<T, W> *reverse);
    void setSelected(bool selected);

  protected:
    W             _info;
    Vertex<T, W> *_origin;
    Vertex<T, W> *_destination;

    bool _selected = false;

    Edge<T, W> *_reverse = nullptr;
};

template <class T, class W>
class Graph {
  public:
    ~Graph();

    Vertex<T, W> *addVertex(const T &info);
    bool          removeVertex(Vertex<T, W> *vertex);

    Edge<T, W> *addEdge(Vertex<T, W> *source, Vertex<T, W> *destination, const W &info);
    Edge<T, W> *addBidirectionalEdge(Vertex<T, W> *vertexA, Vertex<T, W> *vertexB, const W &info);
    bool        removeEdge(Edge<T, W> *edge);

    std::vector<Vertex<T, W> *> getVertexSet() const;
    int                         getNumVertex() const;

  protected:
    std::vector<Vertex<T, W> *> _vertexSet;
};

/**********************  Vertex  ****************************/

template <class T, class W>
Vertex<T, W>::Vertex(T info) : _info(info) {}

template <class T, class W>
Vertex<T, W>::~Vertex() {
    // Keep deleting first outgoing vertex
    // The deconstructor of the edge removes itself from the vertexes
    while (not this->_outgoing.empty()) delete this->_outgoing.front();

    // Keep deleting first incoming edge
    // The deconstructor of the edge removes itself from the vertexes
    while (not this->_incoming.empty()) delete this->_incoming.front();
}

template <class T, class W>
bool Vertex<T, W>::operator<(Vertex<T, W> &vertex) const {
    return this->dist < vertex.dist;
}

template <class T, class W>
T Vertex<T, W>::getInfo() const {
    return this->_info;
}

template <class T, class W>
std::vector<Edge<T, W> *> Vertex<T, W>::getOutgoing() const {
    return this->_outgoing;
}

template <class T, class W>
std::vector<Edge<T, W> *> Vertex<T, W>::getIncoming() const {
    return this->_incoming;
}

template <class T, class W>
bool Vertex<T, W>::isVisited() const {
    return this->_visited;
}

template <class T, class W>
bool Vertex<T, W>::isProcessing() const {
    return this->_processing;
}

template <class T, class W>
double Vertex<T, W>::getDistance() const {
    return this->_distance;
}

template <class T, class W>
Edge<T, W> *Vertex<T, W>::getPath() const {
    return this->_path;
}

template <class T, class W>
void Vertex<T, W>::setInfo(T info) {
    this->_info = info;
}

template <class T, class W>
void Vertex<T, W>::setVisited(bool visited) {
    this->_visited = visited;
}

template <class T, class W>
void Vertex<T, W>::setProcessing(bool processing) {
    this->_processing = processing;
}

template <class T, class W>
void Vertex<T, W>::setDist(double dist) {
    this->_distance = dist;
}

template <class T, class W>
void Vertex<T, W>::setPath(Edge<T, W> *path) {
    this->_path = path;
}

/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge info (info).
 */
template <class T, class W>
Edge<T, W> *Vertex<T, W>::addEdge(Vertex<T, W> *destination, W info) {
    Edge<T, W> *newEdge = new Edge<T, W>(this, destination, info);
    this->_outgoing.push_back(newEdge);
    destination->_incoming.push_back(newEdge);
    return newEdge;
}

/*
 * Auxiliary function that removes an edge pointer from it's outgoing
 * list of edges. Returns true if removed, false otherwise.
 */
template <class T, class W>
bool Vertex<T, W>::removeOutgoingEdge(Edge<T, W> *edge) {
    auto it = this->_outgoing.begin();
    while (it != this->_outgoing.end()) {
        if ((*it) == edge) {
            this->_outgoing.erase(it);
            return true;
        }
        it++;
    }
    return false;
}

/*
 * Auxiliary function that removes an edge pointer from it's incoming
 * list of edges. Returns true if removed, false otherwise.
 */
template <class T, class W>
bool Vertex<T, W>::removeIncomingEdge(Edge<T, W> *edge) {
    auto it = this->_incoming.begin();
    while (it != this->_incoming.end()) {
        if ((*it) == edge) {
            this->_incoming.erase(it);
            return true;
        }
        it++;
    }
    return false;
}

/********************** Edge  ****************************/

template <class T, class W>
Edge<T, W>::Edge(Vertex<T, W> *origin, Vertex<T, W> *destination, W info)
    : _origin(origin), _destination(destination), _info(info) {}

template <class T, class W>
Edge<T, W>::~Edge() {
    this->_origin->removeOutgoingEdge(this);
    this->_destination->removeIncomingEdge(this);
}

template <class T, class W>
W Edge<T, W>::getInfo() const {
    return this->_info;
}

template <class T, class W>
Vertex<T, W> *Edge<T, W>::getOrigin() const {
    return this->_origin;
}

template <class T, class W>
Vertex<T, W> *Edge<T, W>::getDestination() const {
    return this->_destination;
}

template <class T, class W>
Edge<T, W> *Edge<T, W>::getReverse() const {
    return this->_reverse;
}

template <class T, class W>
bool Edge<T, W>::isSelected() const {
    return this->_selected;
}

template <class T, class W>
void Edge<T, W>::setSelected(bool selected) {
    this->_selected = selected;
}

template <class T, class W>
void Edge<T, W>::setReverse(Edge<T, W> *reverse) {
    this->_reverse = reverse;
}

/********************** Graph  ****************************/

/*
 *  Adds a vertex with a given content (info) to a graph (this).
 */
template <class T, class W>
Vertex<T, W> *Graph<T, W>::addVertex(const T &info) {
    Vertex<T, W> *vertex = new Vertex<T, W>(info);
    this->_vertexSet.push_back(vertex);
    return vertex;
}

template <class T, class W>
Graph<T, W>::~Graph() {
    for (Vertex<T, W> *v : this->_vertexSet) delete v;
}

/*
 *  Removes a vertex (vertex) from a graph (this), and
 *  all outgoing and incoming edges.
 *  Returns true if successful, and false if such vertex does not exist.
 */
template <class T, class W>
bool Graph<T, W>::removeVertex(Vertex<T, W> *vertex) {
    for (auto it = this->_vertexSet.begin(); it != this->_vertexSet.end(); it++) {
        if ((*it) == vertex) {
            delete (*it);                    // Delete Vertex
            it = this->_vertexSet.erase(it); // Remove pointer from list
            return true;
        }
    }
    return false;
}

template <class T, class W>
std::vector<Vertex<T, W> *> Graph<T, W>::getVertexSet() const {
    return this->_vertexSet;
}

template <class T, class W>
int Graph<T, W>::getNumVertex() const {
    return this->_vertexSet.size();
}

/*
 * Adds an edge to a graph (this), given the source and
 * destination vertices and the edge info.
 * Returns a pointer to the edge on success, nullptr on fail.
 */
template <class T, class W>
Edge<T, W> *Graph<T, W>::addEdge(Vertex<T, W> *source, Vertex<T, W> *destination, const W &info) {
    if (source == nullptr || destination == nullptr) return nullptr;
    return source->addEdge(destination, info);
}

/*
 * Adds an edge to a graph (this), given A and B vertices and the edge info.
 * Returns a pointer to one of the edges on success, nullptr on fail.
 */
template <class T, class W>
Edge<T, W> *Graph<T, W>::addBidirectionalEdge(Vertex<T, W> *vertexA, Vertex<T, W> *vertexB, const W &info) {
    if (vertexA == nullptr || vertexB == nullptr) return nullptr;
    Edge<T, W> *edgeA = vertexA->addEdge(vertexB, info);
    Edge<T, W> *edgeB = vertexB->addEdge(vertexA, info);
    edgeA->setReverse(edgeB);
    edgeB->setReverse(edgeA);
    return edgeA;
}

/*
 * Removes an edge from a graph (this).
 * The edge is identified by the source (sourc) and destination (dest) contents.
 * Returns true if successful, and false if such edge does not exist.
 */
template <class T, class W>
bool Graph<T, W>::removeEdge(Edge<T, W> *edge) {
    if (edge == nullptr) return false;

    // Check if Edge belongs to the graph
    bool foundSource      = std::find(_vertexSet.begin(), _vertexSet.end(), edge->getOrigin()) != _vertexSet.end();
    bool foundDestination = std::find(_vertexSet.begin(), _vertexSet.end(), edge->getDestination()) != _vertexSet.end();

    // Delete Edge
    if (foundSource and foundDestination) {
        delete edge;
        return true;
    }
    return false;
}

#endif /* __GRAPH__ */
