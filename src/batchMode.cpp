#include <algorithm>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "functions.h"

using namespace std;

enum PathMode {
    driving,
    driving_walking,
};

size_t skipWhitespace(const string &str, size_t pos) {
    while (pos < str.size() && isspace(static_cast<unsigned char>(str[pos]))) ++pos;
    return pos;
}

vector<string> readAllLines(ifstream &file) {
    vector<string> lines;
    string         line;

    while (getline(file, line)) lines.push_back(trim(line));

    if (!file.eof()) throw new runtime_error("An error outside of this program occurred reading the input file!");

    return lines;
}

vector<vector<string>> splitLinesIntoGroups(const vector<string> &lines) {
    vector<vector<string>> sections;
    vector<string>         currentSection;

    for (const string &line : lines) {
        if (line.empty()) {
            // If we hit an empty line and there is a current section accumulated, add it to sections.
            if (not currentSection.empty()) {
                sections.push_back(currentSection);
                currentSection.clear();
            }
        } else currentSection.push_back(line);
    }

    if (not currentSection.empty()) sections.push_back(currentSection);

    return sections;
}

void printInvalidLine(ostream &out, const string &line, const string &expected) {
    out << "Invalid line!" << endl << "-> \"" << line << "\"" << endl << "Expected \"" << expected << "\"" << endl;
}

void printInvalidIdentifier(ostream &out, const string &value) {
    out << "Invalid node identifier!" << endl << "-> \"" << value << "\"" << endl;
}

void printInvalidIdentifier(ostream &out, const pair<string, string> &value) {
    out << "Invalid node identifier!" << endl;
    out << "-> \"" << value.first << "\" or \"" << value.second << "\"" << endl;
    out << "Or the edge does not exist..." << endl;
}

bool validateTestName(const string &line) { return line[0] == '#'; }

bool parseKeyValue(const string &line, string &key, string &value) {
    if (count(line.begin(), line.end(), ':') != 1) return false;
    size_t pos = line.find(':');
    key        = line.substr(0, pos);
    value      = line.substr(pos + 1);
    trim(key);
    trim(value);
    return true;
}

bool parseAvoidNodes(
    Graph<Location, Distance> *graph, vector<Vertex<Location, Distance> *> &avoidNodes, const string &line,
    const string &identifiers, ostream &out
) {
    string                      token;
    Vertex<Location, Distance> *vertex;
    vector<string>              nodeIdentifiers;
    istringstream               stream(identifiers);

    while (getline(stream, token, ',')) nodeIdentifiers.push_back(token);

    for (const string &identifier : nodeIdentifiers) {
        vertex = findVertex(graph, identifier);
        avoidNodes.push_back(vertex);
        if (vertex == nullptr) {
            out << "Error in line:" << endl;
            out << "-> \"" << line << "\"" << endl;
            printInvalidIdentifier(out, identifier);
            return false;
        }
    }

    return true;
}

bool parseAvoidSegmentes(
    Graph<Location, Distance> *graph, vector<Edge<Location, Distance> *> &avoidSegments, const string &line,
    const string &identifiers, ostream &out
) {
    vector<pair<string, string>> nodeIdentifierPairs;
    Edge<Location, Distance>    *edge;

    { //! Regex
        // Regular expression pattern:
        // \(\s*      matches an opening parenthesis with optional whitespace
        // ([^,]+?)   lazily matches one or more characters that are not a comma (first id)
        // \s*,\s*    matches a comma surrounded by optional whitespace
        // ([^,]+?)   lazily matches one or more characters that are not a comma (second id)
        // \s*\)      matches optional whitespace followed by a closing parenthesis
        regex        pattern(R"(\(\s*(.*?)\s*,\s*(.*?)\s*\))");
        smatch       match;
        size_t       pos = 0;
        const size_t len = identifiers.length();
        while (pos < len) {
            pos              = skipWhitespace(identifiers, pos);
            string remaining = identifiers.substr(pos);

            // Match regex
            if (regex_search(remaining, match, pattern) && match.position() == 0) {
                nodeIdentifierPairs.emplace_back(match[1].str(), match[2].str());
                pos += match.length();
                pos  = skipWhitespace(identifiers, pos);
                if (pos < len) {
                    if (identifiers[pos] == ',') ++pos; // Skip the comma.
                    else {
                        printInvalidLine(out, line, "AvoidSegments:[(<id>/<code>,<id>/<code>),...]");
                        return false;
                    }
                }
            }

            // Failed to match regex
            else {
                printInvalidLine(out, line, "AvoidSegments:[(<id>/<code>,<id>/<code>),...]");
                return false;
            }
        }
    }

    for (const pair<string, string> &identifier : nodeIdentifierPairs) {
        edge = findEdge(graph, identifier.first, identifier.second);
        avoidSegments.push_back(edge);
        if (edge == nullptr) {
            out << "Error in line:" << endl;
            out << "-> \"" << line << "\"" << endl;
            printInvalidIdentifier(out, identifier);
            return false;
        }
    }
    return true;
}

void runTest(const vector<string> &test, Graph<Location, Distance> *graph, ostream &out) {
    // Sanity check
    if (not test.size()) return;

    int    numberOfLines = test.size();
    int    lineNumber    = 0;
    string currentLine;

    { //! Test Name
        string testName;
        currentLine = test[lineNumber++];
        testName    = currentLine;

        // Check test name
        if (not validateTestName(testName)) {
            out << "Invalid Test Name!" << endl;
            out << "-> \"" << testName << "\"" << endl;
            out << "First line of a test is the name and must start with '#'." << endl;
            return;
        }
        out << testName << endl;
    }

    // Util values
    string key, value;

    //! Mode
    PathMode pathMode;
    {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No mode was given!" << endl;
            out << "-> Mode:[driving/driving-walking]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "Mode:[driving/driving-walking]");
            return;
        }
        if (key != "Mode" or (value != "driving" and value != "driving-walking")) {
            printInvalidLine(out, currentLine, "Mode:[driving/driving-walking]");
            return;
        }
        pathMode = (value == "driving") ? PathMode::driving : PathMode::driving_walking;
    }

    //! Source Node
    Vertex<Location, Distance> *sourceNode;
    {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No source node was given!" << endl;
            out << "-> Source:[<id>/<code>]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "Source:[<id>/<code>]");
            return;
        }
        if (key != "Source") {
            printInvalidLine(out, currentLine, "Source:[<id>/<code>]");
            return;
        }

        sourceNode = findVertex(graph, value);
        if (sourceNode == nullptr) {
            out << "Error in line:" << endl;
            out << "-> \"" << currentLine << "\"" << endl;
            printInvalidIdentifier(out, value);
            return;
        }
    }

    //! Destination Node
    Vertex<Location, Distance> *destinationNode;
    {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No destination node was given!" << endl;
            out << "-> Destination:[<id>/<code>]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "Destination:[<id>/<code>]");
            return;
        }
        if (key != "Destination") {
            printInvalidLine(out, currentLine, "Destination:[<id>/<code>]");
            return;
        }

        destinationNode = findVertex(graph, value);
        if (destinationNode == nullptr) {
            out << "Error in line:" << endl;
            out << "-> \"" << currentLine << "\"" << endl;
            printInvalidIdentifier(out, value);
            return;
        }
    }

    //! Simple Driving
    // Driving mode with only source and destination nodes
    // Output best and alternative driving routes
    if (pathMode == PathMode::driving and lineNumber == numberOfLines) {
        // Output Information
        out << "Source:" << sourceNode->getInfo().getId() << endl;
        out << "Destination:" << destinationNode->getInfo().getId() << endl;

        // Calculate best route
        Path bestRoute = findShortestPath(graph, sourceNode, destinationNode, {}, {}, true);

        // Calculate alternative route
        vector<Vertex<Location, Distance> *> nodesToAvoid;
        if (bestRoute.nodes.size() > 2)
            nodesToAvoid.insert(nodesToAvoid.end(), bestRoute.nodes.begin() + 1, bestRoute.nodes.end() - 1);
        Path alternativeRoute = findShortestPath(graph, sourceNode, destinationNode, nodesToAvoid, {}, true);

        // Output best route
        out << "BestDrivingRoute:";
        if (bestRoute.nodes.size()) {
            for (auto it = bestRoute.nodes.begin(); it != bestRoute.nodes.end(); it++) {
                if (it == bestRoute.nodes.end() - 1) out << (*it)->getInfo().getId();
                else out << (*it)->getInfo().getId() << ",";
            }
            out << "(" << bestRoute.distance << ")" << endl;
        } else out << "none" << endl;

        // Output alternative route
        out << "AlternativeDrivingRoute:";
        if (alternativeRoute.nodes.size()) {
            for (auto it = alternativeRoute.nodes.begin(); it != alternativeRoute.nodes.end(); it++) {
                if (it == alternativeRoute.nodes.end() - 1) out << (*it)->getInfo().getId();
                else out << (*it)->getInfo().getId() << ",";
            }
            out << "(" << alternativeRoute.distance << ")" << endl;
        } else out << "none" << endl;
        return;
    }

    //! Max Walk Time
    double maxWalkTime;
    if (pathMode == PathMode::driving_walking) {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No max walk time was given!" << endl;
            out << "-> MaxWalkTime:[<time>]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "MaxWalkTime:[<time>]");
            return;
        }
        if (key != "MaxWalkTime") {
            printInvalidLine(out, currentLine, "MaxWalkTime:[<time>]");
            return;
        }

        try {
            maxWalkTime = stoi(trim(value));
        } catch (const exception &e) {
            printInvalidLine(out, currentLine, "MaxWalkTime:[<time>]");
            return;
        }
    }

    //! Avoid Nodes
    vector<Vertex<Location, Distance> *> avoidNodes;
    {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No avoid nodes list was given!" << endl;
            out << "-> AvoidNodes:[<id>/<code>,...]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "AvoidNodes:[<id>/<code>,...]");
            return;
        }
        if (key != "AvoidNodes") {
            printInvalidLine(out, currentLine, "AvoidNodes:[<id>/<code>,...]");
            return;
        }
        if (not parseAvoidNodes(graph, avoidNodes, currentLine, value, out)) return;
    }

    //! Avoid Segments
    vector<Edge<Location, Distance> *> avoidSegments;
    {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No avoid segments list was given!" << endl;
            out << "-> AvoidSegments:[(<id>/<code>,<id>/<code>),...]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "AvoidSegments:[(<id>/<code>,<id>/<code>),...]");
            return;
        }
        if (key != "AvoidSegments") {
            printInvalidLine(out, currentLine, "AvoidSegments:[(<id>/<code>,<id>/<code>),...]");
            return;
        }
        if (not parseAvoidSegmentes(graph, avoidSegments, currentLine, value, out)) return;
    }

    //! Include Node
    Vertex<Location, Distance> *includeNode = nullptr;
    if (pathMode == PathMode::driving) {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No include node was given!" << endl;
            out << "-> IncludeNode:[<id>/<code>]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "IncludeNode:[<id>/<code>]");
            return;
        }
        if (key != "IncludeNode") {
            printInvalidLine(out, currentLine, "IncludeNode:[<id>/<code>]");
            return;
        }
        if (value.size()) {
            includeNode = findVertex(graph, value);
            if (includeNode == nullptr) {
                out << "Error in line:" << endl;
                out << "-> \"" << currentLine << "\"" << endl;
                printInvalidIdentifier(out, value);
                return;
            }
        }
    }

    //! Restricted Driving
    // Driving mode with avoid nodes, avoid segments and include node
    // Output restricted driving route
    if (pathMode == PathMode::driving and lineNumber == numberOfLines) {
        // Output Information
        out << "Source:" << sourceNode->getInfo().getId() << endl;
        out << "Destination:" << destinationNode->getInfo().getId() << endl;

        // Calculate nodes to connect
        vector<Vertex<Location, Distance> *> nodesToConnect;
        nodesToConnect.push_back(sourceNode);
        if (includeNode != nullptr) nodesToConnect.push_back(includeNode);
        nodesToConnect.push_back(destinationNode);

        // Calculate path
        Path path = findShortestPathMultipleNodes(graph, nodesToConnect, avoidNodes, avoidSegments, true);

        // Output route
        out << "RestrictedDrivingRoute:";
        if (path.nodes.size()) {
            for (auto it = path.nodes.begin(); it != path.nodes.end(); it++) {
                if (it == path.nodes.end() - 1) out << (*it)->getInfo().getId();
                else out << (*it)->getInfo().getId() << ",";
            }
            out << "(" << path.distance << ")" << endl;
        } else out << "none" << endl;
        return;
    }

    //! Approximate
    bool approximateOnFail;
    if (pathMode == PathMode::driving_walking) {
        if (lineNumber == numberOfLines) {
            out << "Incomplete test!" << endl;
            out << "No approximation flag was given!" << endl;
            out << "-> ApproximateOnFail:[true/false]" << endl;
            return;
        }
        currentLine = test[lineNumber++];

        if (not parseKeyValue(currentLine, key, value)) {
            printInvalidLine(out, currentLine, "ApproximateOnFail:[true/false]");
            return;
        }
        if (key != "ApproximateOnFail") {
            printInvalidLine(out, currentLine, "ApproximateOnFail:[true/false]");
            return;
        }

        if (value != "False" and value != "True") {
            printInvalidLine(out, currentLine, "ApproximateOnFail:[true/false]");
            return;
        }

        approximateOnFail = (value == "True");
    }

    //! Walking-Driving
    // Driving then park and walking
    // Avoid nodes and segments
    // Output driving-walking route
    if (pathMode == PathMode::driving_walking and lineNumber == numberOfLines) {
        // Output Information
        out << "Source:" << sourceNode->getInfo().getId() << endl;
        out << "Destination:" << destinationNode->getInfo().getId() << endl;

        // Calculate path
        pair<Path, Path> path = findPathForDrivingWalking(
            graph, sourceNode, destinationNode, avoidNodes, avoidSegments, maxWalkTime, false
        );

        // Path with restrictions exists
        if (path.first.nodes.size() and path.second.nodes.size()) {
            // Print driving route
            out << "DrivingRoute:";
            if (path.first.nodes.size()) {
                for (auto it = path.first.nodes.begin(); it != path.first.nodes.end(); it++) {
                    if (it == path.first.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.first.distance << ")" << endl;
            } else out << "none" << endl;

            // Print parking node
            out << "ParkingNode:" << path.second.nodes.front()->getInfo().getId() << endl;

            // Print walking route
            out << "WalkingRoute:";
            if (path.second.nodes.size()) {
                for (auto it = path.second.nodes.begin(); it != path.second.nodes.end(); it++) {
                    if (it == path.second.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.second.distance << ")" << endl;
            } else out << "none" << endl;

            // Print total time
            out << "TotalTime:";
            out << path.first.distance + path.second.distance << endl;
            return;
        }

        // Calculate first approximate path
        path = findPathForDrivingWalking(graph, sourceNode, destinationNode, avoidNodes, avoidSegments, INF, false);

        // If no path exists independent of max walking time
        if (not path.first.nodes.size() or not path.second.nodes.size()) {
            out << "DrivingRoute:" << endl;
            out << "ParkingNode:" << endl;
            out << "WalkingRoute:" << endl;
            out << "TotalTime:" << endl;
            out << "Message:No path from the source to the destination exists, independente of the walking time."
                << endl;
            return;
        }

        // Approximate path exists
        // No approximate on fail
        if (not approximateOnFail) {
            out << "DrivingRoute:" << endl;
            out << "ParkingNode:" << endl;
            out << "WalkingRoute:" << endl;
            out << "TotalTime:" << endl;
            out << "Message:No path from the source to the destination exists with the max walking time constraint of "
                << maxWalkTime << " minutes." << endl;
            return;
        }

        // Approximate path exists
        // Approximate on fail
        else {
            // Print driving route 1
            out << "DrivingRoute1:";
            if (path.first.nodes.size()) {
                for (auto it = path.first.nodes.begin(); it != path.first.nodes.end(); it++) {
                    if (it == path.first.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.first.distance << ")" << endl;
            } else out << "none" << endl;

            // Print parking node
            out << "ParkingNode1:" << path.second.nodes.front()->getInfo().getId() << endl;

            // Print walking route
            out << "WalkingRoute1:";
            if (path.second.nodes.size()) {
                for (auto it = path.second.nodes.begin(); it != path.second.nodes.end(); it++) {
                    if (it == path.second.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.second.distance << ")" << endl;
            } else out << "none" << endl;

            // Print total time
            out << "TotalTime1:";
            out << path.first.distance + path.second.distance << endl;

            // Calculate second approximate path
            path = findPathForDrivingWalking(graph, sourceNode, destinationNode, avoidNodes, avoidSegments, INF, true);

            // Print driving route 2
            out << "DrivingRoute2:";
            if (path.first.nodes.size()) {
                for (auto it = path.first.nodes.begin(); it != path.first.nodes.end(); it++) {
                    if (it == path.first.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.first.distance << ")" << endl;
            } else out << "none" << endl;

            // Print parking node
            out << "ParkingNode2:" << path.second.nodes.front()->getInfo().getId() << endl;

            // Print walking route
            out << "WalkingRoute2:";
            if (path.second.nodes.size()) {
                for (auto it = path.second.nodes.begin(); it != path.second.nodes.end(); it++) {
                    if (it == path.second.nodes.end() - 1) out << (*it)->getInfo().getId();
                    else out << (*it)->getInfo().getId() << ",";
                }
                out << "(" << path.second.distance << ")" << endl;
            } else out << "none" << endl;

            // Print total time
            out << "TotalTime2:";
            out << path.first.distance + path.second.distance << endl;
            return;
        }
    }

    //! Extra lines
    currentLine = test[lineNumber++];
    out << "Invalid line!" << endl
        << "-> \"" << currentLine << "\"" << endl
        << "Test should not Contain any more lines." << endl;
    return;
}

void runBatchMode(Graph<Location, Distance> *graph, ifstream &inputFile, ostream &out) {
    // Sanity check
    if (graph == nullptr) return;
    if (not inputFile or not out) return;

    vector<vector<string>> tests = splitLinesIntoGroups(readAllLines(inputFile));

    for (const vector<string> &test : tests) {
        runTest(test, graph, out);
        out << endl << endl;
    }
}
