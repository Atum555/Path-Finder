# Path Finder

This is a project for a class on Algorithm Design at FEUP.\
The goal was to write a program that interpretes data files, builds an internal representation of an Undirected Weighted Graph, and then answer to user inputted question.\
The program has an interactive mode and a file mode.

### Usage

The 2 necessary data files must be given, first locations then distances.\
If no more arguments are given the program runs in interactive mode.\
Otherwise an input file and optionally an output file can be given to run in file mode.

```
./program <locations.csv> <distances.csv> [<input.txt> [<output.txt>]]
```

### Extras

This project also contains some example data files, some inputs for those files (well and mal-formed inputs) and theirs expected outputs.\
There's also html doxygen documentation.

## File Mode

> Write inputs on a file. Each input should have a name in a line staring with '#' followed by one of the the Input Formats available.

-   Route planning - Driving mode - No restrictions

    -   Input Format

    ```
    Mode:driving
    Source:<id>
    Destination:<id>
    ```

    -   Output Format

    ```
    Source:<id>
    Destination:<id>
    BestDrivingRoute:<id>,<id>,<id>(<int>)
    AlternativeDrivingRoute:<id>,<id>,<id>({total time})
    ```

-   Route planning - Driving mode - Restrictions

    -   Input Format

    ```
    Mode:driving
    Source:<id>
    Destination:<id>
    AvoidNodes:<id>,<id>,...
    AvoidSegments:(id,id),(id,id),..
    IncludeNode:<id>
    ```

    -   Output Format

    ```
    Source:<id>
    Destination:<id>
    RestrictedDrivingRoute:<id>,<id>,<id>({total time})
    ```

-   Route planning - Driving-Walking mode

    -   Input Format

    ```
    Mode:driving-walking
    Source:<id>
    Destination:<id>
    MaxWalkTime:<int>
    AvoidNodes:<id>,<id>,...
    AvoidSegments:(id,id),(id,id),...
    ApproximateOnFail:[True/False]
    ```

    -   Output Format

    ```
    Source:<id>
    Destination:<id>
    DrivingRoute:<id>,<id>,<id>({total time})
    ParkingNode:<id>
    WalkingRoute:<id>,<id>,<id>({total time})
    TotalTime:<int>
    ```

    -   Error Format

    ```
    Source:<id>
    Destination:<id>
    DrivingRoute:
    ParkingNode:
    WalkingRoute:
    TotalTime:
    Message:<string>
    ```

    -   Approximate Solution Format

    ```
    Source<id>
    Destination:<id>
    DrivingRoute1:<id>,<id>,<id>({total time})
    ParkingNode1:<id>
    WalkingRoute1:<id>,<id>,<id>({total time})
    TotalTime1:<int>
    DrivingRoute2:<id>,<id>,<id>({total time})
    ParkingNode2:<id>
    WalkingRoute2:<id>,<id>,<id>({total time})
    TotalTime2:<int>
    ```
