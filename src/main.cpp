#include <fstream>
#include <iostream>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

/**
 * @brief Prints the program usage instructions.
 *
 * This function outputs to the standard output the correct usage of the program,
 * detailing the required command-line arguments for different modes:
 * - Interactive mode: Requires the locations and distances CSV files.
 * - Batch mode (console output): Requires the locations and distances CSV files along with an input text file.
 * - Batch mode (file output): Requires the locations and distances CSV files, an input text file, and an output text file.
 */
void printUsage() {
    cout << "Usage:" << endl;
    cout << "  ./program <locations.csv> <distances.csv>" << endl;
    cout << "    - Runs in interactive menu mode." << endl << endl;
    cout << "  ./program <locations.csv> <distances.csv> <input.txt>" << endl;
    cout << "    - Runs in batch mode, output to console." << endl << endl;
    cout << "  ./program <locations.csv> <distances.csv> <input.txt> <output.txt>" << endl;
    cout << "    - Runs in batch mode, output to file." << endl;
}




/**
 * @brief Entry point for the application.
 *
 * This function validates the number of command line arguments and opens the required input files.
 * It then attempts to parse these files to create a graph structure. Depending on the number of
 * command line arguments, the program will execute in one of the following modes:
 * - Menu mode (2 arguments): Allows the user to interact with the program via a menu system.
 * - Batch mode with screen output (3 arguments): Processes input from a file and outputs the result to the console.
 * - Batch mode with file output (4 arguments): Processes input from a file and writes the result to another file.
 *
 * In case of any errors—such as an incorrect number of arguments, failure to open any of the provided files,
 * or an unsuccessful parsing process—an appropriate error message is displayed and the program exits with a non-zero code.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line argument strings:
 *             - argv[1]: Path to the locations file.
 *             - argv[2]: Path to the distances file.
 *             - argv[3] (optional): Path to the input file for batch mode.
 *             - argv[4] (optional): Path to the output file for batch mode.
 *
 * @return int Returns 0 on successful execution; otherwise, returns a non-zero error code.
 */
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
        runMenuMode(graph);
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
