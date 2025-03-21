#include "data_structs/graph.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    // TODO
    //  - Parse arguments
    //  - Call function to parse data files
    //  - Call execution code

    // Test Code
    Graph<int, int>   g;
    Vertex<int, int> *v1 = g.addVertex(34);
    Vertex<int, int> *v2 = g.addVertex(723);
    Vertex<int, int> *v3 = g.addVertex(3425);
    Vertex<int, int> *v4 = g.addVertex(243543);

    g.addEdge(v1, v2, 0);
    g.addEdge(v1, v3, -32432);
    g.addEdge(v2, v3, 43);
    g.addBidirectionalEdge(v3, v4, -123);

    cout << "Graph:" << endl;

    for (Vertex<int, int> *v : g.getVertexSet()) cout << v->getInfo() << ", ";
    cout << endl << endl;

    for (Vertex<int, int> *v : g.getVertexSet()) {
        cout << v->getInfo() << ":" << endl;
        for (Edge<int, int> *e : v->getOutgoing())
            cout << "  -> " << e->getDestination()->getInfo() << " : " << e->getInfo() << endl;
        cout << endl;
    }
}
