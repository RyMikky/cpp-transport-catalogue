/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Request Handler                                                                //
//        Обобщающий модуль-класс - связывает собой все модули системы.                        //
//        Основной функционал - выполнение запросов к различным элементам програмы.            //
//        Требования и зависимости:                                                            //
//           1. domain.h - общая библиотека структур для работы программы                      //
//           2. transport_catalogue.h - модуль транспортного каталога                          //
//           3. transport_router.h - модуль роутера маршрутов                                  //
//           4. serialization.h - модуль сериализации данных                                   //
//           5. map_renderer.h - модуль визуализации карт                                      //
//           6. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include <iostream>

#include "transport_catalogue.h"                       // модуль транспортного каталога
#include "transport_router.h"                          // модуль роутинга маршрутов
#include "serialization.h"                             // модуль сериализации данных
#include "map_renderer.h"                              // модуль визуализации
#include "domain.h"                                    // модуль основных структур

using namespace std::literals;

namespace transport_catalogue {

	namespace request_handler {

		class RequestHandler {
		public:
			RequestHandler() = default;

			// ----------------------- сеттеры настроек рендера, роутера и обработчика ---------------------------------------------

			RequestHandler& SetTransportCatalogue(transport_catalogue::TransportCatalogue&&);        // загрузить сторонний каталог
			RequestHandler& SetRedererSettings(map_renderer::RendererSettings&&);                    // загрузить настройки рендера
			RequestHandler& SetRouterSettings(router::RouterSettings&&);                             // загрузить настройки роутера
			RequestHandler& SetModulesToDefault();                                   // привести модули к состоянию по умолчанию
			RequestHandler& SetCatalogueDataStatus(
				transport_catalogue::CatalogueDataStatus);                           // установить флаг состояния данных каталога

			// ----------------------- активация всторенных рендара, роутера и сериализатора ---------------------------------------

			RequestHandler& EnableMapRenderer();                                     // инициализация и первичная настройка рендера
			RequestHandler& EnableTransportRouter();                                 // инициализация и первичная настройка роутера
			RequestHandler& EnableRouterGraphs();                                    // инициализация и расчёт базы данных роутера
			RequestHandler& EnableSerializer();                                      // инициализация обработчика сериализации

			// ----------------------- обработка запросов на добавление информации -------------------------------------------------

			RequestHandler& AddStop(JsonRequestPtr);                                 // добавить остановку в каталог
			RequestHandler& AddRoute(JsonRequestPtr);                                // добавить маршрут в каталог
			RequestHandler& AddDistance(JsonRequestPtr);                             // добавить расстояние в каталог

			// ----------------------- пакетная обработка запросов на добавление информации ----------------------------------------

			RequestHandler& AddStopsProcess(const std::deque<JsonRequestPtr>&);      // пакетное добавление остановок в каталог
			RequestHandler& AddRoutesProcess(const std::deque<JsonRequestPtr>&);     // пакетное добавление маршрутов в каталог

			// ----------------------- получение информации из базы по запросу -----------------------------------------------------
			
			RouteStatPtr GetRouteInfo(JsonRequestPtr);                               // получить информацию по автобусу
			StopStatPtr GetStopInfo(JsonRequestPtr);                                 // получить информацию по остановке
			RouterData GetRouterData(JsonRequestPtr);                                // получить информацию маршрутизатора
			RendererData GetRendererData(JsonRequestPtr);                            // получить карту маршрутов

			// ----------------------- I/O блок сериализации данных ----------------------------------------------------------------

			bool SerializeDataToOstream(std::ostream&);                              // отправить данные в поток
			bool DeserializeDataFromIstream(std::istream&);                          // загрузить данные из потока

		private:
			transport_catalogue::TransportCatalogue _transport_catalogue;            // внутренний каталог
			std::shared_ptr<router::TransportRouter> _transport_router = nullptr;    // внутренний роутер
			std::shared_ptr<map_renderer::MapRenderer> _map_renderer = nullptr;      // внутренний рендер
			std::shared_ptr<serial_handler::SerialHandler> _serializer = nullptr;    // внутренний сериалайзер

			router::RouterSettings _router_settings;                                 // блок настроек роутера
			map_renderer::RendererSettings _renderer_settings;                       // блок настроек рендера

			// временное хранилище для sub-запросов на добавление дистанции
			std::map<std::string, std::map<std::string, int64_t>*> _current_distance_json_requests = {};

			Route MakeRoute(JsonRequestPtr);                                         // создаёт маршрут на основе запроса
			Stop MakeStop(JsonRequestPtr);                                           // создаёт остановку на основе запроса

			RequestHandler& AddDistancesProcess();                                   // добавление дистанций в каталог
			
		};

	} // namespace request_handler

} // namespace transport_catalogue