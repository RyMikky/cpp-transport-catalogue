#pragma once    
#include "domain.h"

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

	// �������� ���������
	class TransportCatalogue
	{
		friend class RequestHandler;
	public:
		TransportCatalogue() = default;
		~TransportCatalogue() = default;
		
		// ---------------------------------------------- ���� ������� �������� ���������� ------------------------------------------------------
		
		void RangeCalculate(Route&);                                                      // ������ ����� ��������
		void AddStop(Stop&&);                                                             // �������� ���������
		void AddRoute(Route&&);                                                           // �������� �������
		void AddDistance(StopPtr, StopPtr, size_t);                                       // �������� ���������� ����� ����� �����������
		
		// ---------------------------------------------- ���� ������� ��������� ���������� -----------------------------------------------------
		
		size_t GetDistance(StopPtr, StopPtr);                                             // �������� ��������� � ������ ��� �������� �����������
		size_t GetDistanceSimple(StopPtr, StopPtr);                                       // �������� ��������� � ������ �����������
		StopPtr GetStopByName(const std::string_view) const;                              // ���������� ��������� �� ��������� �� �� �����
		RoutePtr GetRouteByName(const std::string_view) const;                            // ���������� ��������� �� ������� �� ��� �����
		RouteStatPtr GetRouteInfo(const std::string_view) const;                          // ���������� ��������� �� ��������� ������� � ��������
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;                    // ���������� ��������� �� ��������� ������� � ���������
		std::map<std::string, RendererData> GetRendererData();                            // ���������� ������ ��� ���������� ���������

	private:
		
		std::deque<Stop> _all_stops_data;                                                 // �������� ������ ������ �� ����������
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;                     // ��� ���������� ���������
		
		std::deque<Route> _all_buses_data;                                                // �������� ������ ���������
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;                    // ��� ���������� ���������

		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;         // ���������� ����� �����������
	
		std::string_view GetStopName(StopPtr);                                            // ���������� string_view � ������ ��������� �� ��������� �� ��������� ��������� Stop	
		std::string_view GetStopName(const Stop);                                         // ���������� string_view � ������ ��������� ��� ���������� ��������� Stop
		
		std::string_view GetBusName(RoutePtr);                                            // ���������� string_view � ������� �������� �� ��������� �� ��������� ��������� Route
		std::string_view GetBusName(const Route);                                         // ���������� string_view � ������� �������� ��� ���������� ��������� Route
	};

}