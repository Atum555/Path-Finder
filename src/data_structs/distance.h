#ifndef __DISTANCE__
#define __DISTANCE__

#include <string>

class Distance {
  public:
    Distance(double walking, double driving) : _walking(walking), _driving(driving) {}

  protected:
    double _walking;
    double _driving;
};

#endif
