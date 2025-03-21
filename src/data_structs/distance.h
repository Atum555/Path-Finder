#ifndef __DISTANCE__
#define __DISTANCE__

#include <string>

class Distance {
  public:
    Distance(double walking, double driving) : _walking(walking), _driving(driving) {}

    double getWalking() const { return this->_walking; }

    double getDriving() const { return this->_driving; }

  protected:
    double _walking;
    double _driving;
};

#endif
