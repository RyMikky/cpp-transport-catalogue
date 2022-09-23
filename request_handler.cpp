#include "request_handler.h"

namespace transport_catalogue {

	namespace request_handler {

		// ------------------------- class RequestHandler ------------------------

		// ��������� ��������� �������
		RequestHandler& RequestHandler::SetTransportCatalogue(transport_catalogue::TransportCatalogue&& catalogue) {
			_catalogue = std::move(catalogue);

			// ���� ������ ��� ��� ���������������, �� ������������� ������ ��������
			if (_transport_router) {
				_transport_router.get()->ImportRoutingData();
			}

			return *this;
		}

		// ��������� ��������� �������
		RequestHandler& RequestHandler::SetRedererSettings(map_renderer::RendererSettings&& settings) {
			_renderer_settings = std::move(settings);

			// ���� ������ ��� ��� ���������������, �� ������������� ��������� ���������� �������
			if (_map_renderer) {
				_map_renderer.get()->SetRendererSettings(_renderer_settings);
			}

			return *this;
		}

		// ��������� ��������� �������
		RequestHandler& RequestHandler::SetRouterSettings(router::RouterSettings&& settings) {
			_router_settings = std::move(settings);

			// ���� ������ ��� ��� ���������������, �� ������������� ��������� ���������� �������
			if (_transport_router) {
				_transport_router.get()->SetRouterSettings(_router_settings);
			}

			return *this;
		}

		// ������������� � ��������� ��������� �������
		RequestHandler& RequestHandler::EnableMapRenderer() {
			if (!_map_renderer) {
				_map_renderer = std::make_shared<map_renderer::MapRenderer>(_renderer_settings);
			}
			return *this;
		}

		// ������������� � ��������� ��������� �������
		RequestHandler& RequestHandler::EnableTransportRouter() {
			if (!_transport_router) {
				_transport_router = std::make_shared<router::TransportRouter>(_catalogue, _router_settings);
			}
			return *this;
		}

		// �������� ��������� � �������
		RequestHandler& RequestHandler::AddStop(JsonRequestPtr request) {
			_catalogue.AddStopsProcess(MakeStop(request));
			AddDistance(request);
			return *this;
		}

		// �������� ������� � �������
		RequestHandler& RequestHandler::AddRoute(JsonRequestPtr request) {
			_catalogue.AddRoute(std::move(MakeRoute(request)));
			return *this;
		}
		
		// �������� ���������� � �������
		RequestHandler& RequestHandler::AddDistance(JsonRequestPtr request) {
			StopPtr this_stop_ = _catalogue.GetStopByName(request->_name);
			for (auto& dist : request->_distances) {
				_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
			}
			return *this;
		}

		// ���������� ����� �������� �� ���������� ���������
		RequestHandler& RequestHandler::AddStopsProcess(const std::deque<JsonRequestPtr>& requests) {
			for (JsonRequestPtr request : requests) {

				// ������ ��������� � ��������
				_catalogue.AddStopsProcess(MakeStop(request));

				// ��������� ��������� ��������� �������� �� ���������� ������� ����� ���������� ����� ���������� ���� ���������
				if (request->_distances.size() != 0) {
					_current_distance_json_requests[request->_name] = &request->_distances;
				}
			}

			// ����� �� ��������� ��� ���������
			AddDistancesProcess();
			return *this;
		}

		// ���������� ����� �������� �� ���������� ���������
		RequestHandler& RequestHandler::AddRoutesProcess(const std::deque<JsonRequestPtr>& requests) {
			for (JsonRequestPtr request : requests) {
				_catalogue.AddRoute(std::move(MakeRoute(request)));
			}
			return *this;
		}

		// �������� ���������� �� ��������
		RouteStatPtr RequestHandler::GetRouteInfo(JsonRequestPtr request) {
			return _catalogue.GetRouteInfo(request->_name);
		}

		// �������� ���������� �� ���������
		StopStatPtr RequestHandler::GetStopInfo(JsonRequestPtr request) {
			return _catalogue.GetBusesForStopInfo(request->_name);
		}

		// �������� ���������� ��������������
		RouterData RequestHandler::GetRouterData(JsonRequestPtr request) {
			if (!_transport_router) {
				EnableTransportRouter();
			}
			return _transport_router.get()->MakeRoute(request->_from, request->_to);
		}

		// �������� ����� ���������
		RendererData RequestHandler::GetRendererData(JsonRequestPtr request) {
			if (!_map_renderer) {
				EnableMapRenderer();
			}

			for (auto& route : _catalogue.GetDataToRenderer()) {
				_map_renderer.get()->AddRendererData(route);
			}

			_map_renderer.get()->CoordsProject();
			return _map_renderer.get()->StructRendererProcess();
		}

		// ������ ������� �� ������ �������
		Route RequestHandler::MakeRoute(JsonRequestPtr request) {
			Route result;
			result._is_circular = request->_is_circular;
			result._route_name = request->_name;
			for (auto stop : request->_stops) {
				result._stops.push_back(_catalogue.GetStopByName(stop));
			}
			return result;
		}

		// ������ ��������� �� ������ �������
		Stop RequestHandler::MakeStop(JsonRequestPtr request) {
			return { std::string(request->_name), request->_coord.lat, request->_coord.lng };
		}

		// ���������� ����� �������� �� ���������� ��������� �� �������
		RequestHandler& RequestHandler::AddDistancesProcess() {
			if (_current_distance_json_requests.size() != 0) {
				for (auto this_stop : _current_distance_json_requests) {
					StopPtr this_stop_ = _catalogue.GetStopByName(this_stop.first);
					for (auto& dist : *this_stop.second) {
						_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
					}
				}
			}
			_current_distance_json_requests.clear();
			return *this;
		}


		// ------------------------- class RequestHandler END ------------------------

	} // namespace request_handler

} // namespace transport_catalogue