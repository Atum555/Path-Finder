#include <iostream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

int main(int argc, char *argv[]) {
    // TODO
    //  - Parse arguments
    //  - Call function to parse data files
    //  - Call execution code


    //! EXAMPLE TEST CODE
    Graph<Location, Distance> *g;

    ifstream locationsFile("../data/Locations.csv");
    ifstream distancesFile("../data/Distances.csv");

    g = parseDataFiles(locationsFile, distancesFile);

    if (g == nullptr) {
        cout << "Nullptr" << endl;
        return 1;
    }

    cout << "Graph:" << endl;

    for (Vertex<Location, Distance> *v : g->getVertexSet()) {
        cout << v->getInfo().getCode() << ":" << endl;
        for (Edge<Location, Distance> *e : v->getOutgoing())
            cout << "  -> " << e->getDestination()->getInfo().getCode() << " d:" << e->getInfo().getDriving()
                 << " w:" << e->getInfo().getWalking() << endl;
        cout << endl;
    }
}
