#include <chrono>
#include <thread>
#include <vector>

#include "data_structs/distance.h"
#include "data_structs/graph.h"
#include "data_structs/location.h"
#include "functions.h"

using namespace std;

enum RouteType {
    Walking,
    Driving,
    Combined,
};

enum Color { Clear, Red, Cyan, Green, Grey, Yellow };

/**
 * @brief Clears the terminal screen.
 *
 * This function sends ANSI escape sequences to clear the entire terminal
 * output and resets the cursor position to the top-left corner.
 */
void clearScreen() { cout << "\033[2J\033[1;1H"; }

/**
 * @brief Clears the last displayed line in the console.
 *
 * This function moves the cursor up one line, returns to the beginning of that line,
 * and erases all characters in it. It is useful for updating or removing the most
 * recently printed line from the terminal.
 */
void clearLastLine() {
    // Move cursor up one line, return to beginning, and clear line
    cout << "\033[A\r\033[K";
}

/**
 * @brief Sets the terminal text color.
 *
 * This function outputs an ANSI escape sequence to change the terminal's text 
 * color based on the provided Color enumerator. Each supported color maps to 
 * a specific ANSI code. If an unsupported color is provided, the default color 
 * is restored.
 *
 * Supported Colors:
 * - Color::Red:    Sets the text color to red.
 * - Color::Cyan:   Sets the text color to cyan.
 * - Color::Green:  Sets the text color to green.
 * - Color::Grey:   Sets the text color to grey.
 * - Color::Yellow: Sets the text color to yellow.
 *
 * @param color The desired text color as defined in the Color enum.
 */
void setScreenColor(Color color) {
    switch (color) {
    case Color::Red   : cout << "\033[31m"; break;
    case Color::Cyan  : cout << "\033[36m"; break;
    case Color::Green : cout << "\033[32m"; break;
    case Color::Grey  : cout << "\033[90m"; break;
    case Color::Yellow: cout << "\033[33m"; break;

    default: cout << "\033[0m"; break;
    }
}

/**
 * @brief Reads a line of text from an input stream.
 *
 * This function attempts to read a line from the provided input stream and
 * stores it in the given string. If reading fails (e.g., due to end-of-file or an error),
 * the function terminates the program using exit(EXIT_SUCCESS).
 *
 * @param in The input stream to read from.
 * @param line The string where the read line will be stored.
 */
void readLine(istream &in, string &line) {
    if (not getline(in, line)) exit(EXIT_SUCCESS);
}

/**
 * @brief Displays a progress bar for calculating paths.
 *
 * This function simulates the progress of a path calculation by printing a multi-stage
 * progress bar to the console. It displays several intermediate stages (25%, 50%, 75%, 99%, and 100%)
 * with corresponding visual indicators and colors. At each stage, the function delays for a short
 * period using std::this_thread::sleep_for and clears the previous lines to update the visual progress.
 *
 * The function uses different colors (Cyan, Red, Yellow, Green) to emphasize different stages of the progress.
 *
 * @note This function does not return any value.
 */
void printCalculating() {
    setScreenColor(Color::Cyan);
    cout << endl << "-- Calculating paths --" << endl;
    setScreenColor(Color::Clear);

    setScreenColor(Color::Red);
    cout << u8"|█████               |" << endl;
    setScreenColor(Color::Green);
    cout << "25%" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearLastLine();
    clearLastLine();

    setScreenColor(Color::Yellow);
    cout << u8"|██████████          |" << endl;
    setScreenColor(Color::Green);
    cout << "50%" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearLastLine();
    clearLastLine();

    setScreenColor(Color::Green);
    cout << u8"|███████████████     |" << endl;
    setScreenColor(Color::Green);
    cout << "75%" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearLastLine();
    clearLastLine();

    setScreenColor(Color::Cyan);
    cout << u8"|███████████████████ |" << endl;
    setScreenColor(Color::Green);
    cout << "99%" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearLastLine();
    clearLastLine();

    setScreenColor(Color::Cyan);
    cout << u8"|████████████████████|" << endl;
    setScreenColor(Color::Green);
    cout << "100%" << endl;
    setScreenColor(Color::Clear);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    clearLastLine();
    clearLastLine();
    clearLastLine();
}

/**
 * @brief Prints the details of a given path, including its number, node count, node sequence, and total distance.
 *
 * This function outputs the path number, the count of nodes present in the path, the sequence of node IDs
 * (formatted with arrows between them, except after the last node), and the total distance of the path.
 * It uses different screen colors to highlight various parts of the output for better readability.
 *
 * @param path The path object that contains the nodes and the distance.
 * @param pathNumber An identifier for the path, used in the output to denote the path number.
 */
void printPath(const Path &path, int pathNumber) {
    setScreenColor(Color::Cyan);
    cout << endl << "Path number: " << pathNumber << endl;
    setScreenColor(Color::Clear);

    cout << "Path has ";
    setScreenColor(Color::Green);
    cout << path.nodes.size();
    setScreenColor(Color::Clear);
    cout << " nodes." << endl;

    cout << "Path: ";
    for (auto it = path.nodes.begin(); it != path.nodes.end(); it++) {
        if (it == path.nodes.end() - 1) cout << (*it)->getInfo().getId();
        else {
            cout << (*it)->getInfo().getId();
            setScreenColor(Color::Grey);
            cout << " -> ";
            setScreenColor(Color::Clear);
        }
    }
    cout << endl;
    cout << "Path Distance: ";
    setScreenColor(Color::Green);
    cout << path.distance << endl;
    setScreenColor(Color::Clear);
}

/**
 * @brief Outputs detailed information about a given pair of paths.
 *
 * This function takes a pair of Path objects representing two segments of a journey.
 * The first Path is treated as the driving route, and the second as the walking route.
 * It calculates and prints the total number of nodes from both segments. Additionally,
 * it displays the sequence of node IDs for the driving route, followed by a marker
 * indicating the transition (i.e., "(Park)"), and then the node IDs for the walking route.
 *
 * Each segment of the node sequence is printed with visual cues by changing the console's
 * text color, enhancing readability. The function also outputs the distances associated
 * with the driving and walking segments, as well as the total distance (the sum of both).
 *
 * @param path A constant reference to a pair of Path objects where:
 *             - path.first corresponds to the driving segment.
 *             - path.second corresponds to the walking segment.
 */
void printPath(const pair<Path, Path> &path) {
    cout << "Path has ";
    setScreenColor(Color::Green);
    cout << path.first.nodes.size() + path.second.nodes.size();
    setScreenColor(Color::Clear);
    cout << " nodes." << endl;

    cout << "Path: ";
    for (auto it = path.first.nodes.begin(); it != path.first.nodes.end(); it++) {
        if (it == path.first.nodes.end() - 1) cout << (*it)->getInfo().getId();
        else {
            cout << (*it)->getInfo().getId();
            setScreenColor(Color::Grey);
            cout << " -> ";
            setScreenColor(Color::Clear);
        }
    }

    setScreenColor(Color::Grey);
    cout << " (Park) -> ";
    setScreenColor(Color::Clear);


    for (auto it = path.second.nodes.begin() + 1; it != path.second.nodes.end(); it++) {
        if (it == path.second.nodes.end() - 1) cout << (*it)->getInfo().getId();
        else {
            cout << (*it)->getInfo().getId();
            setScreenColor(Color::Grey);
            cout << " -> ";
            setScreenColor(Color::Clear);
        }
    }
    cout << endl;

    cout << "Driving Distance: ";
    setScreenColor(Color::Green);
    cout << path.first.distance << endl;
    setScreenColor(Color::Clear);

    cout << "Walking Distance: ";
    setScreenColor(Color::Green);
    cout << path.second.distance << endl;
    setScreenColor(Color::Clear);

    cout << "Total Distance: ";
    setScreenColor(Color::Green);
    cout << path.first.distance + path.second.distance << endl;
    setScreenColor(Color::Clear);
}

/**
 * @brief Displays and manages the interactive route planning menu.
 *
 * This function implements a continuous loop that presents an interactive menu
 * for planning routes on a graph by configuring various parameters such as:
 *   - Route type selection (Walking, Driving, or Combined Driving + Walking).
 *   - Specifying nodes to include in the route by their IDs or codes.
 *   - For Combined routes, selecting source and destination nodes.
 *   - Entering a maximum walking time constraint for combined routes.
 *   - Specifying nodes and segments to avoid during path computation.
 *   - Inputting the maximum number of distinct paths to display.
 *
 * The function uses helper routines for:
 *   - Reading and processing user input.
 *   - Displaying colored text and clearing screen lines.
 *   - Finding vertices and edges in the provided graph.
 *   - Calculating and printing the resulting paths based on the given criteria.
 *
 * User interactions allow for clearing lists, quitting to the main menu, or exiting
 * the application, ensuring a flexible and dynamic route planning experience.
 *
 * @param graph A pointer to a Graph object containing nodes and edges used for route planning.
 */
void runMenuMode(Graph<Location, Distance> *graph) {
    while (true) {
        clearScreen();

        //! Route Type
        RouteType routeType;
        {
            // Title
            setScreenColor(Color::Cyan);
            cout << "--- Route Planning Menu ---" << endl;
            setScreenColor(Color::Clear);

            // Show Options
            cout << "Select route type:" << endl;
            cout << "1. Walking" << endl;
            cout << "2. Driving" << endl;
            cout << "3. Combined (Driving + Walking)" << endl << endl;

            // Get input
            bool repeat;
            do {
                repeat = false;
                cout << "Enter your choice (or 'q' to quit): ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") return;
                else if (input == "1") routeType = RouteType::Walking;
                else if (input == "2") routeType = RouteType::Driving;
                else if (input == "3") routeType = RouteType::Combined;

                else {
                    clearLastLine();
                    repeat = true;
                }
            } while (repeat);

            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();

            cout << "Route type: ";
            setScreenColor(Color::Green);
            switch (routeType) {
            case RouteType::Driving : cout << "Driving"; break;
            case RouteType::Walking : cout << "Walking"; break;
            case RouteType::Combined: cout << "Driving + Walking"; break;
            }
            cout << endl;
            setScreenColor(Color::Clear);
        }

        //! Include Nodes
        vector<Vertex<Location, Distance> *> nodesToConnect;
        if (routeType == RouteType::Walking or routeType == RouteType::Driving) {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Path Nodes --" << endl;
            setScreenColor(Color::Clear);

            // Show Options
            cout << "Enter nodes you wish to add to the path." << endl;
            cout << "You can clear the node list (c)." << endl;
            cout << "You can go back to the menu (q)." << endl << endl;

            // Get input
            bool goBackToMenu = false;
            do {
                cout << "Path currently has ";
                setScreenColor(Color::Green);
                cout << nodesToConnect.size();
                setScreenColor(Color::Clear);
                cout << " nodes." << endl;

                cout << "Path: ";
                for (auto it = nodesToConnect.begin(); it != nodesToConnect.end(); it++) {
                    if (it == nodesToConnect.end() - 1) cout << (*it)->getInfo().getId();
                    else {
                        cout << (*it)->getInfo().getId();
                        setScreenColor(Color::Grey);
                        cout << " -> ";
                        setScreenColor(Color::Clear);
                    }
                }
                cout << endl;

                cout << "Enter node id or code: ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }
                if (input == "c" or input == "C") nodesToConnect.clear();
                if (not input.size() and nodesToConnect.size() >= 2) break;

                Vertex<Location, Distance> *v = findVertex(graph, input);
                if (nodesToConnect.size()) {
                    if (v != nodesToConnect.back() and v != nullptr) nodesToConnect.push_back(v);
                } else if (v != nullptr) nodesToConnect.push_back(v);

                clearLastLine();
                clearLastLine();
                clearLastLine();
            } while (true);
            if (goBackToMenu) continue;

            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();

            cout << "Path has ";
            setScreenColor(Color::Green);
            cout << nodesToConnect.size();
            setScreenColor(Color::Clear);
            cout << " nodes." << endl;

            cout << "Path: ";
            for (auto it = nodesToConnect.begin(); it != nodesToConnect.end(); it++) {
                if (it == nodesToConnect.end() - 1) cout << (*it)->getInfo().getId();
                else {
                    cout << (*it)->getInfo().getId();
                    setScreenColor(Color::Grey);
                    cout << " -> ";
                    setScreenColor(Color::Clear);
                }
            }
            cout << endl;
        }

        //! Source Node (Combined only)
        Vertex<Location, Distance> *sourceNode;
        if (routeType == RouteType::Combined) {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Source Node --" << endl;
            setScreenColor(Color::Clear);

            // Get input
            bool repeat;
            bool goBackToMenu = false;
            do {
                repeat = false;
                cout << "Enter node id or code: ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }

                sourceNode = findVertex(graph, input);
                if (sourceNode == nullptr) {
                    repeat = true;
                    clearLastLine();
                }
            } while (repeat);
            if (goBackToMenu) continue;

            clearLastLine();
            cout << "Source node: ";
            setScreenColor(Color::Green);
            cout << sourceNode->getInfo().getId();
            setScreenColor(Color::Clear);
            cout << endl;
        }

        //! Destination Node (Combined only)
        Vertex<Location, Distance> *destinationNode;
        if (routeType == RouteType::Combined) {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Destination Node --" << endl;
            setScreenColor(Color::Clear);

            // Get input
            bool repeat;
            bool goBackToMenu = false;
            do {
                repeat = false;
                cout << "Enter node id or code: ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }

                destinationNode = findVertex(graph, input);
                if (destinationNode == nullptr or destinationNode == sourceNode) {
                    repeat = true;
                    clearLastLine();
                }
            } while (repeat);
            if (goBackToMenu) continue;

            clearLastLine();
            cout << "Destination node: ";
            setScreenColor(Color::Green);
            cout << destinationNode->getInfo().getId();
            setScreenColor(Color::Clear);
            cout << endl;
        }

        //! Max Walk Time
        double maxWalkTime;
        if (routeType == RouteType::Combined) {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Max Walking Time --" << endl;
            setScreenColor(Color::Clear);

            // Get input
            bool repeat;
            bool goBackToMenu = false;
            do {
                repeat = false;
                cout << "Enter max walking time: ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }

                try {
                    maxWalkTime = stoi(input);
                } catch (const exception &e) {
                    repeat = true;
                    clearLastLine();
                }
            } while (repeat);
            if (goBackToMenu) continue;

            clearLastLine();
            cout << "Max walking time set to: " << maxWalkTime << endl;
        }

        //! Avoid Nodes
        vector<Vertex<Location, Distance> *> avoidNodes;
        {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Nodes to Avoid --" << endl;
            setScreenColor(Color::Clear);

            // Show Options
            cout << "Enter nodes you wish to avoid." << endl;
            cout << "You can clear the node list (c)." << endl;
            cout << "You can go back to the menu (q)." << endl << endl;

            // Get input
            bool goBackToMenu = false;
            do {
                cout << "List currently has ";
                setScreenColor(Color::Green);
                cout << avoidNodes.size();
                setScreenColor(Color::Clear);
                cout << " nodes." << endl;

                cout << "List: ";
                for (auto it = avoidNodes.begin(); it != avoidNodes.end(); it++) {
                    if (it == avoidNodes.end() - 1) cout << (*it)->getInfo().getId();
                    else {
                        cout << (*it)->getInfo().getId();
                        setScreenColor(Color::Grey);
                        cout << ", ";
                        setScreenColor(Color::Clear);
                    }
                }
                cout << endl;

                cout << "Enter node id or code: ";
                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }
                if (input == "c" or input == "C") avoidNodes.clear();
                if (not input.size()) break;

                Vertex<Location, Distance> *v = findVertex(graph, input);
                if (v != nullptr) avoidNodes.push_back(v);

                clearLastLine();
                clearLastLine();
                clearLastLine();

            } while (true);
            if (goBackToMenu) continue;

            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();

            cout << "List has ";
            setScreenColor(Color::Green);
            cout << avoidNodes.size();
            setScreenColor(Color::Clear);
            cout << " nodes." << endl;

            cout << "List: ";
            for (auto it = avoidNodes.begin(); it != avoidNodes.end(); it++) {
                if (it == avoidNodes.end() - 1) cout << (*it)->getInfo().getId();
                else {
                    cout << (*it)->getInfo().getId();
                    setScreenColor(Color::Grey);
                    cout << ", ";
                    setScreenColor(Color::Clear);
                }
            }
            cout << endl;
        }

        //! Avoid Segments
        vector<Edge<Location, Distance> *> avoidSegments;
        {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Segments to Avoid --" << endl;
            setScreenColor(Color::Clear);

            // Show Options
            cout << "Enter Segments you wish to avoid." << endl;
            cout << "You can clear the node list (c)." << endl;
            cout << "You can go back to the menu (q)." << endl << endl;

            // Get input
            bool goBackToMenu = false;
            do {
                cout << "List currently has ";
                setScreenColor(Color::Green);
                cout << avoidSegments.size();
                setScreenColor(Color::Clear);
                cout << " segments." << endl;

                cout << "List: ";
                for (auto it = avoidSegments.begin(); it != avoidSegments.end(); it++) {
                    setScreenColor(Color::Grey);
                    cout << "(";
                    setScreenColor(Color::Clear);
                    cout << (*it)->getOrigin()->getInfo().getId() << " -> "
                         << (*it)->getDestination()->getInfo().getId();

                    setScreenColor(Color::Grey);
                    cout << ")";
                    setScreenColor(Color::Clear);

                    if (it != avoidSegments.end() - 1) {
                        setScreenColor(Color::Grey);
                        cout << ", ";
                        setScreenColor(Color::Clear);
                    }
                }
                cout << endl;

                //! First node
                cout << "Enter first node id or code: ";
                string first;
                readLine(cin, first);
                trim(first);

                // Check input
                if (first == "q" or first == "Q") {
                    goBackToMenu = true;
                    break;
                }
                if (first == "c" or first == "C") {
                    avoidSegments.clear();
                    clearLastLine();
                    clearLastLine();
                    clearLastLine();
                    continue;
                }
                if (not first.size()) break;

                cout << "Enter second node id or code: ";
                string second;
                readLine(cin, second);
                trim(second);

                // Check input
                if (second == "q" or second == "Q") {
                    goBackToMenu = true;
                    break;
                }
                if (second == "c" or second == "C") {
                    avoidSegments.clear();
                    clearLastLine();
                    clearLastLine();
                    clearLastLine();
                    clearLastLine();
                    continue;
                }
                if (not second.size()) {
                    clearLastLine();
                    clearLastLine();
                    clearLastLine();
                    clearLastLine();
                    continue;
                }

                Edge<Location, Distance> *e = findEdge(graph, first, second);
                if (e != nullptr) avoidSegments.push_back(e);

                clearLastLine();
                clearLastLine();
                clearLastLine();
                clearLastLine();
            } while (true);
            if (goBackToMenu) continue;

            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();
            clearLastLine();

            cout << "List has ";
            setScreenColor(Color::Green);
            cout << avoidSegments.size();
            setScreenColor(Color::Clear);
            cout << " segments." << endl;

            cout << "List: ";
            for (auto it = avoidSegments.begin(); it != avoidSegments.end(); it++) {
                setScreenColor(Color::Grey);
                cout << "(";
                setScreenColor(Color::Clear);
                cout << (*it)->getOrigin()->getInfo().getId() << " -> " << (*it)->getDestination()->getInfo().getId();

                setScreenColor(Color::Grey);
                cout << ")";
                setScreenColor(Color::Clear);

                if (it != avoidSegments.end() - 1) {
                    setScreenColor(Color::Grey);
                    cout << ", ";
                    setScreenColor(Color::Clear);
                }
            }
            cout << endl;
        }

        //! Number of Paths
        int numberOfPaths;
        if (routeType == RouteType::Walking or routeType == RouteType::Driving) {
            // Title
            cout << endl;
            setScreenColor(Color::Cyan);
            cout << "-- Number of Paths --" << endl;
            setScreenColor(Color::Clear);
            cout << "Enter 0 to see all paths." << endl;

            // Get input
            bool repeat;
            bool goBackToMenu = false;
            do {
                repeat = false;
                cout << "Enter the max number of distinct paths you want to see: ";

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q") {
                    goBackToMenu = true;
                    break;
                }
                if (input.empty()) {
                    numberOfPaths = 1;
                    break;
                }

                try {
                    numberOfPaths = stoi(input);
                } catch (const exception &e) {
                    repeat = true;
                    clearLastLine();
                }
            } while (repeat);
            if (goBackToMenu) continue;

            clearLastLine();
            clearLastLine();
            cout << "Number of distinct paths set to: ";
            setScreenColor(Color::Green);
            if (numberOfPaths == 0) cout << "All" << endl;
            else cout << numberOfPaths << endl;
            setScreenColor(Color::Clear);
        }

        //! Calculating
        printCalculating();

        { //! Results
            // Driving or Walking
            if (routeType == RouteType::Driving or routeType == RouteType::Walking) {
                setScreenColor(Color::Red);
                cout << "-- Paths --" << endl;
                setScreenColor(Color::Clear);

                vector<Path> paths;
                Path         path;
                bool         driving = (routeType == RouteType::Driving) ? true : false;

                // Calculate paths
                do {
                    path = findShortestPathMultipleNodes(graph, nodesToConnect, avoidNodes, avoidSegments, driving);
                    if (not paths.empty())
                        if (paths.back() == path) break;

                    if (not path.nodes.empty()) paths.push_back(path);
                    if (path.nodes.size() > 2)
                        avoidNodes.insert(avoidNodes.end(), path.nodes.begin() + 1, path.nodes.end() - 1);
                } while (not path.nodes.empty());

                // Print paths
                if (paths.empty()) {
                    setScreenColor(Color::Red);
                    cout << "No paths found!" << endl;
                    setScreenColor(Color::Clear);
                } else {
                    auto it         = paths.begin();
                    int  pathNumber = 1;

                    cout << "Paths found: ";
                    setScreenColor(Color::Green);
                    cout << paths.size() << endl;
                    setScreenColor(Color::Clear);

                    while (it != paths.end() and (pathNumber <= numberOfPaths or numberOfPaths == 0)) {
                        // Print Path result
                        printPath((*(it)), pathNumber++);
                        it++;
                    }
                }
            }

            // Driving + Walking
            else {
                setScreenColor(Color::Red);
                cout << "-- Paths --" << endl;
                setScreenColor(Color::Clear);

                pair<Path, Path> path = findPathForDrivingWalking(
                    graph, sourceNode, destinationNode, avoidNodes, avoidSegments, maxWalkTime, false
                );

                // Path exists with max walk time
                if ((not path.first.nodes.empty()) and (not path.second.nodes.empty())) printPath(path);

                // Path does not exist with max walk time
                else {
                    path = findPathForDrivingWalking(
                        graph, sourceNode, destinationNode, avoidNodes, avoidSegments, INF, false
                    );

                    // No path exists independent of max walk time
                    if (path.first.nodes.empty() or path.second.nodes.empty()) {
                        setScreenColor(Color::Red);
                        cout << "No path exists independent of max walk time!" << endl;
                        setScreenColor(Color::Clear);
                    }

                    // Path with a larger walking time exists
                    else {
                        setScreenColor(Color::Red);
                        cout << "No path exists with a max walk time of " << maxWalkTime << " minutes." << endl;
                        setScreenColor(Color::Clear);

                        cout << endl;

                        // Get input
                        bool goBackToMenu       = false;
                        bool printApproximation = false;
                        do {
                            setScreenColor(Color::Cyan);
                            cout << "Do you want to see the best and second best possible paths? (y/n) ";
                            setScreenColor(Color::Clear);

                            string input;
                            readLine(cin, input);
                            trim(input);

                            // Check input
                            if (input == "q" or input == "Q") {
                                goBackToMenu = true;
                                break;
                            }
                            if (input == "n" or input == "N") break;

                            if (input == "s" or input == "S" or input == "y" or input == "Y") {
                                printApproximation = true;
                                break;
                            }
                            clearLastLine();
                        } while (true);
                        if (goBackToMenu) continue;

                        clearLastLine();

                        if (printApproximation) {
                            clearLastLine();
                            clearLastLine();

                            setScreenColor(Color::Cyan);
                            cout << "Best possible path" << endl;
                            setScreenColor(Color::Clear);
                            printPath(path);
                            cout << endl;

                            path = findPathForDrivingWalking(
                                graph, sourceNode, destinationNode, avoidNodes, avoidSegments, INF, true
                            );

                            if ((not path.first.nodes.empty()) and (not path.second.nodes.empty())) {
                                setScreenColor(Color::Cyan);
                                cout << "Second best possible path" << endl;
                                setScreenColor(Color::Clear);
                                printPath(path);
                            } else {
                                setScreenColor(Color::Yellow);
                                cout << "No second best possible path exists!" << endl;
                                setScreenColor(Color::Yellow);
                            }
                        }
                    }
                }
            }
        }

        { //! Repeat
            cout << endl;

            // Get input
            do {
                setScreenColor(Color::Cyan);
                cout << "Do you want to find more paths? (y/n) ";
                setScreenColor(Color::Clear);

                string input;
                readLine(cin, input);
                trim(input);

                // Check input
                if (input == "q" or input == "Q" or input == "n" or input == "N") {
                    clearLastLine();
                    exit(EXIT_SUCCESS);
                }

                if (input == "s" or input == "S" or input == "y" or input == "Y") break;

                clearLastLine();
            } while (true);
        }
    }
}
