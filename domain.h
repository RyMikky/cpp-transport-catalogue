#pragma once
#include "geo.h"

#include <string>
#include <string_view>
#include <set>
#include <map>
#include <vector>

using namespace std::literals;

namespace transport_catalogue {

	// ������� ������� �������� ��� ������ �������� �������

	struct Stop;
	struct Route;

	using StopPtr = const Stop*;
	using RoutePtr = const Route*;

	// ���������
	struct Stop
	{
		Stop() = default;
		Stop(const std::string_view, const double, const double);
		Stop(StopPtr);

		std::string _name;                   // �������� ���������
		geo::Coordinates _coords{ 0L,0L };   // ����������

		bool operator==(const Stop&) const;
	};

	// ����� ��� ����������
	class StopHasher {
	public:
		std::size_t operator()(const StopPtr) const noexcept;
	private:
		std::hash<std::string> _hasher;
	};

	// ��������� ������ �� ������ � ���������
	struct StopStat
	{
		explicit StopStat(std::string_view, std::set<std::string_view>&);
		std::string_view _name;
		std::set<std::string_view> _buses;
	};

	using StopStatPtr = const StopStat*;

	// ����� ��� pair<StopPtr, StopPtr>
	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<StopPtr, StopPtr>) const noexcept;
	private:
		std::hash<const void*> _hasher;
	};

	// �������
	struct Route {

		Route() = default;
		Route(RoutePtr);

		std::string _route_name;              // �����/�������� �������� 
		std::vector<StopPtr> _stops;          // ������ ���������� �� ��������� ��������
		size_t _unique_stops_qty = 0U;        // ���������� ���������
		double _geo_route_length = 0L;        // ����� �������� (�������������� ����������)
		size_t _real_route_length = 0U;       // �������������� ����� �������� � ������
		double _curvature = 1L;               // ���������� ��������������� ���������� �� ���������������
		bool _is_circular = false;            // ��� ��������

		bool operator==(const Route&) const;
	};

	// ����� ��� ���������
	class RouteHasher {
	public:
		std::size_t operator()(const RoutePtr) const noexcept;
	private:
		std::hash<std::string> _hasher;
	};

	// ��������� ������ �� ���� �� ��������
	struct RouteStat
	{

		explicit RouteStat(size_t, size_t, size_t, double, std::string_view);
		size_t _stops_on_route = 0U;           // ���������� ��������� �� ��������
		size_t _unique_stops = 0U;             // ���������� ���������� ���������
		size_t _real_route_length = 0U;        // �������������� ����� ��������
		double _curvature = 1L;                // ���������� ��������
		std::string _name;                     // �����/��� ��������
	};

	using RouteStatPtr = const RouteStat*;

	// ���� �������� ��������

	enum RequestType {
		null_req = 0,
		add_stop, add_route, add_dist, stop_info, route_info, stat_info, map_render, print_history

		// stat_info - ����� ��� ������� �� ��������� ���������� �� �������� ��� ���������
		// ����� �� ������� ������, ��� ��� ��� ������� �� ��������� ���������� ������ 
		// �������������� � ������� �����������, � �� �� ������� ��� � ���� � ����

		// add_dist - ���� �� �����������, ��� ��� ���������� ��������� ���������� ���
		// sub-������ � ���������� ���������, ���� ���� _distances ����� ������

		// print_history - ���� �� �����������, ��� ��� ������� � ����������� ������
		// ����� ����� ��� ������ ������ � ������������ ����������� ��������� ��������
		// ������� �������� � ��������� �������
	};

	// ������ �� ���������� ���� � simple-������
	struct SimpleRequest;
	using SimpleRequestPtr = SimpleRequest*;

	// ������ �� ��������� ���������� � simple-������
	struct StatRequestSimple;
	using StatRequestSimplePtr = StatRequestSimple*;

	// ������ �� ���������� ���� � json-������
	struct JsonRequest;
	using JsonRequestPtr = JsonRequest*;

	// ������ �� ��������� ���������� � json-������
	struct StatRequestJSON;
	using StatRequestJSONPtr = StatRequestJSON*;

	// ������ �� ��������� ������ ��� �������
	struct RendererData;
	using RendererDataPtr = RendererData*;

	// ��������� ������� �� ���������� ���� � simple-������
	struct SimpleRequest {
		SimpleRequest() = default;

		explicit SimpleRequest(std::string&& line);

		SimpleRequestPtr GetPtr();

		size_t _id = 0;                                          // id �������
		RequestType _type = RequestType::null_req;               // ��� �������
		std::string_view _key = ""sv;                            // ���� ������� (Bus, Stop)
		std::string_view _key_name = ""sv;                       // ��� ������� (��������, ���������)
		geo::Coordinates _coord = { 0L, 0L };                    // ����������, ���� ���������
		std::vector<std::string_view> _stops = {};               // ���������, ���� �������
		std::map<std::string_view, int64_t> _distances = {};     // ���������� �� ���������
		bool _is_circular = true;                                // ��� ��������
		std::string _input_line = ""s;                           // ������� ������
	};

	// ��������� ������� �� ��������� ���������� � simple-������
	struct StatRequestSimple {

		StatRequestSimple() = default;

		explicit StatRequestSimple(std::string&& line);

		StatRequestSimplePtr GetPtr();

		RequestType _type = RequestType::stat_info;              // ��� �������
		std::string_view _key = ""sv;                            // ���� �������
		std::string_view _key_name = ""sv;                       // ��� ������� (��������, ���������)
		std::string _input_line = ""s;                           // ������� ������
	};

	// ��������� ������� �� ���������� ���� � simpjsonle-������
	struct JsonRequest {
		JsonRequest() = default;

		JsonRequestPtr GetPtr();

		size_t _id = 0;                                          // id �������
		RequestType _type = RequestType::null_req;               // ��� �������
		std::string _key = ""s;                                  // ���� ������� (Bus, Stop)
		std::string _name = ""s;                                 // ��� ������� (��������, ���������)
		geo::Coordinates _coord = { 0L, 0L };                    // ����������, ���� ���������
		std::vector<std::string> _stops = {};                    // ���������, ���� �������
		std::map<std::string, int64_t> _distances = {};          // ���������� �� ���������
		bool _is_circular = true;                                // ��� ��������

	};

	// ��������� ������� �� ��������� ���������� � json-������
	struct StatRequestJSON {

		StatRequestJSON() = default;

		StatRequestJSONPtr GetPtr();

		RequestType _type = RequestType::stat_info;              // ��� �������
		std::string _key = ""s;                                  // ���� ������� (Bus, Stop)
		std::string _name = ""s;                                 // ��� ������� (��������, ���������)
	};

	// ������ �� ���������� ���������� �������
	struct RendererData {
		RendererData() = default;
		RendererData(bool, std::vector<StopPtr>);

		bool _is_circular = false;
		std::vector<StopPtr> _stops;
	};

}