#include "geo.h"

namespace geo {

    bool Coordinates::operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }

    bool Coordinates::operator!=(const Coordinates& other) const {
        return !(*this == other);
    }

    std::size_t CoordinatesHasher::operator()(const Coordinates& coord) const noexcept {
        return static_cast<size_t>(coord.lat * 11)
            + static_cast<size_t>(coord.lng * 22);
    }
		
    double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = PI / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
            + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EARTH_RADIUS;
    }

}