#pragma once
#include <iostream>
#include "transport_catalogue.h"

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
	}
}