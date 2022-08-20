#include "transport_catalogue.h" 

namespace transport_catalogue
{

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

	std::map<std::string, RendererData> TransportCatalogue::GetRenderData() {
		std::map<std::string, RendererData> result;

		for (auto& route : _all_buses_data) {
			if (route._stops.size() > 0) {
				RendererData data(route._is_circular, route._stops);
				result.emplace(std::make_pair(route._route_name, data));
			}
		}

		return result;
	}
}