#include "transport_catalogue.h" 

namespace transport_catalogue
{

	Stop::Stop(StopPtr other) :
		_name(other->_name), _coords(other->_coords) {
	}

	Stop::Stop(const std::string_view stop_name, const double lat, const double lng) :
		_name(stop_name), _coords(geo::Coordinates{ lat, lng }) {
	}

	bool Stop::operator==(const Stop& other) const {
		return _name == other._name &&
			_coords.lat == other._coords.lat && _coords.lng == other._coords.lng;
	}

	std::size_t StopHasher::operator()(const StopPtr stop_point) const noexcept {
		return hasher_(stop_point->_name) + static_cast<size_t>(stop_point->_coords.lat * 11)
			+ static_cast<size_t>(stop_point->_coords.lng * 22);
	}

	StopStat::StopStat(std::string_view stop_name, std::set<std::string_view>& buses) :
		name(stop_name), buses(buses) {
	}

	std::size_t PairPointersHasher::operator()(const std::pair<StopPtr, StopPtr> pair_of_pointers) const noexcept {
		return hasher_(static_cast<const void*>(pair_of_pointers.first)) * 37
			+ hasher_(static_cast<const void*>(pair_of_pointers.second));
	}

	Route::Route(RoutePtr other) : _route_name(other->_route_name), _stops(other->_stops),
		_unique_stops_qty(other->_unique_stops_qty), _geo_route_length(other->_geo_route_length),
		_real_route_length(other->_real_route_length), _curvature(other->_curvature), 
		_is_circular(other->_is_circular) {
	}

	bool Route::operator==(const Route& other) const {
		return _route_name == other._route_name && _stops == other._stops &&
			_unique_stops_qty == other._unique_stops_qty && _geo_route_length == other._geo_route_length &&
			_real_route_length == other._real_route_length && _curvature == other._curvature &&
			_is_circular == other._is_circular;
	}

	std::size_t RouteHasher::operator()(const RoutePtr route) const noexcept {
		return hasher_(route->_route_name) + static_cast<size_t>(route->_unique_stops_qty * 11)
			+ static_cast<size_t>(route->_real_route_length * 22) + static_cast<size_t>(route->_curvature)
			+ static_cast<size_t>(route->_geo_route_length * 33) + static_cast<size_t>(route->_is_circular);
	}

	RouteStat::RouteStat(size_t stops, size_t unique_stops, size_t real_range, 
		double curvature, std::string_view name) : _stops_on_route(stops), 
		_unique_stops(unique_stops), _real_route_length(real_range), _curvature(curvature), _name(name) {
	}

	void TransportCatalogue::RangeCalculate(Route& route) {
		int stops_num = static_cast<int>(route._stops.size());
		for (int i = 0; i < stops_num - 1; ++i) {
			route._geo_route_length += geo::ComputeDistance(route._stops[i]->_coords, route._stops[i + 1]->_coords);
			route._real_route_length += GetDistance(route._stops[i], route._stops[i + 1]);
		}
		route._curvature = route._real_route_length / route._geo_route_length;
	}

	void TransportCatalogue::AddStop(Stop&& stop) {
		// добавляем если такой остановки нет в базе
		if (_all_stops_map.count(GetStopName(&stop)) == 0) {
			auto& ref = _all_stops_data.emplace_back(std::move(stop));
			_all_stops_map.insert({ std::string_view(ref._name), &ref });
		}
	}

	void TransportCatalogue::AddRoute(Route&& route) {
		// добавляем если такого маршрута нет в базе
		if (_all_buses_map.count(route._route_name) == 0) {
			auto& ref = _all_buses_data.emplace_back(std::move(route));
			_all_buses_map.insert({ std::string_view(ref._route_name), &ref });

			// уникальные остановки
			std::vector<StopPtr> tmp = ref._stops;
			std::sort(tmp.begin(), tmp.end());
			// на случай если вдруг в конце будет начальная остановка
			auto last = std::unique(tmp.begin(), tmp.end());
			ref._unique_stops_qty = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());

			// проверка типа маршрута
			if (!ref._is_circular) {
				// достраиваем обратный маршрут для расчёта расстояния
				for (int i = ref._stops.size() - 2; i >= 0; --i) {
					ref._stops.push_back(ref._stops[i]);
				}
			}

			// длина маршрута
			if (ref._stops.size() > 1) {
				RangeCalculate(ref);
			}
		}
	}

	void TransportCatalogue::AddDistance(StopPtr stop_from, StopPtr stop_to, size_t dist) {
		if (stop_from != nullptr && stop_to != nullptr) {
			_distances_map.insert({ { stop_from, stop_to }, dist });
		}
	}

	size_t TransportCatalogue::GetDistance(StopPtr stop_from, StopPtr stop_to) {
		// получаем дистанцию в прямом направлении
		size_t result = GetDistanceSimple(stop_from, stop_to);
		// ищем дистанцию в обратном направлении, если в прямом направлении равно "0"
		return (result > 0 ? result : GetDistanceSimple(stop_to, stop_from));
	}

	size_t TransportCatalogue::GetDistanceSimple(StopPtr stop_from, StopPtr stop_to) {
		if (_distances_map.count({ stop_from, stop_to }) > 0) {
			return _distances_map.at({ stop_from, stop_to });
		}
		else {
			return 0U;
		}
	}

	std::string_view TransportCatalogue::GetStopName(StopPtr stop_ptr) {
		return std::string_view(stop_ptr->_name);
	}

	std::string_view TransportCatalogue::GetStopName(const Stop stop) {
		return std::string_view(stop._name);
	}

	std::string_view TransportCatalogue::GetBusName(RoutePtr route_ptr) {
		return std::string_view(route_ptr->_route_name);
	}

	std::string_view TransportCatalogue::GetBusName(const Route route) {
		return std::string_view(route._route_name);
	}

	StopPtr TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
		if (_all_stops_map.count(stop_name) == 0) {
			return nullptr;
		}
		else {
			return _all_stops_map.at(stop_name);
		}
	}

	RoutePtr TransportCatalogue::GetRouteByName(const std::string_view bus_name) const {
		if (_all_buses_map.count(bus_name) == 0) {
			return nullptr;
		}
		else {
			return _all_buses_map.at(bus_name);
		}
	}

	RouteStatPtr TransportCatalogue::GetRouteInfo(const std::string_view route_name) const {
		RoutePtr ptr = GetRouteByName(route_name);
		if (ptr == nullptr) {
			return nullptr;
		}
		return new RouteStat(ptr->_stops.size(), ptr->_unique_stops_qty, 
			ptr->_real_route_length, ptr->_curvature, ptr->_route_name);
	}

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

}