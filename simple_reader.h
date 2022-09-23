#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>

#include "request_handler.h"

using namespace std::literals;

// фаил компиляция двух ранних модулей - input_reader и stat_reader
// модуль необходим для работы каталога в режимах Simple и SimpleTrerminal

namespace simple_reader {

	namespace detail {

		// парсинг слов со сплиттером-разделителем
		std::vector<std::string_view> SplitIntoWords(std::string_view, std::string_view);

		// парсинг запросов на добавление остановки - дистанции
		void ParseDistances(transport_catalogue::SimpleRequestPtr, std::string_view);

		// парсинг запросов на добавление остановки - координаты
		size_t ParseCoordinates(transport_catalogue::SimpleRequestPtr, std::string_view);

	} // namespace detail

	// парсинг запросов на добавление остановки
	void ParseStop(transport_catalogue::SimpleRequestPtr, std::string_view);

	// парсинг запросов на добавление маршрута
	void ParseRoute(transport_catalogue::SimpleRequestPtr, std::string_view);

	// парсинг атрибутов запросов
	void ParseAtribute(transport_catalogue::SimpleRequestPtr, std::string_view);

	// начало пасинга запроса, определение ключа
	void ParseRequest(transport_catalogue::SimpleRequestPtr, const std::string&);

} // namespace input_reader



namespace transport_catalogue {

	namespace stat_reporter {

		// перегрузка вывода результата по маршрутам
		inline std::ostream& operator<<(std::ostream&, const transport_catalogue::RouteStatPtr&);

		void RouteInfoPrinter(std::ostream&, std::string_view, transport_catalogue::RouteStatPtr);

		// перегрузка вывода результатов по остановкам
		inline std::ostream& operator<<(std::ostream&, const transport_catalogue::StopStatPtr&);

		void StopInfoPrinter(std::ostream&, std::string_view, transport_catalogue::StopStatPtr);

		// если задачи усложнятся то можно будет сделать класс-принтер
		//class CatalogueReporter
		//{
		//public:
		//	CatalogueReporter() = default;
		//	~CatalogueReporter() = default;

		//	// печать инфо по маршруту
		//	void RoutePrinter(std::ostream&, std::string_view, transport_catalogue::RouteStatPtr);

		//private:
		//};

	} // namespace stat_reporter

	namespace simple_reader {

		class SimpleReader {
		private:
			std::istream& _input;
			std::ostream& _output;
			transport_catalogue::request_handler::RequestHandler _handler;                           // обработчик запросов к базе данных транспортного каталога
		public:
			SimpleReader(std::istream&, std::ostream&);

			SimpleReader& run_terminal_process();                                                    // запуск режима-терминала

		private:
			bool InProcess = true;                                                                   // флаг работы запуска терминала
			std::deque<transport_catalogue::JsonRequest> _requests_history;                          // история поступивших запросов 

			void AddStopParser(transport_catalogue::JsonRequestPtr, const std::string&);
			void AddRouteParser(transport_catalogue::JsonRequestPtr, const std::string&);
			void AddDistanceParser(transport_catalogue::JsonRequestPtr, const std::string&);

			SimpleReader& CommandLineViewer();                                                       // показать список доступных комманд

		};

	} // namespace simple_reader

} // namespace transport_catalogue
