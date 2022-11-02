#include "transport_catalogue.h" 

namespace transport_catalogue
{

	// Расчёт длины маршрута
	void TransportCatalogue::RangeCalculate(Route& route) {
		int stops_num = static_cast<int>(route._stops.size());
		for (int i = 0; i < stops_num - 1; ++i) {
			route._geo_route_length += geo::ComputeDistance(route._stops[i]->_coords, route._stops[i + 1]->_coords);
			route._real_route_length += GetDistance(route._stops[i], route._stops[i + 1]);
		}
		route._curvature = route._real_route_length / route._geo_route_length;
	}

	// Добавить остановку
	void TransportCatalogue::AddStop(Stop&& stop) {
		// добавляем если такой остановки нет в базе
		if (_all_stops_map.count(GetStopName(&stop)) == 0) {
			// заполнение основной бащы
			auto& ref = _all_stops_data.emplace_back(std::move(stop));
			// заполнение базы для роутера
			_all_stops_to_router.push_back(&ref);
			// заполнение мапы для ускоренного поиска
			_all_stops_map.insert({ std::string_view(ref._name), &ref });
		}
	}

	// Добавить маршрут
	void TransportCatalogue::AddRoute(Route&& route) {
		// добавляем если такого маршрута нет в базе
		if (_all_buses_map.count(route._route_name) == 0) {
			// заполнение основной базы
			auto& ref = _all_buses_data.emplace_back(std::move(route));
			// заполнение базы для роутера
			_all_buses_to_router.push_back(&ref);
			// заполнение мапы для ускоренного поиска
			_all_buses_map.insert({ std::string_view(ref._route_name), &ref });

			// вычисляем количество уникальных остановок
			std::vector<StopPtr> tmp = ref._stops;
			std::sort(tmp.begin(), tmp.end());
			auto last = std::unique(tmp.begin(), tmp.end());
			// может быть два случая, когда конечная остановка равна начальной и когда все остановки уникальны
			ref._unique_stops_qty = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());

			// проверка типа маршрута
			if (!ref._is_circular) {
				// достраиваем обратный маршрут для расчёта расстояния
				for (int i = ref._stops.size() - 2; i >= 0; --i) {
					ref._stops.push_back(ref._stops[i]);
				}
			}

			// расчёт длины маршрута
			if (ref._stops.size() > 1) {
				RangeCalculate(ref);
			}
		}
	}

	// Добавить маршрут из блока сериализации
	void TransportCatalogue::AddRouteFromSerializer(Route&& route) {
		// добавляем если такого маршрута нет в базе
		if (_all_buses_map.count(route._route_name) == 0) {
			// заполнение основной базы
			auto& ref = _all_buses_data.emplace_back(std::move(route));
			// заполнение базы для роутера
			_all_buses_to_router.push_back(&ref);
			// заполнение мапы для ускоренного поиска
			_all_buses_map.insert({ std::string_view(ref._route_name), &ref });

			// в методе не выполняются математические расчёты расстояний и т.п, так как в базе всё уже есть
		}
	}

	// Добавить дистанцию между остановками
	void TransportCatalogue::AddDistance(StopPtr stop_from, StopPtr stop_to, size_t dist) {
		if (stop_from != nullptr && stop_to != nullptr) {
			_all_distances_map.insert({ { stop_from, stop_to }, dist });
		}
	}

	// Получить количество остановок в базе
	size_t TransportCatalogue::GetStopsCount() const {
		return _all_stops_data.size();
	}

	// Получить дистанцию в прямом или обратном направлении
	size_t TransportCatalogue::GetDistance(StopPtr stop_from, StopPtr stop_to) {
		// получаем дистанцию в прямом направлении
		size_t result = GetDistanceSimple(stop_from, stop_to);
		// ищем дистанцию в обратном направлении, если в прямом направлении равно "0"
		return (result > 0 ? result : GetDistanceSimple(stop_to, stop_from));
	}

	// Получить дистанцию в прямом направлении
	size_t TransportCatalogue::GetDistanceSimple(StopPtr stop_from, StopPtr stop_to) {
		if (_all_distances_map.count({ stop_from, stop_to }) > 0) {
			return _all_distances_map.at({ stop_from, stop_to });
		}
		else {
			return 0U;
		}
	}

	// Вспомогательная функция privat-секции класса
	std::string_view TransportCatalogue::GetStopName(StopPtr stop_ptr) {
		return std::string_view(stop_ptr->_name);
	}
	// Вспомогательная функция privat-секции класса
	std::string_view TransportCatalogue::GetStopName(const Stop stop) {
		return std::string_view(stop._name);
	}
	// Вспомогательная функция privat-секции класса
	std::string_view TransportCatalogue::GetBusName(RoutePtr route_ptr) {
		return std::string_view(route_ptr->_route_name);
	}
	// Вспомогательная функция privat-секции класса
	std::string_view TransportCatalogue::GetBusName(const Route route) {
		return std::string_view(route._route_name);
	}

	// Возвращает указатель на остановку по ее имени
	StopPtr TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
		if (_all_stops_map.count(stop_name) == 0) {
			return nullptr;
		}
		else {
			return _all_stops_map.at(stop_name);
		}
	}

	// Возвращает указатель на маршрут по его имени
	RoutePtr TransportCatalogue::GetRouteByName(const std::string_view bus_name) const {
		if (_all_buses_map.count(bus_name) == 0) {
			return nullptr;
		}
		else {
			return _all_buses_map.at(bus_name);
		}
	}

	// Возвращает указатель на результат запроса о маршруте
	RouteStatPtr TransportCatalogue::GetRouteInfo(const std::string_view route_name) const {
		RoutePtr ptr = GetRouteByName(route_name);
		if (ptr == nullptr) {
			return nullptr;
		}
		return new RouteStat(ptr->_stops.size(), ptr->_unique_stops_qty, 
			ptr->_real_route_length, ptr->_curvature, ptr->_route_name);
	}

	// Возвращает указатель на результат запроса о остановке
	StopStatPtr TransportCatalogue::GetBusesForStopInfo(const std::string_view stop_name) const {
		StopPtr ptr = GetStopByName(stop_name);

		if (ptr == nullptr) {
			return nullptr;
		}

		std::set<std::string_view> found_buses_sv;
		for (const auto& bus : _all_buses_map) {
			auto tmp = std::find_if(bus.second->_stops.begin(), bus.second->_stops.end(),
				[stop_name](StopPtr curr_stop) {
					return (curr_stop->_name == stop_name);
				});
			if (tmp != bus.second->_stops.end()) {
				found_buses_sv.insert(bus.second->_route_name);
			}
		}
		return new StopStat(stop_name, found_buses_sv);
	}

	// Возвращает данные для рендеринга маршрутов
	std::map<std::string, RendererRequest> TransportCatalogue::GetDataToRenderer() {
		std::map<std::string, RendererRequest> result;

		for (auto& route : _all_buses_data) {
			if (route._stops.size() > 0) {
				RendererRequest data(route._is_circular, route._stops);
				result.emplace(std::make_pair(route._route_name, data));
			}
		}

		return result;
	}

	// Возвращает дек указателей на остановки для роутера
	const std::deque<StopPtr>& TransportCatalogue::GetAllStopsData() const {
		return _all_stops_to_router;
	}

	// Возвращает дек указателей на маршруты для роутера
	const std::deque<RoutePtr>& TransportCatalogue::GetAllBusesData() const {
		return _all_buses_to_router;
	}

	// Возвращает мап указателей на остановки и расстояния между ними
	const DistancesMap& TransportCatalogue::GetAllDistancesData() const {
		return _all_distances_map;
	}

	// Возвращает флаг состояния данных каталога
	CatalogueDataStatus TransportCatalogue::GetCatalogueDataStatus() const {
		return _data_status;
	}

	// Назначение флага состояния данных
	TransportCatalogue& TransportCatalogue::SetDataStatus(CatalogueDataStatus status) {
		_data_status = status;
		return *this;
	}

	// Очищает все поля класса;
	bool TransportCatalogue::ClearAllData() {

		if (_data_status == EMPTY) {
			return false;
		}

		_all_distances_map.clear();                 // очищаем ап расстояния между остановками

		_all_buses_map.clear();                     // очиаем мап указателей маршрутов для быстрого поиска по базе
		_all_buses_to_router.clear();               // очищаем массив указателей на маршруты для роутера
		_all_buses_data.clear();                    // очищаем основной массив маршрутов

		_all_stops_map.clear();                     // очищаем мап указателей остановок для быстрого поиска по базе
		_all_stops_to_router.clear();               // очищаем массив указателей на остановки для роутера
		_all_stops_data.clear();                    // очищаем основной массив данных по остановкам

		_data_status = EMPTY;
		return true;
	}

} // namespace transport_catalogue