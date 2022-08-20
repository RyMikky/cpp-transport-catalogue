#include "domain.h"

namespace transport_catalogue {

	// ------------------------ struct Stop ------------------------------

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
		return _hasher(stop_point->_name) + static_cast<size_t>(stop_point->_coords.lat * 11)
			+ static_cast<size_t>(stop_point->_coords.lng * 22);
	}

	// ------------------------ struct StopStat --------------------------

	StopStat::StopStat(std::string_view stop_name, std::set<std::string_view>& buses) :
		_name(stop_name), _buses(buses) {
	}

	std::size_t PairPointersHasher::operator()(const std::pair<StopPtr, StopPtr> pair_of_pointers) const noexcept {
		return _hasher(static_cast<const void*>(pair_of_pointers.first)) * 37
			+ _hasher(static_cast<const void*>(pair_of_pointers.second));
	}

	// ------------------------ struct Route -----------------------------

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
		return _hasher(route->_route_name) + static_cast<size_t>(route->_unique_stops_qty * 11)
			+ static_cast<size_t>(route->_real_route_length * 22) + static_cast<size_t>(route->_curvature)
			+ static_cast<size_t>(route->_geo_route_length * 33) + static_cast<size_t>(route->_is_circular);
	}

	// ------------------------ struct RouteStat -------------------------

	RouteStat::RouteStat(size_t stops, size_t unique_stops, size_t real_range,
		double curvature, std::string_view name) : _stops_on_route(stops),
		_unique_stops(unique_stops), _real_route_length(real_range), _curvature(curvature), _name(name) {
	}

	// ------------------------ struct RequestSimple ---------------------

	SimpleRequest::SimpleRequest(std::string&& line) : _input_line(line) {
	}

	SimpleRequestPtr SimpleRequest::GetPtr() {
		return this;
	}

	// ------------------------ struct StatRequestSimple -----------------

	StatRequestSimple::StatRequestSimple(std::string&& line) : _input_line(line) {
	}

	StatRequestSimplePtr StatRequestSimple::GetPtr() {
		return this;
	}

	// ------------------------ struct RequestJSON -----------------------

	JsonRequestPtr JsonRequest::GetPtr() {
		return this;
	}

	// ------------------------ struct StatRequestJSON -----------------

	StatRequestJSONPtr StatRequestJSON::GetPtr() {
		return this;
	}

	// ------------------------ struct RenderData ----------------------

	RendererData::RendererData(bool type, std::vector<StopPtr> stops) 
		: _is_circular(type), _stops(stops) {
	}

	// ------------------------ struct RenderData END ------------------

}