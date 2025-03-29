## Features

-   Menu

    -   Shows all of the available functionalities
    -   Main entry point to the app

-   Read and Parse Data

    -   Select files
    -   Parse files
    -   Request options

-   Batch mode

    -   Read from Input
    -   Write onto Output

-   Doxygen Documentation

    -   Must include Time Complexity for algorithms

-   Route planning - Driving mode - No restrictions

    -   2 Independent paths for driving

    -   Parse input

    ```
    Mode:driving
    Source:<id>
    Destination:<id>
    ```

    -   Print output

    ```
    Source:<id>
    Destination:<id>
    BestDrivingRoute:<id>,<id>,<id>(<int>)
    AlternativeDrivingRoute:<id>,<id>,<id>({total time})
    ```

-   Route planning - Driving mode - Restrictions

    -   Exclude nodes
    -   Exclude segments
    -   Include a node (a stop on the way)

    -   Parse input

    ```
    Mode:driving
    Source:<id>
    Destination:<id>
    AvoidNodes:<id>,<id>,...
    AvoidSegments:(id,id),(id,id),..
    IncludeNode:<id>
    ```

    -   Print output

    ```
    Source:<id>
    Destination:<id>
    RestrictedDrivingRoute:<id>,<id>,<id>({total time})
    ```

-   Route planning - Driving-Walking mode

    -   Shortest time
    -   On collision longest walk wins
    -   On fail indicate reason, max walk time and/or no parking found
    -   Parse input

    ```
    Mode:driving-walking
    Source:<id>
    Destination:<id>
    MaxWalkTime:<int>
    AvoidNodes:<id>,<id>,...
    AvoidSegments:(id,id),(id,id),...
    ```

    -   Print output

    ```
    Source:<id>
    Destination:<id>
    DrivingRoute:<id>,<id>,<id>({total time})
    ParkingNode:<id>
    WalkingRoute:<id>,<id>,<id>({total time})
    TotalTime:<int>
    ```

    -   Print error

    ```
    Source:<id>
    Destination:<id>
    DrivingRoute:
    ParkingNode:
    WalkingRoute:
    TotalTime:
    Message:<string>
    ```

    -   Print approximate solution

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

## Plan

-   Main function

    -   Program entry point
    -   Parse arguments
    -   Call function to parse data files
    -   Decide who to call (menu / batch mode)

-   Data files parse function

    -   Parse data files
    -   Generate graph

-   Batch mode function

    -   Interprete input.txt file
    -   Execute algorithms to get answer
    -   Print output

-   Menu function
    -   Show available options [driving, driving-walking]
    -   Ask to select start node
    -   Ask to select destination node
    -   Ask restrictions...
    -   Execute algorithms to get answer
    -   Print output
    -   Loop to start
