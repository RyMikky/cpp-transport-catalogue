#pragma once
#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <cassert>
#include <map>

inline const double EPSILON = 1e-6;

namespace transport_catalogue {

    namespace map_renderer {

        bool IsZero(double value);

        class SphereProjector {
        public:
            SphereProjector() = default;

            template <typename PointInputIt>
            SphereProjector(PointInputIt, PointInputIt, double, double, double);  // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates

            svg::Point operator()(geo::Coordinates) const;                        // Проецирует широту и долготу в координаты внутри SVG-изображения

        private:
            double padding_ = 0;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        struct LabelOffset
        {
            LabelOffset() = default;                      // смещение надписи с названием маршрута относительно координат конечной остановки на карте
            LabelOffset(double, double);

            LabelOffset& SetOffset(double, double);       // назначение параметров для дефолтной структуры
            LabelOffset GetOffset() const;                // получить параметры структуры

            double _dx = 0.0;                             // смещение по оси X
            double _dy = 0.0;                             // смещение по оси Y
        };

        // вспомогательная структура обработки данных маршрута
        struct RouteLabelData {
            RouteLabelData() = default;

            RouteLabelData(const svg::Point&, const svg::Point&, const std::string&, const svg::Color&);

            svg::Point _end_point = {};
            svg::Point _middle_point = {};
            std::string _route_label = ""s;
            svg::Color _route_color = "none";
        };

        // структура настроек рендера
        struct RendererSettings {

            RendererSettings() = default;

            RendererSettings(double, double, double, double, double, size_t, LabelOffset, size_t, LabelOffset, svg::Color, double, std::vector<svg::Color>);


            RendererSettings& SetWidth(double);                         // задать ширину отображаемого изображения
            RendererSettings& SetHeight(double);                        // задать высоту отображаемого изображения
            RendererSettings& SetPadding(double);                       // задать отступ краёв карты от границ SVG-документа

            RendererSettings& SetLineWidth(double);                     // задать толщину линий, которыми рисуются автобусные маршруты
            RendererSettings& SetStopRadius(double);                    // задать радиус окружностей, которыми обозначаются остановки

            RendererSettings& SetBusLabelFont(size_t);                  // задать размер текста написания автобусных остановок
            RendererSettings& SetBusLabelOffset(LabelOffset);           // задать смещение надписи с названием маршрута

            RendererSettings& SetStopLabelFont(size_t);                 // задать размер текста написания автобусных остановок
            RendererSettings& SetStopLabelOffset(LabelOffset);          // задать смещение надписи названия остановки

            RendererSettings& SetUnderlayerColor(svg::Color);           // задать цвет подложки под названиями
            RendererSettings& SetUnderlayerWidth(double);               // задать толщину подложки под названиями
            RendererSettings& AddColorInPalette(svg::Color);            // добавить цвет в палетту

            RendererSettings& ResetColorPalette();                      // обнулить палетту цветов


            double _width = 1200.0;                                     // ширина отображения в пикселях
            double _height = 1200.0;                                    // высота отображения в пикселях

            double _padding = 50.0;                                     // отступ краёв карты от границ SVG-документа. Вещественное число не меньше 0 и меньше min(width, height)/2
            double _line_width = 14.0;                                  // толщина линий, которыми рисуются автобусные маршруты
            double _stop_radius = 5.0;                                  // радиус окружностей, которыми обозначаются остановки

            size_t _bus_label_font_size = 20;                           // размер текста, которым написаны названия автобусных маршрутов
            LabelOffset _bus_label_offset = { 7.0, 15.0 };              // смещение надписи с названием маршрута относительно координат конечной остановки на карте

            size_t _stop_label_font_size = 20;                          // размер текста, которым отображаются названия остановок
            LabelOffset _stop_label_offset = { 7.0, -3.0 };             // смещение названия остановки относительно её координат на карте

            svg::Color _underlayer_color = svg::Rgba{ 255, 255, 255, 0.85 };         // цвет подложки под названиями остановок и маршрутов
            double _underlayer_width = 3.0;                                          // толщина подложки под названиями остановок и маршрутов
            std::vector<svg::Color> _color_palette = { std::string("green"), svg::Rgb{255, 160, 0}, std::string("red") };      // цветовая палитра
        };

        // класс названий маршрутов
        class RouteLabel : public svg::Drawable
        {
        public:
            RouteLabel(const svg::Point&, const svg::Point&, const std::string&, const svg::Color&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _end_point = {};                  // конечная остановка
            svg::Point _middle_point = {};               // промежуточная конечная
            std::string _route_label;                    // название маршрута
            svg::Color _route_color;                     // цвет надписи маршрута
            const RendererSettings& _settings;           // настройки рендерера для параметров по-умолчанию
        };

        // класс линий маршрутов
        class RouteLine : public svg::Drawable
        {
        public:
            RouteLine(const std::vector<svg::Point>&, const svg::Color&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            std::vector<svg::Point> _points;             // вектор нормализованных координат линии
            svg::Color _stroke_color;                    // цвет контура линии
            const RendererSettings& _settings;           // настройки рендерера для параметров по-умолчанию
        };

        // класс названий остановок
        class PointLabel : public svg::Drawable
        {
        public:
            PointLabel(const svg::Point&, const std::string&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _point ;                          // конечная остановка
            std::string _stop_label;                     // название маршрута
            const RendererSettings& _settings;           // настройки рендерера для параметров по-умолчанию
        };

        // класс точек остановок
        class RoutePoint : public svg::Drawable
        {
        public:
            RoutePoint(const svg::Point&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _point;                           // координата остановки
            const RendererSettings& _settings;           // настройки рендерера для параметров по-умолчанию
        };

        // основной класс рендера
        class MapRenderer {
        protected:
            std::ostream& _output;
        public:
            MapRenderer() = default;
            MapRenderer(std::ostream&);                                                         // конструктор для самостоятельной работы
            MapRenderer(std::ostream&, const RendererSettings&);                                // конструктор для вызова из обработчика запросов

            MapRenderer& SetRendererSettings(RendererSettings&&);                               // загрузка настроек рендера

            // ---------------------------- блок загрузки данных ---------------------------------------

            MapRenderer& AddRendererData(std::pair<std::string, RendererData>);                   // загрузка данных для рендеринга

            // ---------------------------- блок обработки ---------------------------------------------

            MapRenderer& CoordsProject();                                                       // калибровка проекции координат
            MapRenderer& RouteRender(std::vector<std::unique_ptr<svg::Drawable>>&);             // отрисовка маршрутов
            MapRenderer& PointLabelRender(std::vector<std::unique_ptr<svg::Drawable>>&);        // отрисовка названий остановок
            MapRenderer& PointRender(std::vector<std::unique_ptr<svg::Drawable>>&);             // отрисовка точек остановок
            
            void RendererProcess();                                                               // модуль отрисовки

            template <typename DrawableIterator>
            void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target);

            template <typename Container>
            void DrawPicture(const Container& container, svg::ObjectContainer& target);

        private:

            // ---------------------------- рабочие поля рендера ----------------------------------------

            RendererSettings _settings;                                                         // блок настроек рендеринга
            size_t _pallette_item = 0;                                                          // счётчик элементов палитры

            std::unordered_set<geo::Coordinates,
                geo::CoordinatesHasher> _input_coord = {};                                      // все координаты для рендеринга 
            std::map<std::string, geo::Coordinates> _unique_stops = {};                         // уникальные остановки для редеринга надписей
            SphereProjector _projector = {};                                                    // проектор координат

            struct ToRouteRender
            {
                ToRouteRender() = default;
                std::string _name = "";
                bool _is_circular = false;
                std::vector<std::pair<std::string, geo::CoordPtr>> _stops = {};
            };

            std::vector<ToRouteRender> _routes = {};                                            // маршруты для отрисовки

            const svg::Color GetColorFromPallete();                                             // возвращает следующий цвет из цветовой палитры
            void ResetPallette();                                                               // сбрасывает счетчик элементов палитры

        };

        template <typename PointInputIt>
        SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
            double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end) {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        template <typename DrawableIterator>
        void MapRenderer::DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target)
        {
            for (auto it = begin; it != end; ++it)
            {
                (*it)->Draw(target);
            }
        }

        template <typename Container>
        void MapRenderer::DrawPicture(const Container& container, svg::ObjectContainer& target)
        {
            DrawPicture(begin(container), end(container), target);
        }

    } //namespace map_render

} // nsmespace transport_catalogue