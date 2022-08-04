#pragma once

#include <iostream>

#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std::literals;

namespace transport_catalogue {

	struct Request;
	using RequestPtr = Request*;

	struct StatRequest;
	using StatRequestPtr = StatRequest*;

	enum RequestType {
		null_req = 0,
		add_stop, add_route, add_dist, stop_info, route_info, stat_info, print_history

		// stat_info - общий тип запроса на получение информации по маршруту или остановке
		// нужен по условию задачи, так как все запросы на получение информации должны 
		// обрабатываться в порядке поступления, а не по очереди как с было у меня

		// add_dist - пока не использован, так как добавление дистанций оформленно как
		// sub-запрос к добавлению остановки, если поле _distances имеет данные

		// print_history - пока не использован, так как система в статическом режиме
		// имеет смысл при онлайн работе с возможностью конвейерной обработки запросов
		// которые задаются в случайном порядке
	};

	// структура запроса общего вида
	struct Request {
		Request() = default;

		explicit Request(std::string&& line) : _input_line(line) {
		}

		RequestPtr GetPtr() {
			return this;
		}

		RequestType _type = RequestType::null_req;               // тип запроса
		std::string_view _key = ""sv;                            // ключ запроса
		std::string_view _key_name = ""sv;                       // имя запроса (маршрута, остановки)
		geo::Coordinates _coord = { 0L, 0L };                    // координаты, если остановка
		std::vector<std::string_view> _stops = {};               // остановки, если маршрут
		std::map<std::string_view, int64_t> _distances = {};     // расстояния до остановок
		bool _is_circular = true;                                // тип маршрута
		std::string _input_line = ""s;                           // входная строка
	};

	// структура запроса стат_запроса на получение информации
	struct StatRequest {
		explicit StatRequest(std::string&& line) : _input_line(line) {
		}

		StatRequestPtr GetPtr() {
			return this;
		}
	
		RequestType _type = RequestType::stat_info;              // тип запроса
		std::string_view _key = ""sv;                            // ключ запроса
		std::string_view _key_name = ""sv;                       // имя запроса (маршрута, остановки)
		std::string _input_line = ""s;                           // входная строка
	};

	// основной класс обработчик запросов
	// формирует очередь запросов к серверу
	// сохраняет историю запросов
	class RequestHandler
	{
	public:

		// создаёт новый запрос в истории и передает работу с ним функциям парсинга
		RequestPtr AddRequest(std::string);

		// добавляет запрос в дек текущих задач после парсинга
		void AddInProcessLine(RequestPtr request) {
			_current_requests[request->_type].push_back(request);
		}

		// добавляет запрос из готовой структуры
		void AddRequest(Request&&);

		// создаёт маршрут на основе запроса
		Route MakeRoute(TransportCatalogue&, RequestPtr);

		// создаёт остановку на основе запроса
		Stop MakeStop(RequestPtr request) {
			return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
		}

		// выполнение запросов на добавление дистанций из массива
		void AddDistanceProcess(TransportCatalogue&);

		// выполнение запросов на добавление дистанций по запросу
		void AddDistanceProcess(TransportCatalogue&, RequestPtr);

		// выполнение запросов на добавление остановки
		void AddStopProcess(TransportCatalogue&);

		// выполнение запросов на добавление маршрута
		void AddRouteProcess(TransportCatalogue&);

		// выполнение запросов на получение информации по остановке
		void StopInfoProcess(TransportCatalogue&, std::ostream&);

		// выполнение запросов на получение информации по маршруту
		void RouteInfoProcess(TransportCatalogue&, std::ostream&);

		// выполнение запросов на получение информации в общем случае
		void StatInfoProcess(TransportCatalogue&, std::ostream&);

		// обработчик текущей очереди запросов
		void RequestsProcess(TransportCatalogue&, std::ostream&);

	private:
		
		// история всех поступивших запросов
		std::deque<Request> _requests_history;
		// мап текущих к обработке запросов
		std::map<RequestType, std::deque<RequestPtr>> _current_requests = {};
		// временное хранилище для sub-запросов на добавление дистанции
		std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_requests = {};
		// принтер для печати результатов на запрос
		//CatalogueReporter _printer;
	};

	namespace input_reader {

		namespace detail {

			// парсинг слов со сплиттером-разделителем
			std::vector<std::string_view> SplitIntoWords(std::string_view, std::string_view);

			// парсинг запросов на добавление остановки - дистанции
			void ParseDistances(RequestPtr, std::string_view);

			// парсинг запросов на добавление остановки - координаты
			size_t ParseCoordinates(RequestPtr, std::string_view);
		}

		// парсинг запросов на добавление остановки
		void ParseStop(RequestPtr, std::string_view);

		// парсинг запросов на добавление маршрута
		void ParseRoute(RequestPtr, std::string_view);

		// парсинг атрибутов запросов
		void ParseAtribute(RequestPtr, std::string_view);

		// начало пасинга запроса, определение ключа
		void ParseRequest(RequestPtr);

		// точка входа в систему, режим безостановочной работы
		// парсит запросы и вносит в RequestHander - основной обработчик запросов
		void RequestProcessor(TransportCatalogue, std::istream&, std::ostream&);

		// точка входа в систему, статический режим работы
		// схема работы алгоритма
		// число - количество запросов на загрузку информации
		// построчное считывание согласно количеству запросов
		// число - количество запросов на получение информации
		// построчное считывание согласно количеству запросов
		// парсинг всех полученных запросов
		// запуск блочной обработки запросов
		void RequestProcessorSimple(TransportCatalogue, std::istream&, std::ostream&);
	}
}