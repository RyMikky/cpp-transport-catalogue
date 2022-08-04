#pragma once
#include "geo.h"           

#include <deque>
#include <map>             
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <ostream>         
#include <unordered_set>
#include <unordered_map>
#include <algorithm>       

namespace transport_catalogue
{

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
		std::hash<std::string> hasher_;
	};

	// ��������� ������ �� ������ � ���������
	struct StopStat
	{
		explicit StopStat(std::string_view, std::set<std::string_view>&);
		std::string_view name;
		std::set<std::string_view> buses;
	};

	using StopStatPtr = const StopStat*;

	// ����� ��� pair<StopPtr, StopPtr>
	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<StopPtr, StopPtr>) const noexcept;
	private:
		std::hash<const void*> hasher_;
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
		std::hash<std::string> hasher_;
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


	// �������� ���������
	class TransportCatalogue
	{
		friend class RequestHandler;
	public:
		TransportCatalogue() = default;
		~TransportCatalogue() = default;
		
		// ������ ����� ��������
		void RangeCalculate(Route&);
		
		// �������� ���������
		void AddStop(Stop&&);
		// �������� �������
		void AddRoute(Route&&);
		// �������� ���������� ����� ����� �����������
		void AddDistance(StopPtr, StopPtr, size_t);
		
		// �������� ��������� � ������ ��� �������� �����������
		size_t GetDistance(StopPtr, StopPtr);

		// �������� ��������� � ������ �����������
		size_t GetDistanceSimple(StopPtr, StopPtr); 

		// ���������� ��������� �� ��������� �� �� �����
		StopPtr GetStopByName(const std::string_view) const;

		// ���������� ��������� �� ������� �� ��� �����
		RoutePtr GetRouteByName(const std::string_view) const;

		// ���������� ��������� �� ��������� ������� � ��������
		RouteStatPtr GetRouteInfo(const std::string_view) const;

		// ���������� ��������� �� ��������� ������� � ���������
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;

	private:
		// �������� ������ ������ �� ����������
		std::deque<Stop> _all_stops_data;
		// ��� ���������� ���������
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;
		// �������� ������ ���������
		std::deque<Route> _all_buses_data;
		// ��� ���������� ���������
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;
		// ���������� ����� �����������
		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;    

		// ���������� string_view � ������ ��������� �� ��������� �� ��������� ��������� Stop
		std::string_view GetStopName(StopPtr);
		// ���������� string_view � ������ ��������� ��� ���������� ��������� Stop
		std::string_view GetStopName(const Stop);

		// ���������� string_view � ������� �������� �� ��������� �� ��������� ��������� Route
		std::string_view GetBusName(RoutePtr);
		// ���������� string_view � ������� �������� ��� ���������� ��������� Route
		std::string_view GetBusName(const Route);
	};

}