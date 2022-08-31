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
            SphereProjector(PointInputIt, PointInputIt, double, double, double);  // points_begin � points_end ������ ������ � ����� ��������� ��������� geo::Coordinates

            svg::Point operator()(geo::Coordinates) const;                        // ���������� ������ � ������� � ���������� ������ SVG-�����������

        private:
            double padding_ = 0;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        struct LabelOffset
        {
            LabelOffset() = default;                      // �������� ������� � ��������� �������� ������������ ��������� �������� ��������� �� �����
            LabelOffset(double, double);

            LabelOffset& SetOffset(double, double);       // ���������� ���������� ��� ��������� ���������
            LabelOffset GetOffset() const;                // �������� ��������� ���������

            double _dx = 0.0;                             // �������� �� ��� X
            double _dy = 0.0;                             // �������� �� ��� Y
        };

        // ��������������� ��������� ��������� ������ ��������
        struct RouteLabelData {
            RouteLabelData() = default;

            RouteLabelData(const svg::Point&, const svg::Point&, const std::string&, const svg::Color&);

            svg::Point _end_point = {};
            svg::Point _middle_point = {};
            std::string _route_label = ""s;
            svg::Color _route_color = "none";
        };

        // ��������� �������� �������
        struct RendererSettings {

            RendererSettings() = default;

            RendererSettings(double, double, double, double, double, size_t, LabelOffset, size_t, LabelOffset, svg::Color, double, std::vector<svg::Color>);


            RendererSettings& SetWidth(double);                         // ������ ������ ������������� �����������
            RendererSettings& SetHeight(double);                        // ������ ������ ������������� �����������
            RendererSettings& SetPadding(double);                       // ������ ������ ���� ����� �� ������ SVG-���������

            RendererSettings& SetLineWidth(double);                     // ������ ������� �����, �������� �������� ���������� ��������
            RendererSettings& SetStopRadius(double);                    // ������ ������ �����������, �������� ������������ ���������

            RendererSettings& SetBusLabelFont(size_t);                  // ������ ������ ������ ��������� ���������� ���������
            RendererSettings& SetBusLabelOffset(LabelOffset);           // ������ �������� ������� � ��������� ��������

            RendererSettings& SetStopLabelFont(size_t);                 // ������ ������ ������ ��������� ���������� ���������
            RendererSettings& SetStopLabelOffset(LabelOffset);          // ������ �������� ������� �������� ���������

            RendererSettings& SetUnderlayerColor(svg::Color);           // ������ ���� �������� ��� ����������
            RendererSettings& SetUnderlayerWidth(double);               // ������ ������� �������� ��� ����������
            RendererSettings& AddColorInPalette(svg::Color);            // �������� ���� � �������

            RendererSettings& ResetColorPalette();                      // �������� ������� ������


            double _width = 1200.0;                                     // ������ ����������� � ��������
            double _height = 1200.0;                                    // ������ ����������� � ��������

            double _padding = 50.0;                                     // ������ ���� ����� �� ������ SVG-���������. ������������ ����� �� ������ 0 � ������ min(width, height)/2
            double _line_width = 14.0;                                  // ������� �����, �������� �������� ���������� ��������
            double _stop_radius = 5.0;                                  // ������ �����������, �������� ������������ ���������

            size_t _bus_label_font_size = 20;                           // ������ ������, ������� �������� �������� ���������� ���������
            LabelOffset _bus_label_offset = { 7.0, 15.0 };              // �������� ������� � ��������� �������� ������������ ��������� �������� ��������� �� �����

            size_t _stop_label_font_size = 20;                          // ������ ������, ������� ������������ �������� ���������
            LabelOffset _stop_label_offset = { 7.0, -3.0 };             // �������� �������� ��������� ������������ � ��������� �� �����

            svg::Color _underlayer_color = svg::Rgba{ 255, 255, 255, 0.85 };         // ���� �������� ��� ���������� ��������� � ���������
            double _underlayer_width = 3.0;                                          // ������� �������� ��� ���������� ��������� � ���������
            std::vector<svg::Color> _color_palette = { std::string("green"), svg::Rgb{255, 160, 0}, std::string("red") };      // �������� �������
        };

        // ����� �������� ���������
        class RouteLabel : public svg::Drawable
        {
        public:
            RouteLabel(const svg::Point&, const svg::Point&, const std::string&, const svg::Color&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _end_point = {};                  // �������� ���������
            svg::Point _middle_point = {};               // ������������� ��������
            std::string _route_label;                    // �������� ��������
            svg::Color _route_color;                     // ���� ������� ��������
            const RendererSettings& _settings;           // ��������� ��������� ��� ���������� ��-���������
        };

        // ����� ����� ���������
        class RouteLine : public svg::Drawable
        {
        public:
            RouteLine(const std::vector<svg::Point>&, const svg::Color&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            std::vector<svg::Point> _points;             // ������ ��������������� ��������� �����
            svg::Color _stroke_color;                    // ���� ������� �����
            const RendererSettings& _settings;           // ��������� ��������� ��� ���������� ��-���������
        };

        // ����� �������� ���������
        class PointLabel : public svg::Drawable
        {
        public:
            PointLabel(const svg::Point&, const std::string&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _point ;                          // �������� ���������
            std::string _stop_label;                     // �������� ��������
            const RendererSettings& _settings;           // ��������� ��������� ��� ���������� ��-���������
        };

        // ����� ����� ���������
        class RoutePoint : public svg::Drawable
        {
        public:
            RoutePoint(const svg::Point&, const RendererSettings&);
            void Draw(svg::ObjectContainer&) const override;
        private:
            svg::Point _point;                           // ���������� ���������
            const RendererSettings& _settings;           // ��������� ��������� ��� ���������� ��-���������
        };

        // �������� ����� �������
        class MapRenderer {
        protected:
            std::ostream& _output;
        public:
            MapRenderer() = default;
            MapRenderer(std::ostream&);                                                         // ����������� ��� ��������������� ������
            MapRenderer(std::ostream&, const RendererSettings&);                                // ����������� ��� ������ �� ����������� ��������

            MapRenderer& SetRendererSettings(RendererSettings&&);                               // �������� �������� �������

            // ---------------------------- ���� �������� ������ ---------------------------------------

            MapRenderer& AddRendererData(std::pair<std::string, RendererData>);                   // �������� ������ ��� ����������

            // ---------------------------- ���� ��������� ---------------------------------------------

            MapRenderer& CoordsProject();                                                       // ���������� �������� ���������
            MapRenderer& RouteRender(std::vector<std::unique_ptr<svg::Drawable>>&);             // ��������� ���������
            MapRenderer& PointLabelRender(std::vector<std::unique_ptr<svg::Drawable>>&);        // ��������� �������� ���������
            MapRenderer& PointRender(std::vector<std::unique_ptr<svg::Drawable>>&);             // ��������� ����� ���������
            
            void RendererProcess();                                                               // ������ ���������

            template <typename DrawableIterator>
            void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target);

            template <typename Container>
            void DrawPicture(const Container& container, svg::ObjectContainer& target);

        private:

            // ---------------------------- ������� ���� ������� ----------------------------------------

            RendererSettings _settings;                                                         // ���� �������� ����������
            size_t _pallette_item = 0;                                                          // ������� ��������� �������

            std::unordered_set<geo::Coordinates,
                geo::CoordinatesHasher> _input_coord = {};                                      // ��� ���������� ��� ���������� 
            std::map<std::string, geo::Coordinates> _unique_stops = {};                         // ���������� ��������� ��� ��������� ��������
            SphereProjector _projector = {};                                                    // �������� ���������

            struct ToRouteRender
            {
                ToRouteRender() = default;
                std::string _name = "";
                bool _is_circular = false;
                std::vector<std::pair<std::string, geo::CoordPtr>> _stops = {};
            };

            std::vector<ToRouteRender> _routes = {};                                            // �������� ��� ���������

            const svg::Color GetColorFromPallete();                                             // ���������� ��������� ���� �� �������� �������
            void ResetPallette();                                                               // ���������� ������� ��������� �������

        };

        template <typename PointInputIt>
        SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
            double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // ���� ����� ����������� ����� �� ������, ��������� ������
            if (points_begin == points_end) {
                return;
            }

            // ������� ����� � ����������� � ������������ ��������
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // ������� ����� � ����������� � ������������ �������
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // ��������� ����������� ��������������� ����� ���������� x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // ��������� ����������� ��������������� ����� ���������� y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // ������������ ��������������� �� ������ � ������ ���������,
                // ���� ����������� �� ���
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                // ����������� ��������������� �� ������ ���������, ���������� ���
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                // ����������� ��������������� �� ������ ���������, ���������� ���
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