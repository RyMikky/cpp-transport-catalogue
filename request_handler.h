#pragma once
#include <iostream>

#include "transport_catalogue.h"                       // модуль транспортного каталога
#include "map_renderer.h"                              // модуль визуализации
#include "domain.h"                                    // модуль основных структур

#include "simple_reader.h"                             // модуль объединивший input и stat reader
#include "json_reader.h"                               // модуль ввода/вывода KSON

using namespace std::literals;

namespace transport_catalogue {

	namespace request_handler {

		// основной класс обработчик запросов
		// задает интерфейс работы классов-наследников
		class RequestHandler
		{
		protected:
			transport_catalogue::TransportCatalogue& _catalogue;
			std::istream& _input;
			std::ostream& _output;

		public:
			RequestHandler() = default;
			RequestHandler(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			virtual ~RequestHandler() = default;
			virtual void AddDistanceProcess() = 0;                               // выполнение запросов на добавление дистанций из массива
			virtual void AddStopProcess() = 0;                                   // выполнение запросов на добавление остановки
			virtual void AddRouteProcess() = 0;                                  // выполнение запросов на добавление маршрута
			virtual void StopInfoProcess() = 0;                                  // выполнение запросов на получение информации по остановке
			virtual void RouteInfoProcess() = 0;                                 // выполнение запросов на получение информации по маршруту
			virtual void StatInfoProcess() = 0;                                  // выполнение запросов на получение информации в общем случае
			virtual void RequestsProcess() = 0;                                  // обработчик текущей очереди запросов
		private:
			virtual void RequestProcessor() = 0;                                 // точка входа, обрабатывается из конструктора
		};

		// класс-наследник от базового RequestHandler
		// работает в режиме терминала принимая простые запросы из командной строки
		class RequestHandlerTerminal : protected RequestHandler {
		public:
			RequestHandlerTerminal() = default;
			RequestHandlerTerminal(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			SimpleRequestPtr AddSimpleRequest(std::string);

			// добавляет запрос в дек текущих задач после парсинга
			void AddInProcessLine(SimpleRequestPtr request) {
				_current_simple_requests[request->_type].push_back(request);
			}

			void AddSimpleRequest(SimpleRequest&&);                               // добавляет запрос из готовой структуры
			Route MakeRoute(SimpleRequestPtr);                                    // создаёт маршрут на основе запроса

			// создаёт остановку на основе запроса
			Stop MakeStop(SimpleRequestPtr request) {
				return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
			}

			void AddDistanceProcess(SimpleRequestPtr);                            // выполнение запросов на добавление дистанций по запросу
			void AddDistanceProcess() override;                                   // выполнение запросов на добавление дистанций из массива
			void AddStopProcess() override;                                       // выполнение запросов на добавление остановки
			void AddRouteProcess() override;                                      // выполнение запросов на добавление маршрута
			void StopInfoProcess() override;                                      // выполнение запросов на получение информации по остановке
			void RouteInfoProcess() override;                                     // выполнение запросов на получение информации по маршруту
			void StatInfoProcess() override;                                      // выполнение запросов на получение информации в общем случае
			void RequestsProcess() override;                                      // обработчик текущей очереди запросов

		private:

			void RequestProcessor() override;                                     // точка входа, обрабатывается из конструктора

			// история поступивших запросов простого типа
			std::deque<SimpleRequest> _simple_requests_history = {};
			// мап текущих к обработке запросов простого типа
			std::map<RequestType, std::deque<SimpleRequestPtr>> _current_simple_requests = {};
			// временное хранилище для sub-запросов на добавление дистанции
			std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_simple_requests = {};
		};

		// класс-наследник от базового RequestHandler
		// работает в режиме обработки бока простых запросов переданых в форме строки
		class RequestHandlerSimple : protected RequestHandler {
		public:
			RequestHandlerSimple() = default;
			RequestHandlerSimple(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			SimpleRequestPtr AddSimpleRequest(std::string);

			// добавляет запрос в дек текущих задач после парсинга
			void AddInProcessLine(SimpleRequestPtr request) {
				_current_simple_requests[request->_type].push_back(request);
			}

			void AddSimpleRequest(SimpleRequest&&);                               // добавляет запрос из готовой структуры
			Route MakeRoute(SimpleRequestPtr);                                    // создаёт маршрут на основе запроса

			// создаёт остановку на основе запроса
			Stop MakeStop(SimpleRequestPtr request) {
				return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
			}

			void AddDistanceProcess(SimpleRequestPtr);                            // выполнение запросов на добавление дистанций по запросу
			void AddDistanceProcess() override;                                   // выполнение запросов на добавление дистанций из массива
			void AddStopProcess() override;                                       // выполнение запросов на добавление остановки
			void AddRouteProcess() override;                                      // выполнение запросов на добавление маршрута
			void StopInfoProcess() override;                                      // выполнение запросов на получение информации по остановке
			void RouteInfoProcess() override;                                     // выполнение запросов на получение информации по маршруту
			void StatInfoProcess() override;                                      // выполнение запросов на получение информации в общем случае
			void RequestsProcess() override;                                      // обработчик текущей очереди запросов

		private:

			void RequestProcessor() override;                                     // точка входа, обрабатывается из конструктора

			// история поступивших запросов простого типа
			std::deque<SimpleRequest> _simple_requests_history = {};
			// мап текущих к обработке запросов простого типа
			std::map<RequestType, std::deque<SimpleRequestPtr>> _current_simple_requests = {};
			// временное хранилище для sub-запросов на добавление дистанции
			std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_simple_requests = {};
		};

		// класс-наследник от базового RequestHandler
		// работает в режиме обработки JSON-блоков
		class RequestHandlerJSON : protected RequestHandler {
		public:
			RequestHandlerJSON() = default;
			RequestHandlerJSON(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			json::Dict& AddJSONlibary(json::Dict&&);                              // добавление JSON-блока в базу
			void AddJSONRequest(JsonRequest&&);                                   // добавляет запрос из готовой структуры

			Route MakeRoute(JsonRequestPtr);                                      // создаёт маршрут на основе запроса
			Stop MakeStop(JsonRequestPtr request);                                // создаёт остановку на основе запроса

			void RendererSetupProcess(json::Dict);                                // назначение настроек рендера
			void RendererProcess(map_renderer::MapRenderer&);                     // запуск рендера по запросу из JSON

			void AddDistanceProcess(JsonRequestPtr);                              // выполнение запросов на добавление дистанций по запросу
			void AddDistanceProcess() override;                                   // выполнение запросов на добавление дистанций из массива
			void AddStopProcess() override;                                       // выполнение запросов на добавление остановки
			void AddRouteProcess() override;                                      // выполнение запросов на добавление маршрута
			void StopInfoProcess() override;                                      // выполнение запросов на получение информации по остановке
			void RouteInfoProcess() override;                                     // выполнение запросов на получение информации по маршруту
			void StatInfoProcess() override;                                      // выполнение запросов на получение информации в общем случае
			void RequestsProcess() override;                                      // обработчик текущей очереди запросов

		private:

			void RequestProcessor() override;                                     // точка входа, вызывается из конструктора

			map_renderer::RendererSettings _renderer_settings;                    // блок настроек рендера
			std::deque<JsonRequest> _json_requests_history;                       // история поступивших запросов JSON-формата
			std::deque<json::Dict> _json_dicts;                                   // история полученных JSON-блоков
			// мап текущих к обработке запросов JSON-формата
			std::map<RequestType, std::deque<JsonRequestPtr>> _current_json_requests = {};
			// временное хранилище для sub-запросов на добавление дистанции
			std::map<std::string, std::map<std::string, int64_t>*> _current_distance_json_requests = {};
		};

	} // namespace request_handler

} // namespace transport_catalogue