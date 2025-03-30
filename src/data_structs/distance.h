#ifndef __DISTANCE__
#define __DISTANCE__

#include <string>

/**
 * @class Distance
 * @brief Represents a pair of distances for walking and driving.
 *
 * The Distance class encapsulates two distance values: one for walking and one for driving.
 * It provides methods to retrieve each of these values.
 *
 * @param walking The distance value representing the walking distance.
 * @param driving The distance value representing the driving distance.
 *
 * @see getWalking() Returns the walking distance.
 * @see getDriving() Returns the driving distance.
 */
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
