#ifndef __LOCATION__
#define __LOCATION__

#include <string>

class Location {
  public:
    Location(int id, std::string code, std::string location, bool parking)
        : _id(id), _code(code), _location(location), _parking(parking) {}

    int getId() const { return this->_id; }

    std::string getCode() const { return this->_code; }

    std::string getLocation() const { return this->_location; }

    bool hasParking() const { return this->_parking; }

  protected:
    int         _id;
    std::string _code;
    std::string _location;
    bool        _parking;
};

#endif
