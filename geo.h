#pragma once

#include <cmath>

const int EARTH_RADIUS = 6371000;
const double PI = 3.1415926535;

namespace geo {

    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates&) const;

        bool operator!=(const Coordinates&) const;
    };

    using CoordPtr = const Coordinates*;

    class CoordinatesHasher {
    public:
        std::size_t operator()(const Coordinates&) const noexcept;
    };

    double ComputeDistance(Coordinates, Coordinates);
}