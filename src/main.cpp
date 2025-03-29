#include <fstream>
#include <iostream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

void printUsage() {
    cout << "Usage:" << endl;
    cout << "  ./program <locations.csv> <distances.csv>" << endl;
    cout << "    - Runs in interactive menu mode." << endl << endl;
    cout << "  ./program <locations.csv> <distances.csv> <input.txt>" << endl;
    cout << "    - Runs in batch mode, output to console." << endl << endl;
    cout << "  ./program <locations.csv> <distances.csv> <input.txt> <output.txt>" << endl;
    cout << "    - Runs in batch mode, output to file." << endl;
}

int main(int argc, char *argv[]) {
    // Check Number of Arguments
    if (argc < 3 || argc > 5) {
        cout << "Error: Invalid number of arguments." << endl;
        printUsage();
        return 1;
    }

    // Open Data Files
    ifstream locFile(argv[1]);
    ifstream distFile(argv[2]);

    if (!locFile.is_open() || !distFile.is_open()) {
        cout << "Error: Couldn't open data files." << endl;
        return 1;
    }

    // Make the graph
    Graph<Location, Distance> *graph = parseDataFiles(locFile, distFile);

    // Check if parsing was successful
    if (graph == nullptr) {
        cout << "Error: Couldn't parse correctly." << endl;
        printUsage();
        return 1;
    }

    // Decide hot to proceed
    switch (argc) {
    case 3: {
        // Two arguments: menu mode
        runMenu(graph);
        break;
    }
    case 4: {
        // Three arguments: batch mode, with output to screen
        ifstream inputFile(argv[3]);
        if (!inputFile.is_open()) {
            cout << "Error: Couldn't open input file." << endl;
            return 1;
        }
        runBatchMode(graph, inputFile, cout);
        break;
    }
    case 5: {
        // Four arguments: batch mode, with output to file
        ifstream inputFile(argv[3]);
        ofstream outputFile(argv[4]);
        if (!inputFile.is_open()) {
            cout << "Error: Couldn't open input file." << endl;
            return 1;
        }
        if (!outputFile.is_open()) {
            cout << "Error: Couldn't open output file." << endl;
            return 1;
        }
        runBatchMode(graph, inputFile, outputFile);
        cout << "All done!" << endl;
        break;
    }
    }

    return 0;
}
