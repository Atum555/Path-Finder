#ifndef __LOCATION__
#define __LOCATION__

#include <string>

/**
 * @class Location
 * @brief Represents a physical location with associated details.
 *
 * This class encapsulates information about a location, including:
 *   - A unique identifier.
 *   - A specific code representing the location.
 *   - A descriptive string detailing the location.
 *   - A boolean flag indicating the availability of parking.
 *
 * The constructor initializes a Location instance by accepting values for these attributes.
 *
 * Member Functions:
 *   - getId() returns the unique identifier.
 *   - getCode() returns the location-specific code.
 *   - getLocation() returns the descriptive text of the location.
 *   - hasParking() indicates whether parking is available.
 */
class Location {
  public:
    Location(const std::string &id, const std::string &code, const std::string &location, bool parking)
        : _id(id), _code(code), _location(location), _parking(parking) {}

    std::string getId() const { return this->_id; }

    std::string getCode() const { return this->_code; }

    std::string getLocation() const { return this->_location; }

    bool hasParking() const { return this->_parking; }

  protected:
    std::string _id;
    std::string _code;
    std::string _location;
    bool        _parking;
};

#endif
