#pragma once

#include <cmath>

const int EARTH_RADIUS = 6371000;                                          // усредненный радиус земли в метрах
const double PI = 3.1415926535;                                            // математическа€ константа ѕи

namespace geo {

    struct Coordinates {
        double lat;                                                        // широта
        double lng;                                                        // долгота

        bool operator==(const Coordinates&) const;
        bool operator!=(const Coordinates&) const;
    };

    using CoordPtr = const Coordinates*;

    class CoordinatesHasher {
    public:
        std::size_t operator()(const Coordinates&) const noexcept;
    };

    // ¬ычисление дистанции между двум€ точками координат
    double ComputeDistance(Coordinates, Coordinates);
}