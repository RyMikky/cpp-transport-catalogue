#include "request_handler.h"

namespace transport_catalogue {

	namespace request_handler {

		// ------------------------- class RequestHandler ------------------------

		// загрузить сторонний каталог
		RequestHandler& RequestHandler::SetTransportCatalogue(transport_catalogue::TransportCatalogue&& catalogue) {
			_transport_catalogue = std::move(catalogue);

			// если роутер уже был сконфигурирован, то перезагружаем данные роутинга
			if (_transport_router) {
				_transport_router.get()->ImportRoutingDataFromCatalogue();
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

		// привести модули к состоянию по умолчанию
		RequestHandler& RequestHandler::SetModulesToDefault() {

			_transport_catalogue.ClearAllData();                // удаляем все данные каталога

			if (_transport_router) {
				_transport_router.reset();                      // удаляем уже настроенный роутер
			}

			if (_map_renderer) {
				_map_renderer.reset();                          // удаляем уже настроенный рендер
			}

			if (_serializer) {
				_serializer.reset();                            // удаляем уже настроенный сериалайзер
			}

			return *this;
		}

		// установить флаг состояния данных каталога
		RequestHandler& RequestHandler::SetCatalogueDataStatus(transport_catalogue::CatalogueDataStatus status) {
			_transport_catalogue.SetDataStatus(status);
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
				_transport_router = std::make_shared<router::TransportRouter>(_transport_catalogue, _router_settings);
			}
			return *this;
		}

		// инициализация и расчёт базы данных роутера
		RequestHandler& RequestHandler::EnableRouterGraphs() {

			if (!_transport_router) {
				// проверяем то, что роутер уже активирован
				EnableTransportRouter();
			}

			// загружаем данные роутера для построения графов
			_transport_router->ImportRoutingDataFromCatalogue();
			return *this;
		}

		// инициализация обработчика сериализации
		RequestHandler& RequestHandler::EnableSerializer() {
			if (!_serializer) {
				_serializer = std::make_shared<serial_handler::SerialHandler>(_transport_catalogue, _router_settings, _renderer_settings);
			}
			return *this;
		}

		// добавить остановку в каталог
		RequestHandler& RequestHandler::AddStop(JsonRequestPtr request) {
			_transport_catalogue.AddStop(MakeStop(request));
			AddDistance(request);
			return *this;
		}

		// добавить маршрут в каталог
		RequestHandler& RequestHandler::AddRoute(JsonRequestPtr request) {
			_transport_catalogue.AddRoute(std::move(MakeRoute(request)));
			return *this;
		}
		
		// добавить расстояние в каталог
		RequestHandler& RequestHandler::AddDistance(JsonRequestPtr request) {
			StopPtr this_stop_ = _transport_catalogue.GetStopByName(request->_name);
			for (auto& dist : request->_distances) {
				_transport_catalogue.AddDistance(this_stop_, _transport_catalogue.GetStopByName(dist.first), dist.second);
			}
			return *this;
		}

		// выполнение блока запросов на добавление остановок
		RequestHandler& RequestHandler::AddStopsProcess(const std::deque<JsonRequestPtr>& requests) {
			for (JsonRequestPtr request : requests) {

				// создаём остановку в каталоге
				_transport_catalogue.AddStop(MakeStop(request));

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
				_transport_catalogue.AddRoute(std::move(MakeRoute(request)));
			}
			return *this;
		}

		// получить информацию по автобусу
		RouteStatPtr RequestHandler::GetRouteInfo(JsonRequestPtr request) {
			return _transport_catalogue.GetRouteInfo(request->_name);
		}

		// получить информацию по остановке
		StopStatPtr RequestHandler::GetStopInfo(JsonRequestPtr request) {
			return _transport_catalogue.GetBusesForStopInfo(request->_name);
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

			for (auto& route : _transport_catalogue.GetDataToRenderer()) {
				_map_renderer.get()->AddRendererData(route);
			}

			_map_renderer.get()->CoordsProject();
			return _map_renderer.get()->StructRendererProcess();
		}

		// отправить данные в поток
		bool RequestHandler::SerializeDataToOstream(std::ostream& output) {

			if (!_serializer) {                                          // если сериалайзер не включен
				EnableSerializer();                                      // активируем модуль сериализации
			}

			_serializer->GetDataFromCatalogue();                         // загружаем данные из каталога и готовим к отправке

			if (_transport_router) {                                     // если роутер уже активирован
				_serializer->SetTransportRouter(_transport_router);      // передаем указатель на роутер в модуль сериализации
				_serializer->GetDataFromRouter();                        // загружаем данные из роутера и готовим к отправке
			}
			return _serializer->UploadDataToOstream(output);          // отправляем данные в поток
		}

		// загрузить данные из потока
		bool RequestHandler::DeserializeDataFromIstream(std::istream& input) {

			if (!_serializer) {                                          // если сериалайзер не включен
				EnableSerializer();                                      // активируем модуль сериализации
			}

			if (_serializer->DownloadDataFromIstream(input)) {        // получам данные из потока

				_serializer->ApplyDataToCatalogue();                     // загружаем данные в каталог

				if (_serializer->IsRouterDataIncomeFromOutside()) {      // если в числе прочего загруженны данные ренедера
					EnableTransportRouter();                             // запускаем рендер без загрузки графов из каталога
					_serializer->SetTransportRouter(_transport_router);  // на всякий пожарный назначаем указатель на роутер
				}

				_serializer->ApplyDataToRouter();                        // загружаем базу роутера

				return true;
			}

			return false;
		}

		// создаёт маршрут на основе запроса
		Route RequestHandler::MakeRoute(JsonRequestPtr request) {
			Route result;
			result._is_circular = request->_is_circular;
			result._route_name = request->_name;
			for (auto stop : request->_stops) {
				result._stops.push_back(_transport_catalogue.GetStopByName(stop));
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
					StopPtr this_stop_ = _transport_catalogue.GetStopByName(this_stop.first);
					for (auto& dist : *this_stop.second) {
						_transport_catalogue.AddDistance(this_stop_, _transport_catalogue.GetStopByName(dist.first), dist.second);
					}
				}
			}
			_current_distance_json_requests.clear();
			return *this;
		}

		// ------------------------- class RequestHandler END ------------------------

	} // namespace request_handler

} // namespace transport_catalogue