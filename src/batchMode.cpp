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

/**
 * Skips all consecutive whitespace characters in the given string starting from a specific position.
 *
 * This function iterates over the string beginning at position 'pos' and advances the position
 * past any whitespace characters (determined by isspace, using unsigned char conversion). It continues
 * until a non-whitespace character is encountered or the end of the string is reached.
 *
 * @param str The string to be processed.
 * @param pos The starting index for processing within the string.
 * @return The index of the first non-whitespace character found, or str.size() if no such character exists.
 */
size_t skipWhitespace(const string &str, size_t pos) {
    while (pos < str.size() && isspace(static_cast<unsigned char>(str[pos]))) ++pos;
    return pos;
}

/**
 * @brief Reads all lines from an input file stream.
 *
 * This function reads each line from the provided ifstream, trims whitespace from both ends of the line,
 * and appends the processed line to a vector of strings. It continues until the end of the file is reached.
 *
 * @param file Reference to an ifstream object representing the open file to read from.
 * @return std::vector<std::string> A vector containing all the trimmed lines from the file.
 * @throws std::runtime_error If an error occurs while reading the file (other than reaching the end of file).
 */
vector<string> readAllLines(ifstream &file) {
    vector<string> lines;
    string         line;

    while (getline(file, line)) lines.push_back(trim(line));

    if (!file.eof()) throw new runtime_error("An error outside of this program occurred reading the input file!");

    return lines;
}

/**
 * Splits a vector of strings into multiple groups based on empty line delimiters.
 *
 * This function processes a given list of lines and groups them into individual sections,
 * where each section is a contiguous group of non-empty lines. An empty line signifies the
 * end of a current section and starts a new one. If the final section is not terminated by
 * an empty line, it is still added to the resulting groups.
 *
 * @param lines A constant reference to a vector of strings representing the lines to be grouped.
 * @return A vector of vectors of strings, where each inner vector represents a group of consecutive non-empty lines.
 */
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

/**
 * @brief Prints an error message for an invalid input line.
 *
 * This function outputs a formatted error message to the provided output stream,
 * indicating that a specific line is invalid and showing what was expected.
 *
 * @param out The output stream where the message is printed.
 * @param line The input line that failed validation.
 * @param expected A string describing the expected format or content.
 */
void printInvalidLine(ostream &out, const string &line, const string &expected) {
    out << "Invalid line!" << endl << "-> \"" << line << "\"" << endl << "Expected \"" << expected << "\"" << endl;
}

/**
 * @brief Prints an error message for an invalid node identifier.
 *
 * This function writes an error message to the specified output stream indicating that a node identifier is invalid.
 * It includes the offending identifier in the message to aid in debugging.
 *
 * @param out The output stream where the error message is printed.
 * @param value The invalid node identifier that triggered the error message.
 */
void printInvalidIdentifier(ostream &out, const string &value) {
    out << "Invalid node identifier!" << endl << "-> \"" << value << "\"" << endl;
}

/**
 * @brief Outputs an error message for an invalid node identifier or missing edge.
 *
 * This function writes a message to the provided output stream indicating that one of the 
 * given node identifiers is invalid or that the corresponding edge does not exist. It displays 
 * both identifiers for clarity.
 *
 * @param out The output stream where the error message is sent.
 * @param value A pair of strings containing the problematic node identifiers.
 */
void printInvalidIdentifier(ostream &out, const pair<string, string> &value) {
    out << "Invalid node identifier!" << endl;
    out << "-> \"" << value.first << "\" or \"" << value.second << "\"" << endl;
    out << "Or the edge does not exist..." << endl;
}

/**
 * Checks if the provided test name string starts with the '#' character.
 *
 * @param line A constant reference to the string containing the test name.
 * @return true if the first character of the string is '#', false otherwise.
 *
 * @note This function assumes the string is non-empty.
 */
bool validateTestName(const string &line) { return line[0] == '#'; }

/**
 * @brief Parses a line that contains a key and a value separated by a colon.
 *
 * This function checks that the input string contains exactly one colon. If so, it splits
 * the string into a key (the portion before the colon) and a value (the portion after the colon),
 * trimming both parts to remove any leading or trailing whitespace.
 *
 * @param line The input string expected to be in "key: value" format.
 * @param key Reference to a string where the trimmed key will be stored.
 * @param value Reference to a string where the trimmed value will be stored.
 * @return true If the input string has exactly one colon and parsing is successful.
 * @return false If the input string does not contain exactly one colon.
 */
bool parseKeyValue(const string &line, string &key, string &value) {
    if (count(line.begin(), line.end(), ':') != 1) return false;
    size_t pos = line.find(':');
    key        = line.substr(0, pos);
    value      = line.substr(pos + 1);
    trim(key);
    trim(value);
    return true;
}

/**
 * @brief Parses and validates identifiers of nodes to avoid in the graph.
 *
 * This function takes a comma-separated list of node identifiers given in the string 
 * "identifiers", splits them into tokens, and then tries to find each corresponding vertex 
 * in the provided graph. For every identifier, it adds the found vertex (or nullptr if not found)
 * to the avoidNodes vector.
 *
 * If any identifier does not correspond to a valid vertex in the graph, the function prints 
 * an error message to the provided output stream "out", including the original input line for 
 * context, and returns false. Should all identifiers be valid, the function returns true.
 *
 * @param graph         Pointer to the Graph containing nodes of type Location and edges of type Distance.
 * @param avoidNodes    Reference to a vector where vertex pointers corresponding to identifiers will be stored.
 * @param line          The original input line being processed (used for error messages).
 * @param identifiers   A comma-separated string containing the identifiers of nodes to avoid.
 * @param out           Output stream to which error messages are written.
 *
 * @return true if all identifiers correspond to valid vertices in the graph, false otherwise.
 */
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

/**
 * @brief Parses and retrieves edges to be avoided from a given identifiers string.
 * 
 * This function takes a string containing pairs of node identifiers formatted as 
 * "([id1], [id2])", where each pair represents an edge to avoid in the graph. It uses 
 * a regular expression to parse these pairs, skipping any unnecessary whitespace and 
 * commas. For each valid pair, it attempts to find the corresponding edge in the provided 
 * graph using the findEdge function. If the edge is found, it is added to the avoidSegments 
 * vector; if not, an error message is printed to the output stream along with the original 
 * input line, and the function returns false.
 * 
 * The expected input format for the identifiers string is:
 * "([<id>/<code>,<id>/<code>), ...]"
 * 
 * @param graph Pointer to the graph containing the locations and distances.
 * @param avoidSegments A reference to a vector that will store pointers to the edges 
 *                      representing segments to be avoided.
 * @param line The original input text line, used for error reporting.
 * @param identifiers The string containing pairs of node identifiers to parse.
 * @param out The output stream used to print error messages.
 * 
 * @return true if all pairs are parsed correctly and the corresponding edges are found;
 *         false if any error occurs during parsing or if an edge is not found in the graph.
 */
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

/**
 * @brief Executes a test scenario for route planning based on provided configuration lines.
 *
 * This function processes a series of configuration commands contained in a vector of strings,
 * constructing and validating a test scenario intended to compute optimal routes on a given graph.
 * The test configuration includes the following steps:
 *
 * 1. Test Name: Validates the test name (the first line) must start with a '#' symbol.
 * 2. Mode: Determines the travel mode ("driving" or "driving-walking").
 * 3. Source Node: Parses and validates the source node identified by its id or code.
 * 4. Destination Node: Parses and validates the destination node.
 * 5. Driving Mode (Simple): If in "driving" mode and no additional parameters are provided,
 *    computes and outputs both best and alternative driving routes.
 * 6. Driving-Walking Mode (Extended): 
 *    - Parses the maximum allowed walking time.
 *    - Parses the list of nodes and segments to avoid.
 *    - Optionally processes a flag for approximating the route if constraints cannot be met.
 *    - Computes appropriate driving and walking segments and outputs the routes and total travel time.
 * 7. Restricted Driving: In "driving" mode with additional parameters, computes a route that respects
 *    avoidance and mandatory inclusion of a specific node.
 * 8. Extra Lines: Detects unexpected extra input and outputs an error message.
 *
 * Each stage involves parsing key-value pairs, validation of input, and error handling through
 * appropriate messages to the output stream.
 *
 * @param test  A vector of strings, each representing a line of configuration for the test scenario.
 * @param graph Pointer to the graph structure containing vertices and edges used for route calculations.
 * @param out   Output stream where results, route details, and error messages are printed.
 */
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

/**
 * @brief Executes batch mode processing on a graph using test cases read from an input file.
 *
 * This function reads all lines from the given input file, groups them into individual test cases 
 * using splitLinesIntoGroups, and processes each test case with runTest. The results are written to 
 * the provided output stream, with extra newlines separating each test case's output.
 *
 * @param graph Pointer to a Graph object containing Location nodes and Distance weights. The function does nothing if this is nullptr.
 * @param inputFile Reference to an ifstream from which the test cases are read. The function does nothing if the stream is not valid.
 * @param out Reference to an ostream where the results for each test case are output. The function does nothing if the stream is not valid.
 */
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
