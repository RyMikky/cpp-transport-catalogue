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

	Stop& Stop::SetStopName(std::string_view stop_name) {
		_name = std::string(stop_name);
		return *this;
	}

	Stop& Stop::SetStopCoordinates(double lat, double lng) {
		_coords.lat = lat;
		_coords.lng = lng;
		return *this;
	}

	Stop& Stop::SetStopCoordinates(const geo::Coordinates& coords) {
		_coords = coords;
		return *this;
	}

	Stop& Stop::SetStopCoordinates(geo::Coordinates&& coords) {
		_coords = std::move(coords);
		return *this;
	}

	const std::string& Stop::GetStopName() const {
		return _name;
	}

	std::string Stop::GetStopName() {
		return _name;
	}

	const geo::Coordinates& Stop::GetStopCoordinates() const {
		return _coords;
	}

	geo::Coordinates Stop::GetStopCoordinates() {
		return _coords;
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

	Route& Route::SetRouteName(std::string_view route_name) {
		_route_name = std::string(route_name);
		return *this;
	}

	Route& Route::SetStopsPtrs(std::vector<StopPtr>&& stops) {
		_stops = std::move(stops);
		return *this;
	}

	Route& Route::SetUniqueStopsQty(size_t unique) {
		_unique_stops_qty = unique;
		return *this;
	}

	Route& Route::SetGeoRouteLength(double geo_length) {
		_geo_route_length = geo_length;
		return *this;
	}

	Route& Route::SetRealRouteLength(size_t real_length) {
		_real_route_length = real_length;
		return *this;
	}

	Route& Route::SetCurvature(double curvature) {
		_curvature = curvature;
		return *this;
	}

	Route& Route::SetIsCircular(bool circular) {
		_is_circular = circular;
		return *this;
	}

	const std::string& Route::GetRouteName() const {
		return _route_name;
	}

	const std::vector<StopPtr>& Route::GetStopsPtrs() const {
		return _stops;
	}

	size_t Route::GetUniqueStopsQty() const {
		return _unique_stops_qty;
	}

	double Route::GetGeoRouteLength() const {
		return _geo_route_length;
	}

	size_t Route::GetRealRouteLength() const {
		return _real_route_length;
	}

	double Route::GetCurvature() const {
		return _curvature;
	}

	bool Route::GetIsCircular() const {
		return _is_circular;
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

	// ------------------------ struct SimpleRequest ---------------------

	SimpleRequest::SimpleRequest(std::string&& line) : _input_line(line) {
	}

	SimpleRequestPtr SimpleRequest::GetPtr() {
		return this;
	}

	// ------------------------ struct JsonRequest -----------------------

	JsonRequestPtr JsonRequest::GetPtr() {
		return this;
	}


	// ------------------------ struct RendererRequest -----------------

	RendererRequest::RendererRequest(bool type, std::vector<StopPtr> stops) 
		: _is_circular(type), _stops(stops) {
	}

	// ------------------------ struct RendererRequest END -------------


	// ------------------------------- struct RouterItem -----------------------------------

		// Задать название участка маршрута
	RouterItem& RouterItem::SetItemName(std::string_view name) {
		_item_name = name;
		return *this;
	}
	// Задать количество преодаленных остановок на маршруте
	RouterItem& RouterItem::SetItemSpanCount(int count) {
		_span_count = count;
		return *this;
	}
	// Задать время прохождения участка маршрута
	RouterItem& RouterItem::SetItemTime(double time) {
		_time = time;
		return *this;
	}
	// Задать тип участка маршрута
	RouterItem& RouterItem::SetItemEdgeType(graph::EdgeType type) {
		_type = type;
		return *this;
	}

	// Получить название участка маршрута
	std::string_view RouterItem::GetItemName() const {
		return _item_name;
	}
	// Получить количество преодаленных остановок на маршруте
	int RouterItem::GetItemSpanCount() const {
		return _span_count;
	}
	// Получить время прохождения участка маршрута
	double RouterItem::GetItemTime() const {
		return _time;
	}
	// Получить тип участка маршрута
	graph::EdgeType RouterItem::GetItemType() const {
		return _type;
	}


	// ------------------------------- struct RouterItem END -------------------------------

} // namespace transport_catalogue