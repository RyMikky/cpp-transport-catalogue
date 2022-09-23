#include "request_handler.h"

namespace transport_catalogue {

	namespace request_handler {

		// ------------------------- class RequestHandler ------------------------

		// загрузить сторонний каталог
		RequestHandler& RequestHandler::SetTransportCatalogue(transport_catalogue::TransportCatalogue&& catalogue) {
			_catalogue = std::move(catalogue);

			// если роутер уже был сконфигурирован, то перезагружаем данные роутинга
			if (_transport_router) {
				_transport_router.get()->ImportRoutingData();
			}

			return *this;
		}

		// загрузить настройки рендера
		RequestHandler& RequestHandler::SetRedererSettings(map_renderer::RendererSettings&& settings) {
			_renderer_settings = std::move(settings);

			// если рендер уже был сконфигурирован, то перезагружаем настройки имеющегося рендера
			if (_map_renderer) {
				_map_renderer.get()->SetRendererSettings(_renderer_settings);
			}

			return *this;
		}

		// загрузить настройки роутера
		RequestHandler& RequestHandler::SetRouterSettings(router::RouterSettings&& settings) {
			_router_settings = std::move(settings);

			// если роутер уже был сконфигурирован, то перезагружаем настройки имеющегося роутера
			if (_transport_router) {
				_transport_router.get()->SetRouterSettings(_router_settings);
			}

			return *this;
		}

		// инициализация и первичная настройка рендера
		RequestHandler& RequestHandler::EnableMapRenderer() {
			if (!_map_renderer) {
				_map_renderer = std::make_shared<map_renderer::MapRenderer>(_renderer_settings);
			}
			return *this;
		}

		// инициализация и первичная настройка роутера
		RequestHandler& RequestHandler::EnableTransportRouter() {
			if (!_transport_router) {
				_transport_router = std::make_shared<router::TransportRouter>(_catalogue, _router_settings);
			}
			return *this;
		}

		// добавить остановку в каталог
		RequestHandler& RequestHandler::AddStop(JsonRequestPtr request) {
			_catalogue.AddStopsProcess(MakeStop(request));
			AddDistance(request);
			return *this;
		}

		// добавить маршрут в каталог
		RequestHandler& RequestHandler::AddRoute(JsonRequestPtr request) {
			_catalogue.AddRoute(std::move(MakeRoute(request)));
			return *this;
		}
		
		// добавить расстояние в каталог
		RequestHandler& RequestHandler::AddDistance(JsonRequestPtr request) {
			StopPtr this_stop_ = _catalogue.GetStopByName(request->_name);
			for (auto& dist : request->_distances) {
				_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
			}
			return *this;
		}

		// выполнение блока запросов на добавление остановок
		RequestHandler& RequestHandler::AddStopsProcess(const std::deque<JsonRequestPtr>& requests) {
			for (JsonRequestPtr request : requests) {

				// создаём остановку в каталоге
				_catalogue.AddStopsProcess(MakeStop(request));

				// заполняем временное хранилище запросов по дистанциям которое будет обработано после добавления всех остановок
				if (request->_distances.size() != 0) {
					_current_distance_json_requests[request->_name] = &request->_distances;
				}
			}

			// сразу же добавляем все дистанции
			AddDistancesProcess();
			return *this;
		}

		// выполнение блока запросов на добавление маршрутов
		RequestHandler& RequestHandler::AddRoutesProcess(const std::deque<JsonRequestPtr>& requests) {
			for (JsonRequestPtr request : requests) {
				_catalogue.AddRoute(std::move(MakeRoute(request)));
			}
			return *this;
		}

		// получить информацию по автобусу
		RouteStatPtr RequestHandler::GetRouteInfo(JsonRequestPtr request) {
			return _catalogue.GetRouteInfo(request->_name);
		}

		// получить информацию по остановке
		StopStatPtr RequestHandler::GetStopInfo(JsonRequestPtr request) {
			return _catalogue.GetBusesForStopInfo(request->_name);
		}

		// получить информацию маршрутизатора
		RouterData RequestHandler::GetRouterData(JsonRequestPtr request) {
			if (!_transport_router) {
				EnableTransportRouter();
			}
			return _transport_router.get()->MakeRoute(request->_from, request->_to);
		}

		// получить карту маршрутов
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

		// создаёт маршрут на основе запроса
		Route RequestHandler::MakeRoute(JsonRequestPtr request) {
			Route result;
			result._is_circular = request->_is_circular;
			result._route_name = request->_name;
			for (auto stop : request->_stops) {
				result._stops.push_back(_catalogue.GetStopByName(stop));
			}
			return result;
		}

		// создаёт остановку на основе запроса
		Stop RequestHandler::MakeStop(JsonRequestPtr request) {
			return { std::string(request->_name), request->_coord.lat, request->_coord.lng };
		}

		// выполнение блока запросов на добавление дистанций из массива
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