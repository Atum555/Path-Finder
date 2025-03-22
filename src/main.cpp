#include <fstream>
#include <iostream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

void printUsage() {
    cout << "Usage:\n";
    cout << "  ./program <locations.csv> <distances.csv>\n";
    cout << "    - Runs in interactive menu mode.\n\n";
    cout << "  ./program <locations.csv> <distances.csv> <input.txt>\n";
    cout << "    - Runs in batch mode, output to console.\n\n";
    cout << "  ./program <locations.csv> <distances.csv> <input.txt> <output.txt>\n";
    cout << "    - Runs in batch mode, output to file.\n";
}

int main(int argc, char *argv[]) {
    // Check Number of Arguments
    if (argc < 3 || argc > 5) {
        cout << "Error: Invalid number of arguments.\n";
        printUsage();
        return 1;
    }

    // Open Data Files
    ifstream locFile(argv[1]);
    ifstream distFile(argv[2]);

    if (!locFile.is_open() || !distFile.is_open()) {
        cout << "Error: Couldn't open the files.\n";
        return 1;
    }

    // Make the graph
    Graph<Location, Distance> *graph = parseDataFiles(locFile, distFile);

    // Check if parsing was successful
    if (graph == nullptr) {
        cout << "Error: Couldn't parse correctly.\n";
        printUsage();
        return 1;
    }

    // Decide hot to proceed
    switch (argc) {
    case 3:
        // Two arguments: menu mode
        runMenu(graph);
        break;

    case 4:
        // Three arguments: batch mode, with output to screen
        runBatchMode(graph, argv[3]);
        break;

    case 5:
        // Four arguments: batch mode, with output to file
        runBatchMode(graph, argv[3], argv[4]);
        break;
    }

    return 0;
}
