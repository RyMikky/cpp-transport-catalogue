#pragma once
#include <iostream>
#include "transport_catalogue.h"

namespace transport_catalogue {

	namespace stat_reporter {

		// ���������� ������ ���������� �� ���������
		inline std::ostream& operator<<(std::ostream&, const transport_catalogue::RouteStatPtr&);

		void RouteInfoPrinter(std::ostream&, std::string_view, transport_catalogue::RouteStatPtr);

		// ���������� ������ ����������� �� ����������
		inline std::ostream& operator<<(std::ostream&, const transport_catalogue::StopStatPtr&);

		void StopInfoPrinter(std::ostream&, std::string_view, transport_catalogue::StopStatPtr);

		// ���� ������ ���������� �� ����� ����� ������� �����-�������
		//class CatalogueReporter
		//{
		//public:
		//	CatalogueReporter() = default;
		//	~CatalogueReporter() = default;

		//	// ������ ���� �� ��������
		//	void RoutePrinter(std::ostream&, std::string_view, transport_catalogue::RouteStatPtr);

		//private:
		//};
	}
}