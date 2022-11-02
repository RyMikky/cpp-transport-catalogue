#include "serialization.h"

namespace transport_catalogue {

	namespace serial_handler {

		// конструктор для вызова из обработчика запросов
		SerialHandler::SerialHandler(transport_catalogue::TransportCatalogue& transport_catalogue
			, router::RouterSettings& router_settings, map_renderer::RendererSettings& renderer_settings)
			: _transport_catalogue(transport_catalogue)
			, _router_settings(router_settings)
			, _renderer_settings(renderer_settings) {
		}

		// назначить транспортный каталог
		SerialHandler& SerialHandler::SetTransporCatalogue
			(transport_catalogue::TransportCatalogue& catalogue) {
			_transport_catalogue = catalogue;
			return *this;
		}

		// назначить настройки рендера
		SerialHandler& SerialHandler::SetRendererSettings
			(map_renderer::RendererSettings& settings) {
			_renderer_settings = settings;
			return *this;
		}

		// назначить настройки роутера
		SerialHandler& SerialHandler::SetRouterSettings
			(router::RouterSettings& settings) {
			_router_settings = settings;
			return *this;
		}

		// назначить готовый роутер
		SerialHandler& SerialHandler::SetTransportRouter(std::shared_ptr<router::TransportRouter> router) {
			_transport_router = router;
			return *this;
		}

		// подготовить данные каталога для сериализации
		SerialHandler& SerialHandler::GetDataFromCatalogue() {

			// загружаем данные по остановкам
			bool stops = SerialHandler::ToOutputStopsDataDecode(); assert(stops);
			// загружаем данные по маршрутам
			bool distances = SerialHandler::ToOutputDistancesDataDecode(); assert(distances);
			// загружаем данные по маршрутам
			bool buses = SerialHandler::ToOutputBusesDataDecode(); assert(buses);
			// загружаем данные по настройкам рендера
			bool renderer = SerialHandler::ToOutputRendererSettingsDecode(); assert(renderer);
			// загружаем данные по настройкам роутера
			bool router = SerialHandler::ToOutputRouterSettingsDecode(); assert(router);

			if (_transport_router) {
				bool router_data = SerialHandler::ToOutputRouterDataDecode(); assert(router_data);
			}

			// обновляем флаги готовности к сериализации
			_is_catalogue_data_prepeared_to_serialize = true;
			_is_catalogue_data_income_from_outside = false;

			// устанавливаем флаг, что данные выгружены и сериализированы
			_transport_catalogue.SetDataStatus(UPLOADED);

			return *this;
		}

		// подготовить данные роутра для сериализации
		SerialHandler& SerialHandler::GetDataFromRouter() {

			// загружаем данные из роутера
			bool router_data = SerialHandler::ToOutputRouterDataDecode(); assert(router_data);

			// обновляем флаги готовности к сериализации
			_is_router_data_prepeared_to_serialize = true;
			_is_router_data_income_from_outside = false;

			// устанавливаем флаг, что данные выгружены и сериализированы
			_transport_router->SetRouterDataStatus(router::UPLOADED);

			return *this;
		}

		// изменить данные в каталоге
		SerialHandler& SerialHandler::ApplyDataToCatalogue() {

			if (!_is_catalogue_data_income_from_outside && _is_catalogue_data_prepeared_to_serialize) {
				std::cerr << "Data from serialize is NOT income"sv << std::endl;
				assert(_is_catalogue_data_income_from_outside);
			}

			// предварительно удаляем все возмоэные данные из каталога
			_transport_catalogue.ClearAllData();
			
			// загружаем данные по остановкам
			bool stops = SerialHandler::ToCatalogueStopsDataDecode(); assert(stops);
			// загружаем данные по расстояниям
			bool distances = SerialHandler::ToCatalogueDistancesDataDecode(); assert(distances);
			// загружаем данные по маршрутам
			bool buses = SerialHandler::ToCatalogueBusesDataDecode(); assert(buses);
			// загружаем данные по настройкам рендера
			bool renderer = SerialHandler::ToCatalogueRendererSettingsDecode(); assert(renderer);
			// загружаем данные по настройкам роутера
			bool router = SerialHandler::ToCatalogueRouterSettingsDecode(); assert(router);

			// устанавливаем флаг, что данные десериализированы и загружены
			_transport_catalogue.SetDataStatus(DOWNLOADED);

			return *this;
		}

		// загрузить данные в роутер
		SerialHandler& SerialHandler::ApplyDataToRouter() {

			if (!_is_catalogue_data_income_from_outside && _is_catalogue_data_prepeared_to_serialize) {
				std::cerr << "Data from serialize is NOT income"sv << std::endl;
				assert(_is_catalogue_data_income_from_outside);
			}

			// загружаем данные в роутер
			bool router = SerialHandler::ToCatalogueRouterDataDecode(); assert(router);

			// устанавливаем флаг, что данные десериализированы и загружены
			_transport_router->SetRouterDataStatus(router::DOWNLOADED);

			return *this;
		}

		// выгрузить подготовленные данные в поток
		bool SerialHandler::UploadDataToOstream(std::ostream& output) {

			// если какие-то данные готовы к сериализации, то можно запускать процесс
			if (_is_catalogue_data_prepeared_to_serialize || _is_router_data_prepeared_to_serialize) {
				return _serial_data.SerializeToOstream(&output);
			}
			std::cerr << "Data to serialize is NOT prepeard"sv << std::endl;
			return false;
		}

		// загрузить данные из потока
		bool SerialHandler::DownloadDataFromIstream(std::istream& input) {

			_serial_data.Clear();                                            // очищаем все поля для чистой загрузки
			_is_router_data_prepeared_to_serialize = false;                  // убираем флаг готовности данных роутера к сериализации
			_is_catalogue_data_prepeared_to_serialize = false;               // убираем флаг готовности данных каталога к сериализации

			if (_serial_data.ParseFromIstream(&input)) {

				if (_serial_data.has_router_data()) {                        // если материалы получены включая данные роутера
					return _is_catalogue_data_income_from_outside = _is_router_data_income_from_outside = true;
				}                                                            // иначе устанавливаем только один флаг
				return _is_catalogue_data_income_from_outside = true;
			}

			return false;
		}

		// возврашает флаг готовности данных каталога к выгрузке
		bool SerialHandler::IsCatalogueDataPrepearedToSerialize() const {
			return _is_catalogue_data_prepeared_to_serialize;
		}
		// возврашает флаг получения данных каталога и готовности к загрузке
		bool SerialHandler::IsCatalogueDataIncomeFromOutside() const {
			return _is_catalogue_data_income_from_outside;
		}
		// возврашает флаг готовности данных роутера к выгрузке
		bool SerialHandler::IsRouterDataPrepearedToSerialize() const {
			return _is_router_data_prepeared_to_serialize;
		}
		// возврашает флаг получения данных роутера и готовности к загрузке
		bool SerialHandler::IsRouterDataIncomeFromOutside() const {
			return _is_router_data_income_from_outside;
		}

		// декодирование цвета по типу
		void SerialHandler::ToOutputColorDecode(const svg::Color& source_color, 
			transport_catalogue_serialize::Color* serial_color) {

			if (std::holds_alternative<svg::Rgb>(source_color)) {

				serial_color->set_is_rgb(true);
				serial_color->set_is_rgba(false);
				serial_color->set_is_title(false);

				svg::Rgb color = std::get<svg::Rgb>(source_color);

				serial_color->clear_color_label();
				serial_color->set_color_component_r(color.red);
				serial_color->set_color_component_g(color.green);
				serial_color->set_color_component_b(color.blue);
				serial_color->clear_color_component_a();
			}
			else if (std::holds_alternative<svg::Rgba>(source_color)) {

				serial_color->set_is_rgb(false);
				serial_color->set_is_rgba(true);
				serial_color->set_is_title(false);

				svg::Rgba color = std::get<svg::Rgba>(source_color);

				serial_color->clear_color_label();
				serial_color->set_color_component_r(color.red);
				serial_color->set_color_component_g(color.green);
				serial_color->set_color_component_b(color.blue);
				serial_color->set_color_component_a(color.opacity);
			}
			else if (std::holds_alternative<std::string>(source_color)) {

				serial_color->set_is_rgb(false);
				serial_color->set_is_rgba(false);
				serial_color->set_is_title(true);

				std::string color = std::get<std::string>(source_color);

				serial_color->set_color_label(color);
				serial_color->clear_color_component_r();
				serial_color->clear_color_component_g();
				serial_color->clear_color_component_b();
				serial_color->clear_color_component_a();
			}
		}
		// подготовка данных по графам роутера
		bool SerialHandler::ToOutputGraphsDecode(transport_catalogue_serialize::RouterData* serial_router_data) {
			try
			{
				// получаем ссылку на сформированные данные графов роутера
				const auto& source_graphs = _transport_router->GetRouterGraphs();

				// перебираем по очереди все сконфигурированные графы
				for (size_t i = 0; i != source_graphs.GetEdgeCount(); ++i) {
					// добавляем очередной граф
					auto serial_edge = serial_router_data->add_router_edges();

					// назначаем тип графа
					(source_graphs.GetEdge(i).GetEdgeType() == graph::wait) ? serial_edge->set_edge_type("wait") : serial_edge->set_edge_type("move");

					// назначаем id полей from и to
					serial_edge->set_edge_from(source_graphs.GetEdge(i).GetVertexFromId());
					serial_edge->set_edge_to(source_graphs.GetEdge(i).GetVertexToId());

					// назначаем вес графа
					serial_edge->set_edge_weight(source_graphs.GetEdge(i).GetEdgeWeight());
					// назначаем имя графа
					serial_edge->set_edge_name(std::string(source_graphs.GetEdge(i).GetEdgeName()));
					// назначаем количество пересадок
					serial_edge->set_span_count(source_graphs.GetEdge(i).GetEdgeSpanCount());

				}
			}
			catch (const std::exception&)
			{
				std::cerr << "Renderer graphs data serialization is Fail"sv << std::endl;
				return false;                                         // выходим с фолсом
			}

			return true;
		}
		// подготовить данные по остановкам
		bool SerialHandler::ToOutputStopsDataDecode() {

			try
			{
				// очищаем текущие возможные данные массива сериализации по остановкам
				_serial_data.clear_stops_data();
				
				// проходим по всему массиву остановок
				for (auto source_stop : _transport_catalogue.GetAllStopsData())
				{
					// добавляем новую остановку в сериализированные данные
					auto serial_stop = _serial_data.add_stops_data();
					// задаём имя остановки в сериализированном массиве
					serial_stop->set_stop_name(source_stop->GetStopName());

					// получаем указатель на координаты остановки в сериализированном массиве
					auto serial_stop_coords = serial_stop->mutable_stop_coordinates();
					// задаём широту и долготу соответственно
					serial_stop_coords->set_latitude(source_stop->GetStopCoordinates().GetLatitude());
					serial_stop_coords->set_longitude(source_stop->GetStopCoordinates().GetLongitude());
				}
			}
			catch (const std::exception&)
			{
				std::cerr << "Stops data serialization is Fail"sv << std::endl;
				std::cerr << "Serial stops data is cleared"sv << std::endl;
				_serial_data.clear_stops_data();         // очищаем то, что получилось сделать 
				return false;                            // выходим с фолсом
			}

			return true;
		}
		// подготовить данные по маршрутам
		bool SerialHandler::ToOutputBusesDataDecode() {

			try
			{
				// очищаем текущие возможные данные массива сериализации по маршрутам
				_serial_data.clear_buses_data();

				// проходим по всему массиву маршрутов
				for (auto source_bus : _transport_catalogue.GetAllBusesData())
				{
					// добавляем новый маршрут в сериализированные данные
					auto serial_bus = _serial_data.add_buses_data();
					// задаём имя маршрута в сериализированном массиве
					serial_bus->set_bus_name(source_bus->GetRouteName());

					// получаем указатель на остановки маршрута в сериализированном массиве
					auto serial_bus_stops = serial_bus->mutable_bus_stops();
					// заполняем названия остановок маршрута в сериализированном массиве
					for (auto stop : source_bus->GetStopsPtrs()) {
						serial_bus_stops->Add(std::move(std::string(stop->GetStopName())));
					}

					// задаём количество уникальных остановок маршрута в сериализированном массиве
					serial_bus->set_unique_stops_qty(source_bus->GetUniqueStopsQty());

					// задаём геометрическую протяженность маршрута в сериализированном массиве
					serial_bus->set_geo_route_length(source_bus->GetGeoRouteLength());

					// задаём действительную протяженность маршрута в сериализированном массиве
					serial_bus->set_real_route_length(source_bus->GetRealRouteLength());

					// задаём отклонение расстояний маршрута в сериализированном массиве
					serial_bus->set_curvature(source_bus->GetCurvature());

					// задаём флаг типа маршрута в сериализированном массиве
					serial_bus->set_is_circular(source_bus->GetIsCircular());
				}
			}
			catch (const std::exception&)
			{
				std::cerr << "Buses data serialization is Fail"sv << std::endl;
				std::cerr << "Serial buses data is cleared"sv << std::endl;
				_serial_data.clear_buses_data();         // очищаем то, что получилось сделать 
				return false;                            // выходим с фолсом
			}

			return true;
		}
		// подготовить данные по дистанциям
		bool SerialHandler::ToOutputDistancesDataDecode() {

			try
			{
				// очищаем текущие возможные данные массива сериализации по расстояниям
				_serial_data.clear_distances_data();

				// проходим по всему массиву расстояний
				for (auto source_distance : _transport_catalogue.GetAllDistancesData())
				{
					// добавляем новую дистанцию в сериализированные данные
					auto serial_distance = _serial_data.add_distances_data();

					// задаём название остановки откуда
					serial_distance->set_from(source_distance.first.first->_name);
					// задаём название остановки куда
					serial_distance->set_to(source_distance.first.second->_name);
					// задаём величину расстояния
					serial_distance->set_range(source_distance.second);
				}
			}
			catch (const std::exception&)
			{
				std::cerr << "Distances data serialization is Fail"sv << std::endl;
				std::cerr << "Serial distances data is cleared"sv << std::endl;
				_serial_data.clear_distances_data();     // очищаем то, что получилось сделать 
				return false;                            // выходим с фолсом
			}

			return true;
		}
		// подготовить данные по рендеру
		bool SerialHandler::ToOutputRendererSettingsDecode() {

			if (!_renderer_settings.IsDefault()) 
			{
				try
				{
					// очищаем текущие возможные данные массива сериализации по настройкам рендера
					_serial_data.clear_renderer_settings();

					// поулчаем указатель на текущие данные настроек рендера
					auto serial_renderer_settings = _serial_data.mutable_renderer_settings();

					// подготавливаем данные о ширине, высоте отрисовки изображения
					serial_renderer_settings->set_width(_renderer_settings.GetWight());
					serial_renderer_settings->set_height(_renderer_settings.GetHeight());

					// подготавливаем данные о величине отступа краёв карты от границ SVG-документа
					serial_renderer_settings->set_padding(_renderer_settings.GetPadding());
					// подготавливаем данные о толщине линий отрисовки автобусных маршрутов
					serial_renderer_settings->set_line_width(_renderer_settings.GetLineWidth());
					// подготавливаем данные о радиусе окружностей отрисовки остановок
					serial_renderer_settings->set_stop_radius(_renderer_settings.GetStopRadius());

					// подготавливаем данные о размере текста отрисовки названий маршрутов
					serial_renderer_settings->set_bus_label_font_size(_renderer_settings.GetBusLabelFontSize());
					// подготавливаем данные о величине смещение надписи с названием маршрута по оси Х
					serial_renderer_settings->set_bus_label_offset_x(_renderer_settings.GetBusLabelOffSet().GetDX());
					// подготавливаем данные о величине смещение надписи с названием маршрута по оси Y
					serial_renderer_settings->set_bus_label_offset_y(_renderer_settings.GetBusLabelOffSet().GetDY());

					// подготавливаем данные о размере текста отрисовки названий остановок
					serial_renderer_settings->set_stop_label_font_size(_renderer_settings.GetStopLabelFontSize());
					// подготавливаем данные о величине смещение надписи с названием остановки по оси Х
					serial_renderer_settings->set_stop_label_offset_x(_renderer_settings.GetStopLabelOffSet().GetDX());
					// подготавливаем данные о величине смещение надписи с названием остановки по оси Y
					serial_renderer_settings->set_stop_label_offset_y(_renderer_settings.GetStopLabelOffSet().GetDY());

					// получаем указатель на подготовленные данные о цвете подложки под названиями
					ToOutputColorDecode(_renderer_settings.GetUnderlaterColor(), serial_renderer_settings->mutable_underlayer_color());
					// подготавливаем данные о толщине подложки под названиями остановок и маршрутов
					serial_renderer_settings->set_underlayer_width(_renderer_settings.GetUnderlayerWidth());

					// предварительно очищаем палитру в блоке подготовленных данных
					serial_renderer_settings->clear_color_palette();
					// перебираем все цвета по имеющимся настройкам
					for (const svg::Color& color : _renderer_settings.GetColorPalette()) {
						ToOutputColorDecode(color, serial_renderer_settings->add_color_palette());
					}

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer settings serialization is Fail"sv << std::endl;
					std::cerr << "Serial renderer settings data is cleared"sv << std::endl;
					_serial_data.clear_renderer_settings();  // очищаем то, что получилось сделать 
					return false;                            // выходим с фолсом
				}

				return true;
			}

			_serial_data.clear_renderer_settings();          // очищаем файл настроек и выходим 
			return true;
		}
		// подготовить данные по роутеру
		bool SerialHandler::ToOutputRouterSettingsDecode() {

			if (!_router_settings.IsDefault())
			{
				try
				{
					// очищаем текущие возможные данные массива сериализации по настройкам роутера
					_serial_data.clear_router_settings();

					// поулчаем указатель на текущие данные настроек роутера
					auto serial_router_settings = _serial_data.mutable_router_settings();

					// подготавливаем данные о времени ожидания автобуса
					serial_router_settings->set_bus_wait_time(_router_settings.GetBusWaitTime());
					// подготавливаем данные о скорости движения автобуса
					serial_router_settings->set_bus_velocity(_router_settings.GetBusVelocity());

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer settings serialization is Fail"sv << std::endl;
					std::cerr << "Serial renderer settings data is cleared"sv << std::endl;
					_serial_data.clear_router_settings();    // очищаем то, что получилось сделать 
					return false;                            // выходим с фолсом
				}

				return true;
			}

			_serial_data.clear_router_settings();            // очищаем файл настроек и выходим 
			return true;
		}
		// подготовить данные по базе роутера
		bool SerialHandler::ToOutputRouterDataDecode() {
			if (_transport_router)
			{
				bool graphs = false;

				try
				{
					// очищаем текущие возможные данные массива сериализации по данным роутера
					_serial_data.clear_router_data();
					// получаем указатель на сериализованные данные графов роутера
					auto serial_router_data = _serial_data.mutable_router_data();
					
					// загружаем колчиество вершин графов
					serial_router_data->set_vertex_count(_transport_router->GetRouterGraphs().GetVertexCount());

					// загружаем данные по графам
					graphs = ToOutputGraphsDecode(serial_router_data);

					// загружаем данные по точкам ожидания
					for (auto item : _transport_router->GetRouterWaitPoints()) {
						// добавляем очередную точку ожидания в блок сериализованных данных
						auto wait_point = serial_router_data->add_router_wait_points();

						wait_point->set_wait_point_name(std::string(item.first));         // передаем название точки
						wait_point->set_wait_point_id(item.second);                      // передаем id точки
					}

					// загружаем данные по точкам перемещения
					for (auto item : _transport_router->GetRouterMovePoints()) {
						// добавляем очередную точку ожидания в блок сериализованных данных
						auto move_point = serial_router_data->add_router_move_points();

						move_point->set_move_point_name(std::string(item.first));         // передаем название точки
						move_point->set_move_point_id(item.second);                      // передаем id точки
					}

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer data serialization is Fail"sv << std::endl;
					std::cerr << "Serial renderer data data is cleared"sv << std::endl;
					_serial_data.clear_router_data();        // очищаем то, что получилось сделать 
					return false;                            // выходим с фолсом
				}

				if (graphs) return true;
			}

			return false;
		}

		// декодирование цвета по типу
		svg::Color SerialHandler::ToCatalogueColorDecode(
			const transport_catalogue_serialize::Color& serial_color) {

			// готовим результирующий объект
			svg::Color result;

			// если тип svg::Rgb
			if (serial_color.is_rgb()) {
				result = std::move(svg::Rgb(
					serial_color.color_component_r(),
					serial_color.color_component_g(),
					serial_color.color_component_b()));
			}
			// если тип svg::Rgba
			else if (serial_color.is_rgba()) {
				result = std::move(svg::Rgba(
					serial_color.color_component_r(),
					serial_color.color_component_g(),
					serial_color.color_component_b(),
					serial_color.color_component_a()));
			}
			// если тип std::string
			else if (serial_color.is_title()) {
				result = std::move(std::string(serial_color.color_label()));
			}

			// напрямую передаём созданный объект
			return std::move(result);
		}
		// загрузить данные по остановкам
		bool SerialHandler::ToCatalogueStopsDataDecode() {

			try
			{
				// обрабатываем полученный массив данных по остановкам
				for (const auto& stop : _serial_data.stops_data()) {

					// добавляем остановки
					_transport_catalogue.AddStop(std::move(
						transport_catalogue::Stop()
						.SetStopName(stop.stop_name())
						.SetStopCoordinates({ 
							stop.stop_coordinates().latitude(),
							stop.stop_coordinates().longitude() })
							));	
				}

				return true;
			}
			catch (const std::exception&)
			{
				std::cerr << "Stops data deserialization is Fail"sv << std::endl;
				return false;                                 // выходим с фолсом
			}

		}
		// загрузить данные по маршрутам
		bool SerialHandler::ToCatalogueBusesDataDecode() {
			try
			{
				// обрабатываем полученный массив данных по маршрутам
				for (const auto& bus : _serial_data.buses_data()) {

					// подготавливаем массив указателей на остановки маршрута
					std::vector<StopPtr> bus_stops;
					// перебираем данные в каталоге
					for (const auto& stop : bus.bus_stops()) {
						bus_stops.push_back(_transport_catalogue.GetStopByName(stop));
					}

					// добавляем данные по маршруту в каталог
					_transport_catalogue.AddRouteFromSerializer(std::move(
						transport_catalogue::Route()
						.SetRouteName(bus.bus_name())
						.SetStopsPtrs(std::move(bus_stops))
						.SetUniqueStopsQty(bus.unique_stops_qty())
						.SetGeoRouteLength(bus.geo_route_length())
						.SetRealRouteLength(bus.real_route_length())
						.SetCurvature(bus.curvature())
						.SetIsCircular(bus.is_circular())
					));
				}

				return true;
			}
			catch (const std::exception&)
			{
				std::cerr << "Stops data deserialization is Fail"sv << std::endl;
				return false;                                 // выходим с фолсом
			}
		}
		// загрузить данные по дистанциям
		bool SerialHandler::ToCatalogueDistancesDataDecode() {
			
			try
			{
				// обрабатываем полученный массив данных по расстояниям
				for (const auto& distance : _serial_data.distances_data()) {

					// добавляем дистанции в каталог
					_transport_catalogue.AddDistance(
						_transport_catalogue.GetStopByName(distance.from()), 
						_transport_catalogue.GetStopByName(distance.to()), distance.range());
				}

				return true;
			}
			catch (const std::exception&)
			{
				std::cerr << "Distances data deserialization is Fail"sv << std::endl;
				return false;                                 // выходим с фолсом
			}

		}
		// загрузить данные по рендеру
		bool SerialHandler::ToCatalogueRendererSettingsDecode() {

			// проверяем наличие данных по настройкам роутера
			if (_serial_data.has_renderer_settings())
			{
				try
				{
					// берем ссылку на полученные данные настроек рендера
					auto serial_renderer_settings = _serial_data.renderer_settings();

					// загружаем данные о ширине и высоте отрисовки изображения
					_renderer_settings.SetWidth(serial_renderer_settings.width());
					_renderer_settings.SetHeight(serial_renderer_settings.height());

					// загружаем данные о величине отступа краёв карты от границ SVG-документа
					_renderer_settings.SetPadding(serial_renderer_settings.padding());
					// загружаем данные о толщине линий отрисовки автобусных маршрутов
					_renderer_settings.SetLineWidth(serial_renderer_settings.line_width());
					// загружаем данные о радиусе окружностей отрисовки остановок
					_renderer_settings.SetStopRadius(serial_renderer_settings.stop_radius());

					// загружаем данные о размере текста отрисовки названий маршрутов
					_renderer_settings.SetBusLabelFont(serial_renderer_settings.bus_label_font_size());
					// загружаем данные о величине смещение надписи с названием маршрута по осям Х и Y
					_renderer_settings.SetBusLabelOffset(
						{ serial_renderer_settings.bus_label_offset_x(), serial_renderer_settings.bus_label_offset_y() });

					// загружаем данные о размере текста отрисовки названий остановок
					_renderer_settings.SetStopLabelFont(serial_renderer_settings.stop_label_font_size());
					// загружаем данные о величине смещение надписи с названием остановки по осям Х и Y
					_renderer_settings.SetStopLabelOffset(
						{ serial_renderer_settings.stop_label_offset_x(), serial_renderer_settings.stop_label_offset_y() });

					// загружаем данные о цвете подложки под названиями остановок и маршрутов
					_renderer_settings.SetUnderlayerColor(
						std::move(ToCatalogueColorDecode(serial_renderer_settings.underlayer_color())));

					// загружаем данные о толщине подложки под названиями остановок и маршрутов
					_renderer_settings.SetUnderlayerWidth(serial_renderer_settings.underlayer_width());

					// предварительно очищаем палитру в блоке настроек рендера
					_renderer_settings.ResetColorPalette();
					// перебираем цвета палитры по переданным данным
					for (const auto& color : serial_renderer_settings.color_palette()) {
						// загружаем цвета в палитру настроек рендера
						_renderer_settings.AddColorInPalette(std::move(ToCatalogueColorDecode(color)));
					}

					// очищаем полученные данные массива сериализации по настройкам рендера
					_serial_data.clear_renderer_settings();

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer settings deserialization is Fail"sv << std::endl;
					return false;                                        // выходим с фолсом
				}

				return true;
			}

			return true;
		}
		// загрузить данные по роутеру
		bool SerialHandler::ToCatalogueRouterSettingsDecode() {

			// проверяем наличие данных по настройкам роутера
			if (_serial_data.has_router_settings()) {

				try
				{
					// берем ссылку на полученные данные настроек роутера
					auto serial_router_settings = _serial_data.router_settings();

					// загружаеим данные о времени ожидания автобуса
					_router_settings.SetBusWaitTime(serial_router_settings.bus_wait_time());
					// загружаеим данные о скорости движения автобуса
					_router_settings.SetBusVelocity(serial_router_settings.bus_velocity());

					// очищаем полученные данные массива сериализации по настройкам роутера
					_serial_data.clear_router_settings();

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer settings deserialization is Fail"sv << std::endl;
					return false;                                        // выходим с фолсом
				}

				return true;

			}

			return true;
		}
		// загрузить данные по базе роутера
		bool SerialHandler::ToCatalogueRouterDataDecode() {

			// проверяем наличие данных по настройкам роутера
			if (_serial_data.has_router_data()) {

				try
				{
					// берем ссылку на полученные данных роутера
					auto serial_router_data = _serial_data.router_data();

					// создаём массив новых графов с помощью количества вершин из сериализованных данных
					graph::DirectedWeightedGraph<double> graphs(serial_router_data.vertex_count());

					// получаем массив графов
					auto edges = serial_router_data.router_edges();
					// набираем новый массив графов
					for (int i = 0; i != edges.size(); ++i) {
						// добавляем графы
						graphs.AddEdge(graph::Edge<double>()
							.SetEdgeType(
								(edges[i].edge_type() == "wait") ? graph::wait : graph::move)
							.SetVertexFromId(edges[i].edge_from())
							.SetVertexToId(edges[i].edge_to())
							.SetEdgeWeight(edges[i].edge_weight())
							.SetEdgeName(std::string(edges[i].edge_name()))
							.SetEdgeSpanCount(edges[i].span_count()));
					}

					// массив точек ожидания
					std::unordered_map<std::string_view, size_t> wait_points;           
					// получаем массив точек ожидания из сериализованных данных
					auto wait_point = serial_router_data.router_wait_points();
					// набираем новый массив точек
					for (int i = 0; i != wait_point.size(); ++i) {
						// так как в точках std::string_view - то указывать их можно только через имеющиеся остановки
						wait_points[_transport_catalogue
							.GetStopByName(wait_point[i]
								.wait_point_name())->_name] = wait_point[i].wait_point_id();
					}

					// массив точек перемещения
					std::unordered_map<std::string_view, size_t> move_points;
					// получаем массив точек ожидания из сериализованных данных
					auto move_point = serial_router_data.router_move_points();
					// набираем новый массив точек
					for (int i = 0; i != move_point.size(); ++i) {
						// так как в точках std::string_view - то указываьб их можно только через имеющиеся остановки
						move_points[_transport_catalogue
							.GetStopByName(move_point[i]
								.move_point_name())->_name] = move_point[i].move_point_id();
					}

					// загружаем полученные графы
					_transport_router->SetRouterGraphs(std::move(graphs));
					// загружаем полученные точки ожидания
					_transport_router->SetRouterWaitPoints(std::move(wait_points));
					// загружаем полученные точки перемещения
					_transport_router->SetRouterMovePoints(std::move(move_points));
					// очищаем полученные данные массива сериализации по данным роутера
					_serial_data.clear_router_data();

				}
				catch (const std::exception&)
				{
					std::cerr << "Renderer data deserialization is Fail"sv << std::endl;
					return false;                                    // выходим с фолсом
				}

				return true;

			}

			return true;
		}

	} // namespace serial_handler

} // namespace transport_catalogue