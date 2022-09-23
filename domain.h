#pragma once
#include "geo.h"
#include "graph.h"

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
		add_stop, add_route, add_dist, stop_info, route_info, stat_info, map_render, print_history, routing

		// stat_info - ����� ��� ������� �� ��������� ���������� �� �������� ��� ���������
		// ����� �� ������� ������, ��� ��� ��� ������� �� ��������� ���������� ������ 
		// �������������� � ������� �����������, � �� �� ������� ��� � ���� � ����

		// add_dist - ���� �� �����������, ��� ��� ���������� ��������� ���������� ���
		// sub-������ � ���������� ���������, ���� ���� _distances ����� ������

		// print_history - ���� �� �����������, ��� ��� ������� � ����������� ������
		// ����� ����� ��� ������ ������ � ������������ ����������� ��������� ��������
		// ������� �������� � ��������� �������
	};

	// ��������� ������� ��� ������ c ����������
	struct SimpleRequest;
	using SimpleRequestPtr = SimpleRequest*;

	// ��������� ������� ��� ������ � json
	struct JsonRequest;
	using JsonRequestPtr = JsonRequest*;

	// ������ �� ��������� ������ ��� �������
	struct RendererRequest;
	using RendererDataPtr = RendererRequest*;

	// ��������� ������� ��� ������ � ����������
	struct SimpleRequest {
		SimpleRequest() = default;

		explicit SimpleRequest(std::string&& line);

		SimpleRequestPtr GetPtr();

		size_t _id = 0;                                          // id �������
		RequestType _type = RequestType::null_req;               // ��� �������
		std::string_view _key = ""sv;                            // ���� ������� (Bus, Stop, Map, Route)
		std::string_view _name = ""sv;                           // ��� ������� (��������, ���������)
		std::string_view _from = ""sv;                           // ������ ���� ��� ������� Route
		std::string_view _to = ""sv;                             // �������� ���� ��� ������� Route
		geo::Coordinates _coord = { 0L, 0L };                    // ����������, ���� ���������
		std::vector<std::string_view> _stops = {};               // ���������, ���� �������
		std::map<std::string_view, int64_t> _distances = {};     // ���������� �� ���������
		bool _is_circular = true;                                // ��� ��������
		std::string _input_line = ""s;                           // ������� ������
	};

	// ��������� ������� ��� ������ � json
	struct JsonRequest {
		JsonRequest() = default;

		JsonRequestPtr GetPtr();

		size_t _id = 0;                                          // id �������
		RequestType _type = RequestType::null_req;               // ��� �������
		std::string _key = ""s;                                  // ���� ������� (Bus, Stop, Map, Route)
		std::string _name = ""s;                                 // ��� ������� (��������, ���������)
		std::string _from = ""s;                                 // ������ ���� ��� ������� Route
		std::string _to = ""s;                                   // �������� ���� ��� ������� Route
		geo::Coordinates _coord = { 0L, 0L };                    // ����������, ���� ���������
		std::vector<std::string> _stops = {};                    // ���������, ���� �������
		std::map<std::string, int64_t> _distances = {};          // ���������� �� ���������
		bool _is_circular = true;                                // ��� ��������

	};

	// ������ �� ���������� ���������� �������
	struct RendererRequest {
		RendererRequest() = default;
		RendererRequest(bool, std::vector<StopPtr>);

		bool _is_circular = false;
		std::vector<StopPtr> _stops;
	};

	// ��������� ������ ������� � ���� ������
	struct RendererData {
		std::string _svg_line = {};
	};

	// ��������� ������� ������������ �������� 
	struct RouterItem
	{
		RouterItem& SetItemName(std::string_view);                    // ������ �������� ������� ��������
		RouterItem& SetItemSpanCount(int);                            // ������ ���������� ������������ ��������� �� ������� ��������
		RouterItem& SetItemTime(double);                              // ������ ����� ����������� ������� ��������
		RouterItem& SetItemEdgeType(graph::EdgeType);                 // ������ ��� ������� ��������

		std::string_view GetItemName() const;                         // �������� �������� ������� ��������
		int GetItemSpanCount() const;                                 // �������� ���������� ������������ ��������� �� ������� ��������
		double GetItemTime() const;                                   // �������� ����� ����������� ������� ��������
		graph::EdgeType GetItemType() const;                          // �������� ��� ������� ��������

		std::string _item_name = {};                                  // �������� ������� ��������
		int _span_count = 0;                                          // ���������� ������������ ��������� �� ������� ��������
		double _time = 0.0;                                           // ����� ����������� ������� ��������
		graph::EdgeType _type;                                        // ��� ������� ��������
	};
	// ��������� ������������ ��������
	struct RouterData
	{
		double _route_time = 0.0;                                     // ����� � ���� �� ��������
		std::vector<RouterItem> _route_items;                         // ������� ��������
		bool _is_founded = false;                                     // ������ ��������
	};

} // namespace transport_catalogue