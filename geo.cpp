#include "geo.h"

namespace geo {

    Coordinates::Coordinates(double lat, double lng)
        : lat(lat), lng(lng) {
    }

    Coordinates::Coordinates(const Coordinates& other)
        : lat(other.GetLatitude()), lng(other.GetLongitude()) {
    }

    Coordinates::Coordinates(Coordinates&& other) noexcept
        : lat(std::move(other.lat)), lng(std::move(other.lng)) {
    }

    Coordinates& Coordinates::operator=(const Coordinates& other) {
        lat = other.GetLatitude();
        lng = other.GetLongitude();
        return *this;
    }

    Coordinates& Coordinates::operator=(Coordinates&& other) noexcept {
        lat = std::move(other.GetLatitude());
        lng = std::move(other.GetLongitude());
        return *this;
    }

    Coordinates& Coordinates::SetLatitude(double latitude) {
        lat = latitude;
        return *this;
    }

    Coordinates& Coordinates::SetLongitude(double longitude) {
        lng = longitude;
        return *this;
    }

    double Coordinates::GetLatitude() const {
        return lat;
    }

    double Coordinates::GetLongitude() const {
        return lng;
    }

    Coordinates& Coordinates::SwapCoordinates(Coordinates& other) {
        double buffer = lat;
        SetLatitude(other.GetLatitude());
        other.SetLatitude(buffer);

        buffer = lng;
        SetLongitude(other.GetLongitude());
        other.SetLongitude(buffer);

        return *this;
    }

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