#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string_view>

#include "domain.h"

using namespace std::literals;

// ���� ���������� ���� ������ ������� - input_reader � stat_reader
// ������ ��������� ��� ������ �������� � ������� Simple � SimpleTrerminal

namespace simple_reader {

	namespace detail {

		// ������� ���� �� ����������-������������
		std::vector<std::string_view> SplitIntoWords(std::string_view, std::string_view);

		// ������� �������� �� ���������� ��������� - ���������
		void ParseDistances(transport_catalogue::SimpleRequestPtr, std::string_view);

		// ������� �������� �� ���������� ��������� - ����������
		size_t ParseCoordinates(transport_catalogue::SimpleRequestPtr, std::string_view);

	} // namespace detail

	// ������� �������� �� ���������� ���������
	void ParseStop(transport_catalogue::SimpleRequestPtr, std::string_view);

	// ������� �������� �� ���������� ��������
	void ParseRoute(transport_catalogue::SimpleRequestPtr, std::string_view);

	// ������� ��������� ��������
	void ParseAtribute(transport_catalogue::SimpleRequestPtr, std::string_view);

	// ������ ������� �������, ����������� �����
	void ParseRequest(transport_catalogue::SimpleRequestPtr);

} // namespace input_reader



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

	} // namespace stat_reporter

} // namespace transport_catalogue
