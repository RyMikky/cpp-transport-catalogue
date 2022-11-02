#pragma once

#include <cmath>
#include <utility>

const int EARTH_RADIUS = 6371000;                                          // усредненный радиус земли в метрах
const double PI = 3.1415926535;                                            // математическая константа Пи

namespace geo {

    struct Coordinates {

        Coordinates() = default;                                           // конструктор по умолчанию
        Coordinates(double, double);                                       // конструктор по двум числам
        Coordinates(const Coordinates&);                                   // конструктор копирования
        Coordinates(Coordinates&&) noexcept;                               // конструктор перемещения

        ~Coordinates() = default;

        Coordinates& operator=(const Coordinates&);                        // оператор присваивания
        Coordinates& operator=(Coordinates&&) noexcept;                    // оператор перемещения

        Coordinates& SetLatitude(double);                                  // задать широту координаты
        Coordinates& SetLongitude(double);                                 // задать долготу координаты

        double GetLatitude() const;                                        // получить широту координаты
        double GetLongitude() const;                                       // получить долготу координаты

        Coordinates& SwapCoordinates(Coordinates&);                        // поменяться данными с другой коородинатой

        double lat = 0.0;                                                  // широта
        double lng = 0.0;                                                  // долгота

        bool operator==(const Coordinates&) const;
        bool operator!=(const Coordinates&) const;
    };

    using CoordPtr = const Coordinates*;

    class CoordinatesHasher {
    public:
        std::size_t operator()(const Coordinates&) const noexcept;
    };

    // Вычисление дистанции между двумя точками координат
    double ComputeDistance(Coordinates, Coordinates);
}