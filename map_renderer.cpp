#include "map_renderer.h"

namespace transport_catalogue {

	namespace map_renderer {

		bool IsZero(double value) {
			return std::abs(value) < EPSILON;
		}

		svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
			return {
				(coords.lng - min_lon_) * zoom_coeff_ + padding_,
				(max_lat_ - coords.lat) * zoom_coeff_ + padding_
			};
		}


		// ---------------------------- struct LabelOffset ------------------------------------------

		LabelOffset::LabelOffset(double dx, double dy) : _dx(dx), _dy(dy) {
		}

		// назначение параметров для дефолтной структуры
		LabelOffset& LabelOffset::SetOffset(double dx, double dy) {
			_dx = dx;
			_dy = dy;
			return *this;
		}

		// получить параметры структуры
		LabelOffset LabelOffset::GetOffset() const {
			return *this;
		}

		// ---------------------------- struct LabelOffset END --------------------------------------


		// ---------------------------- struct RouteLabelData ---------------------------------------

		RouteLabelData::RouteLabelData(const svg::Point& end, const svg::Point& middle, const std::string& label, const svg::Color& color)
			: _end_point(end), _middle_point(middle), _route_label(label), _route_color(color) {
		}

		// ---------------------------- struct RouteLabelData END -----------------------------------


		// ---------------------------- struct RenderSettings ---------------------------------------

		RendererSettings::RendererSettings(double width, double height, double padding, double line_width, double stop_radius,
			size_t bus_font_size, LabelOffset bus_font_offset, size_t stop_font_size, LabelOffset stop_font_offset,
			svg::Color underlayer_color, double underlayer_width, std::vector<svg::Color> color_palette)
			: _width(width), _height(height), _padding(padding), _line_width(line_width), _stop_radius(stop_radius)
			, _bus_label_font_size(bus_font_size), _bus_label_offset(bus_font_offset), _stop_label_font_size(stop_font_size)
			, _stop_label_offset(stop_font_offset), _underlayer_color(underlayer_color), _underlayer_width(underlayer_width), _color_palette(color_palette) {
		}

		// задать ширину отображаемого изображения
		RendererSettings& RendererSettings::SetWidth(double width) {
			_width = std::move(width);
			return *this;
		}

		// задать высоту отображаемого изображения
		RendererSettings& RendererSettings::SetHeight(double height) {
			_height = std::move(height);
			return *this;
		}

		// задать отступ краёв карты от границ SVG-документа
		RendererSettings& RendererSettings::SetPadding(double padding) {
			_padding = std::move(padding);
			return *this;
		}

		// задать толщину линий, которыми рисуются автобусные маршруты
		RendererSettings& RendererSettings::SetLineWidth(double l_width) {
			_line_width = std::move(l_width);
			return *this;
		}

		// задать радиус окружностей, которыми обозначаются остановки
		RendererSettings& RendererSettings::SetStopRadius(double radius) {
			_stop_radius = std::move(radius);
			return *this;
		}

		// задать размер текста написания автобусных остановок
		RendererSettings& RendererSettings::SetBusLabelFont(size_t size) {
			_bus_label_font_size = std::move(size);
			return *this;
		}

		// задать смещение надписи с названием маршрута
		RendererSettings& RendererSettings::SetBusLabelOffset(LabelOffset offset) {
			_bus_label_offset = std::move(offset);
			return *this;
		}

		// задать размер текста написания автобусных остановок
		RendererSettings& RendererSettings::SetStopLabelFont(size_t size) {
			_stop_label_font_size = std::move(size);
			return *this;
		}

		// задать смещение надписи названия остановки
		RendererSettings& RendererSettings::SetStopLabelOffset(LabelOffset offset) {
			_stop_label_offset = std::move(offset);
			return *this;
		}

		// задать цвет подложки под названиями
		RendererSettings& RendererSettings::SetUnderlayerColor(svg::Color color) {
			_underlayer_color = std::move(color);
			return *this;
		}

		// задать толщину подложки под названиями
		RendererSettings& RendererSettings::SetUnderlayerWidth(double width) {
			_underlayer_width = std::move(width);
			return *this;
		}

		// добавить цвет в палетту
		RendererSettings& RendererSettings::AddColorInPalette(svg::Color color) {
			_color_palette.push_back(std::move(color));
			return *this;
		}

		// обнулить палетту цветов
		RendererSettings& RendererSettings::ResetColorPalette() {
			_color_palette.clear();
			return *this;
		}

		// ---------------------------- struct RenderSettings END -----------------------------------


		// ---------------------------- class RouteLabel --------------------------------------------

		RouteLabel::RouteLabel(const svg::Point& end, const svg::Point& middle, const std::string& label,
			const svg::Color& color, const RendererSettings& renderer_settings) : _end_point(end), _middle_point(middle),
			_route_label(label), _route_color(color), _settings(renderer_settings) {
		}

		void RouteLabel::Draw(svg::ObjectContainer& container) const
		{

			svg::Text label_underlayer_1;
			label_underlayer_1.SetData(_route_label);
			label_underlayer_1.SetPosition(_end_point);
			label_underlayer_1.SetOffset({ _settings._bus_label_offset._dx, _settings._bus_label_offset._dy });
			label_underlayer_1.SetFontSize(static_cast<uint32_t>(_settings._bus_label_font_size));
			label_underlayer_1.SetFontFamily("Verdana");
			label_underlayer_1.SetFontWeight("bold");
			label_underlayer_1.SetFillColor(_settings._underlayer_color);
			label_underlayer_1.SetStrokeColor(_settings._underlayer_color);
			label_underlayer_1.SetStrokeWidth(_settings._underlayer_width);
			label_underlayer_1.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			label_underlayer_1.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

			svg::Text label_1;
			label_1.SetData(_route_label);
			label_1.SetPosition(_end_point);
			label_1.SetOffset({ _settings._bus_label_offset._dx, _settings._bus_label_offset._dy });
			label_1.SetFontSize(static_cast<uint32_t>(_settings._bus_label_font_size));
			label_1.SetFontFamily("Verdana");
			label_1.SetFontWeight("bold");
			label_1.SetFillColor(_route_color);

			container.Add(label_underlayer_1);
			container.Add(label_1);

			if (_end_point.x != _middle_point.x
				&& _end_point.y != _middle_point.y) {

				svg::Text label_underlayer_2;
				label_underlayer_2.SetData(_route_label);
				label_underlayer_2.SetPosition(_middle_point);
				label_underlayer_2.SetOffset({ _settings._bus_label_offset._dx, _settings._bus_label_offset._dy });
				label_underlayer_2.SetFontSize(static_cast<uint32_t>(_settings._bus_label_font_size));
				label_underlayer_2.SetFontFamily("Verdana");
				label_underlayer_2.SetFontWeight("bold");
				label_underlayer_2.SetFillColor(_settings._underlayer_color);
				label_underlayer_2.SetStrokeColor(_settings._underlayer_color);
				label_underlayer_2.SetStrokeWidth(_settings._underlayer_width);
				label_underlayer_2.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				label_underlayer_2.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				svg::Text label_2;
				label_2.SetData(_route_label);
				label_2.SetPosition(_middle_point);
				label_2.SetOffset({ _settings._bus_label_offset._dx, _settings._bus_label_offset._dy });
				label_2.SetFontSize(static_cast<uint32_t>(_settings._bus_label_font_size));
				label_2.SetFontFamily("Verdana");
				label_2.SetFontWeight("bold");
				label_2.SetFillColor(_route_color);

				container.Add(label_underlayer_2);
				container.Add(label_2);
			}

		}

		// ---------------------------- class RouteLabel END ----------------------------------------


		// ---------------------------- class RouteLine ---------------------------------------------

		RouteLine::RouteLine(const std::vector<svg::Point>& stop_points,
			const svg::Color& stroke_color, const RendererSettings& renderer_settings) :
			_points(stop_points), _stroke_color(stroke_color), _settings(renderer_settings){
		}

		void RouteLine::Draw(svg::ObjectContainer& container) const
		{
			svg::Polyline polyline;
			for (const auto& point : _points)
			{
				polyline.AddPoint(point);
			}
			polyline.SetStrokeColor(_stroke_color);
			polyline.SetFillColor(svg::NoneColor);
			polyline.SetStrokeWidth(_settings._line_width);
			polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			container.Add(polyline);
		}

		// ---------------------------- class RouteLine END -----------------------------------------


		// ---------------------------- class PointLabel --------------------------------------------

		PointLabel::PointLabel(const svg::Point& point, const std::string& label, 
			const RendererSettings& renderer_settings) : _point(point), _stop_label(label),
			_settings(renderer_settings) {
		}

		void PointLabel::Draw(svg::ObjectContainer& container) const
		{

			svg::Text label_underlayer;
			label_underlayer.SetData(_stop_label);
			label_underlayer.SetPosition(_point);
			label_underlayer.SetOffset({ _settings._stop_label_offset._dx, _settings._stop_label_offset._dy });
			label_underlayer.SetFontSize(static_cast<uint32_t>(_settings._stop_label_font_size));
			label_underlayer.SetFontFamily("Verdana");
			label_underlayer.SetFillColor(_settings._underlayer_color);
			label_underlayer.SetStrokeColor(_settings._underlayer_color);
			label_underlayer.SetStrokeWidth(_settings._underlayer_width);
			label_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			label_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

			svg::Text label;
			label.SetData(_stop_label);
			label.SetPosition(_point);
			label.SetOffset({ _settings._stop_label_offset._dx, _settings._stop_label_offset._dy });
			label.SetFontSize(static_cast<uint32_t>(_settings._stop_label_font_size));
			label.SetFontFamily("Verdana");
			label.SetFillColor("black");

			container.Add(label_underlayer);
			container.Add(label);

		}

		// ---------------------------- class PointLabel END ----------------------------------------


		// ---------------------------- class RoutePoint --------------------------------------------

		RoutePoint::RoutePoint(const svg::Point& point, const RendererSettings& renderer_settings) :
			_point(point), _settings(renderer_settings) {
		}

		void RoutePoint::Draw(svg::ObjectContainer& container) const
		{
			svg::Circle circle;

			circle.SetCenter(_point);
			circle.SetFillColor("white");
			circle.SetRadius(_settings._stop_radius);
			container.Add(circle);
		}

		// ---------------------------- class RoutePoint END ----------------------------------------


		// ---------------------------- class MapRender ---------------------------------------------

		// конструктор для самостоятельной работы
		MapRenderer::MapRenderer(std::ostream& out) : _output(out) {
		}

		// конструктор для вызова из обработчика запросов
		MapRenderer::MapRenderer(std::ostream& out, const RendererSettings& settings) : _output(out), _settings(settings) {
		}

		// загрузка настроек рендера
		MapRenderer& MapRenderer::SetRendererSettings(RendererSettings&& settings) {
			_settings = std::move(settings);
			return *this;
		}


		// ---------------------------- блок загрузки данных ---------------------------------------

		// загрузка данных для рендеринга
		MapRenderer& MapRenderer::AddRendererData(std::pair<std::string, RendererData> data) {

			ToRouteRender route;

			for (auto& item : data.second._stops) {

				_input_coord.emplace(item->_coords);
				_unique_stops[item->_name] = item->_coords;

				route._stops.push_back(std::move(std::make_pair(item->_name, &item->_coords)));
			}

			route._is_circular = data.second._is_circular;
			route._name = data.first;

			_routes.push_back(std::move(route));

			return *this;
		}

		// ---------------------------- блок обработки ---------------------------------------------

		// калибровка проекции координат
		MapRenderer& MapRenderer::CoordsProject(){ 
			SphereProjector projector(_input_coord.begin(), _input_coord.end(), 
				_settings._width, _settings._height, _settings._padding);
			_projector = projector;
			return *this;
		}

		// отрисовка маршрутов
		MapRenderer& MapRenderer::RouteRender(std::vector<std::unique_ptr<svg::Drawable>>& container) {

			std::vector<RouteLabelData> to_label_render;

			for (const auto& route : _routes)
			{

				auto route_color = GetColorFromPallete();
				auto route_name = route._name;

				// набираем полилинии маршрутов
				std::vector<svg::Point> points;  

				for (const auto& stop : route._stops){
					points.push_back(_projector(*stop.second));
				}

				// добавляем информацию о маршруте в рендер
				container.emplace_back(std::make_unique<RouteLine>(RouteLine{ points, route_color , _settings }));

				// подготавливаем данные о названии маршрута
				if (route._is_circular) {
					to_label_render.push_back(RouteLabelData(_projector
					(*route._stops[0].second), _projector(*route._stops[0].second), route_name, route_color));
				}
				else {
					to_label_render.push_back(RouteLabelData(_projector
					(*route._stops[0].second), _projector(*route._stops[route._stops.size() / 2].second), route_name, route_color));
				}
			}

			// добавляем информацию о названии маршрута в рендер
			for (auto& item : to_label_render) {
				container.emplace_back(std::make_unique<RouteLabel>(RouteLabel
					{ item._end_point, item._middle_point, item._route_label, item._route_color, _settings }));
			}

			return *this;
		}

		// отрисовка названий остановок
		MapRenderer& MapRenderer::PointRender(std::vector<std::unique_ptr<svg::Drawable>>& container) {

			for (const auto& stop : _unique_stops)
			{
				container.emplace_back(std::make_unique<RoutePoint>
					(RoutePoint{ _projector(stop.second), _settings }));
			}

			return *this;
		}

		// отрисовка точек остановок
		MapRenderer& MapRenderer::PointLabelRender(std::vector<std::unique_ptr<svg::Drawable>>& container) {

			for (const auto& stop : _unique_stops)
			{
				container.emplace_back(std::make_unique<PointLabel>
					(PointLabel{ _projector(stop.second), stop.first , _settings }));
			}

			return *this;
		}

		// модуль отрисовки
		void MapRenderer::RendererProcess() {

			std::vector<std::unique_ptr<svg::Drawable>> picture_;   

			RouteRender(picture_);                           // отрисовываем линии маршрутов
			PointRender(picture_);                           // отрисовываем точки остановок
			PointLabelRender(picture_);                      // отрисовываем названия остановок

			svg::Document map;
			DrawPicture(picture_, map);
			map.Render(_output);

		}

		// возвращает следующий цвет из цветовой палитры
		const svg::Color MapRenderer::GetColorFromPallete() {
			if (_pallette_item == _settings._color_palette.size())
			{
				_pallette_item = 0;
			}
			return _settings._color_palette[_pallette_item++];
		}

		// сбрасывает счетчик элементов палитры
		void MapRenderer::ResetPallette() {
			_pallette_item = 0;
		}

		// ---------------------------- class MapRender END -----------------------------------------

	} //namespace map_renderer

} // nsmespace transport_catalogue